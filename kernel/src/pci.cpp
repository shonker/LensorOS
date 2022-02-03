#include "pci.h"
#include "ahci.h"

namespace PCI {
    void enumerate_function(u64 device_address, u64 function_number) {
        u64 offset = function_number << 12;
        u64 function_address = device_address + offset;
        gPTM.map_memory((void*)function_address, (void*)function_address);
        PCIDeviceHeader* pciDevHdr = (PCIDeviceHeader*)function_address;
        if (pciDevHdr->DeviceID == 0x0000)
            return;
        if (pciDevHdr->DeviceID == 0xFFFF)
            return;
        srl.writestr("  ");
        srl.writestr(get_vendor_name(pciDevHdr->VendorID));
        srl.writestr(" / ");
        srl.writestr(get_device_name(pciDevHdr->VendorID, pciDevHdr->DeviceID));
        srl.writestr(" / ");
        srl.writestr(DeviceClasses[pciDevHdr->Class]);
        srl.writestr(" / ");
        srl.writestr(get_subclass_name(pciDevHdr->Class, pciDevHdr->Subclass));
        srl.writestr(" / ");
        srl.writestr(get_prog_if_name(pciDevHdr->Class, pciDevHdr->Subclass, pciDevHdr->ProgIF));
        srl.writestr("\r\n");
        if (pciDevHdr->Class == 0x01) {
            // Mass Storage Controller
            if (pciDevHdr->Subclass == 0x06) {
                // Serial ATA
                if (pciDevHdr->ProgIF == 0x01) {
                    // AHCI 1.0 Device
                    srl.writestr("[PCI]: Found AHCI 1.0 Serial ATA Mass Storage Controller\r\n");
                    AHCI::Drivers[AHCI::NumDrivers] = new AHCI::AHCIDriver(pciDevHdr);
                    ++AHCI::NumDrivers;
                }
            }
        }
    }
    
    void enumerate_device(u64 bus_address, u64 device_number) {
        u64 offset = device_number << 15;
        u64 device_address = bus_address + offset;
        gPTM.map_memory((void*)device_address, (void*)device_address);
        PCIDeviceHeader* pciDevHdr = (PCIDeviceHeader*)device_address;
        if (pciDevHdr->DeviceID == 0x0000) 
            return;
        if (pciDevHdr->DeviceID == 0xFFFF) 
            return;
        for (u64 function = 0; function < 8; ++function)
            enumerate_function(device_address, function);
    }
    
    void enumerate_bus(u64 base_address, u64 bus_number) {
        u64 offset = bus_number << 20;
        u64 bus_address = base_address + offset;
        gPTM.map_memory((void*)bus_address, (void*)bus_address);
        PCIDeviceHeader* pciDevHdr = (PCIDeviceHeader*)bus_address;
        if (pciDevHdr->DeviceID == 0x0000) 
            return;
        if (pciDevHdr->DeviceID == 0xFFFF) 
            return;
        for (u64 device = 0; device < 32; ++device)
            enumerate_device(bus_address, device);
    }
    
    void enumerate_pci(ACPI::MCFGHeader* mcfg) {
        srl.writestr("[PCI]: \r\n");
        int entries = ((mcfg->Header.Length) - sizeof(ACPI::MCFGHeader)) / sizeof(ACPI::DeviceConfig);
        for (int t = 0; t < entries; ++t) {
            ACPI::DeviceConfig* devCon = (ACPI::DeviceConfig*)((u64)mcfg
                                                               + sizeof(ACPI::MCFGHeader)
                                                               + (sizeof(ACPI::DeviceConfig) * t));
            for (u64 bus = devCon->StartBus; bus < devCon->EndBus; ++bus)
                enumerate_bus(devCon->BaseAddress, bus);
        }
    }
}
