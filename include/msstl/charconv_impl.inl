// Copyright (c) 2021 Borislav Stanimirov
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include <cassert>
#include <cstring>
#include <cstdint>
#include <climits>
#include <type_traits>
#include <algorithm>

namespace msstl {

namespace impl {

using ulong32 = std::conditional_t<sizeof(unsigned long) == 4, unsigned long, uint32_t>;

#if INTPTR_MAX == INT64_MAX
#   define MSCHARCONV_64_BIT
#endif

#if defined(_MSC_VER)
#   define MSCHARCONV_FORCE_INLINE __forceinline
#else
#   define MSCHARCONV_FORCE_INLINE __attribute__((always_inline)) inline
#endif

#define MSCHARCONV_ASSERT_MSG(cnd, msg) assert(cnd)

#define MSCHARCONV_VERIFY_RANGE(first, last) { assert(first <= last); }

template <class To, class From>
To bit_cast(const From& val) noexcept {
#if defined(_MSC_VER)
    return __builtin_bit_cast(To, val);
#else
    To to;
    std::memcpy(&to, &val, sizeof(To));
    return to;
#endif
}

// intrinsics
#if defined(_WIN32)
#   include <intrin.h>
#else
inline char _BitScanForward(uint32_t* bit, uint32_t n) {
    if (!n) return 0;
    *bit = uint32_t(__builtin_ctz(n));
    return 1;
}
inline char _BitScanForward64(uint32_t* bit, uint64_t n) {
    if (!n) return 0;
    *bit = uint32_t(__builtin_ctzl(n));
    return 1;
}
inline char _BitScanReverse(uint32_t* bit, uint32_t n) {
    if (!n) return 0;
    *bit = uint32_t(32 - 1 - __builtin_clz(n));
    return 1;
}
inline char _BitScanReverse64(uint32_t* bit, uint64_t n) {
    if (!n) return 0;
    *bit = uint32_t(64 - 1 - __builtin_clzl(n));
    return 1;
}
#endif

#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 10
#define MSCHARCONV_DISABLE_WARNINGS \
     _Pragma("GCC diagnostic push") \
     _Pragma("GCC diagnostic ignored \"-Wunused-but-set-variable\"") \
     _Pragma("GCC diagnostic ignored \"-Wunused-but-set-parameter\"") \
    /*preserve this line*/
#define MSCHARCONV_REENABLE_WARNINGS _Pragma("GCC diagnostic pop")
#else
#define MSCHARCONV_DISABLE_WARNINGS
#define MSCHARCONV_REENABLE_WARNINGS
#endif

MSCHARCONV_DISABLE_WARNINGS

#include "converted/m_floating_type_traits.inl"
#include "converted/xbit_ops.h.inl"
#include "converted/xcharconv_ryu_tables.h.inl"
#include "converted/xcharconv_ryu.h.inl"
#include "converted/charconv.inl"

MSCHARCONV_REENABLE_WARNINGS

} // namespace impl

#if !defined(MSCHARCONV_IMPLEMENT)
#   define MSCCHARCONV_I inline
#else
#   define MSCCHARCONV_I
#endif

MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, const char val, int base) noexcept { return impl::to_chars(first, last, val, base); }
MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, signed char val, int base) noexcept { return impl::to_chars(first, last, val, base); }
MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, unsigned char val, int base) noexcept { return impl::to_chars(first, last, val, base); }
MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, short val, int base) noexcept { return impl::to_chars(first, last, val, base); }
MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, unsigned short val, int base) noexcept { return impl::to_chars(first, last, val, base); }
MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, int val, int base) noexcept { return impl::to_chars(first, last, val, base); }
MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, unsigned int val, int base) noexcept { return impl::to_chars(first, last, val, base); }
MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, long val, int base) noexcept { return impl::to_chars(first, last, val, base); }
MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, unsigned long val, int base) noexcept { return impl::to_chars(first, last, val, base); }
MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, long long val, int base) noexcept { return impl::to_chars(first, last, val, base); }
MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, unsigned long long val, int base) noexcept { return impl::to_chars(first, last, val, base); }

MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, float val) noexcept { return impl::to_chars(first, last, val); }
MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, double val) noexcept { return impl::to_chars(first, last, val); }
MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, long double val) noexcept { return impl::to_chars(first, last, val); }
MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, float val, chars_format fmt) noexcept { return impl::to_chars(first, last, val, fmt); }
MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, double val, chars_format fmt) noexcept { return impl::to_chars(first, last, val, fmt); }
MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, long double val, chars_format fmt) noexcept { return impl::to_chars(first, last, val, fmt); }
MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, float val, chars_format fmt, int precision) noexcept { return impl::to_chars(first, last, val, fmt, precision); }
MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, double val, chars_format fmt, int precision) noexcept { return impl::to_chars(first, last, val, fmt, precision); }
MSCCHARCONV_I to_chars_result to_chars(char* first, char* last, long double val, chars_format fmt, int precision) noexcept { return impl::to_chars(first, last, val, fmt, precision); }


MSCCHARCONV_I from_chars_result from_chars(const char* first, const char* last, char& val, int base) noexcept { return impl::from_chars(first, last, val, base); }
MSCCHARCONV_I from_chars_result from_chars(const char* first, const char* last, signed char& val, int base) noexcept { return impl::from_chars(first, last, val, base); }
MSCCHARCONV_I from_chars_result from_chars(const char* first, const char* last, unsigned char& val, int base) noexcept { return impl::from_chars(first, last, val, base); }
MSCCHARCONV_I from_chars_result from_chars(const char* first, const char* last, short& val, int base) noexcept { return impl::from_chars(first, last, val, base); }
MSCCHARCONV_I from_chars_result from_chars(const char* first, const char* last, unsigned short& val, int base) noexcept { return impl::from_chars(first, last, val, base); }
MSCCHARCONV_I from_chars_result from_chars(const char* first, const char* last, int& val, int base) noexcept { return impl::from_chars(first, last, val, base); }
MSCCHARCONV_I from_chars_result from_chars(const char* first, const char* last, unsigned int& val, int base) noexcept { return impl::from_chars(first, last, val, base); }
MSCCHARCONV_I from_chars_result from_chars(const char* first, const char* last, long& val, int base) noexcept { return impl::from_chars(first, last, val, base); }
MSCCHARCONV_I from_chars_result from_chars(const char* first, const char* last, unsigned long& val, int base) noexcept { return impl::from_chars(first, last, val, base); }
MSCCHARCONV_I from_chars_result from_chars(const char* first, const char* last, long long& val, int base) noexcept { return impl::from_chars(first, last, val, base); }
MSCCHARCONV_I from_chars_result from_chars(const char* first, const char* last, unsigned long long& val, int base) noexcept { return impl::from_chars(first, last, val, base); }

MSCCHARCONV_I from_chars_result from_chars(const char* first, const char* last, float& val, chars_format fmt) noexcept { return impl::from_chars(first, last, val, fmt); }
MSCCHARCONV_I from_chars_result from_chars(const char* first, const char* last, double& val, chars_format fmt) noexcept { return impl::from_chars(first, last, val, fmt); }
MSCCHARCONV_I from_chars_result from_chars(const char* first, const char* last, long double& val, chars_format fmt) noexcept { return impl::from_chars(first, last, val, fmt); }


} // namespace msstl
