# Kamutoke: Low-Level Storage Sanitization and Cryptographic Wiping Engine

The Kamutoke Project provides an unyielding, high-throughput storage media sanitization framework designed to perform irreversible physical block destruction on targeted block devices. Named after the legendary lightning staff of Sukuna, the engine acts as an absolute execution force against persistent data states. It completely rejects soft-deletion paradigms, file system unlinking tricks, and surface-level partitioning flags. Instead, Kamutoke interacts directly with underlying raw block interfaces to execute systematic, multi-pass pseudo-random overwrite sequences followed by active block zeroization before final logical release.

---

## Technical Overview and Operational Architecture

Modern operating systems typically respond to standard format commands by marking allocation maps or index nodes as available, leaving the actual physical magnetic or NAND flash bits entirely intact and recoverable via low-level forensic reconstruction. Kamutoke prevents all vectors of data leakage by bypassing the operating system's file system drivers and acquiring exclusive raw write handles on target block identifiers.

```
+-----------------------------------------------------------------------+
|                           Kamutoke Engine                             |
+-----------------------------------------------------------------------+
| 1. Device Enumeration   -> Query active bus & volume map              |
| 2. User Target Locking  -> Isolate specific block device (e.g., /dev) |
| 3. Pass 1: Entropy Overwrite -> High-density pseudo-random stream      |
| 4. Pass 2: Cryptographic Zero -> Full-range 0x00 bit flush            |
| 5. Pass 3: Partition Table Purge -> Destruction of MBR/GPT headers    |
+-----------------------------------------------------------------------+

```

### Direct Block Interfacing and Memory Pipeline

Kamutoke operates strictly at the kernel-level block boundary. Upon initialization, the core process enumerates all active logical and physical drives attached to the host interface (NVMe, SATA, SAS, or USB storage arrays) and prompts the administrator for precise target target selection via indexed hardware IDs.

* **Non-Buffered Direct I/O:** To prevent operating system page caches from staging write operations in RAM, Kamutoke streams writes via direct memory flags (`O_DIRECT` or target platform raw device equivalents). Every byte emitted from the generation pipeline hits the storage controller directly.
* **Continuous Cryptographic Entropy Injection:** Overwrite iterations utilize high-entropy PRNG streams to fill hardware sectors with dynamic noise, destroying magnetic polarities on legacy media and forcing state mutations across flash memory cells.
* **Structural Table Obliteration:** Following the primary stream over-write phases, the engine targets sector zero and backup offset regions, completely overwriting Master Boot Record (MBR), GUID Partition Table (GPT), and secondary metadata structures to invalidate all geometry parameters.

---

## Cryptographic Wiping Standard

The sanitization sequence enforced by Kamutoke conforms to high-assurance data destruction protocols, structured in distinct structural phases:

| Pass Phase | Pattern Type | Target Sector Range | Purpose |
| --- | --- | --- | --- |
| **Phase I** | High-Entropy Pseudo-Random | $0 \to Sector_{Max}$ | Bit-level remanence disruption and noise injection |
| **Phase II** | Static Null Bytes (`0x00`) | $0 \to Sector_{Max}$ | Complete state clearing and baseline verification |
| **Phase III** | Dynamic Sector Inversion | Partition Boundaries | Header destruction, table corruption, and partition purge |

By executing this continuous overwrite pipeline, Kamutoke renders low-level remanence analysis impossible, eliminating recovery vectors from specialized hardware analyzers or forensic extraction utilities.

---

## System Requirements and Compilation Directives

Kamutoke is engineered in standard C++20 for raw execution speed, absolute hardware access, and zero reliance on heavy external framework dependencies.

### Toolchain Dependencies

* POSIX-compliant platform or Windows NT Native API access
* Modern C++20 compiler (`g++` $\ge 11.0$, `clang++` $\ge 13.0$, or MSVC $\ge 19.29$)
* Administrator / Root privileges (required for raw block device acquisition)

### Building the Native Binary

To build the executable manually with maximum performance optimizations, execute the native compilation string within your toolchain environment:

```bash
g++ -std=c++20 -O3 -Wall -Wextra -pedantic src/main.cpp -o kamutoke

```

---

## Safety Warnings and Operational Risk

> **CRITICAL WARNING:** Kamutoke performs destructive, irreversible storage sanitization. Executing this utility against a target drive identifier will result in total, permanent loss of all file systems, partition tables, and raw data records on that device. Data destroyed by Kamutoke cannot be recovered by any soft or hardware-based forensic method. System administrators must double-check target disk indices before confirming execution.

---

## Licensing and Maintenance

The Kamutoke project is an open-source security tool maintained by **Eymen**. The core source code and associated sanitization algorithms are distributed under the terms of the **GNU General Public License v3.0 (GPLv3)**. See the `LICENSE` file in the root repository for complete terms and rights regarding redistribution and modification.

> hypernova-developer
