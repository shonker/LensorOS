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

#include <event.h>

#include <bit>
#include <integers.h>
#include <stddef.h>
#include <stdint.h>
#include <scheduler.h>
#include <vector>

EventManager gEvents;

void EventManager::notify(const Event& event) {
    if (event.Type >= EventType::COUNT) return;

    for (auto pid : Listeners[event.Type]) {
        Process* process = Scheduler::process(pid);
        if (!process) continue;

        // For each event queue in the process, check if it's filter has this
        // event type enabled.
        bool found = false;
        for (auto queue : process->EventQueues) {
            // If it doesn't, we move on.
            if (!queue.listens(event.Type, event.Filter)) continue;
            // If it does, we push this event to the event queue.
            queue.push(event);
            found = true;
        }
        // If none of the event queue's had this filter, then that means the
        // book-keeping went wrong and we should remove this process from this
        // Listeners[event.Type] vector.
        if (!found) unregister_listener(event.Type, pid);
    }
}
