# Kamutoke: Low-Level Storage Sanitization and Cryptographic Wiping Engine
The Kamutoke Project provides an unyielding, high-throughput storage media sanitization framework designed to perform irreversible physical block destruction on targeted block devices. Named after the legendary lightning staff of Sukuna, the engine acts as an absolute execution force against persistent data states. It completely rejects soft-deletion paradigms, file system unlinking tricks, and surface-level partitioning flags. Instead, Kamutoke interacts directly with underlying raw block interfaces to execute systematic, multi-pass pseudo-random overwrite sequences followed by active block zeroization before final logical release.
---## Technical Overview and Operational Architecture
Modern operating systems typically respond to standard format commands by marking allocation maps or index nodes as available, leaving the actual physical magnetic or NAND flash bits entirely intact and recoverable via low-level forensic reconstruction. Kamutoke prevents all vectors of data leakage by bypassing the operating system's file system drivers and acquiring exclusive raw write handles on target block identifiers.


+-----------------------------------------------------------------------+
| Kamutoke Engine |
+-----------------------------------------------------------------------+
| 1. Device Enumeration -> Query active bus & volume map |
| 2. User Target Locking -> Isolate specific block device (e.g., /dev) |
| 3. Pass 1: Entropy Overwrite -> High-density pseudo-random stream |
| 4. Pass 2: Cryptographic Zero -> Full-range 0x00 bit flush |
| 5. Pass 3: Partition Table Purge -> Destruction of MBR/GPT headers |
+-----------------------------------------------------------------------+


### Direct Block Interfacing and Memory Pipeline

Kamutoke operates at the kernel-level block boundary, bypassing page caches via direct I/O (`O_DIRECT`), injecting continuous cryptographic entropy, and obliterating structural partition tables (MBR/GPT).

---

## Cryptographic Wiping Standard

The sanitization follows a three-pass protocol (Entropy Overwrite, Static Null Bytes, and Dynamic Sector Inversion) to prevent forensic recovery.

---

## System Requirements and Compilation Directives

Engineered in C++20 for performance. Requires a POSIX/Windows NT environment, a modern C++20 compiler, and root privileges. Compile via:

```bash
g++ -std=c++20 -O3 -Wall -Wextra -pedantic src/main.cpp -o kamutoke
```

---

## Safety Warnings and Operational Risk

> **CRITICAL WARNING:** Kamutoke performs destructive, irreversible storage sanitization resulting in permanent data loss.

---

## Licensing and Maintenance

The Kamutoke project is maintained by **hypernova-developer** under the **GNU General Public License v3.0 (GPLv3)**.

> hypernova-developer