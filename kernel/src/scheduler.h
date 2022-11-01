/* Copyright 2022, Contributors To LensorOS.
 * All rights reserved.
 *
 * This file is part of LensorOS.
 *
 * LensorOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LensorOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LensorOS. If not, see <https://www.gnu.org/licenses
 */

#ifndef LENSOR_OS_SCHEDULER_H
#define LENSOR_OS_SCHEDULER_H

#include <vector>
#include <integers.h>
#include <interrupts/interrupts.h>
#include <linked_list.h>
#include <memory/region.h>

namespace Memory {
    class PageTable;
}

/// Interrupt handler function found in `scheduler.asm`
extern "C" void irq0_handler();

/* TODO: Take into account different CPU architectures.
 *  This can be done by including ${ARCH}/ directory
 *  with the build system, and putting this there.
 */
struct CPUState {
    u64 RSP;
    u64 RBX;
    u64 RCX;
    u64 RDX;
    u64 RSI;
    u64 RDI;
    u64 RBP;
    u64 R8;
    u64 R9;
    u64 R10;
    u64 R11;
    u64 R12;
    u64 R13;
    u64 R14;
    u64 R15;
    u64 FS;
    u64 GS;
    u64 RAX;
    InterruptFrame Frame;
} __attribute__((packed));

typedef u64 pid_t;

/* TODO:
 * |-- Each process should store a list of memory regions that are
 * |   associated with that process. These memory regions are created
 * |   through memory-allocating system calls (similar to `mmap`).
 * |-- File Descriptor Table (Dynamic list of process' open file descriptors).
 * |   We should probably just store indices into `VFS::Opened` table.
 * `-- As only processes should make syscalls, should syscalls be defined in terms of process?
 */
struct Process {
    pid_t ProcessID = 0;

    // Keep track of memory that should be freed when the process exits.
    SinglyLinkedList<Memory::Region> Memories;
    usz next_region_vaddr = 0xf8000000;

    // Keep track of opened files that may be freed when the process
    // exits, if no other process has it open.
    std::vector<u64> FileDescriptorTable;
    std::vector<u64> FreeFileDescriptors;

    Memory::PageTable* CR3 { nullptr };

    // Used to save/restore CPU state when a context switch occurs.
    CPUState CPU;


    void add_memory_region(void* vaddr, void* paddr, usz size) {
        Memories.add({vaddr, paddr, size});
    }

    /// Find region in memories by vaddr and remove it.
    void remove_memory_region(void* vaddr) {
        usz index = 0;
        SinglyLinkedListNode<Memory::Region>* region_it = Memories.head();
        for (; region_it; region_it = region_it->next()) {
            if (region_it->value().vaddr == vaddr) {
                break;
            }
            ++index;
        }
        if (region_it) {
            Memories.remove(index);
        }
    }

    void destroy() {
        // TODO: Free memory regions.
        // TODO: Close open files.
        // TODO: Free page table?
    }
};

/// External symbols for 'scheduler.asm', defined in `scheduler.cpp`
extern void(*scheduler_switch_process)(CPUState*)
    __attribute__((no_caller_saved_registers));
extern void(*timer_tick)();

namespace Scheduler {
    /// External symbol defined in `scheduler.cpp`
    // The list node of the currently executing process.
    extern SinglyLinkedListNode<Process*>* CurrentProcess;

    bool initialize();

    /// Get a process ID number that is unique.
    pid_t request_pid();

    /* Switch to the next available task.
     * | Called by IRQ0 Handler (System Timer Interrupt).
     * |-- Copy registers saved from IRQ0 to current process.
     * |-- Update current process to next available process.
     * `-- Manipulate stack to trick `iretq` into doing what we want.
     */
    void switch_process(CPUState*);

    /// Add an existing process to the list of processes.
    void add_process(Process*);

    /// remove the process with PID list of processes.
    bool remove_process(pid_t);

    void print_debug();
}

__attribute__((no_caller_saved_registers))
void scheduler_switch(CPUState*);

#endif
