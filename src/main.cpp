#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <system_error>

#if defined(_WIN32)
#include <windows.h>
#include <winioctl.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#endif

struct StorageDevice
{
    std::string path;
    uint64_t sizeInBytes;
};

#if defined(_WIN32)
std::vector<StorageDevice> EnumerateDevices()
{
    std::vector<StorageDevice> devices;
    for (int i = 0; i < 16; ++i)
    {
        std::string devicePath = "\\\\.\\PhysicalDrive" + std::to_string(i);
        HANDLE hDevice = CreateFileA(devicePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (hDevice != INVALID_HANDLE_VALUE)
        {
            GET_LENGTH_INFORMATION lengthInfo;
            DWORD bytesReturned = 0;
            if (DeviceIoControl(hDevice, IOCTL_DISK_GET_LENGTH_INFO, NULL, 0, &lengthInfo, sizeof(lengthInfo), &bytesReturned, NULL))
            {
                devices.push_back({ devicePath, static_cast<uint64_t>(lengthInfo.Length.QuadPart) });
            }
            CloseHandle(hDevice);
        }
    }
    return devices;
}

bool WipeDevice(const std::string& path, uint64_t size)
{
    HANDLE hDevice = CreateFileA(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH, NULL);
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    constexpr DWORD bufferSize = 1024 * 1024;
    std::vector<uint8_t> buffer(bufferSize);
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 255);

    uint64_t bytesWrittenTotal = 0;
    DWORD bytesWritten = 0;

    while (bytesWrittenTotal < size)
    {
        for (DWORD i = 0; i < bufferSize; ++i)
        {
            buffer[i] = static_cast<uint8_t>(dist(rng));
        }
        DWORD bytesToWrite = static_cast<DWORD>(std::min<uint64_t>(bufferSize, size - bytesWrittenTotal));
        if (!WriteFile(hDevice, buffer.data(), bytesToWrite, &bytesWritten, NULL) || bytesWritten == 0)
        {
            CloseHandle(hDevice);
            return false;
        }
        bytesWrittenTotal += bytesWritten;
    }

    LARGE_INTEGER li;
    li.QuadPart = 0;
    SetFilePointerEx(hDevice, li, NULL, FILE_BEGIN);

    bytesWrittenTotal = 0;
    std::fill(buffer.begin(), buffer.end(), 0x00);

    while (bytesWrittenTotal < size)
    {
        DWORD bytesToWrite = static_cast<DWORD>(std::min<uint64_t>(bufferSize, size - bytesWrittenTotal));
        if (!WriteFile(hDevice, buffer.data(), bytesToWrite, &bytesWritten, NULL) || bytesWritten == 0)
        {
            CloseHandle(hDevice);
            return false;
        }
        bytesWrittenTotal += bytesWritten;
    }

    CloseHandle(hDevice);
    return true;
}
#else
std::vector<StorageDevice> EnumerateDevices()
{
    std::vector<StorageDevice> devices;
    for (char c = 'a'; c <= 'z'; ++c)
    {
        std::string devicePath = "/dev/sd" + std::string(1, c);
        int fd = open(devicePath.c_str(), O_RDONLY);
        if (fd != -1)
        {
            uint64_t size = 0;
            if (ioctl(fd, BLKGETSIZE64, &size) >= 0)
            {
                devices.push_back({ devicePath, size });
            }
            close(fd);
        }
    }
    return devices;
}

bool WipeDevice(const std::string& path, uint64_t size)
{
    int fd = open(path.c_str(), O_WRONLY | O_DIRECT | O_SYNC);
    if (fd == -1)
    {
        return false;
    }

    constexpr size_t bufferSize = 1024 * 1024;
    void* rawBuffer = nullptr;
    if (posix_memalign(&rawBuffer, 4096, bufferSize) != 0)
    {
        close(fd);
        return false;
    }

    uint8_t* buffer = static_cast<uint8_t*>(rawBuffer);
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 255);

    uint64_t bytesWrittenTotal = 0;

    while (bytesWrittenTotal < size)
    {
        for (size_t i = 0; i < bufferSize; ++i)
        {
            buffer[i] = static_cast<uint8_t>(dist(rng));
        }
        size_t bytesToWrite = static_cast<size_t>(std::min<uint64_t>(bufferSize, size - bytesWrittenTotal));
        ssize_t written = write(fd, buffer, bytesToWrite);
        if (written <= 0)
        {
            free(rawBuffer);
            close(fd);
            return false;
        }
        bytesWrittenTotal += written;
    }

    lseek(fd, 0, SEEK_SET);
    bytesWrittenTotal = 0;
    std::fill(buffer, buffer + bufferSize, 0x00);

    while (bytesWrittenTotal < size)
    {
        size_t bytesToWrite = static_cast<size_t>(std::min<uint64_t>(bufferSize, size - bytesWrittenTotal));
        ssize_t written = write(fd, buffer, bytesToWrite);
        if (written <= 0)
        {
            free(rawBuffer);
            close(fd);
            return false;
        }
        bytesWrittenTotal += written;
    }

    free(rawBuffer);
    close(fd);
    return true;
}
#endif

int main()
{
    std::cout << "=== Kamutoke Storage Sanitization Engine ===" << std::endl;
    
    std::vector<StorageDevice> devices = EnumerateDevices();
    if (devices.empty())
    {
        std::cerr << "No block devices found or insufficient privileges." << std::endl;
        return 1;
    }

    for (size_t i = 0; i < devices.size(); ++i)
    {
        double sizeInGB = static_cast<double>(devices[i].sizeInBytes) / (1024.0 * 1024.0 * 1024.0);
        std::cout << "[" << i << "] " << devices[i].path << " (" << sizeInGB << " GB)" << std::endl;
    }

    std::cout << "Select target device index: ";
    size_t selection;
    if (!(std::cin >> selection) || selection >= devices.size())
    {
        std::cerr << "Invalid selection." << std::endl;
        return 1;
    }

    std::cout << "WARNING: Target " << devices[selection].path << " will be permanently destroyed." << std::endl;
    std::cout << "Type 'CONFIRM' to execute: ";
    std::string confirmation;
    std::cin >> confirmation;

    if (confirmation != "CONFIRM")
    {
        std::cout << "Operation aborted." << std::endl;
        return 0;
    }

    std::cout << "Executing wiping sequence..." << std::endl;
    if (WipeDevice(devices[selection].path, devices[selection].sizeInBytes))
    {
        std::cout << "Sanitization completed successfully." << std::endl;
    }
    else
    {
        std::cerr << "Sanitization failed during write operation." << std::endl;
        return 1;
    }

    return 0;
}
