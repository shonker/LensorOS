#ifndef LENSOR_OS_FAT_FS_H
#define LENSOR_OS_FAT_FS_H

/// NUMBER -> STRING FUNCTIONS
#include "cstr.h"
/// FILE SYSTEM
#include "ahci.h"
#include "filesystem.h"
#include "fat_driver.h"
#include "fat_definitions.h"
/// MEMORY MANIPULATION: `memcpy`
#include "memory.h"
/// SERIAL COMMUNICATION FO RDEBUG PURPOSES
#include "uart.h"

/// Resource Used: https://wiki.osdev.org/FAT

/// The FAT File System
/// FAT = File Allocation Table

/* TODO:
 * `- Cache useful data (total size, amount free/used, File Allocation Table, etc)
 */

/// This class will be created for each FAT-formatted file-system found using the AHCI driver.
class FatFS final : public FileSystem {
public:
    FATDriver* Driver;
    BootRecord BR;
    FATType Type;
    
    FatFS(u16 index, AHCI::AHCIDriver* ahci, u8 portNumber) {
        Index = index;
        Format = FileSystemFormat::FAT;
        AHCI = ahci;
        PortNumber = portNumber;
        Driver = &gFATDriver;
        // Get boot record from device.
        if (ahci->Ports[portNumber]->read(0, 1, &BR, sizeof(BootRecord))) {
            // Set type based on boot record information.
            Type = Driver->get_type(&BR);
        }
        else {
            // Read from device failed.
            UART::out("[FatFS]: ERROR -> Could not read from device at port ");
            UART::out(to_string(portNumber));
            UART::out("\r\n");
        }
    }

    virtual ~FatFS() {}

    void read (Inode* inode) override {
        // TODO: Read from FAT file-system based on VFS intermediate-representation's data.
        (void)inode;

        // For now this just lists the files in the root directory.
        Driver->read_root_directory(AHCI->Ports[PortNumber], &BR, Type);
    }

    void write(Inode* inode) override {
        // TODO: Write to FAT file-system based on VFS intermediate-representation's data.
        (void)inode;
    }

    u64 get_total_size() override {
        return Driver->get_total_sectors(&BR) * BR.BPB.NumBytesPerSector;
    }
};

#endif
