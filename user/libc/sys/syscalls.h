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
 * along with LensorOS. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include "../stdint.h"
#include "../bits/decls.h"
#include "../bits/std/type_traits"

#define SYS_open  0
#define SYS_close 1
#define SYS_read  2
#define SYS_write 3
#define SYS_poke  4
#define SYS_exit  5

#define SYS_MAXSYSCALL 6

/// ===========================================================================
///  Syscall functions.
/// ===========================================================================
__BEGIN_DECLS__
#define __a uintptr_t

#define _DEFINE_SYSCALL(_N, _Args, ...)                                                       \
    __attribute__((__always_inline__, __artificial__)) inline uintptr_t __syscall##_N _Args { \
        uintptr_t __result;                                                                   \
        asm volatile("int $0x80"                                                              \
                     : "=a"(__result)                                                         \
                     : "a"(__n) __VA_OPT__(, ) __VA_ARGS__                                    \
                     : "memory"                                                               \
        );                                                                                    \
        return __result;                                                                      \
    }

#define _DEFINE_SYSCALL_ARGS(...) (__a __n __VA_OPT__(, ) __VA_ARGS__)

_DEFINE_SYSCALL(0, _DEFINE_SYSCALL_ARGS())
_DEFINE_SYSCALL(1, _DEFINE_SYSCALL_ARGS(__a __1), "D"(__1))
_DEFINE_SYSCALL(2, _DEFINE_SYSCALL_ARGS(__a __1, __a __2), "D"(__1), "S"(__2))
_DEFINE_SYSCALL(3, _DEFINE_SYSCALL_ARGS(__a __1, __a __2, __a __3), "D"(__1), "S"(__2), "d"(__3))
_DEFINE_SYSCALL(4, _DEFINE_SYSCALL_ARGS(__a __1, __a __2, __a __3, __a __4), "D"(__1), "S"(__2), "d"(__3), "c"(__4))

__attribute__((__always_inline__, __artificial__))
inline __a __syscall5(__a __n, __a __1, __a __2, __a __3, __a __4, __a __5) {
    __a __result;
    asm volatile("mov %0, %%r8\n"
                 "int $0x80\n"
                 : "=a"(__result)
                 : "a"(__n), "D"(__1), "S"(__2), "d"(__3), "c"(__4), "r"(__5)
                 : "memory", "r8");
    return __result;
}

__attribute__((__always_inline__, __artificial__))
inline __a __syscall6(__a __n, __a __1, __a __2, __a __3, __a __4, __a __5, __a __6) {
    __a __result;
    asm volatile("mov %0, %%r8\n"
                 "mov %1, %%r9\n"
                 "int $0x80\n"
                 : "=a"(__result)
                 : "a"(__n), "D"(__1), "S"(__2), "d"(__3), "c"(__4), "r"(__5), "r"(__6)
                 : "memory", "r8", "r9");
    return __result;
}

#undef __a

__END_DECLS__


/// ===========================================================================
///  C Interface.
/// ===========================================================================
#ifndef __cplusplus

/// Abandon all hope, ye who enter here.
typedef struct {int __unused;}* __empty_type_t;
#define _VA_FIRST(_First, ...) _First
#define _VA_REST(_X, ...) __VA_ARGS__
#define _EMPTY_VAL ((__empty_type_t)0)

#define __syscall_at_least5(...)                                \
_Generic((_VA_FIRST(__VA_ARGS__ __VA_OPT__(,) _EMPTY_VAL)),     \
     __empty_type_t: __syscall5,                                \
     default:        __syscall6)

#define __syscall_at_least4(...)                                \
_Generic((_VA_FIRST(__VA_ARGS__ __VA_OPT__(,) _EMPTY_VAL)),     \
     __empty_type_t: __syscall4,                                \
     default:        __syscall_at_least5(_VA_REST(__VA_ARGS__)))

#define __syscall_at_least3(...)                                \
_Generic((_VA_FIRST(__VA_ARGS__ __VA_OPT__(,) _EMPTY_VAL)),     \
     __empty_type_t: __syscall3,                                \
     default:        __syscall_at_least4(_VA_REST(__VA_ARGS__)))

#define __syscall_at_least2(...)                                \
_Generic((_VA_FIRST(__VA_ARGS__ __VA_OPT__(,) _EMPTY_VAL)),     \
     __empty_type_t: __syscall2,                                \
     default:        __syscall_at_least3(_VA_REST(__VA_ARGS__)))

#define __syscall_at_least1(...)                                \
_Generic((_VA_FIRST(__VA_ARGS__ __VA_OPT__(,) _EMPTY_VAL)),     \
     __empty_type_t: __syscall1,                                \
     default:        __syscall_at_least2(_VA_REST(__VA_ARGS__)))

#define syscall(_Sys, ...)                                      \
_Generic((_VA_FIRST(__VA_ARGS__ __VA_OPT__(,) _EMPTY_VAL)),     \
    __empty_type_t: __syscall0,                                 \
    default:        __syscall_at_least1(_VA_REST(__VA_ARGS__))) \
    (_Sys __VA_OPT__(, (uintptr_t)) __VA_ARGS__)


/// ===========================================================================
///  C++ Interface.
/// ===========================================================================
#else
namespace std::__detail {
/// Perform a system call.
template <
    typename... _Args,
    typename = _Requires<
        _And<bool_constant<sizeof...(_Args) <= 6>,
        _Or<_Number<_Args>,
            _Pointer<_Args>>...>>
>
[[__gnu__::__always_inline__, __gnu__::__artificial__]]
inline uintptr_t syscall(uintptr_t __sys, _Args&& ...__args) {
    __if   (sizeof...(_Args) == 0) return __syscall0(__sys);
    __elif (sizeof...(_Args) == 1) return __syscall1(__sys, (uintptr_t)__args...);
    __elif (sizeof...(_Args) == 2) return __syscall2(__sys, (uintptr_t)__args...);
    __elif (sizeof...(_Args) == 3) return __syscall3(__sys, (uintptr_t)__args...);
    __elif (sizeof...(_Args) == 4) return __syscall4(__sys, (uintptr_t)__args...);
    __elif (sizeof...(_Args) == 5) return __syscall5(__sys, (uintptr_t)__args...);
    __elif (sizeof...(_Args) == 6) return __syscall6(__sys, (uintptr_t)__args...);
    __else __builtin_unreachable();
}
}

using std::__detail::syscall;
#endif /* #if defined (__cplusplus) */

#endif /* #ifndef _SYSCALLS_H */
