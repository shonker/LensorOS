#ifndef LENSOR_OS_PCI_H
#define LENSOR_OS_PCI_H

#include <stdint.h>
#include "acpi.h"
#include "paging/page_table_manager.h"

// FIXME TEMPORARY FOR DEBUG PURPOSES
#include "basic_renderer.h"
#include "cstr.h"

namespace PCI {
	struct PCIDeviceHeader {
		uint16_t VendorID;
		uint16_t DeviceID;
		uint16_t Command;
		uint16_t Status;
		uint8_t RevisionID;
		uint8_t ProgIF;
		uint8_t Subclass;
		uint8_t Class;
		uint8_t CacheLineSize;
		uint8_t LatencyTimer;
		uint8_t HeaderType;
		uint8_t BIST;
	};
	
	void enumerate_pci(ACPI::MCFGHeader* mcfg);
}

#endif
