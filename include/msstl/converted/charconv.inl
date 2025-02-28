// charconv standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

//~ #pragma once
#ifndef _CHARCONV_
#define _CHARCONV_
//~ #include <yvals.h>
//~ #if _STL_COMPILER_PREPROCESSOR

#if 0 //~#if !_HAS_CXX17
//~ #pragma message("The contents of <charconv> are available only with C++17 or later.")
#else // ^^^ !_HAS_CXX17 / _HAS_CXX17 vvv
//~ #include <cstring>
//~ #include <intrin0.h>
//~ #include <xbit_ops.h>
//~ #include <xcharconv.h>
//~ #include <xcharconv_ryu.h>
//~ #include <xutility>

//~ #pragma pack(push, _CRT_PACKING)
//~ #pragma warning(push, _STL_WARNING_LEVEL)
//~ #pragma warning(disable : _STL_DISABLED_WARNINGS)
//~ _STL_DISABLE_CLANG_WARNINGS
//~ #pragma push_macro("new")
//~ #undef new

// This implementation is dedicated to the memory of Mary and Thavatchai.

//~ _STD_BEGIN
inline constexpr char _Charconv_digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e',
    'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
static_assert(std:: size(_Charconv_digits) == 36);

// FUNCTION to_chars (INTEGER TO STRING)
template <class _RawTy>
[[nodiscard]] to_chars_result _Integer_to_chars(
    char* _First, char* const _Last, const _RawTy _Raw_value, const int _Base) noexcept {
    MSCHARCONV_VERIFY_RANGE(_First, _Last);
    MSCHARCONV_ASSERT_MSG(_Base >= 2 && _Base <= 36, "invalid base in to_chars()");

    using _Unsigned = std::make_unsigned_t<_RawTy>;

    _Unsigned _Value = static_cast<_Unsigned>(_Raw_value);

    if constexpr (std::is_signed_v<_RawTy>) {
        if (_Raw_value < 0) {
            if (_First == _Last) {
                return {_Last, std::errc::value_too_large};
            }

            *_First++ = '-';

            _Value = static_cast<_Unsigned>(0 - _Value);
        }
    }

    constexpr size_t _Buff_size = sizeof(_Unsigned) * CHAR_BIT; // enough for base 2
    char _Buff[_Buff_size];
    char* const _Buff_end = _Buff + _Buff_size;
    char* _RNext          = _Buff_end;

    switch (_Base) {
    case 10:
        { // Derived from _UIntegral_to_buff()
            // Performance note: Ryu's digit table should be faster here.
            constexpr bool _Use_chunks = sizeof(_Unsigned) > sizeof(size_t);

            if constexpr (_Use_chunks) { // For 64-bit numbers on 32-bit platforms, work in chunks to avoid 64-bit
                                         // divisions.
                while (_Value > 0xFFFF'FFFFU) {
                    // Performance note: Ryu's division workaround would be faster here.
                    ulong32 _Chunk = static_cast<ulong32>(_Value % 1'000'000'000);
                    _Value               = static_cast<_Unsigned>(_Value / 1'000'000'000);

                    for (int _Idx = 0; _Idx != 9; ++_Idx) {
                        *--_RNext = static_cast<char>('0' + _Chunk % 10);
                        _Chunk /= 10;
                    }
                }
            }

            using _Truncated = std::conditional_t<_Use_chunks, ulong32, _Unsigned>;

            _Truncated _Trunc = static_cast<_Truncated>(_Value);

            do {
                *--_RNext = static_cast<char>('0' + _Trunc % 10);
                _Trunc /= 10;
            } while (_Trunc != 0);
            break;
        }

    case 2:
        do {
            *--_RNext = static_cast<char>('0' + (_Value & 0b1));
            _Value >>= 1;
        } while (_Value != 0);
        break;

    case 4:
        do {
            *--_RNext = static_cast<char>('0' + (_Value & 0b11));
            _Value >>= 2;
        } while (_Value != 0);
        break;

    case 8:
        do {
            *--_RNext = static_cast<char>('0' + (_Value & 0b111));
            _Value >>= 3;
        } while (_Value != 0);
        break;

    case 16:
        do {
            *--_RNext = _Charconv_digits[_Value & 0b1111];
            _Value >>= 4;
        } while (_Value != 0);
        break;

    case 32:
        do {
            *--_RNext = _Charconv_digits[_Value & 0b11111];
            _Value >>= 5;
        } while (_Value != 0);
        break;

    case 3:
    case 5:
    case 6:
    case 7:
    case 9:
        do {
            *--_RNext = static_cast<char>('0' + _Value % _Base);
            _Value    = static_cast<_Unsigned>(_Value / _Base);
        } while (_Value != 0);
        break;

    default:
        do {
            *--_RNext = _Charconv_digits[_Value % _Base];
            _Value    = static_cast<_Unsigned>(_Value / _Base);
        } while (_Value != 0);
        break;
    }

    const ptrdiff_t _Digits_written = _Buff_end - _RNext;

    if (_Last - _First < _Digits_written) {
        return {_Last, std::errc::value_too_large};
    }

    std:: memcpy(_First, _RNext, static_cast<size_t>(_Digits_written));

    return {_First + _Digits_written, std::errc{}};
}

inline to_chars_result to_chars(char* const _First, char* const _Last, const char _Value, const int _Base = 10) noexcept
/* strengthened */ {
    return _Integer_to_chars(_First, _Last, _Value, _Base);
}
inline to_chars_result to_chars(
    char* const _First, char* const _Last, const signed char _Value, const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_to_chars(_First, _Last, _Value, _Base);
}
inline to_chars_result to_chars(char* const _First, char* const _Last, const unsigned char _Value,
    const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_to_chars(_First, _Last, _Value, _Base);
}
inline to_chars_result to_chars(
    char* const _First, char* const _Last, const short _Value, const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_to_chars(_First, _Last, _Value, _Base);
}
inline to_chars_result to_chars(char* const _First, char* const _Last, const unsigned short _Value,
    const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_to_chars(_First, _Last, _Value, _Base);
}
inline to_chars_result to_chars(char* const _First, char* const _Last, const int _Value, const int _Base = 10) noexcept
/* strengthened */ {
    return _Integer_to_chars(_First, _Last, _Value, _Base);
}
inline to_chars_result to_chars(char* const _First, char* const _Last, const unsigned int _Value,
    const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_to_chars(_First, _Last, _Value, _Base);
}
inline to_chars_result to_chars(char* const _First, char* const _Last, const long _Value, const int _Base = 10) noexcept
/* strengthened */ {
    return _Integer_to_chars(_First, _Last, _Value, _Base);
}
inline to_chars_result to_chars(char* const _First, char* const _Last, const unsigned long _Value,
    const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_to_chars(_First, _Last, _Value, _Base);
}
inline to_chars_result to_chars(
    char* const _First, char* const _Last, const long long _Value, const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_to_chars(_First, _Last, _Value, _Base);
}
inline to_chars_result to_chars(char* const _First, char* const _Last, const unsigned long long _Value,
    const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_to_chars(_First, _Last, _Value, _Base);
}

to_chars_result to_chars(char* _First, char* _Last, bool _Value, int _Base = 10) = delete;


// STRUCT from_chars_result
//~ struct from_chars_result {
//~     const char* ptr;
//~     errc ec;
#if 0 //~#if _HAS_CXX20
//~     _NODISCARD friend bool operator==(const from_chars_result&, const from_chars_result&) = default;
#endif // _HAS_CXX20
//~ };

// FUNCTION from_chars (STRING TO INTEGER)
[[nodiscard]] inline unsigned char _Digit_from_char(const char _Ch) noexcept {
    // convert ['0', '9'] ['A', 'Z'] ['a', 'z'] to [0, 35], everything else to 255
    static constexpr unsigned char _Digit_from_byte[] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 255, 255,
        255, 255, 255, 255, 255, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
        32, 33, 34, 35, 255, 255, 255, 255, 255, 255, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
        26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255};
    static_assert(std:: size(_Digit_from_byte) == 256);

    return _Digit_from_byte[static_cast<unsigned char>(_Ch)];
}

template <class _RawTy>
[[nodiscard]] from_chars_result _Integer_from_chars(
    const char* const _First, const char* const _Last, _RawTy& _Raw_value, const int _Base) noexcept {
    MSCHARCONV_VERIFY_RANGE(_First, _Last);
    MSCHARCONV_ASSERT_MSG(_Base >= 2 && _Base <= 36, "invalid base in from_chars()");

    bool _Minus_sign = false;

    const char* _Next = _First;

    if constexpr (std::is_signed_v<_RawTy>) {
        if (_Next != _Last && *_Next == '-') {
            _Minus_sign = true;
            ++_Next;
        }
    }

    using _Unsigned = std::make_unsigned_t<_RawTy>;

    constexpr _Unsigned _Uint_max    = static_cast<_Unsigned>(-1);
    constexpr _Unsigned _Int_max     = static_cast<_Unsigned>(_Uint_max >> 1);
    constexpr _Unsigned _Abs_int_min = static_cast<_Unsigned>(_Int_max + 1);

    _Unsigned _Risky_val;
    _Unsigned _Max_digit;

    if constexpr (std::is_signed_v<_RawTy>) {
        if (_Minus_sign) {
            _Risky_val = static_cast<_Unsigned>(_Abs_int_min / _Base);
            _Max_digit = static_cast<_Unsigned>(_Abs_int_min % _Base);
        } else {
            _Risky_val = static_cast<_Unsigned>(_Int_max / _Base);
            _Max_digit = static_cast<_Unsigned>(_Int_max % _Base);
        }
    } else {
        _Risky_val = static_cast<_Unsigned>(_Uint_max / _Base);
        _Max_digit = static_cast<_Unsigned>(_Uint_max % _Base);
    }

    _Unsigned _Value = 0;

    bool _Overflowed = false;

    for (; _Next != _Last; ++_Next) {
        const unsigned char _Digit = _Digit_from_char(*_Next);

        if (_Digit >= _Base) {
            break;
        }

        if (_Value < _Risky_val // never overflows
            || (_Value == _Risky_val && _Digit <= _Max_digit)) { // overflows for certain digits
            _Value = static_cast<_Unsigned>(_Value * _Base + _Digit);
        } else { // _Value > _Risky_val always overflows
            _Overflowed = true; // keep going, _Next still needs to be updated, _Value is now irrelevant
        }
    }

    if (_Next - _First == static_cast<ptrdiff_t>(_Minus_sign)) {
        return {_First, std::errc::invalid_argument};
    }

    if (_Overflowed) {
        return {_Next, std::errc::result_out_of_range};
    }

    if constexpr (std::is_signed_v<_RawTy>) {
        if (_Minus_sign) {
            _Value = static_cast<_Unsigned>(0 - _Value);
        }
    }

    _Raw_value = static_cast<_RawTy>(_Value); // implementation-defined for negative, N4713 7.8 [conv.integral]/3

    return {_Next, std::errc{}};
}

inline from_chars_result from_chars(
    const char* const _First, const char* const _Last, char& _Value, const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_from_chars(_First, _Last, _Value, _Base);
}
inline from_chars_result from_chars(const char* const _First, const char* const _Last, signed char& _Value,
    const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_from_chars(_First, _Last, _Value, _Base);
}
inline from_chars_result from_chars(const char* const _First, const char* const _Last, unsigned char& _Value,
    const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_from_chars(_First, _Last, _Value, _Base);
}
inline from_chars_result from_chars(const char* const _First, const char* const _Last, short& _Value,
    const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_from_chars(_First, _Last, _Value, _Base);
}
inline from_chars_result from_chars(const char* const _First, const char* const _Last, unsigned short& _Value,
    const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_from_chars(_First, _Last, _Value, _Base);
}
inline from_chars_result from_chars(
    const char* const _First, const char* const _Last, int& _Value, const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_from_chars(_First, _Last, _Value, _Base);
}
inline from_chars_result from_chars(const char* const _First, const char* const _Last, unsigned int& _Value,
    const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_from_chars(_First, _Last, _Value, _Base);
}
inline from_chars_result from_chars(
    const char* const _First, const char* const _Last, long& _Value, const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_from_chars(_First, _Last, _Value, _Base);
}
inline from_chars_result from_chars(const char* const _First, const char* const _Last, unsigned long& _Value,
    const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_from_chars(_First, _Last, _Value, _Base);
}
inline from_chars_result from_chars(const char* const _First, const char* const _Last, long long& _Value,
    const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_from_chars(_First, _Last, _Value, _Base);
}
inline from_chars_result from_chars(const char* const _First, const char* const _Last, unsigned long long& _Value,
    const int _Base = 10) noexcept /* strengthened */ {
    return _Integer_from_chars(_First, _Last, _Value, _Base);
}

// vvvvvvvvvv DERIVED FROM corecrt_internal_big_integer.h vvvvvvvvvv

// A lightweight, sufficiently functional high-precision integer type for use in the binary floating-point <=> decimal
// string conversions. We define only the operations (and in some cases, parts of operations) that are actually used.

// We require sufficient precision to represent the reciprocal of the smallest representable value (the smallest
// denormal, 2^-1074). During parsing, we may also consider up to 768 decimal digits. For this, we require an
// additional log2(10^768) bits of precision. Finally, we require 54 bits of space for pre-division numerator shifting,
// because double explicitly stores 52 bits, implicitly stores 1 bit, and we need 1 more bit for rounding.

// PERFORMANCE NOTE: We intentionally do not initialize the _Mydata array when a _Big_integer_flt object is constructed.
// Profiling showed that zero-initialization caused a substantial performance hit. Initialization of the _Mydata
// array is not necessary: all operations on the _Big_integer_flt type are carefully written to only access elements at
// indices [0, _Myused), and all operations correctly update _Myused as the utilized size increases.

// _Big_integer_flt _Xval{}; is direct-list-initialization (N4750 11.6.4 [dcl.init.list]/1).
// N4750 11.6.4 [dcl.init.list]/3.5:
// "Otherwise, if the initializer list has no elements and T is a class type with a default constructor,
// the object is value-initialized."
// N4750 11.6 [dcl.init]/8, /8.1:
// "To value-initialize an object of type T means:
// - if T is a (possibly cv-qualified) class type (Clause 12) with either no default constructor (15.1)
// or a default constructor that is user-provided or deleted, then the object is default-initialized;"
// N4750 11.6 [dcl.init]/7, /7.1:
// "To default-initialize an object of type T means:
// - If T is a (possibly cv-qualified) class type (Clause 12), constructors are considered. The applicable constructors
// are enumerated (16.3.1.3), and the best one for the initializer () is chosen through overload resolution (16.3).
// The constructor thus selected is called, with an empty argument list, to initialize the object."
// N4750 15.6.2 [class.base.init]/9, /9.3:
// "In a non-delegating constructor, if a given potentially constructed subobject is not designated by a
// mem-initializer-id (including the case where there is no mem-initializer-list because the constructor has no
// ctor-initializer), then [...] - otherwise, the entity is default-initialized (11.6)."
// N4750 11.6 [dcl.init]/7, /7.2, /7.3:
// "To default-initialize an object of type T means: [...]
// - If T is an array type, each element is default-initialized.
// - Otherwise, no initialization is performed."
// Therefore, _Mydata's elements are not initialized.
struct _Big_integer_flt {
    _Big_integer_flt() noexcept : _Myused(0) {}

    _Big_integer_flt(const _Big_integer_flt& _Other) noexcept : _Myused(_Other._Myused) {
        std:: memcpy(_Mydata, _Other._Mydata, _Other._Myused * sizeof(uint32_t));
    }

    _Big_integer_flt& operator=(const _Big_integer_flt& _Other) noexcept {
        _Myused = _Other._Myused;
        std:: memmove(_Mydata, _Other._Mydata, _Other._Myused * sizeof(uint32_t));
        return *this;
    }

    [[nodiscard]] bool operator<(const _Big_integer_flt& _Rhs) const noexcept {
        if (_Myused != _Rhs._Myused) {
            return _Myused < _Rhs._Myused;
        }

        for (uint32_t _Ix = _Myused - 1; _Ix != static_cast<uint32_t>(-1); --_Ix) {
            if (_Mydata[_Ix] != _Rhs._Mydata[_Ix]) {
                return _Mydata[_Ix] < _Rhs._Mydata[_Ix];
            }
        }

        return false;
    }

    static constexpr uint32_t _Maximum_bits = 1074 // 1074 bits required to represent 2^1074
                                            + 2552 // ceil(log2(10^768))
                                            + 54; // shift space

    static constexpr uint32_t _Element_bits = 32;

    static constexpr uint32_t _Element_count = (_Maximum_bits + _Element_bits - 1) / _Element_bits;

    uint32_t _Myused; // The number of elements currently in use
    uint32_t _Mydata[_Element_count]; // The number, stored in little-endian form
};

[[nodiscard]] inline _Big_integer_flt _Make_big_integer_flt_one() noexcept {
    _Big_integer_flt _Xval{};
    _Xval._Mydata[0] = 1;
    _Xval._Myused    = 1;
    return _Xval;
}

[[nodiscard]] inline _Big_integer_flt _Make_big_integer_flt_u32(const uint32_t _Value) noexcept {
    _Big_integer_flt _Xval{};
    _Xval._Mydata[0] = _Value;
    _Xval._Myused    = 1;
    return _Xval;
}

[[nodiscard]] inline _Big_integer_flt _Make_big_integer_flt_u64(const uint64_t _Value) noexcept {
    _Big_integer_flt _Xval{};
    _Xval._Mydata[0] = static_cast<uint32_t>(_Value);
    _Xval._Mydata[1] = static_cast<uint32_t>(_Value >> 32);
    _Xval._Myused    = _Xval._Mydata[1] == 0 ? 1u : 2u;
    return _Xval;
}

[[nodiscard]] inline _Big_integer_flt _Make_big_integer_flt_power_of_two(const uint32_t _Power) noexcept {
    const uint32_t _Element_index = _Power / _Big_integer_flt::_Element_bits;
    const uint32_t _Bit_index     = _Power % _Big_integer_flt::_Element_bits;

    _Big_integer_flt _Xval{};
    std:: memset(_Xval._Mydata, 0, _Element_index * sizeof(uint32_t));
    _Xval._Mydata[_Element_index] = 1u << _Bit_index;
    _Xval._Myused                 = _Element_index + 1;
    return _Xval;
}

[[nodiscard]] inline uint32_t _Bit_scan_reverse(const _Big_integer_flt& _Xval) noexcept {
    if (_Xval._Myused == 0) {
        return 0;
    }

    const uint32_t _Bx = _Xval._Myused - 1;

    assert(_Xval._Mydata[_Bx] != 0); // _Big_integer_flt should always be trimmed

    ulong32 _Index; // Intentionally uninitialized for better codegen

    _BitScanReverse(&_Index, _Xval._Mydata[_Bx]); // assumes _Xval._Mydata[_Bx] != 0

    return _Index + 1 + _Bx * _Big_integer_flt::_Element_bits;
}

// Shifts the high-precision integer _Xval by _Nx bits to the left. Returns true if the left shift was successful;
// false if it overflowed. When overflow occurs, the high-precision integer is reset to zero.
[[nodiscard]] inline bool _Shift_left(_Big_integer_flt& _Xval, const uint32_t _Nx) noexcept {
    if (_Xval._Myused == 0) {
        return true;
    }

    const uint32_t _Unit_shift = _Nx / _Big_integer_flt::_Element_bits;
    const uint32_t _Bit_shift  = _Nx % _Big_integer_flt::_Element_bits;

    if (_Xval._Myused + _Unit_shift > _Big_integer_flt::_Element_count) {
        // Unit shift will overflow.
        _Xval._Myused = 0;
        return false;
    }

    if (_Bit_shift == 0) {
        std:: memmove(_Xval._Mydata + _Unit_shift, _Xval._Mydata, _Xval._Myused * sizeof(uint32_t));
        _Xval._Myused += _Unit_shift;
    } else {
        const bool _Bit_shifts_into_next_unit =
            _Bit_shift > (_Big_integer_flt::_Element_bits - _Bit_scan_reverse(_Xval._Mydata[_Xval._Myused - 1]));

        const uint32_t _New_used = _Xval._Myused + _Unit_shift + static_cast<uint32_t>(_Bit_shifts_into_next_unit);

        if (_New_used > _Big_integer_flt::_Element_count) {
            // Bit shift will overflow.
            _Xval._Myused = 0;
            return false;
        }

        const uint32_t _Msb_bits = _Bit_shift;
        const uint32_t _Lsb_bits = _Big_integer_flt::_Element_bits - _Msb_bits;

        const uint32_t _Lsb_mask = (1UL << _Lsb_bits) - 1UL;
        const uint32_t _Msb_mask = ~_Lsb_mask;

        // If _Unit_shift == 0, this will wraparound, which is okay.
        for (uint32_t _Dest_index = _New_used - 1; _Dest_index != _Unit_shift - 1; --_Dest_index) {
            // performance note: PSLLDQ and PALIGNR instructions could be more efficient here

            // If _Bit_shifts_into_next_unit, the first iteration will trigger the bounds check below, which is okay.
            const uint32_t _Upper_source_index = _Dest_index - _Unit_shift;

            // When _Dest_index == _Unit_shift, this will wraparound, which is okay (see bounds check below).
            const uint32_t _Lower_source_index = _Dest_index - _Unit_shift - 1;

            const uint32_t _Upper_source = _Upper_source_index < _Xval._Myused ? _Xval._Mydata[_Upper_source_index] : 0;
            const uint32_t _Lower_source = _Lower_source_index < _Xval._Myused ? _Xval._Mydata[_Lower_source_index] : 0;

            const uint32_t _Shifted_upper_source = (_Upper_source & _Lsb_mask) << _Msb_bits;
            const uint32_t _Shifted_lower_source = (_Lower_source & _Msb_mask) >> _Lsb_bits;

            const uint32_t _Combined_shifted_source = _Shifted_upper_source | _Shifted_lower_source;

            _Xval._Mydata[_Dest_index] = _Combined_shifted_source;
        }

        _Xval._Myused = _New_used;
    }

    std:: memset(_Xval._Mydata, 0, _Unit_shift * sizeof(uint32_t));

    return true;
}

// Adds a 32-bit _Value to the high-precision integer _Xval. Returns true if the addition was successful;
// false if it overflowed. When overflow occurs, the high-precision integer is reset to zero.
[[nodiscard]] inline bool _Add(_Big_integer_flt& _Xval, const uint32_t _Value) noexcept {
    if (_Value == 0) {
        return true;
    }

    uint32_t _Carry = _Value;
    for (uint32_t _Ix = 0; _Ix != _Xval._Myused; ++_Ix) {
        const uint64_t _Result = static_cast<uint64_t>(_Xval._Mydata[_Ix]) + _Carry;
        _Xval._Mydata[_Ix]     = static_cast<uint32_t>(_Result);
        _Carry                 = static_cast<uint32_t>(_Result >> 32);
    }

    if (_Carry != 0) {
        if (_Xval._Myused < _Big_integer_flt::_Element_count) {
            _Xval._Mydata[_Xval._Myused] = _Carry;
            ++_Xval._Myused;
        } else {
            _Xval._Myused = 0;
            return false;
        }
    }

    return true;
}

[[nodiscard]] inline uint32_t _Add_carry(uint32_t& _Ux1, const uint32_t _Ux2, const uint32_t _U_carry) noexcept {
    const uint64_t _Uu = static_cast<uint64_t>(_Ux1) + _Ux2 + _U_carry;
    _Ux1               = static_cast<uint32_t>(_Uu);
    return static_cast<uint32_t>(_Uu >> 32);
}

[[nodiscard]] inline uint32_t _Add_multiply_carry(
    uint32_t& _U_add, const uint32_t _U_mul_1, const uint32_t _U_mul_2, const uint32_t _U_carry) noexcept {
    const uint64_t _Uu_res = static_cast<uint64_t>(_U_mul_1) * _U_mul_2 + _U_add + _U_carry;
    _U_add                 = static_cast<uint32_t>(_Uu_res);
    return static_cast<uint32_t>(_Uu_res >> 32);
}

[[nodiscard]] inline uint32_t _Multiply_core(
    uint32_t* const _Multiplicand, const uint32_t _Multiplicand_count, const uint32_t _Multiplier) noexcept {
    uint32_t _Carry = 0;
    for (uint32_t _Ix = 0; _Ix != _Multiplicand_count; ++_Ix) {
        const uint64_t _Result = static_cast<uint64_t>(_Multiplicand[_Ix]) * _Multiplier + _Carry;
        _Multiplicand[_Ix]     = static_cast<uint32_t>(_Result);
        _Carry                 = static_cast<uint32_t>(_Result >> 32);
    }

    return _Carry;
}

// Multiplies the high-precision _Multiplicand by a 32-bit _Multiplier. Returns true if the multiplication
// was successful; false if it overflowed. When overflow occurs, the _Multiplicand is reset to zero.
[[nodiscard]] inline bool _Multiply(_Big_integer_flt& _Multiplicand, const uint32_t _Multiplier) noexcept {
    if (_Multiplier == 0) {
        _Multiplicand._Myused = 0;
        return true;
    }

    if (_Multiplier == 1) {
        return true;
    }

    if (_Multiplicand._Myused == 0) {
        return true;
    }

    const uint32_t _Carry = _Multiply_core(_Multiplicand._Mydata, _Multiplicand._Myused, _Multiplier);
    if (_Carry != 0) {
        if (_Multiplicand._Myused < _Big_integer_flt::_Element_count) {
            _Multiplicand._Mydata[_Multiplicand._Myused] = _Carry;
            ++_Multiplicand._Myused;
        } else {
            _Multiplicand._Myused = 0;
            return false;
        }
    }

    return true;
}

// This high-precision integer multiplication implementation was translated from the implementation of
// System.Numerics.BigIntegerBuilder.Mul in the .NET Framework sources. It multiplies the _Multiplicand
// by the _Multiplier and returns true if the multiplication was successful; false if it overflowed.
// When overflow occurs, the _Multiplicand is reset to zero.
[[nodiscard]] inline bool _Multiply(_Big_integer_flt& _Multiplicand, const _Big_integer_flt& _Multiplier) noexcept {
    if (_Multiplicand._Myused == 0) {
        return true;
    }

    if (_Multiplier._Myused == 0) {
        _Multiplicand._Myused = 0;
        return true;
    }

    if (_Multiplier._Myused == 1) {
        return _Multiply(_Multiplicand, _Multiplier._Mydata[0]); // when overflow occurs, resets to zero
    }

    if (_Multiplicand._Myused == 1) {
        const uint32_t _Small_multiplier = _Multiplicand._Mydata[0];
        _Multiplicand                    = _Multiplier;
        return _Multiply(_Multiplicand, _Small_multiplier); // when overflow occurs, resets to zero
    }

    // We prefer more iterations on the inner loop and fewer on the outer:
    const bool _Multiplier_is_shorter = _Multiplier._Myused < _Multiplicand._Myused;
    const uint32_t* const _Rgu1       = _Multiplier_is_shorter ? _Multiplier._Mydata : _Multiplicand._Mydata;
    const uint32_t* const _Rgu2       = _Multiplier_is_shorter ? _Multiplicand._Mydata : _Multiplier._Mydata;

    const uint32_t _Cu1 = _Multiplier_is_shorter ? _Multiplier._Myused : _Multiplicand._Myused;
    const uint32_t _Cu2 = _Multiplier_is_shorter ? _Multiplicand._Myused : _Multiplier._Myused;

    _Big_integer_flt _Result{};
    for (uint32_t _Iu1 = 0; _Iu1 != _Cu1; ++_Iu1) {
        const uint32_t _U_cur = _Rgu1[_Iu1];
        if (_U_cur == 0) {
            if (_Iu1 == _Result._Myused) {
                _Result._Mydata[_Iu1] = 0;
                _Result._Myused       = _Iu1 + 1;
            }

            continue;
        }

        uint32_t _U_carry = 0;
        uint32_t _Iu_res  = _Iu1;
        for (uint32_t _Iu2 = 0; _Iu2 != _Cu2 && _Iu_res != _Big_integer_flt::_Element_count; ++_Iu2, ++_Iu_res) {
            if (_Iu_res == _Result._Myused) {
                _Result._Mydata[_Iu_res] = 0;
                _Result._Myused          = _Iu_res + 1;
            }

            _U_carry = _Add_multiply_carry(_Result._Mydata[_Iu_res], _U_cur, _Rgu2[_Iu2], _U_carry);
        }

        while (_U_carry != 0 && _Iu_res != _Big_integer_flt::_Element_count) {
            if (_Iu_res == _Result._Myused) {
                _Result._Mydata[_Iu_res] = 0;
                _Result._Myused          = _Iu_res + 1;
            }

            _U_carry = _Add_carry(_Result._Mydata[_Iu_res++], 0, _U_carry);
        }

        if (_Iu_res == _Big_integer_flt::_Element_count) {
            _Multiplicand._Myused = 0;
            return false;
        }
    }

    // Store the _Result in the _Multiplicand and compute the actual number of elements used:
    _Multiplicand = _Result;
    return true;
}

// Multiplies the high-precision integer _Xval by 10^_Power. Returns true if the multiplication was successful;
// false if it overflowed. When overflow occurs, the high-precision integer is reset to zero.
[[nodiscard]] inline bool _Multiply_by_power_of_ten(_Big_integer_flt& _Xval, const uint32_t _Power) noexcept {
    // To improve performance, we use a table of precomputed powers of ten, from 10^10 through 10^380, in increments
    // of ten. In its unpacked form, as an array of _Big_integer_flt objects, this table consists mostly of zero
    // elements. Thus, we store the table in a packed form, trimming leading and trailing zero elements. We provide an
    // index that is used to unpack powers from the table, using the function that appears after this function in this
    // file.

    // The minimum value representable with double-precision is 5E-324.
    // With this table we can thus compute most multiplications with a single multiply.

    static constexpr uint32_t _Large_power_data[] = {0x540be400, 0x00000002, 0x63100000, 0x6bc75e2d, 0x00000005,
        0x40000000, 0x4674edea, 0x9f2c9cd0, 0x0000000c, 0xb9f56100, 0x5ca4bfab, 0x6329f1c3, 0x0000001d, 0xb5640000,
        0xc40534fd, 0x926687d2, 0x6c3b15f9, 0x00000044, 0x10000000, 0x946590d9, 0xd762422c, 0x9a224501, 0x4f272617,
        0x0000009f, 0x07950240, 0x245689c1, 0xc5faa71c, 0x73c86d67, 0xebad6ddc, 0x00000172, 0xcec10000, 0x63a22764,
        0xefa418ca, 0xcdd17b25, 0x6bdfef70, 0x9dea3e1f, 0x0000035f, 0xe4000000, 0xcdc3fe6e, 0x66bc0c6a, 0x2e391f32,
        0x5a450203, 0x71d2f825, 0xc3c24a56, 0x000007da, 0xa82e8f10, 0xaab24308, 0x8e211a7c, 0xf38ace40, 0x84c4ce0b,
        0x7ceb0b27, 0xad2594c3, 0x00001249, 0xdd1a4000, 0xcc9f54da, 0xdc5961bf, 0xc75cabab, 0xf505440c, 0xd1bc1667,
        0xfbb7af52, 0x608f8d29, 0x00002a94, 0x21000000, 0x17bb8a0c, 0x56af8ea4, 0x06479fa9, 0x5d4bb236, 0x80dc5fe0,
        0xf0feaa0a, 0xa88ed940, 0x6b1a80d0, 0x00006323, 0x324c3864, 0x8357c796, 0xe44a42d5, 0xd9a92261, 0xbd3c103d,
        0x91e5f372, 0xc0591574, 0xec1da60d, 0x102ad96c, 0x0000e6d3, 0x1e851000, 0x6e4f615b, 0x187b2a69, 0x0450e21c,
        0x2fdd342b, 0x635027ee, 0xa6c97199, 0x8e4ae916, 0x17082e28, 0x1a496e6f, 0x0002196e, 0x32400000, 0x04ad4026,
        0xf91e7250, 0x2994d1d5, 0x665bcdbb, 0xa23b2e96, 0x65fa7ddb, 0x77de53ac, 0xb020a29b, 0xc6bff953, 0x4b9425ab,
        0x0004e34d, 0xfbc32d81, 0x5222d0f4, 0xb70f2850, 0x5713f2f3, 0xdc421413, 0xd6395d7d, 0xf8591999, 0x0092381c,
        0x86b314d6, 0x7aa577b9, 0x12b7fe61, 0x000b616a, 0x1d11e400, 0x56c3678d, 0x3a941f20, 0x9b09368b, 0xbd706908,
        0x207665be, 0x9b26c4eb, 0x1567e89d, 0x9d15096e, 0x7132f22b, 0xbe485113, 0x45e5a2ce, 0x001a7f52, 0xbb100000,
        0x02f79478, 0x8c1b74c0, 0xb0f05d00, 0xa9dbc675, 0xe2d9b914, 0x650f72df, 0x77284b4c, 0x6df6e016, 0x514391c2,
        0x2795c9cf, 0xd6e2ab55, 0x9ca8e627, 0x003db1a6, 0x40000000, 0xf4ecd04a, 0x7f2388f0, 0x580a6dc5, 0x43bf046f,
        0xf82d5dc3, 0xee110848, 0xfaa0591c, 0xcdf4f028, 0x192ea53f, 0xbcd671a0, 0x7d694487, 0x10f96e01, 0x791a569d,
        0x008fa475, 0xb9b2e100, 0x8288753c, 0xcd3f1693, 0x89b43a6b, 0x089e87de, 0x684d4546, 0xfddba60c, 0xdf249391,
        0x3068ec13, 0x99b44427, 0xb68141ee, 0x5802cac3, 0xd96851f1, 0x7d7625a2, 0x014e718d, 0xfb640000, 0xf25a83e6,
        0x9457ad0f, 0x0080b511, 0x2029b566, 0xd7c5d2cf, 0xa53f6d7d, 0xcdb74d1c, 0xda9d70de, 0xb716413d, 0x71d0ca4e,
        0xd7e41398, 0x4f403a90, 0xf9ab3fe2, 0x264d776f, 0x030aafe6, 0x10000000, 0x09ab5531, 0xa60c58d2, 0x566126cb,
        0x6a1c8387, 0x7587f4c1, 0x2c44e876, 0x41a047cf, 0xc908059e, 0xa0ba063e, 0xe7cfc8e8, 0xe1fac055, 0xef0144b2,
        0x24207eb0, 0xd1722573, 0xe4b8f981, 0x071505ae, 0x7a3b6240, 0xcea45d4f, 0x4fe24133, 0x210f6d6d, 0xe55633f2,
        0x25c11356, 0x28ebd797, 0xd396eb84, 0x1e493b77, 0x471f2dae, 0x96ad3820, 0x8afaced1, 0x4edecddb, 0x5568c086,
        0xb2695da1, 0x24123c89, 0x107d4571, 0x1c410000, 0x6e174a27, 0xec62ae57, 0xef2289aa, 0xb6a2fbdd, 0x17e1efe4,
        0x3366bdf2, 0x37b48880, 0xbfb82c3e, 0x19acde91, 0xd4f46408, 0x35ff6a4e, 0x67566a0e, 0x40dbb914, 0x782a3bca,
        0x6b329b68, 0xf5afc5d9, 0x266469bc, 0xe4000000, 0xfb805ff4, 0xed55d1af, 0x9b4a20a8, 0xab9757f8, 0x01aefe0a,
        0x4a2ca67b, 0x1ebf9569, 0xc7c41c29, 0xd8d5d2aa, 0xd136c776, 0x93da550c, 0x9ac79d90, 0x254bcba8, 0x0df07618,
        0xf7a88809, 0x3a1f1074, 0xe54811fc, 0x59638ead, 0x97cbe710, 0x26d769e8, 0xb4e4723e, 0x5b90aa86, 0x9c333922,
        0x4b7a0775, 0x2d47e991, 0x9a6ef977, 0x160b40e7, 0x0c92f8c4, 0xf25ff010, 0x25c36c11, 0xc9f98b42, 0x730b919d,
        0x05ff7caf, 0xb0432d85, 0x2d2b7569, 0xa657842c, 0xd01fef10, 0xc77a4000, 0xe8b862e5, 0x10d8886a, 0xc8cd98e5,
        0x108955c5, 0xd059b655, 0x58fbbed4, 0x03b88231, 0x034c4519, 0x194dc939, 0x1fc500ac, 0x794cc0e2, 0x3bc980a1,
        0xe9b12dd1, 0x5e6d22f8, 0x7b38899a, 0xce7919d8, 0x78c67672, 0x79e5b99f, 0xe494034e, 0x00000001, 0xa1000000,
        0x6c5cd4e9, 0x9be47d6f, 0xf93bd9e7, 0x77626fa1, 0xc68b3451, 0xde2b59e8, 0xcf3cde58, 0x2246ff58, 0xa8577c15,
        0x26e77559, 0x17776753, 0xebe6b763, 0xe3fd0a5f, 0x33e83969, 0xa805a035, 0xf631b987, 0x211f0f43, 0xd85a43db,
        0xab1bf596, 0x683f19a2, 0x00000004, 0xbe7dfe64, 0x4bc9042f, 0xe1f5edb0, 0x8fa14eda, 0xe409db73, 0x674fee9c,
        0xa9159f0d, 0xf6b5b5d6, 0x7338960e, 0xeb49c291, 0x5f2b97cc, 0x0f383f95, 0x2091b3f6, 0xd1783714, 0xc1d142df,
        0x153e22de, 0x8aafdf57, 0x77f5e55f, 0xa3e7ca8b, 0x032f525b, 0x42e74f3d, 0x0000000a, 0xf4dd1000, 0x5d450952,
        0xaeb442e1, 0xa3b3342e, 0x3fcda36f, 0xb4287a6e, 0x4bc177f7, 0x67d2c8d0, 0xaea8f8e0, 0xadc93b67, 0x6cc856b3,
        0x959d9d0b, 0x5b48c100, 0x4abe8a3d, 0x52d936f4, 0x71dbe84d, 0xf91c21c5, 0x4a458109, 0xd7aad86a, 0x08e14c7c,
        0x759ba59c, 0xe43c8800, 0x00000017, 0x92400000, 0x04f110d4, 0x186472be, 0x8736c10c, 0x1478abfb, 0xfc51af29,
        0x25eb9739, 0x4c2b3015, 0xa1030e0b, 0x28fe3c3b, 0x7788fcba, 0xb89e4358, 0x733de4a4, 0x7c46f2c2, 0x8f746298,
        0xdb19210f, 0x2ea3b6ae, 0xaa5014b2, 0xea39ab8d, 0x97963442, 0x01dfdfa9, 0xd2f3d3fe, 0xa0790280, 0x00000037,
        0x509c9b01, 0xc7dcadf1, 0x383dad2c, 0x73c64d37, 0xea6d67d0, 0x519ba806, 0xc403f2f8, 0xa052e1a2, 0xd710233a,
        0x448573a9, 0xcf12d9ba, 0x70871803, 0x52dc3a9b, 0xe5b252e8, 0x0717fb4e, 0xbe4da62f, 0x0aabd7e1, 0x8c62ed4f,
        0xceb9ec7b, 0xd4664021, 0xa1158300, 0xcce375e6, 0x842f29f2, 0x00000081, 0x7717e400, 0xd3f5fb64, 0xa0763d71,
        0x7d142fe9, 0x33f44c66, 0xf3b8f12e, 0x130f0d8e, 0x734c9469, 0x60260fa8, 0x3c011340, 0xcc71880a, 0x37a52d21,
        0x8adac9ef, 0x42bb31b4, 0xd6f94c41, 0xc88b056c, 0xe20501b8, 0x5297ed7c, 0x62c361c4, 0x87dad8aa, 0xb833eade,
        0x94f06861, 0x13cc9abd, 0x8dc1d56a, 0x0000012d, 0x13100000, 0xc67a36e8, 0xf416299e, 0xf3493f0a, 0x77a5a6cf,
        0xa4be23a3, 0xcca25b82, 0x3510722f, 0xbe9d447f, 0xa8c213b8, 0xc94c324e, 0xbc9e33ad, 0x76acfeba, 0x2e4c2132,
        0x3e13cd32, 0x70fe91b4, 0xbb5cd936, 0x42149785, 0x46cc1afd, 0xe638ddf8, 0x690787d2, 0x1a02d117, 0x3eb5f1fe,
        0xc3b9abae, 0x1c08ee6f, 0x000002be, 0x40000000, 0x8140c2aa, 0x2cf877d9, 0x71e1d73d, 0xd5e72f98, 0x72516309,
        0xafa819dd, 0xd62a5a46, 0x2a02dcce, 0xce46ddfe, 0x2713248d, 0xb723d2ad, 0xc404bb19, 0xb706cc2b, 0x47b1ebca,
        0x9d094bdc, 0xc5dc02ca, 0x31e6518e, 0x8ec35680, 0x342f58a8, 0x8b041e42, 0xfebfe514, 0x05fffc13, 0x6763790f,
        0x66d536fd, 0xb9e15076, 0x00000662, 0x67b06100, 0xd2010a1a, 0xd005e1c0, 0xdb12733b, 0xa39f2e3f, 0x61b29de2,
        0x2a63dce2, 0x942604bc, 0x6170d59b, 0xc2e32596, 0x140b75b9, 0x1f1d2c21, 0xb8136a60, 0x89d23ba2, 0x60f17d73,
        0xc6cad7df, 0x0669df2b, 0x24b88737, 0x669306ed, 0x19496eeb, 0x938ddb6f, 0x5e748275, 0xc56e9a36, 0x3690b731,
        0xc82842c5, 0x24ae798e, 0x00000ede, 0x41640000, 0xd5889ac1, 0xd9432c99, 0xa280e71a, 0x6bf63d2e, 0x8249793d,
        0x79e7a943, 0x22fde64a, 0xe0d6709a, 0x05cacfef, 0xbd8da4d7, 0xe364006c, 0xa54edcb3, 0xa1a8086e, 0x748f459e,
        0xfc8e54c8, 0xcc74c657, 0x42b8c3d4, 0x57d9636e, 0x35b55bcc, 0x6c13fee9, 0x1ac45161, 0xb595badb, 0xa1f14e9d,
        0xdcf9e750, 0x07637f71, 0xde2f9f2b, 0x0000229d, 0x10000000, 0x3c5ebd89, 0xe3773756, 0x3dcba338, 0x81d29e4f,
        0xa4f79e2c, 0xc3f9c774, 0x6a1ce797, 0xac5fe438, 0x07f38b9c, 0xd588ecfa, 0x3e5ac1ac, 0x85afccce, 0x9d1f3f70,
        0xe82d6dd3, 0x177d180c, 0x5e69946f, 0x648e2ce1, 0x95a13948, 0x340fe011, 0xb4173c58, 0x2748f694, 0x7c2657bd,
        0x758bda2e, 0x3b8090a0, 0x2ddbb613, 0x6dcf4890, 0x24e4047e, 0x00005099};

    struct _Unpack_index {
        uint16_t _Offset; // The offset of this power's initial element in the array
        uint8_t _Zeroes; // The number of omitted leading zero elements
        uint8_t _Size; // The number of elements present for this power
    };

    static constexpr _Unpack_index _Large_power_indices[] = {{0, 0, 2}, {2, 0, 3}, {5, 0, 4}, {9, 1, 4}, {13, 1, 5},
        {18, 1, 6}, {24, 2, 6}, {30, 2, 7}, {37, 2, 8}, {45, 3, 8}, {53, 3, 9}, {62, 3, 10}, {72, 4, 10}, {82, 4, 11},
        {93, 4, 12}, {105, 5, 12}, {117, 5, 13}, {130, 5, 14}, {144, 5, 15}, {159, 6, 15}, {174, 6, 16}, {190, 6, 17},
        {207, 7, 17}, {224, 7, 18}, {242, 7, 19}, {261, 8, 19}, {280, 8, 21}, {301, 8, 22}, {323, 9, 22}, {345, 9, 23},
        {368, 9, 24}, {392, 10, 24}, {416, 10, 25}, {441, 10, 26}, {467, 10, 27}, {494, 11, 27}, {521, 11, 28},
        {549, 11, 29}};

    for (uint32_t _Large_power = _Power / 10; _Large_power != 0;) {
        const uint32_t _Current_power =
            (std:: min)(_Large_power, static_cast<uint32_t>(std:: size(_Large_power_indices)));

        const _Unpack_index& _Index = _Large_power_indices[_Current_power - 1];
        _Big_integer_flt _Multiplier{};
        _Multiplier._Myused = static_cast<uint32_t>(_Index._Size + _Index._Zeroes);

        const uint32_t* const _Source = _Large_power_data + _Index._Offset;

        std:: memset(_Multiplier._Mydata, 0, _Index._Zeroes * sizeof(uint32_t));
        std:: memcpy(_Multiplier._Mydata + _Index._Zeroes, _Source, _Index._Size * sizeof(uint32_t));

        if (!_Multiply(_Xval, _Multiplier)) { // when overflow occurs, resets to zero
            return false;
        }

        _Large_power -= _Current_power;
    }

    static constexpr uint32_t _Small_powers_of_ten[9] = {
        10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000, 1'000'000'000};

    const uint32_t _Small_power = _Power % 10;

    if (_Small_power == 0) {
        return true;
    }

    return _Multiply(_Xval, _Small_powers_of_ten[_Small_power - 1]); // when overflow occurs, resets to zero
}

// The following non-compiled code is the generator for the big powers of ten table found in
// _Multiply_by_power_of_ten(). This code is provided for future use if the table needs to be amended.
// Do not remove this code.
#if 0
//~ #include <algorithm>
//~ #include <charconv>
//~ #include <stdint.h>
//~ #include <stdio.h>
//~ #include <tuple>
//~ #include <vector>
using namespace std;

int main() {
    vector<uint32_t> elements;
    vector<tuple<uint32_t, uint32_t, uint32_t>> indices;

    for (uint32_t power = 10; power != 390; power += 10) {
        _Big_integer_flt big = _Make_big_integer_flt_one();

        for (uint32_t i = 0; i != power; ++i) {
            (void) _Multiply(big, 10); // assumes no overflow
        }

        const uint32_t* const first = big._Mydata;
        const uint32_t* const last = first + big._Myused;
        const uint32_t* const mid = find_if(first, last, [](const uint32_t elem) { return elem != 0; });

        indices.emplace_back(static_cast<uint32_t>(elements.size()), static_cast<uint32_t>(mid - first),
            static_cast<uint32_t>(last - mid));

        elements.insert(elements.end(), mid, last);
    }

    printf("static constexpr uint32_t _Large_power_data[] =\n{");
    for (uint32_t i = 0; i != elements.size(); ++i) {
        printf("%s0x%08x, ", i % 8 == 0 ? "\n\t" : "", elements[i]);
    }
    printf("\n};\n");

    printf("static constexpr _Unpack_index _Large_power_indices[] =\n{");
    for (uint32_t i = 0; i != indices.size(); ++i) {
        printf(
            "%s{ %u, %u, %u }, ", i % 6 == 0 ? "\n\t" : "", get<0>(indices[i]), get<1>(indices[i]), get<2>(indices[i]));
    }
    printf("\n};\n");
}
#endif // 0

// Computes the number of zeroes higher than the most significant set bit in _Ux
[[nodiscard]] inline uint32_t _Count_sequential_high_zeroes(const uint32_t _Ux) noexcept {
    ulong32 _Index; // Intentionally uninitialized for better codegen
    return _BitScanReverse(&_Index, _Ux) ? 31 - _Index : 32;
}

// This high-precision integer division implementation was translated from the implementation of
// System.Numerics.BigIntegerBuilder.ModDivCore in the .NET Framework sources.
// It computes both quotient and remainder: the remainder is stored in the _Numerator argument,
// and the least significant 64 bits of the quotient are returned from the function.
[[nodiscard]] inline uint64_t _Divide(_Big_integer_flt& _Numerator, const _Big_integer_flt& _Denominator) noexcept {
    // If the _Numerator is zero, then both the quotient and remainder are zero:
    if (_Numerator._Myused == 0) {
        return 0;
    }

    // If the _Denominator is zero, then uh oh. We can't divide by zero:
    assert(_Denominator._Myused != 0); // Division by zero

    uint32_t _Max_numerator_element_index         = _Numerator._Myused - 1;
    const uint32_t _Max_denominator_element_index = _Denominator._Myused - 1;

    // The _Numerator and _Denominator are both nonzero.
    // If the _Denominator is only one element wide, we can take the fast route:
    if (_Max_denominator_element_index == 0) {
        const uint32_t _Small_denominator = _Denominator._Mydata[0];

        if (_Max_numerator_element_index == 0) {
            const uint32_t _Small_numerator = _Numerator._Mydata[0];

            if (_Small_denominator == 1) {
                _Numerator._Myused = 0;
                return _Small_numerator;
            }

            _Numerator._Mydata[0] = _Small_numerator % _Small_denominator;
            _Numerator._Myused    = _Numerator._Mydata[0] > 0 ? 1u : 0u;
            return _Small_numerator / _Small_denominator;
        }

        if (_Small_denominator == 1) {
            uint64_t _Quotient = _Numerator._Mydata[1];
            _Quotient <<= 32;
            _Quotient |= _Numerator._Mydata[0];
            _Numerator._Myused = 0;
            return _Quotient;
        }

        // We count down in the next loop, so the last assignment to _Quotient will be the correct one.
        uint64_t _Quotient = 0;

        uint64_t _Uu = 0;
        for (uint32_t _Iv = _Max_numerator_element_index; _Iv != static_cast<uint32_t>(-1); --_Iv) {
            _Uu       = (_Uu << 32) | _Numerator._Mydata[_Iv];
            _Quotient = (_Quotient << 32) + static_cast<uint32_t>(_Uu / _Small_denominator);
            _Uu %= _Small_denominator;
        }

        _Numerator._Mydata[1] = static_cast<uint32_t>(_Uu >> 32);
        _Numerator._Mydata[0] = static_cast<uint32_t>(_Uu);
        _Numerator._Myused    = _Numerator._Mydata[1] > 0 ? 2u : 1u;
        return _Quotient;
    }

    if (_Max_denominator_element_index > _Max_numerator_element_index) {
        return 0;
    }

    const uint32_t _Cu_den = _Max_denominator_element_index + 1;
    const int32_t _Cu_diff = static_cast<int32_t>(_Max_numerator_element_index - _Max_denominator_element_index);

    // Determine whether the result will have _Cu_diff or _Cu_diff + 1 digits:
    int32_t _Cu_quo = _Cu_diff;
    for (int32_t _Iu = static_cast<int32_t>(_Max_numerator_element_index);; --_Iu) {
        if (_Iu < _Cu_diff) {
            ++_Cu_quo;
            break;
        }

        if (_Denominator._Mydata[_Iu - _Cu_diff] != _Numerator._Mydata[_Iu]) {
            if (_Denominator._Mydata[_Iu - _Cu_diff] < _Numerator._Mydata[_Iu]) {
                ++_Cu_quo;
            }

            break;
        }
    }

    if (_Cu_quo == 0) {
        return 0;
    }

    // Get the uint to use for the trial divisions. We normalize so the high bit is set:
    uint32_t _U_den      = _Denominator._Mydata[_Cu_den - 1];
    uint32_t _U_den_next = _Denominator._Mydata[_Cu_den - 2];

    const uint32_t _Cbit_shift_left  = _Count_sequential_high_zeroes(_U_den);
    const uint32_t _Cbit_shift_right = 32 - _Cbit_shift_left;
    if (_Cbit_shift_left > 0) {
        _U_den = (_U_den << _Cbit_shift_left) | (_U_den_next >> _Cbit_shift_right);
        _U_den_next <<= _Cbit_shift_left;

        if (_Cu_den > 2) {
            _U_den_next |= _Denominator._Mydata[_Cu_den - 3] >> _Cbit_shift_right;
        }
    }

    uint64_t _Quotient = 0;
    for (int32_t _Iu = _Cu_quo; --_Iu >= 0;) {
        // Get the high (normalized) bits of the _Numerator:
        const uint32_t _U_num_hi =
            (_Iu + _Cu_den <= _Max_numerator_element_index) ? _Numerator._Mydata[_Iu + _Cu_den] : 0;

        uint64_t _Uu_num =
            (static_cast<uint64_t>(_U_num_hi) << 32) | static_cast<uint64_t>(_Numerator._Mydata[_Iu + _Cu_den - 1]);

        uint32_t _U_num_next = _Numerator._Mydata[_Iu + _Cu_den - 2];
        if (_Cbit_shift_left > 0) {
            _Uu_num = (_Uu_num << _Cbit_shift_left) | (_U_num_next >> _Cbit_shift_right);
            _U_num_next <<= _Cbit_shift_left;

            if (_Iu + _Cu_den >= 3) {
                _U_num_next |= _Numerator._Mydata[_Iu + _Cu_den - 3] >> _Cbit_shift_right;
            }
        }

        // Divide to get the quotient digit:
        uint64_t _Uu_quo = _Uu_num / _U_den;
        uint64_t _Uu_rem = static_cast<uint32_t>(_Uu_num % _U_den);

        if (_Uu_quo > UINT32_MAX) {
            _Uu_rem += _U_den * (_Uu_quo - UINT32_MAX);
            _Uu_quo = UINT32_MAX;
        }

        while (_Uu_rem <= UINT32_MAX && _Uu_quo * _U_den_next > ((_Uu_rem << 32) | _U_num_next)) {
            --_Uu_quo;
            _Uu_rem += _U_den;
        }

        // Multiply and subtract. Note that _Uu_quo may be one too large.
        // If we have a borrow at the end, we'll add the _Denominator back on and decrement _Uu_quo.
        if (_Uu_quo > 0) {
            uint64_t _Uu_borrow = 0;

            for (uint32_t _Iu2 = 0; _Iu2 < _Cu_den; ++_Iu2) {
                _Uu_borrow += _Uu_quo * _Denominator._Mydata[_Iu2];

                const uint32_t _U_sub = static_cast<uint32_t>(_Uu_borrow);
                _Uu_borrow >>= 32;
                if (_Numerator._Mydata[_Iu + _Iu2] < _U_sub) {
                    ++_Uu_borrow;
                }

                _Numerator._Mydata[_Iu + _Iu2] -= _U_sub;
            }

            if (_U_num_hi < _Uu_borrow) {
                // Add, tracking carry:
                uint32_t _U_carry = 0;
                for (uint32_t _Iu2 = 0; _Iu2 < _Cu_den; ++_Iu2) {
                    const uint64_t _Sum = static_cast<uint64_t>(_Numerator._Mydata[_Iu + _Iu2])
                                        + static_cast<uint64_t>(_Denominator._Mydata[_Iu2]) + _U_carry;

                    _Numerator._Mydata[_Iu + _Iu2] = static_cast<uint32_t>(_Sum);
                    _U_carry                       = static_cast<uint32_t>(_Sum >> 32);
                }

                --_Uu_quo;
            }

            _Max_numerator_element_index = _Iu + _Cu_den - 1;
        }

        _Quotient = (_Quotient << 32) + static_cast<uint32_t>(_Uu_quo);
    }

    // Trim the remainder:
    for (uint32_t _Ix = _Max_numerator_element_index + 1; _Ix < _Numerator._Myused; ++_Ix) {
        _Numerator._Mydata[_Ix] = 0;
    }

    uint32_t _Used = _Max_numerator_element_index + 1;

    while (_Used != 0 && _Numerator._Mydata[_Used - 1] == 0) {
        --_Used;
    }

    _Numerator._Myused = _Used;

    return _Quotient;
}

// ^^^^^^^^^^ DERIVED FROM corecrt_internal_big_integer.h ^^^^^^^^^^


// vvvvvvvvvv DERIVED FROM corecrt_internal_strtox.h vvvvvvvvvv

// This type is used to hold a partially-parsed string representation of a floating-point number.
// The number is stored in the following form:

// [sign] 0._Mymantissa * B^_Myexponent

// The _Mymantissa buffer stores the mantissa digits in big-endian, binary-coded decimal form. The _Mymantissa_count
// stores the number of digits present in the _Mymantissa buffer. The base B is not stored; it must be tracked
// separately. Note that the base of the mantissa digits may not be the same as B (e.g., for hexadecimal
// floating-point, the mantissa digits are in base 16 but the exponent is a base 2 exponent).

// We consider up to 768 decimal digits during conversion. In most cases, we require nowhere near this many digits
// of precision to compute the correctly rounded binary floating-point value for the input string. The worst case is
// (2 - 3 * 2^-53) * 2^-1022, which has an exact decimal representation of 768 decimal digits after trimming zeroes.
// This value is exactly between 0x1.ffffffffffffep-1022 and 0x1.fffffffffffffp-1022. For round-to-nearest,
// ties-to-even behavior, we also need to consider whether there are any nonzero trailing decimal digits.

// NOTE: The mantissa buffer count here must be kept in sync with the precision of the _Big_integer_flt type.
struct _Floating_point_string {
    bool _Myis_negative;
    int32_t _Myexponent;
    uint32_t _Mymantissa_count;
    uint8_t _Mymantissa[768];
};

// Stores a positive or negative zero into the _Result object
template <class _FloatingType>
void _Assemble_floating_point_zero(const bool _Is_negative, _FloatingType& _Result) noexcept {
    using _Floating_traits = _Floating_type_traits<_FloatingType>;
    using _Uint_type       = typename _Floating_traits::_Uint_type;

    _Uint_type _Sign_component = _Is_negative;
    _Sign_component <<= _Floating_traits::_Sign_shift;

    _Result = bit_cast<_FloatingType>(_Sign_component);
}

// Stores a positive or negative infinity into the _Result object
template <class _FloatingType>
void _Assemble_floating_point_infinity(const bool _Is_negative, _FloatingType& _Result) noexcept {
    using _Floating_traits = _Floating_type_traits<_FloatingType>;
    using _Uint_type       = typename _Floating_traits::_Uint_type;

    _Uint_type _Sign_component = _Is_negative;
    _Sign_component <<= _Floating_traits::_Sign_shift;

    const _Uint_type _Exponent_component = _Floating_traits::_Shifted_exponent_mask;

    _Result = bit_cast<_FloatingType>(_Sign_component | _Exponent_component);
}

// Determines whether a mantissa should be rounded up according to round_to_nearest given [1] the value of the least
// significant bit of the mantissa, [2] the value of the next bit after the least significant bit (the "round" bit)
// and [3] whether any trailing bits after the round bit are set.

// The mantissa is treated as an unsigned integer magnitude.

// For this function, "round up" is defined as "increase the magnitude" of the mantissa. (Note that this means that
// if we need to round a negative value to the next largest representable value, we return false, because the next
// largest representable value has a smaller magnitude.)
[[nodiscard]] inline bool _Should_round_up(
    const bool _Lsb_bit, const bool _Round_bit, const bool _Has_tail_bits) noexcept {
    // If there are no insignificant set bits, the value is exactly-representable and should not be rounded.
    // We could detect this with:
    // const bool _Is_exactly_representable = !_Round_bit && !_Has_tail_bits;
    // if (_Is_exactly_representable) { return false; }
    // However, this is unnecessary given the logic below.

    // If there are insignificant set bits, we need to round according to round_to_nearest.
    // We need to handle two cases: we round up if either [1] the value is slightly greater
    // than the midpoint between two exactly-representable values or [2] the value is exactly the midpoint
    // between two exactly-representable values and the greater of the two is even (this is "round-to-even").
    return _Round_bit && (_Has_tail_bits || _Lsb_bit);
}

// Computes _Value / 2^_Shift, then rounds the result according to round_to_nearest.
// By the time we call this function, we will already have discarded most digits.
// The caller must pass true for _Has_zero_tail if all discarded bits were zeroes.
[[nodiscard]] inline uint64_t _Right_shift_with_rounding(
    const uint64_t _Value, const uint32_t _Shift, const bool _Has_zero_tail) noexcept {
    constexpr uint32_t _Total_number_of_bits = 64;
    if (_Shift >= _Total_number_of_bits) {
        if (_Shift == _Total_number_of_bits) {
            constexpr uint64_t _Extra_bits_mask = (1ULL << (_Total_number_of_bits - 1)) - 1;
            constexpr uint64_t _Round_bit_mask  = (1ULL << (_Total_number_of_bits - 1));

            const bool _Round_bit = (_Value & _Round_bit_mask) != 0;
            const bool _Tail_bits = !_Has_zero_tail || (_Value & _Extra_bits_mask) != 0;

            // We round up the answer to 1 if the answer is greater than 0.5. Otherwise, we round down the answer to 0
            // if either [1] the answer is less than 0.5 or [2] the answer is exactly 0.5.
            return static_cast<uint64_t>(_Round_bit && _Tail_bits);
        } else {
            // If we'd need to shift 65 or more bits, the answer is less than 0.5 and is always rounded to zero:
            return 0;
        }
    }

    // Reference implementation with suboptimal codegen:
    // const uint64_t _Extra_bits_mask = (1ULL << (_Shift - 1)) - 1;
    // const uint64_t _Round_bit_mask  = (1ULL << (_Shift - 1));
    // const uint64_t _Lsb_bit_mask    = 1ULL << _Shift;

    // const bool _Lsb_bit   = (_Value & _Lsb_bit_mask) != 0;
    // const bool _Round_bit = (_Value & _Round_bit_mask) != 0;
    // const bool _Tail_bits = !_Has_zero_tail || (_Value & _Extra_bits_mask) != 0;

    // return (_Value >> _Shift) + _Should_round_up(_Lsb_bit, _Round_bit, _Tail_bits);

    // Example for optimized implementation: Let _Shift be 8.
    // Bit index: ...[8]76543210
    //    _Value: ...[L]RTTTTTTT
    // By focusing on the bit at index _Shift, we can avoid unnecessary branching and shifting.

    // Bit index: ...[8]76543210
    //  _Lsb_bit: ...[L]RTTTTTTT
    const uint64_t _Lsb_bit = _Value;

    //  Bit index: ...9[8]76543210
    // _Round_bit: ...L[R]TTTTTTT0
    const uint64_t _Round_bit = _Value << 1;

    // We can detect (without branching) whether any of the trailing bits are set.
    // Due to _Should_round below, this computation will be used if and only if R is 1, so we can assume that here.
    //      Bit index: ...9[8]76543210
    //     _Round_bit: ...L[1]TTTTTTT0
    // _Has_tail_bits: ....[H]........

    // If all of the trailing bits T are 0, and _Has_zero_tail is true,
    // then `_Round_bit - static_cast<uint64_t>(_Has_zero_tail)` will produce 0 for H (due to R being 1).
    // If any of the trailing bits T are 1, or _Has_zero_tail is false,
    // then `_Round_bit - static_cast<uint64_t>(_Has_zero_tail)` will produce 1 for H (due to R being 1).
    const uint64_t _Has_tail_bits = _Round_bit - static_cast<uint64_t>(_Has_zero_tail);

    // Finally, we can use _Should_round_up() logic with bitwise-AND and bitwise-OR,
    // selecting just the bit at index _Shift.
    const uint64_t _Should_round = ((_Round_bit & (_Has_tail_bits | _Lsb_bit)) >> _Shift) & uint64_t{1};

    // This rounding technique is dedicated to the memory of Peppermint. =^..^=
    return (_Value >> _Shift) + _Should_round;
}

// Converts the floating-point value [sign] (mantissa / 2^(precision-1)) * 2^exponent into the correct form for
// _FloatingType and stores the result into the _Result object.
// The caller must ensure that the mantissa and exponent are correctly computed such that either:
// [1] min_exponent <= exponent <= max_exponent && 2^(precision-1) <= mantissa <= 2^precision, or
// [2] exponent == min_exponent && 0 < mantissa <= 2^(precision-1).
// (The caller should round the mantissa before calling this function. The caller doesn't need to renormalize the
// mantissa when the mantissa carries over to a higher bit after rounding up.)

// This function correctly handles overflow and stores an infinity in the _Result object.
// (The result overflows if and only if exponent == max_exponent && mantissa == 2^precision)
template <class _FloatingType>
void _Assemble_floating_point_value_no_shift(const bool _Is_negative, const int32_t _Exponent,
    const typename _Floating_type_traits<_FloatingType>::_Uint_type _Mantissa, _FloatingType& _Result) noexcept {
    // The following code assembles floating-point values based on an alternative interpretation of the IEEE 754 binary
    // floating-point format. It is valid for all of the following cases:
    // [1] normal value,
    // [2] normal value, needs renormalization and exponent increment after rounding up the mantissa,
    // [3] normal value, overflows after rounding up the mantissa,
    // [4] subnormal value,
    // [5] subnormal value, becomes a normal value after rounding up the mantissa.

    // Examples for float:
    // | Case |     Input     | Exponent |  Exponent  |  Exponent  |  Rounded  | Result Bits |     Result      |
    // |      |               |          | + Bias - 1 |  Component |  Mantissa |             |                 |
    // | ---- | ------------- | -------- | ---------- | ---------- | --------- | ----------- | --------------- |
    // | [1]  | 1.000000p+0   |     +0   |    126     | 0x3f000000 |  0x800000 | 0x3f800000  | 0x1.000000p+0   |
    // | [2]  | 1.ffffffp+0   |     +0   |    126     | 0x3f000000 | 0x1000000 | 0x40000000  | 0x1.000000p+1   |
    // | [3]  | 1.ffffffp+127 |   +127   |    253     | 0x7e800000 | 0x1000000 | 0x7f800000  |     inf         |
    // | [4]  | 0.fffffep-126 |   -126   |      0     | 0x00000000 |  0x7fffff | 0x007fffff  | 0x0.fffffep-126 |
    // | [5]  | 0.ffffffp-126 |   -126   |      0     | 0x00000000 |  0x800000 | 0x00800000  | 0x1.000000p-126 |
    using _Floating_traits = _Floating_type_traits<_FloatingType>;
    using _Uint_type       = typename _Floating_traits::_Uint_type;

    _Uint_type _Sign_component = _Is_negative;
    _Sign_component <<= _Floating_traits::_Sign_shift;

    _Uint_type _Exponent_component = static_cast<uint32_t>(_Exponent + (_Floating_traits::_Exponent_bias - 1));
    _Exponent_component <<= _Floating_traits::_Exponent_shift;

    _Result = bit_cast<_FloatingType>(_Sign_component | (_Exponent_component + _Mantissa));
}

// Converts the floating-point value [sign] (mantissa / 2^(precision-1)) * 2^exponent into the correct form for
// _FloatingType and stores the result into the _Result object. The caller must ensure that the mantissa and exponent
// are correctly computed such that either [1] the most significant bit of the mantissa is in the correct position for
// the _FloatingType, or [2] the exponent has been correctly adjusted to account for the shift of the mantissa that will
// be required.

// This function correctly handles range errors and stores a zero or infinity in the _Result object
// on underflow and overflow errors, respectively. This function correctly forms denormal numbers when required.

// If the provided mantissa has more bits of precision than can be stored in the _Result object, the mantissa is
// rounded to the available precision. Thus, if possible, the caller should provide a mantissa with at least one
// more bit of precision than is required, to ensure that the mantissa is correctly rounded.
// (The caller should not round the mantissa before calling this function.)
template <class _FloatingType>
[[nodiscard]] std::errc _Assemble_floating_point_value(const uint64_t _Initial_mantissa, const int32_t _Initial_exponent,
    const bool _Is_negative, const bool _Has_zero_tail, _FloatingType& _Result) noexcept {
    using _Traits = _Floating_type_traits<_FloatingType>;

    // Assume that the number is representable as a normal value.
    // Compute the number of bits by which we must adjust the mantissa to shift it into the correct position,
    // and compute the resulting base two exponent for the normalized mantissa:
    const uint32_t _Initial_mantissa_bits = _Bit_scan_reverse(_Initial_mantissa);
    const int32_t _Normal_mantissa_shift  = static_cast<int32_t>(_Traits::_Mantissa_bits - _Initial_mantissa_bits);
    const int32_t _Normal_exponent        = _Initial_exponent - _Normal_mantissa_shift;

    if (_Normal_exponent > _Traits::_Maximum_binary_exponent) {
        // The exponent is too large to be represented by the floating-point type; report the overflow condition:
        _Assemble_floating_point_infinity(_Is_negative, _Result);
        return std::errc::result_out_of_range; // Overflow example: "1e+1000"
    }

    uint64_t _Mantissa = _Initial_mantissa;
    int32_t _Exponent  = _Normal_exponent;
    std::errc _Error_code{};

    if (_Normal_exponent < _Traits::_Minimum_binary_exponent) {
        // The exponent is too small to be represented by the floating-point type as a normal value, but it may be
        // representable as a denormal value.

        // The exponent of subnormal values (as defined by the mathematical model of floating-point numbers, not the
        // exponent field in the bit representation) is equal to the minimum exponent of normal values.
        _Exponent = _Traits::_Minimum_binary_exponent;

        // Compute the number of bits by which we need to shift the mantissa in order to form a denormal number.
        const int32_t _Denormal_mantissa_shift = _Initial_exponent - _Exponent;

        if (_Denormal_mantissa_shift < 0) {
            _Mantissa =
                _Right_shift_with_rounding(_Mantissa, static_cast<uint32_t>(-_Denormal_mantissa_shift), _Has_zero_tail);

            // from_chars in MSVC STL and strto[f|d|ld] in UCRT reports underflow only when the result is zero after
            // rounding to the floating-point format. This behavior is different from IEEE 754 underflow exception.
            if (_Mantissa == 0) {
                _Error_code = std::errc::result_out_of_range; // Underflow example: "1e-1000"
            }

            // When we round the mantissa, the result may be so large that the number becomes a normal value.
            // For example, consider the single-precision case where the mantissa is 0x01ffffff and a right shift
            // of 2 is required to shift the value into position. We perform the shift in two steps: we shift by
            // one bit, then we shift again and round using the dropped bit. The initial shift yields 0x00ffffff.
            // The rounding shift then yields 0x007fffff and because the least significant bit was 1, we add 1
            // to this number to round it. The final result is 0x00800000.

            // 0x00800000 is 24 bits, which is more than the 23 bits available in the mantissa.
            // Thus, we have rounded our denormal number into a normal number.

            // We detect this case here and re-adjust the mantissa and exponent appropriately, to form a normal number.
            // This is handled by _Assemble_floating_point_value_no_shift.
        } else {
            _Mantissa <<= _Denormal_mantissa_shift;
        }
    } else {
        if (_Normal_mantissa_shift < 0) {
            _Mantissa =
                _Right_shift_with_rounding(_Mantissa, static_cast<uint32_t>(-_Normal_mantissa_shift), _Has_zero_tail);

            // When we round the mantissa, it may produce a result that is too large. In this case,
            // we divide the mantissa by two and increment the exponent (this does not change the value).
            // This is handled by _Assemble_floating_point_value_no_shift.

            // The increment of the exponent may have generated a value too large to be represented.
            // In this case, report the overflow:
            if (_Mantissa > _Traits::_Normal_mantissa_mask && _Exponent == _Traits::_Maximum_binary_exponent) {
                _Error_code = std::errc::result_out_of_range; // Overflow example: "1.ffffffp+127" for float
                                                         // Overflow example: "1.fffffffffffff8p+1023" for double
            }
        } else {
            _Mantissa <<= _Normal_mantissa_shift;
        }
    }

    // Assemble the floating-point value from the computed components:
    using _Uint_type = typename _Traits::_Uint_type;

    _Assemble_floating_point_value_no_shift(_Is_negative, _Exponent, static_cast<_Uint_type>(_Mantissa), _Result);

    return _Error_code;
}

// This function is part of the fast track for integer floating-point strings. It takes an integer and a sign and
// converts the value into its _FloatingType representation, storing the result in the _Result object. If the value
// is not representable, +/-infinity is stored and overflow is reported (since this function deals with only integers,
// underflow is impossible).
template <class _FloatingType>
[[nodiscard]] std::errc _Assemble_floating_point_value_from_big_integer_flt(const _Big_integer_flt& _Integer_value,
    const uint32_t _Integer_bits_of_precision, const bool _Is_negative, const bool _Has_nonzero_fractional_part,
    _FloatingType& _Result) noexcept {
    using _Traits = _Floating_type_traits<_FloatingType>;

    const int32_t _Base_exponent = _Traits::_Mantissa_bits - 1;

    // Very fast case: If we have 64 bits of precision or fewer,
    // we can just take the two low order elements from the _Big_integer_flt:
    if (_Integer_bits_of_precision <= 64) {
        const int32_t _Exponent = _Base_exponent;

        const uint32_t _Mantissa_low  = _Integer_value._Myused > 0 ? _Integer_value._Mydata[0] : 0;
        const uint32_t _Mantissa_high = _Integer_value._Myused > 1 ? _Integer_value._Mydata[1] : 0;
        const uint64_t _Mantissa      = _Mantissa_low + (static_cast<uint64_t>(_Mantissa_high) << 32);

        return _Assemble_floating_point_value(
            _Mantissa, _Exponent, _Is_negative, !_Has_nonzero_fractional_part, _Result);
    }

    const uint32_t _Top_element_bits  = _Integer_bits_of_precision % 32;
    const uint32_t _Top_element_index = _Integer_bits_of_precision / 32;

    const uint32_t _Middle_element_index = _Top_element_index - 1;
    const uint32_t _Bottom_element_index = _Top_element_index - 2;

    // Pretty fast case: If the top 64 bits occupy only two elements, we can just combine those two elements:
    if (_Top_element_bits == 0) {
        const int32_t _Exponent = static_cast<int32_t>(_Base_exponent + _Bottom_element_index * 32);

        const uint64_t _Mantissa = _Integer_value._Mydata[_Bottom_element_index]
                                 + (static_cast<uint64_t>(_Integer_value._Mydata[_Middle_element_index]) << 32);

        bool _Has_zero_tail = !_Has_nonzero_fractional_part;
        for (uint32_t _Ix = 0; _Has_zero_tail && _Ix != _Bottom_element_index; ++_Ix) {
            _Has_zero_tail = _Integer_value._Mydata[_Ix] == 0;
        }

        return _Assemble_floating_point_value(_Mantissa, _Exponent, _Is_negative, _Has_zero_tail, _Result);
    }

    // Not quite so fast case: The top 64 bits span three elements in the _Big_integer_flt. Assemble the three pieces:
    const uint32_t _Top_element_mask  = (1u << _Top_element_bits) - 1;
    const uint32_t _Top_element_shift = 64 - _Top_element_bits; // Left

    const uint32_t _Middle_element_shift = _Top_element_shift - 32; // Left

    const uint32_t _Bottom_element_bits  = 32 - _Top_element_bits;
    const uint32_t _Bottom_element_mask  = ~_Top_element_mask;
    const uint32_t _Bottom_element_shift = 32 - _Bottom_element_bits; // Right

    const int32_t _Exponent = static_cast<int32_t>(_Base_exponent + _Bottom_element_index * 32 + _Top_element_bits);

    const uint64_t _Mantissa =
        (static_cast<uint64_t>(_Integer_value._Mydata[_Top_element_index] & _Top_element_mask) << _Top_element_shift)
        + (static_cast<uint64_t>(_Integer_value._Mydata[_Middle_element_index]) << _Middle_element_shift)
        + (static_cast<uint64_t>(_Integer_value._Mydata[_Bottom_element_index] & _Bottom_element_mask)
            >> _Bottom_element_shift);

    bool _Has_zero_tail =
        !_Has_nonzero_fractional_part && (_Integer_value._Mydata[_Bottom_element_index] & _Top_element_mask) == 0;

    for (uint32_t _Ix = 0; _Has_zero_tail && _Ix != _Bottom_element_index; ++_Ix) {
        _Has_zero_tail = _Integer_value._Mydata[_Ix] == 0;
    }

    return _Assemble_floating_point_value(_Mantissa, _Exponent, _Is_negative, _Has_zero_tail, _Result);
}

// Accumulates the decimal digits in [_First_digit, _Last_digit) into the _Result high-precision integer.
// This function assumes that no overflow will occur.
inline void _Accumulate_decimal_digits_into_big_integer_flt(
    const uint8_t* const _First_digit, const uint8_t* const _Last_digit, _Big_integer_flt& _Result) noexcept {
    // We accumulate nine digit chunks, transforming the base ten string into base one billion on the fly,
    // allowing us to reduce the number of high-precision multiplication and addition operations by 8/9.
    uint32_t _Accumulator       = 0;
    uint32_t _Accumulator_count = 0;
    for (const uint8_t* _It = _First_digit; _It != _Last_digit; ++_It) {
        if (_Accumulator_count == 9) {
            [[maybe_unused]] const bool _Success1 = _Multiply(_Result, 1'000'000'000); // assumes no overflow
            assert(_Success1);
            [[maybe_unused]] const bool _Success2 = _Add(_Result, _Accumulator); // assumes no overflow
            assert(_Success2);

            _Accumulator       = 0;
            _Accumulator_count = 0;
        }

        _Accumulator *= 10;
        _Accumulator += *_It;
        ++_Accumulator_count;
    }

    if (_Accumulator_count != 0) {
        [[maybe_unused]] const bool _Success3 =
            _Multiply_by_power_of_ten(_Result, _Accumulator_count); // assumes no overflow
        assert(_Success3);
        [[maybe_unused]] const bool _Success4 = _Add(_Result, _Accumulator); // assumes no overflow
        assert(_Success4);
    }
}

// The core floating-point string parser for decimal strings. After a subject string is parsed and converted
// into a _Floating_point_string object, if the subject string was determined to be a decimal string,
// the object is passed to this function. This function converts the decimal real value to floating-point.
template <class _FloatingType>
[[nodiscard]] std::errc _Convert_decimal_string_to_floating_type(
    const _Floating_point_string& _Data, _FloatingType& _Result, bool _Has_zero_tail) noexcept {
    using _Traits = _Floating_type_traits<_FloatingType>;

    // To generate an N bit mantissa we require N + 1 bits of precision. The extra bit is used to correctly round
    // the mantissa (if there are fewer bits than this available, then that's totally okay;
    // in that case we use what we have and we don't need to round).
    const uint32_t _Required_bits_of_precision = static_cast<uint32_t>(_Traits::_Mantissa_bits + 1);

    // The input is of the form 0.mantissa * 10^exponent, where 'mantissa' are the decimal digits of the mantissa
    // and 'exponent' is the decimal exponent. We decompose the mantissa into two parts: an integer part and a
    // fractional part. If the exponent is positive, then the integer part consists of the first 'exponent' digits,
    // or all present digits if there are fewer digits. If the exponent is zero or negative, then the integer part
    // is empty. In either case, the remaining digits form the fractional part of the mantissa.
    const uint32_t _Positive_exponent      = static_cast<uint32_t>((std:: max)(0, _Data._Myexponent));
    const uint32_t _Integer_digits_present = (std:: min)(_Positive_exponent, _Data._Mymantissa_count);
    const uint32_t _Integer_digits_missing = _Positive_exponent - _Integer_digits_present;
    const uint8_t* const _Integer_first    = _Data._Mymantissa;
    const uint8_t* const _Integer_last     = _Data._Mymantissa + _Integer_digits_present;

    const uint8_t* const _Fractional_first    = _Integer_last;
    const uint8_t* const _Fractional_last     = _Data._Mymantissa + _Data._Mymantissa_count;
    const uint32_t _Fractional_digits_present = static_cast<uint32_t>(_Fractional_last - _Fractional_first);

    // First, we accumulate the integer part of the mantissa into a _Big_integer_flt:
    _Big_integer_flt _Integer_value{};
    _Accumulate_decimal_digits_into_big_integer_flt(_Integer_first, _Integer_last, _Integer_value);

    if (_Integer_digits_missing > 0) {
        if (!_Multiply_by_power_of_ten(_Integer_value, _Integer_digits_missing)) {
            _Assemble_floating_point_infinity(_Data._Myis_negative, _Result);
            return std::errc::result_out_of_range; // Overflow example: "1e+2000"
        }
    }

    // At this point, the _Integer_value contains the value of the integer part of the mantissa. If either
    // [1] this number has more than the required number of bits of precision or
    // [2] the mantissa has no fractional part, then we can assemble the result immediately:
    const uint32_t _Integer_bits_of_precision = _Bit_scan_reverse(_Integer_value);
    {
        const bool _Has_zero_fractional_part = _Fractional_digits_present == 0 && _Has_zero_tail;

        if (_Integer_bits_of_precision >= _Required_bits_of_precision || _Has_zero_fractional_part) {
            return _Assemble_floating_point_value_from_big_integer_flt(
                _Integer_value, _Integer_bits_of_precision, _Data._Myis_negative, !_Has_zero_fractional_part, _Result);
        }
    }

    // Otherwise, we did not get enough bits of precision from the integer part, and the mantissa has a fractional
    // part. We parse the fractional part of the mantissa to obtain more bits of precision. To do this, we convert
    // the fractional part into an actual fraction N/M, where the numerator N is computed from the digits of the
    // fractional part, and the denominator M is computed as the power of 10 such that N/M is equal to the value
    // of the fractional part of the mantissa.
    _Big_integer_flt _Fractional_numerator{};
    _Accumulate_decimal_digits_into_big_integer_flt(_Fractional_first, _Fractional_last, _Fractional_numerator);

    const uint32_t _Fractional_denominator_exponent =
        _Data._Myexponent < 0 ? _Fractional_digits_present + static_cast<uint32_t>(-_Data._Myexponent)
                              : _Fractional_digits_present;

    _Big_integer_flt _Fractional_denominator = _Make_big_integer_flt_one();
    if (!_Multiply_by_power_of_ten(_Fractional_denominator, _Fractional_denominator_exponent)) {
        // If there were any digits in the integer part, it is impossible to underflow (because the exponent
        // cannot possibly be small enough), so if we underflow here it is a true underflow and we return zero.
        _Assemble_floating_point_zero(_Data._Myis_negative, _Result);
        return std::errc::result_out_of_range; // Underflow example: "1e-2000"
    }

    // Because we are using only the fractional part of the mantissa here, the numerator is guaranteed to be smaller
    // than the denominator. We normalize the fraction such that the most significant bit of the numerator is in the
    // same position as the most significant bit in the denominator. This ensures that when we later shift the
    // numerator N bits to the left, we will produce N bits of precision.
    const uint32_t _Fractional_numerator_bits   = _Bit_scan_reverse(_Fractional_numerator);
    const uint32_t _Fractional_denominator_bits = _Bit_scan_reverse(_Fractional_denominator);

    const uint32_t _Fractional_shift = _Fractional_denominator_bits > _Fractional_numerator_bits
                                         ? _Fractional_denominator_bits - _Fractional_numerator_bits
                                         : 0;

    if (_Fractional_shift > 0) {
        [[maybe_unused]] const bool _Shift_success1 =
            _Shift_left(_Fractional_numerator, _Fractional_shift); // assumes no overflow
        assert(_Shift_success1);
    }

    const uint32_t _Required_fractional_bits_of_precision = _Required_bits_of_precision - _Integer_bits_of_precision;

    uint32_t _Remaining_bits_of_precision_required = _Required_fractional_bits_of_precision;
    if (_Integer_bits_of_precision > 0) {
        // If the fractional part of the mantissa provides no bits of precision and cannot affect rounding,
        // we can just take whatever bits we got from the integer part of the mantissa. This is the case for numbers
        // like 5.0000000000000000000001, where the significant digits of the fractional part start so far to the
        // right that they do not affect the floating-point representation.

        // If the fractional shift is exactly equal to the number of bits of precision that we require,
        // then no fractional bits will be part of the result, but the result may affect rounding.
        // This is e.g. the case for large, odd integers with a fractional part greater than or equal to .5.
        // Thus, we need to do the division to correctly round the result.
        if (_Fractional_shift > _Remaining_bits_of_precision_required) {
            return _Assemble_floating_point_value_from_big_integer_flt(_Integer_value, _Integer_bits_of_precision,
                _Data._Myis_negative, _Fractional_digits_present != 0 || !_Has_zero_tail, _Result);
        }

        _Remaining_bits_of_precision_required -= _Fractional_shift;
    }

    // If there was no integer part of the mantissa, we will need to compute the exponent from the fractional part.
    // The fractional exponent is the power of two by which we must multiply the fractional part to move it into the
    // range [1.0, 2.0). This will either be the same as the shift we computed earlier, or one greater than that shift:
    const uint32_t _Fractional_exponent =
        _Fractional_numerator < _Fractional_denominator ? _Fractional_shift + 1 : _Fractional_shift;

    [[maybe_unused]] const bool _Shift_success2 =
        _Shift_left(_Fractional_numerator, _Remaining_bits_of_precision_required); // assumes no overflow
    assert(_Shift_success2);

    uint64_t _Fractional_mantissa = _Divide(_Fractional_numerator, _Fractional_denominator);

    _Has_zero_tail = _Has_zero_tail && _Fractional_numerator._Myused == 0;

    // We may have produced more bits of precision than were required. Check, and remove any "extra" bits:
    const uint32_t _Fractional_mantissa_bits = _Bit_scan_reverse(_Fractional_mantissa);
    if (_Fractional_mantissa_bits > _Required_fractional_bits_of_precision) {
        const uint32_t _Shift = _Fractional_mantissa_bits - _Required_fractional_bits_of_precision;
        _Has_zero_tail        = _Has_zero_tail && (_Fractional_mantissa & ((1ULL << _Shift) - 1)) == 0;
        _Fractional_mantissa >>= _Shift;
    }

    // Compose the mantissa from the integer and fractional parts:
    const uint32_t _Integer_mantissa_low  = _Integer_value._Myused > 0 ? _Integer_value._Mydata[0] : 0;
    const uint32_t _Integer_mantissa_high = _Integer_value._Myused > 1 ? _Integer_value._Mydata[1] : 0;
    const uint64_t _Integer_mantissa = _Integer_mantissa_low + (static_cast<uint64_t>(_Integer_mantissa_high) << 32);

    const uint64_t _Complete_mantissa =
        (_Integer_mantissa << _Required_fractional_bits_of_precision) + _Fractional_mantissa;

    // Compute the final exponent:
    // * If the mantissa had an integer part, then the exponent is one less than the number of bits we obtained
    // from the integer part. (It's one less because we are converting to the form 1.11111,
    // with one 1 to the left of the decimal point.)
    // * If the mantissa had no integer part, then the exponent is the fractional exponent that we computed.
    // Then, in both cases, we subtract an additional one from the exponent,
    // to account for the fact that we've generated an extra bit of precision, for use in rounding.
    const int32_t _Final_exponent = _Integer_bits_of_precision > 0
                                      ? static_cast<int32_t>(_Integer_bits_of_precision - 2)
                                      : -static_cast<int32_t>(_Fractional_exponent) - 1;

    return _Assemble_floating_point_value(
        _Complete_mantissa, _Final_exponent, _Data._Myis_negative, _Has_zero_tail, _Result);
}

template <class _FloatingType>
[[nodiscard]] std::errc _Convert_hexadecimal_string_to_floating_type(
    const _Floating_point_string& _Data, _FloatingType& _Result, bool _Has_zero_tail) noexcept {
    using _Traits = _Floating_type_traits<_FloatingType>;

    uint64_t _Mantissa = 0;
    int32_t _Exponent  = _Data._Myexponent + _Traits::_Mantissa_bits - 1;

    // Accumulate bits into the mantissa buffer
    const uint8_t* const _Mantissa_last = _Data._Mymantissa + _Data._Mymantissa_count;
    const uint8_t* _Mantissa_it         = _Data._Mymantissa;
    while (_Mantissa_it != _Mantissa_last && _Mantissa <= _Traits::_Normal_mantissa_mask) {
        _Mantissa *= 16;
        _Mantissa += *_Mantissa_it++;
        _Exponent -= 4; // The exponent is in binary; log2(16) == 4
    }

    while (_Has_zero_tail && _Mantissa_it != _Mantissa_last) {
        _Has_zero_tail = *_Mantissa_it++ == 0;
    }

    return _Assemble_floating_point_value(_Mantissa, _Exponent, _Data._Myis_negative, _Has_zero_tail, _Result);
}

// ^^^^^^^^^^ DERIVED FROM corecrt_internal_strtox.h ^^^^^^^^^^


// FUNCTION from_chars (STRING TO FLOATING-POINT)

// C11 6.4.2.1 "General"
// digit: one of
//     0 1 2 3 4 5 6 7 8 9

// C11 6.4.4.1 "Integer constants"
// hexadecimal-digit: one of
//     0 1 2 3 4 5 6 7 8 9 a b c d e f A B C D E F

// C11 6.4.4.2 "Floating constants" (without floating-suffix, hexadecimal-prefix)
// amended by C11 7.22.1.3 "The strtod, strtof, and strtold functions" making exponents optional
// LWG-3080: "the sign '+' may only appear in the exponent part"

// digit-sequence:
//     digit
//     digit-sequence digit

// hexadecimal-digit-sequence:
//     hexadecimal-digit
//     hexadecimal-digit-sequence hexadecimal-digit

// sign: one of
//     + -

// decimal-floating-constant:
//     fractional-constant exponent-part[opt]
//     digit-sequence exponent-part[opt]

// fractional-constant:
//     digit-sequence[opt] . digit-sequence
//     digit-sequence .

// exponent-part:
//     e sign[opt] digit-sequence
//     E sign[opt] digit-sequence

// hexadecimal-floating-constant:
//     hexadecimal-fractional-constant binary-exponent-part[opt]
//     hexadecimal-digit-sequence binary-exponent-part[opt]

// hexadecimal-fractional-constant:
//     hexadecimal-digit-sequence[opt] . hexadecimal-digit-sequence
//     hexadecimal-digit-sequence .

// binary-exponent-part:
//     p sign[opt] digit-sequence
//     P sign[opt] digit-sequence

template <class _Floating>
[[nodiscard]] from_chars_result _Ordinary_floating_from_chars(const char* const _First, const char* const _Last,
    _Floating& _Value, const chars_format _Fmt, const bool _Minus_sign, const char* _Next) noexcept {
    // vvvvvvvvvv DERIVED FROM corecrt_internal_strtox.h WITH SIGNIFICANT MODIFICATIONS vvvvvvvvvv

    const bool _Is_hexadecimal = _Fmt == chars_format::hex;
    const int _Base{_Is_hexadecimal ? 16 : 10};

    // PERFORMANCE NOTE: _Fp_string is intentionally left uninitialized. Zero-initialization is quite expensive
    // and is unnecessary. The benefit of not zero-initializing is greatest for short inputs.
    _Floating_point_string _Fp_string;

    // Record the optional minus sign:
    _Fp_string._Myis_negative = _Minus_sign;

    uint8_t* const _Mantissa_first = _Fp_string._Mymantissa;
    uint8_t* const _Mantissa_last  = std:: end(_Fp_string._Mymantissa);
    uint8_t* _Mantissa_it          = _Mantissa_first;

    // [_Whole_begin, _Whole_end) will contain 0 or more digits/hexits
    const char* const _Whole_begin = _Next;

    // Skip past any leading zeroes in the mantissa:
    for (; _Next != _Last && *_Next == '0'; ++_Next) {
    }
    const char* const _Leading_zero_end = _Next;

    // Scan the integer part of the mantissa:
    for (; _Next != _Last; ++_Next) {
        const unsigned char _Digit_value = _Digit_from_char(*_Next);

        if (_Digit_value >= _Base) {
            break;
        }

        if (_Mantissa_it != _Mantissa_last) {
            *_Mantissa_it++ = _Digit_value;
        }
    }
    const char* const _Whole_end = _Next;

    // Defend against _Exponent_adjustment integer overflow. (These values don't need to be strict.)
    constexpr ptrdiff_t _Maximum_adjustment = 1'000'000;
    constexpr ptrdiff_t _Minimum_adjustment = -1'000'000;

    // The exponent adjustment holds the number of digits in the mantissa buffer that appeared before the radix point.
    // It can be negative, and leading zeroes in the integer part are ignored. Examples:
    // For "03333.111", it is 4.
    // For "00000.111", it is 0.
    // For "00000.001", it is -2.
    int _Exponent_adjustment = static_cast<int>((std:: min)(_Whole_end - _Leading_zero_end, _Maximum_adjustment));

    // [_Whole_end, _Dot_end) will contain 0 or 1 '.' characters
    if (_Next != _Last && *_Next == '.') {
        ++_Next;
    }
    const char* const _Dot_end = _Next;

    // [_Dot_end, _Frac_end) will contain 0 or more digits/hexits

    // If we haven't yet scanned any nonzero digits, continue skipping over zeroes,
    // updating the exponent adjustment to account for the zeroes we are skipping:
    if (_Exponent_adjustment == 0) {
        for (; _Next != _Last && *_Next == '0'; ++_Next) {
        }

        _Exponent_adjustment = static_cast<int>((std:: max)(_Dot_end - _Next, _Minimum_adjustment));
    }

    // Scan the fractional part of the mantissa:
    bool _Has_zero_tail = true;

    for (; _Next != _Last; ++_Next) {
        const unsigned char _Digit_value = _Digit_from_char(*_Next);

        if (_Digit_value >= _Base) {
            break;
        }

        if (_Mantissa_it != _Mantissa_last) {
            *_Mantissa_it++ = _Digit_value;
        } else {
            _Has_zero_tail = _Has_zero_tail && _Digit_value == 0;
        }
    }
    const char* const _Frac_end = _Next;

    // We must have at least 1 digit/hexit
    if (_Whole_begin == _Whole_end && _Dot_end == _Frac_end) {
        return {_First, std::errc::invalid_argument};
    }

    const char _Exponent_prefix{_Is_hexadecimal ? 'p' : 'e'};

    bool _Exponent_is_negative = false;
    int _Exponent              = 0;

    constexpr int _Maximum_temporary_decimal_exponent = 5200;
    constexpr int _Minimum_temporary_decimal_exponent = -5200;

    if (_Fmt != chars_format::fixed // N4713 23.20.3 [charconv.from.chars]/7.3
                                    // "if fmt has chars_format::fixed set but not chars_format::scientific,
                                    // the optional exponent part shall not appear"
        && _Next != _Last && (static_cast<unsigned char>(*_Next) | 0x20) == _Exponent_prefix) { // found exponent prefix
        const char* _Unread = _Next + 1;

        if (_Unread != _Last && (*_Unread == '+' || *_Unread == '-')) { // found optional sign
            _Exponent_is_negative = *_Unread == '-';
            ++_Unread;
        }

        while (_Unread != _Last) {
            const unsigned char _Digit_value = _Digit_from_char(*_Unread);

            if (_Digit_value >= 10) {
                break;
            }

            // found decimal digit

            if (_Exponent <= _Maximum_temporary_decimal_exponent) {
                _Exponent = _Exponent * 10 + _Digit_value;
            }

            ++_Unread;
            _Next = _Unread; // consume exponent-part/binary-exponent-part
        }

        if (_Exponent_is_negative) {
            _Exponent = -_Exponent;
        }
    }

    // [_Frac_end, _Exponent_end) will either be empty or contain "[EPep] sign[opt] digit-sequence"
    const char* const _Exponent_end = _Next;

    if (_Fmt == chars_format::scientific
        && _Frac_end == _Exponent_end) { // N4713 23.20.3 [charconv.from.chars]/7.2
                                         // "if fmt has chars_format::scientific set but not chars_format::fixed,
                                         // the otherwise optional exponent part shall appear"
        return {_First, std::errc::invalid_argument};
    }

    // Remove trailing zeroes from mantissa:
    while (_Mantissa_it != _Mantissa_first && *(_Mantissa_it - 1) == 0) {
        --_Mantissa_it;
    }

    // If the mantissa buffer is empty, the mantissa was composed of all zeroes (so the mantissa is 0).
    // All such strings have the value zero, regardless of what the exponent is (because 0 * b^n == 0 for all b and n).
    // We can return now. Note that we defer this check until after we scan the exponent, so that we can correctly
    // update _Next to point past the end of the exponent.
    if (_Mantissa_it == _Mantissa_first) {
        assert(_Has_zero_tail);
        _Assemble_floating_point_zero(_Fp_string._Myis_negative, _Value);
        return {_Next, std::errc{}};
    }

    // Before we adjust the exponent, handle the case where we detected a wildly
    // out of range exponent during parsing and clamped the value:
    if (_Exponent > _Maximum_temporary_decimal_exponent) {
        _Assemble_floating_point_infinity(_Fp_string._Myis_negative, _Value);
        return {_Next, std::errc::result_out_of_range}; // Overflow example: "1e+9999"
    }

    if (_Exponent < _Minimum_temporary_decimal_exponent) {
        _Assemble_floating_point_zero(_Fp_string._Myis_negative, _Value);
        return {_Next, std::errc::result_out_of_range}; // Underflow example: "1e-9999"
    }

    // In hexadecimal floating constants, the exponent is a base 2 exponent. The exponent adjustment computed during
    // parsing has the same base as the mantissa (so, 16 for hexadecimal floating constants).
    // We therefore need to scale the base 16 multiplier to base 2 by multiplying by log2(16):
    const int _Exponent_adjustment_multiplier{_Is_hexadecimal ? 4 : 1};

    _Exponent += _Exponent_adjustment * _Exponent_adjustment_multiplier;

    // Verify that after adjustment the exponent isn't wildly out of range (if it is, it isn't representable
    // in any supported floating-point format).
    if (_Exponent > _Maximum_temporary_decimal_exponent) {
        _Assemble_floating_point_infinity(_Fp_string._Myis_negative, _Value);
        return {_Next, std::errc::result_out_of_range}; // Overflow example: "10e+5199"
    }

    if (_Exponent < _Minimum_temporary_decimal_exponent) {
        _Assemble_floating_point_zero(_Fp_string._Myis_negative, _Value);
        return {_Next, std::errc::result_out_of_range}; // Underflow example: "0.001e-5199"
    }

    _Fp_string._Myexponent       = _Exponent;
    _Fp_string._Mymantissa_count = static_cast<uint32_t>(_Mantissa_it - _Mantissa_first);

    if (_Is_hexadecimal) {
        const std::errc _Ec = _Convert_hexadecimal_string_to_floating_type(_Fp_string, _Value, _Has_zero_tail);
        return {_Next, _Ec};
    } else {
        const std::errc _Ec = _Convert_decimal_string_to_floating_type(_Fp_string, _Value, _Has_zero_tail);
        return {_Next, _Ec};
    }

    // ^^^^^^^^^^ DERIVED FROM corecrt_internal_strtox.h WITH SIGNIFICANT MODIFICATIONS ^^^^^^^^^^
}

[[nodiscard]] inline bool _Starts_with_case_insensitive(
    const char* _First, const char* const _Last, const char* _Lowercase) noexcept {
    // pre: _Lowercase contains only ['a', 'z'] and is null-terminated
    for (; _First != _Last && *_Lowercase != '\0'; ++_First, ++_Lowercase) {
        if ((static_cast<unsigned char>(*_First) | 0x20) != *_Lowercase) {
            return false;
        }
    }

    return *_Lowercase == '\0';
}

template <class _Floating>
[[nodiscard]] from_chars_result _Infinity_from_chars(const char* const _First, const char* const _Last, _Floating& _Value,
    const bool _Minus_sign, const char* _Next) noexcept {
    // pre: _Next points at 'i' (case-insensitively)
    if (!_Starts_with_case_insensitive(_Next + 1, _Last, "nf")) { // definitely invalid
        return {_First, std::errc::invalid_argument};
    }

    // definitely inf
    _Next += 3;

    if (_Starts_with_case_insensitive(_Next, _Last, "inity")) { // definitely infinity
        _Next += 5;
    }

    _Assemble_floating_point_infinity(_Minus_sign, _Value);

    return {_Next, std::errc{}};
}

template <class _Floating>
[[nodiscard]] from_chars_result _Nan_from_chars(const char* const _First, const char* const _Last, _Floating& _Value,
    bool _Minus_sign, const char* _Next) noexcept {
    // pre: _Next points at 'n' (case-insensitively)
    if (!_Starts_with_case_insensitive(_Next + 1, _Last, "an")) { // definitely invalid
        return {_First, std::errc::invalid_argument};
    }

    // definitely nan
    _Next += 3;

    bool _Quiet = true;

    if (_Next != _Last && *_Next == '(') { // possibly nan(n-char-sequence[opt])
        const char* const _Seq_begin = _Next + 1;

        for (const char* _Temp = _Seq_begin; _Temp != _Last; ++_Temp) {
            if (*_Temp == ')') { // definitely nan(n-char-sequence[opt])
                _Next = _Temp + 1;

                if (_Temp - _Seq_begin == 3
                    && _Starts_with_case_insensitive(_Seq_begin, _Temp, "ind")) { // definitely nan(ind)
                    // The UCRT considers indeterminate NaN to be negative quiet NaN with no payload bits set.
                    // It parses "nan(ind)" and "-nan(ind)" identically.
                    _Minus_sign = true;
                } else if (_Temp - _Seq_begin == 4
                           && _Starts_with_case_insensitive(_Seq_begin, _Temp, "snan")) { // definitely nan(snan)
                    _Quiet = false;
                }

                break;
            } else if (*_Temp == '_' || ('0' <= *_Temp && *_Temp <= '9') || ('A' <= *_Temp && *_Temp <= 'Z')
                       || ('a' <= *_Temp && *_Temp <= 'z')) { // possibly nan(n-char-sequence[opt]), keep going
            } else { // definitely nan, not nan(n-char-sequence[opt])
                break;
            }
        }
    }

    // Intentional behavior difference between the UCRT and the STL:
    // strtod()/strtof() parse plain "nan" as being a quiet NaN with all payload bits set.
    // numeric_limits::quiet_NaN() returns a quiet NaN with no payload bits set.
    // This implementation of from_chars() has chosen to be consistent with numeric_limits.

    using _Traits    = _Floating_type_traits<_Floating>;
    using _Uint_type = typename _Traits::_Uint_type;

    _Uint_type _Uint_value = _Traits::_Shifted_exponent_mask;

    if (_Minus_sign) {
        _Uint_value |= _Traits::_Shifted_sign_mask;
    }

    if (_Quiet) {
        _Uint_value |= _Traits::_Special_nan_mantissa_mask;
    } else {
        _Uint_value |= 1;
    }

    _Value = bit_cast<_Floating>(_Uint_value);

    return {_Next, std::errc{}};
}

template <class _Floating>
[[nodiscard]] from_chars_result _Floating_from_chars(
    const char* const _First, const char* const _Last, _Floating& _Value, const chars_format _Fmt) noexcept {
    MSCHARCONV_VERIFY_RANGE(_First, _Last);

    MSCHARCONV_ASSERT_MSG(_Fmt == chars_format::general || _Fmt == chars_format::scientific || _Fmt == chars_format::fixed
                    || _Fmt == chars_format::hex,
        "invalid format in from_chars()");

    bool _Minus_sign = false;

    const char* _Next = _First;

    if (_Next == _Last) {
        return {_First, std::errc::invalid_argument};
    }

    if (*_Next == '-') {
        _Minus_sign = true;
        ++_Next;

        if (_Next == _Last) {
            return {_First, std::errc::invalid_argument};
        }
    }

    // Distinguish ordinary numbers versus inf/nan with a single test.
    // ordinary numbers start with ['.'] ['0', '9'] ['A', 'F'] ['a', 'f']
    // inf/nan start with ['I'] ['N'] ['i'] ['n']
    // All other starting characters are invalid.
    // Setting the 0x20 bit folds these ranges in a useful manner.
    // ordinary (and some invalid) starting characters are folded to ['.'] ['0', '9'] ['a', 'f']
    // inf/nan starting characters are folded to ['i'] ['n']
    // These are ordered: ['.'] ['0', '9'] ['a', 'f'] < ['i'] ['n']
    // Note that invalid starting characters end up on both sides of this test.
    const unsigned char _Folded_start = static_cast<unsigned char>(static_cast<unsigned char>(*_Next) | 0x20);

    if (_Folded_start <= 'f') { // possibly an ordinary number
        return _Ordinary_floating_from_chars(_First, _Last, _Value, _Fmt, _Minus_sign, _Next);
    } else if (_Folded_start == 'i') { // possibly inf
        return _Infinity_from_chars(_First, _Last, _Value, _Minus_sign, _Next);
    } else if (_Folded_start == 'n') { // possibly nan
        return _Nan_from_chars(_First, _Last, _Value, _Minus_sign, _Next);
    } else { // definitely invalid
        return {_First, std::errc::invalid_argument};
    }
}

inline from_chars_result from_chars(const char* const _First, const char* const _Last, float& _Value,
    const chars_format _Fmt = chars_format::general) noexcept /* strengthened */ {
    return _Floating_from_chars(_First, _Last, _Value, _Fmt);
}
inline from_chars_result from_chars(const char* const _First, const char* const _Last, double& _Value,
    const chars_format _Fmt = chars_format::general) noexcept /* strengthened */ {
    return _Floating_from_chars(_First, _Last, _Value, _Fmt);
}
inline from_chars_result from_chars(const char* const _First, const char* const _Last, long double& _Value,
    const chars_format _Fmt = chars_format::general) noexcept /* strengthened */ {
    double _Dbl; // intentionally default-init
    const from_chars_result _Result = _Floating_from_chars(_First, _Last, _Dbl, _Fmt);

    if (_Result.ec == std::errc{}) {
        _Value = _Dbl;
    }

    return _Result;
}

// FUNCTION to_chars (FLOATING-POINT TO STRING)
template <class _Floating>
[[nodiscard]] to_chars_result _Floating_to_chars_hex_precision(
    char* _First, char* const _Last, const _Floating _Value, int _Precision) noexcept {

    // * Determine the effective _Precision.
    // * Later, we'll decrement _Precision when printing each hexit after the decimal point.

    // The hexits after the decimal point correspond to the explicitly stored fraction bits.
    // float explicitly stores 23 fraction bits. 23 / 4 == 5.75, which is 6 hexits.
    // double explicitly stores 52 fraction bits. 52 / 4 == 13, which is 13 hexits.
    constexpr int _Full_precision         = std::is_same_v<_Floating, float> ? 6 : 13;
    constexpr int _Adjusted_explicit_bits = _Full_precision * 4;

    if (_Precision < 0) {
        // C11 7.21.6.1 "The fprintf function"/5: "A negative precision argument is taken as if the precision were
        // omitted." /8: "if the precision is missing and FLT_RADIX is a power of 2, then the precision is sufficient
        // for an exact representation of the value"
        _Precision = _Full_precision;
    }

    // * Extract the _Ieee_mantissa and _Ieee_exponent.
    using _Traits    = _Floating_type_traits<_Floating>;
    using _Uint_type = typename _Traits::_Uint_type;

    const _Uint_type _Uint_value    = bit_cast<_Uint_type>(_Value);
    const _Uint_type _Ieee_mantissa = _Uint_value & _Traits::_Denormal_mantissa_mask;
    const int32_t _Ieee_exponent    = static_cast<int32_t>(_Uint_value >> _Traits::_Exponent_shift);

    // * Prepare the _Adjusted_mantissa. This is aligned to hexit boundaries,
    // * with the implicit bit restored (0 for zero values and subnormal values, 1 for normal values).
    // * Also calculate the _Unbiased_exponent. This unifies the processing of zero, subnormal, and normal values.
    _Uint_type _Adjusted_mantissa;

    if constexpr (std::is_same_v<_Floating, float>) {
        _Adjusted_mantissa = _Ieee_mantissa << 1; // align to hexit boundary (23 isn't divisible by 4)
    } else {
        _Adjusted_mantissa = _Ieee_mantissa; // already aligned (52 is divisible by 4)
    }

    int32_t _Unbiased_exponent;

    if (_Ieee_exponent == 0) { // zero or subnormal
        // implicit bit is 0

        if (_Ieee_mantissa == 0) { // zero
            // C11 7.21.6.1 "The fprintf function"/8: "If the value is zero, the exponent is zero."
            _Unbiased_exponent = 0;
        } else { // subnormal
            _Unbiased_exponent = 1 - _Traits::_Exponent_bias;
        }
    } else { // normal
        _Adjusted_mantissa |= _Uint_type{1} << _Adjusted_explicit_bits; // implicit bit is 1
        _Unbiased_exponent = _Ieee_exponent - _Traits::_Exponent_bias;
    }

    // _Unbiased_exponent is within [-126, 127] for float, [-1022, 1023] for double.

    // * Decompose _Unbiased_exponent into _Sign_character and _Absolute_exponent.
    char _Sign_character;
    uint32_t _Absolute_exponent;

    if (_Unbiased_exponent < 0) {
        _Sign_character    = '-';
        _Absolute_exponent = static_cast<uint32_t>(-_Unbiased_exponent);
    } else {
        _Sign_character    = '+';
        _Absolute_exponent = static_cast<uint32_t>(_Unbiased_exponent);
    }

    // _Absolute_exponent is within [0, 127] for float, [0, 1023] for double.

    // * Perform a single bounds check.
    {
        int32_t _Exponent_length;

        if (_Absolute_exponent < 10) {
            _Exponent_length = 1;
        } else if (_Absolute_exponent < 100) {
            _Exponent_length = 2;
        } else if constexpr (std::is_same_v<_Floating, float>) {
            _Exponent_length = 3;
        } else if (_Absolute_exponent < 1000) {
            _Exponent_length = 3;
        } else {
            _Exponent_length = 4;
        }

        // _Precision might be enormous; avoid integer overflow by testing it separately.
        ptrdiff_t _Buffer_size = _Last - _First;

        if (_Buffer_size < _Precision) {
            return {_Last, std::errc::value_too_large};
        }

        _Buffer_size -= _Precision;

        const int32_t _Length_excluding_precision = 1 // leading hexit
                                                  + static_cast<int32_t>(_Precision > 0) // possible decimal point
                                                  // excluding `+ _Precision`, hexits after decimal point
                                                  + 2 // "p+" or "p-"
                                                  + _Exponent_length; // exponent

        if (_Buffer_size < _Length_excluding_precision) {
            return {_Last, std::errc::value_too_large};
        }
    }

    // * Perform rounding when we've been asked to omit hexits.
    if (_Precision < _Full_precision) {
        // _Precision is within [0, 5] for float, [0, 12] for double.

        // _Dropped_bits is within [4, 24] for float, [4, 52] for double.
        const int _Dropped_bits = (_Full_precision - _Precision) * 4;

        // Perform rounding by adding an appropriately-shifted bit.

        // This can propagate carries all the way into the leading hexit. Examples:
        // "0.ff9" rounded to a precision of 2 is "1.00".
        // "1.ff9" rounded to a precision of 2 is "2.00".

        // Note that the leading hexit participates in the rounding decision. Examples:
        // "0.8" rounded to a precision of 0 is "0".
        // "1.8" rounded to a precision of 0 is "2".

        // Reference implementation with suboptimal codegen:
        // const bool _Lsb_bit       = (_Adjusted_mantissa & (_Uint_type{1} << _Dropped_bits)) != 0;
        // const bool _Round_bit     = (_Adjusted_mantissa & (_Uint_type{1} << (_Dropped_bits - 1))) != 0;
        // const bool _Has_tail_bits = (_Adjusted_mantissa & ((_Uint_type{1} << (_Dropped_bits - 1)) - 1)) != 0;
        // const bool _Should_round = _Should_round_up(_Lsb_bit, _Round_bit, _Has_tail_bits);
        // _Adjusted_mantissa += _Uint_type{_Should_round} << _Dropped_bits;

        // Example for optimized implementation: Let _Dropped_bits be 8.
        //          Bit index: ...[8]76543210
        // _Adjusted_mantissa: ...[L]RTTTTTTT (not depicting known details, like hexit alignment)
        // By focusing on the bit at index _Dropped_bits, we can avoid unnecessary branching and shifting.

        // Bit index: ...[8]76543210
        //  _Lsb_bit: ...[L]RTTTTTTT
        const _Uint_type _Lsb_bit = _Adjusted_mantissa;

        //  Bit index: ...9[8]76543210
        // _Round_bit: ...L[R]TTTTTTT0
        const _Uint_type _Round_bit = _Adjusted_mantissa << 1;

        // We can detect (without branching) whether any of the trailing bits are set.
        // Due to _Should_round below, this computation will be used if and only if R is 1, so we can assume that here.
        //      Bit index: ...9[8]76543210
        //     _Round_bit: ...L[1]TTTTTTT0
        // _Has_tail_bits: ....[H]........

        // If all of the trailing bits T are 0, then `_Round_bit - 1` will produce 0 for H (due to R being 1).
        // If any of the trailing bits T are 1, then `_Round_bit - 1` will produce 1 for H (due to R being 1).
        const _Uint_type _Has_tail_bits = _Round_bit - 1;

        // Finally, we can use _Should_round_up() logic with bitwise-AND and bitwise-OR,
        // selecting just the bit at index _Dropped_bits. This is the appropriately-shifted bit that we want.
        const _Uint_type _Should_round = _Round_bit & (_Has_tail_bits | _Lsb_bit) & (_Uint_type{1} << _Dropped_bits);

        // This rounding technique is dedicated to the memory of Peppermint. =^..^=
        _Adjusted_mantissa += _Should_round;
    }

    // * Print the leading hexit, then mask it away.
    {
        const uint32_t _Nibble = static_cast<uint32_t>(_Adjusted_mantissa >> _Adjusted_explicit_bits);
        assert(_Nibble < 3);
        const char _Leading_hexit = static_cast<char>('0' + _Nibble);

        *_First++ = _Leading_hexit;

        constexpr _Uint_type _Mask = (_Uint_type{1} << _Adjusted_explicit_bits) - 1;
        _Adjusted_mantissa &= _Mask;
    }

    // * Print the decimal point and trailing hexits.

    // C11 7.21.6.1 "The fprintf function"/8:
    // "if the precision is zero and the # flag is not specified, no decimal-point character appears."
    if (_Precision > 0) {
        *_First++ = '.';

        int32_t _Number_of_bits_remaining = _Adjusted_explicit_bits; // 24 for float, 52 for double

        for (;;) {
            assert(_Number_of_bits_remaining >= 4);
            assert(_Number_of_bits_remaining % 4 == 0);
            _Number_of_bits_remaining -= 4;

            const uint32_t _Nibble = static_cast<uint32_t>(_Adjusted_mantissa >> _Number_of_bits_remaining);
            assert(_Nibble < 16);
            const char _Hexit = _Charconv_digits[_Nibble];

            *_First++ = _Hexit;

            // _Precision is the number of hexits that still need to be printed.
            --_Precision;
            if (_Precision == 0) {
                break; // We're completely done with this phase.
            }
            // Otherwise, we need to keep printing hexits.

            if (_Number_of_bits_remaining == 0) {
                // We've finished printing _Adjusted_mantissa, so all remaining hexits are '0'.
                std:: memset(_First, '0', static_cast<size_t>(_Precision));
                _First += _Precision;
                break;
            }

            // Mask away the hexit that we just printed, then keep looping.
            // (We skip this when breaking out of the loop above, because _Adjusted_mantissa isn't used later.)
            const _Uint_type _Mask = (_Uint_type{1} << _Number_of_bits_remaining) - 1;
            _Adjusted_mantissa &= _Mask;
        }
    }

    // * Print the exponent.

    // C11 7.21.6.1 "The fprintf function"/8: "The exponent always contains at least one digit, and only as many more
    // digits as necessary to represent the decimal exponent of 2."

    // Performance note: We should take advantage of the known ranges of possible exponents.

    *_First++ = 'p';
    *_First++ = _Sign_character;

    // We've already printed '-' if necessary, so uint32_t _Absolute_exponent avoids testing that again.
    return to_chars(_First, _Last, _Absolute_exponent);
}

template <class _Floating>
[[nodiscard]] to_chars_result _Floating_to_chars_hex_shortest(
    char* _First, char* const _Last, const _Floating _Value) noexcept {

    // This prints "1.728p+0" instead of "2.e5p-1".
    // This prints "0.000002p-126" instead of "1p-149" for float.
    // This prints "0.0000000000001p-1022" instead of "1p-1074" for double.
    // This prioritizes being consistent with printf's de facto behavior (and hex-precision's behavior)
    // over minimizing the number of characters printed.

    using _Traits    = _Floating_type_traits<_Floating>;
    using _Uint_type = typename _Traits::_Uint_type;

    const _Uint_type _Uint_value = bit_cast<_Uint_type>(_Value);

    if (_Uint_value == 0) { // zero detected; write "0p+0" and return
        // C11 7.21.6.1 "The fprintf function"/8: "If the value is zero, the exponent is zero."
        // Special-casing zero is necessary because of the exponent.
        const char* const _Str = "0p+0";
        const size_t _Len      = 4;

        if (_Last - _First < static_cast<ptrdiff_t>(_Len)) {
            return {_Last, std::errc::value_too_large};
        }

        std:: memcpy(_First, _Str, _Len);

        return {_First + _Len, std::errc{}};
    }

    const _Uint_type _Ieee_mantissa = _Uint_value & _Traits::_Denormal_mantissa_mask;
    const int32_t _Ieee_exponent    = static_cast<int32_t>(_Uint_value >> _Traits::_Exponent_shift);

    char _Leading_hexit; // implicit bit
    int32_t _Unbiased_exponent;

    if (_Ieee_exponent == 0) { // subnormal
        _Leading_hexit     = '0';
        _Unbiased_exponent = 1 - _Traits::_Exponent_bias;
    } else { // normal
        _Leading_hexit     = '1';
        _Unbiased_exponent = _Ieee_exponent - _Traits::_Exponent_bias;
    }

    // Performance note: Consider avoiding per-character bounds checking when there's plenty of space.

    if (_First == _Last) {
        return {_Last, std::errc::value_too_large};
    }

    *_First++ = _Leading_hexit;

    if (_Ieee_mantissa == 0) {
        // The fraction bits are all 0. Trim them away, including the decimal point.
    } else {
        if (_First == _Last) {
            return {_Last, std::errc::value_too_large};
        }

        *_First++ = '.';

        // The hexits after the decimal point correspond to the explicitly stored fraction bits.
        // float explicitly stores 23 fraction bits. 23 / 4 == 5.75, so we'll print at most 6 hexits.
        // double explicitly stores 52 fraction bits. 52 / 4 == 13, so we'll print at most 13 hexits.
        _Uint_type _Adjusted_mantissa;
        int32_t _Number_of_bits_remaining;

        if constexpr (std::is_same_v<_Floating, float>) {
            _Adjusted_mantissa        = _Ieee_mantissa << 1; // align to hexit boundary (23 isn't divisible by 4)
            _Number_of_bits_remaining = 24; // 23 fraction bits + 1 alignment bit
        } else {
            _Adjusted_mantissa        = _Ieee_mantissa; // already aligned (52 is divisible by 4)
            _Number_of_bits_remaining = 52; // 52 fraction bits
        }

        // do-while: The condition _Adjusted_mantissa != 0 is initially true - we have nonzero fraction bits and we've
        // printed the decimal point. Each iteration, we print a hexit, mask it away, and keep looping if we still have
        // nonzero fraction bits. If there would be trailing '0' hexits, this trims them. If there wouldn't be trailing
        // '0' hexits, the same condition works (as we print the final hexit and mask it away); we don't need to test
        // _Number_of_bits_remaining.
        do {
            assert(_Number_of_bits_remaining >= 4);
            assert(_Number_of_bits_remaining % 4 == 0);
            _Number_of_bits_remaining -= 4;

            const uint32_t _Nibble = static_cast<uint32_t>(_Adjusted_mantissa >> _Number_of_bits_remaining);
            assert(_Nibble < 16);
            const char _Hexit = _Charconv_digits[_Nibble];

            if (_First == _Last) {
                return {_Last, std::errc::value_too_large};
            }

            *_First++ = _Hexit;

            const _Uint_type _Mask = (_Uint_type{1} << _Number_of_bits_remaining) - 1;
            _Adjusted_mantissa &= _Mask;

        } while (_Adjusted_mantissa != 0);
    }

    // C11 7.21.6.1 "The fprintf function"/8: "The exponent always contains at least one digit, and only as many more
    // digits as necessary to represent the decimal exponent of 2."

    // Performance note: We should take advantage of the known ranges of possible exponents.

    // float: _Unbiased_exponent is within [-126, 127].
    // double: _Unbiased_exponent is within [-1022, 1023].

    if (_Last - _First < 2) {
        return {_Last, std::errc::value_too_large};
    }

    *_First++ = 'p';

    if (_Unbiased_exponent < 0) {
        *_First++          = '-';
        _Unbiased_exponent = -_Unbiased_exponent;
    } else {
        *_First++ = '+';
    }

    // We've already printed '-' if necessary, so static_cast<uint32_t> avoids testing that again.
    return to_chars(_First, _Last, static_cast<uint32_t>(_Unbiased_exponent));
}

// For general precision, we can use lookup tables to avoid performing trial formatting.

// For a simple example, imagine counting the number of digits D in an integer, and needing to know
// whether D is less than 3, equal to 3/4/5/6, or greater than 6. We could use a lookup table:
// D | Largest integer with D digits
// 2 |      99
// 3 |     999
// 4 |   9'999
// 5 |  99'999
// 6 | 999'999
// 7 | table end
// Looking up an integer in this table with lower_bound() will work:
// * Too-small integers, like 7, 70, and 99, will cause lower_bound() to return the D == 2 row. (If all we care
//   about is whether D is less than 3, then it's okay to smash the D == 1 and D == 2 cases together.)
// * Integers in [100, 999] will cause lower_bound() to return the D == 3 row, and so forth.
// * Too-large integers, like 1'000'000 and above, will cause lower_bound() to return the end of the table. If we
//   compute D from that index, this will be considered D == 7, which will activate any "greater than 6" logic.

// Floating-point is slightly more complicated.

// The ordinary lookup tables are for X within [-5, 38] for float, and [-5, 308] for double.
// (-5 absorbs too-negative exponents, outside the P > X >= -4 criterion. 38 and 308 are the maximum exponents.)
// Due to the P > X condition, we can use a subset of the table for X within [-5, P - 1], suitably clamped.

// When P is small, rounding can affect X. For example:
// For P == 1, the largest double with X == 0 is: 9.4999999999999982236431605997495353221893310546875
// For P == 2, the largest double with X == 0 is: 9.949999999999999289457264239899814128875732421875
// For P == 3, the largest double with X == 0 is: 9.9949999999999992184029906638897955417633056640625

// Exponent adjustment is a concern for P within [1, 7] for float, and [1, 15] for double (determined via
// brute force). While larger values of P still perform rounding, they can't trigger exponent adjustment.
// This is because only values with repeated '9' digits can undergo exponent adjustment during rounding,
// and floating-point granularity limits the number of consecutive '9' digits that can appear.

// So, we need special lookup tables for small values of P.
// These tables have varying lengths due to the P > X >= -4 criterion. For example:
// For P == 1, need table entries for X: -5, -4, -3, -2, -1, 0
// For P == 2, need table entries for X: -5, -4, -3, -2, -1, 0, 1
// For P == 3, need table entries for X: -5, -4, -3, -2, -1, 0, 1, 2
// For P == 4, need table entries for X: -5, -4, -3, -2, -1, 0, 1, 2, 3

// We can concatenate these tables for compact storage, using triangular numbers to access them.
// The table for P begins at index (P - 1) * (P + 10) / 2 with length P + 5.

// For both the ordinary and special lookup tables, after an index I is returned by lower_bound(), X is I - 5.

// We need to special-case the floating-point value 0.0, which is considered to have X == 0.
// Otherwise, the lookup tables would consider it to have a highly negative X.

// Finally, because we're working with positive floating-point values,
// representation comparisons behave identically to floating-point comparisons.

// The following code generated the lookup tables for the scientific exponent X. Don't remove this code.
#if 0
// cl /EHsc /nologo /W4 /MT /O2 /std:c++17 generate_tables.cpp && generate_tables

//~ #include <algorithm>
//~ #include <assert.h>
//~ #include <charconv>
//~ #include <cmath>
//~ #include <limits>
//~ #include <map>
//~ #include <stdint.h>
//~ #include <stdio.h>
//~ #include <system_error>
//~ #include <type_traits>
//~ #include <vector>
using namespace std;

template <typename UInt, typename Pred>
[[nodiscard]] UInt uint_partition_point(UInt first, const UInt last, Pred pred) {
    // Find the beginning of the false partition in [first, last).
    // [first, last) is partitioned when all of the true values occur before all of the false values.

    static_assert(is_unsigned_v<UInt>);
    assert(first <= last);

    for (UInt n = last - first; n > 0;) {
        const UInt n2  = n / 2;
        const UInt mid = first + n2;

        if (pred(mid)) {
            first = mid + 1;
            n     = n - n2 - 1;
        } else {
            n = n2;
        }
    }

    return first;
}

template <typename Floating>
[[nodiscard]] int scientific_exponent_X(const int P, const Floating flt) {
    char buf[400]; // more than enough

    // C11 7.21.6.1 "The fprintf function"/8 performs trial formatting with scientific precision P - 1.
    const auto to_result = to_chars(buf, end(buf), flt, chars_format::scientific, P - 1);
    assert(to_result.ec == std::errc{});

    const char* exp_ptr = find(buf, to_result.ptr, 'e');
    assert(exp_ptr != to_result.ptr);

    ++exp_ptr; // advance past 'e'

    if (*exp_ptr == '+') {
        ++exp_ptr; // advance past '+' which from_chars() won't parse
    }

    int X;
    const auto from_result = from_chars(exp_ptr, to_result.ptr, X);
    assert(from_result.ec == std::errc{});
    return X;
}

template <typename UInt>
void print_table(const vector<UInt>& v, const char* const name) {
    constexpr const char* UIntName = std::is_same_v<UInt, uint32_t> ? "uint32_t" : "uint64_t";

    printf("static constexpr %s %s[%zu] = {\n", UIntName, name, v.size());

    for (const auto& val : v) {
        if constexpr (std::is_same_v<UInt, uint32_t>) {
            printf("0x%08Xu,\n", val);
        } else {
            printf("0x%016llXu,\n", val);
        }
    }

    printf("};\n");
}

enum class Mode { Tables, Tests };

template <typename Floating>
void generate_tables(const Mode mode) {
    using Limits = numeric_limits<Floating>;
    using UInt   = std::conditional_t<std::is_same_v<Floating, float>, uint32_t, uint64_t>;

    map<int, map<int, UInt>> P_X_LargestValWithX;

    constexpr int MaxP = Limits::max_exponent10 + 1; // MaxP performs no rounding during trial formatting

    for (int P = 1; P <= MaxP; ++P) {
        for (int X = -5; X < P; ++X) {
            constexpr Floating first = static_cast<Floating>(9e-5); // well below 9.5e-5, otherwise arbitrary
            constexpr Floating last  = Limits::infinity(); // one bit above Limits::max()

            const UInt val_beyond_X = uint_partition_point(reinterpret_cast<const UInt&>(first),
                reinterpret_cast<const UInt&>(last),
                [P, X](const UInt u) { return scientific_exponent_X(P, reinterpret_cast<const Floating&>(u)) <= X; });

            P_X_LargestValWithX[P][X] = val_beyond_X - 1;
        }
    }

    constexpr const char* FloatingName = std::is_same_v<Floating, float> ? "float" : "double";

    constexpr int MaxSpecialP = std::is_same_v<Floating, float> ? 7 : 15; // MaxSpecialP is affected by exponent adjustment

    if (mode == Mode::Tables) {
        printf("template <>\n");
        printf("struct _General_precision_tables<%s> {\n", FloatingName);

        printf("static constexpr int _Max_special_P = %d;\n", MaxSpecialP);

        vector<UInt> special;

        for (int P = 1; P <= MaxSpecialP; ++P) {
            for (int X = -5; X < P; ++X) {
                const UInt val = P_X_LargestValWithX[P][X];
                special.push_back(val);
            }
        }

        print_table(special, "_Special_X_table");

        for (int P = MaxSpecialP + 1; P < MaxP; ++P) {
            for (int X = -5; X < P; ++X) {
                const UInt val = P_X_LargestValWithX[P][X];
                assert(val == P_X_LargestValWithX[MaxP][X]);
            }
        }

        printf("static constexpr int _Max_P = %d;\n", MaxP);

        vector<UInt> ordinary;

        for (int X = -5; X < MaxP; ++X) {
            const UInt val = P_X_LargestValWithX[MaxP][X];
            ordinary.push_back(val);
        }

        print_table(ordinary, "_Ordinary_X_table");

        printf("};\n");
    } else {
        printf("==========\n");
        printf("Test cases for %s:\n", FloatingName);

        constexpr int Hexits         = std::is_same_v<Floating, float> ? 6 : 13;
        constexpr const char* Suffix = std::is_same_v<Floating, float> ? "f" : "";

        for (int P = 1; P <= MaxP; ++P) {
            for (int X = -5; X < P; ++X) {
                if (P <= MaxSpecialP || P == 25 || P == MaxP || X == P - 1) {
                    const UInt val1   = P_X_LargestValWithX[P][X];
                    const Floating f1 = reinterpret_cast<const Floating&>(val1);
                    const UInt val2   = val1 + 1;
                    const Floating f2 = reinterpret_cast<const Floating&>(val2);

                    printf("{%.*a%s, chars_format::general, %d, \"%.*g\"},\n", Hexits, f1, Suffix, P, P, f1);
                    if (isfinite(f2)) {
                        printf("{%.*a%s, chars_format::general, %d, \"%.*g\"},\n", Hexits, f2, Suffix, P, P, f2);
                    }
                }
            }
        }
    }
}

int main() {
    printf("template <class _Floating>\n");
    printf("struct _General_precision_tables;\n");
    generate_tables<float>(Mode::Tables);
    generate_tables<double>(Mode::Tables);
    generate_tables<float>(Mode::Tests);
    generate_tables<double>(Mode::Tests);
}
#endif // 0

template <class _Floating>
struct _General_precision_tables;

template <>
struct _General_precision_tables<float> {
    static constexpr int _Max_special_P = 7;

    static constexpr uint32_t _Special_X_table[63] = {0x38C73ABCu, 0x3A79096Bu, 0x3C1BA5E3u, 0x3DC28F5Cu, 0x3F733333u,
        0x4117FFFFu, 0x38D0AAA7u, 0x3A826AA8u, 0x3C230553u, 0x3DCBC6A7u, 0x3F7EB851u, 0x411F3333u, 0x42C6FFFFu,
        0x38D19C3Fu, 0x3A8301A7u, 0x3C23C211u, 0x3DCCB295u, 0x3F7FDF3Bu, 0x411FEB85u, 0x42C7E666u, 0x4479DFFFu,
        0x38D1B468u, 0x3A8310C1u, 0x3C23D4F1u, 0x3DCCCA2Du, 0x3F7FFCB9u, 0x411FFDF3u, 0x42C7FD70u, 0x4479FCCCu,
        0x461C3DFFu, 0x38D1B6D2u, 0x3A831243u, 0x3C23D6D4u, 0x3DCCCC89u, 0x3F7FFFACu, 0x411FFFCBu, 0x42C7FFBEu,
        0x4479FFAEu, 0x461C3FCCu, 0x47C34FBFu, 0x38D1B710u, 0x3A83126Au, 0x3C23D704u, 0x3DCCCCC6u, 0x3F7FFFF7u,
        0x411FFFFAu, 0x42C7FFF9u, 0x4479FFF7u, 0x461C3FFAu, 0x47C34FF9u, 0x497423F7u, 0x38D1B716u, 0x3A83126Eu,
        0x3C23D709u, 0x3DCCCCCCu, 0x3F7FFFFFu, 0x411FFFFFu, 0x42C7FFFFu, 0x4479FFFFu, 0x461C3FFFu, 0x47C34FFFu,
        0x497423FFu, 0x4B18967Fu};

    static constexpr int _Max_P = 39;

    static constexpr uint32_t _Ordinary_X_table[44] = {0x38D1B717u, 0x3A83126Eu, 0x3C23D70Au, 0x3DCCCCCCu, 0x3F7FFFFFu,
        0x411FFFFFu, 0x42C7FFFFu, 0x4479FFFFu, 0x461C3FFFu, 0x47C34FFFu, 0x497423FFu, 0x4B18967Fu, 0x4CBEBC1Fu,
        0x4E6E6B27u, 0x501502F8u, 0x51BA43B7u, 0x5368D4A5u, 0x551184E7u, 0x56B5E620u, 0x58635FA9u, 0x5A0E1BC9u,
        0x5BB1A2BCu, 0x5D5E0B6Bu, 0x5F0AC723u, 0x60AD78EBu, 0x6258D726u, 0x64078678u, 0x65A96816u, 0x6753C21Bu,
        0x69045951u, 0x6AA56FA5u, 0x6C4ECB8Fu, 0x6E013F39u, 0x6FA18F07u, 0x7149F2C9u, 0x72FC6F7Cu, 0x749DC5ADu,
        0x76453719u, 0x77F684DFu, 0x799A130Bu, 0x7B4097CEu, 0x7CF0BDC2u, 0x7E967699u, 0x7F7FFFFFu};
};

template <>
struct _General_precision_tables<double> {
    static constexpr int _Max_special_P = 15;

    static constexpr uint64_t _Special_X_table[195] = {0x3F18E757928E0C9Du, 0x3F4F212D77318FC5u, 0x3F8374BC6A7EF9DBu,
        0x3FB851EB851EB851u, 0x3FEE666666666666u, 0x4022FFFFFFFFFFFFu, 0x3F1A1554FBDAD751u, 0x3F504D551D68C692u,
        0x3F8460AA64C2F837u, 0x3FB978D4FDF3B645u, 0x3FEFD70A3D70A3D7u, 0x4023E66666666666u, 0x4058DFFFFFFFFFFFu,
        0x3F1A3387ECC8EB96u, 0x3F506034F3FD933Eu, 0x3F84784230FCF80Du, 0x3FB99652BD3C3611u, 0x3FEFFBE76C8B4395u,
        0x4023FD70A3D70A3Du, 0x4058FCCCCCCCCCCCu, 0x408F3BFFFFFFFFFFu, 0x3F1A368D04E0BA6Au, 0x3F506218230C7482u,
        0x3F847A9E2BCF91A3u, 0x3FB99945B6C3760Bu, 0x3FEFFF972474538Eu, 0x4023FFBE76C8B439u, 0x4058FFAE147AE147u,
        0x408F3F9999999999u, 0x40C387BFFFFFFFFFu, 0x3F1A36DA54164F19u, 0x3F506248748DF16Fu, 0x3F847ADA91B16DCBu,
        0x3FB99991361DC93Eu, 0x3FEFFFF583A53B8Eu, 0x4023FFF972474538u, 0x4058FFF7CED91687u, 0x408F3FF5C28F5C28u,
        0x40C387F999999999u, 0x40F869F7FFFFFFFFu, 0x3F1A36E20F35445Du, 0x3F50624D49814ABAu, 0x3F847AE09BE19D69u,
        0x3FB99998C2DA04C3u, 0x3FEFFFFEF39085F4u, 0x4023FFFF583A53B8u, 0x4058FFFF2E48E8A7u, 0x408F3FFEF9DB22D0u,
        0x40C387FF5C28F5C2u, 0x40F869FF33333333u, 0x412E847EFFFFFFFFu, 0x3F1A36E2D51EC34Bu, 0x3F50624DC5333A0Eu,
        0x3F847AE136800892u, 0x3FB9999984200AB7u, 0x3FEFFFFFE5280D65u, 0x4023FFFFEF39085Fu, 0x4058FFFFEB074A77u,
        0x408F3FFFE5C91D14u, 0x40C387FFEF9DB22Du, 0x40F869FFEB851EB8u, 0x412E847FE6666666u, 0x416312CFEFFFFFFFu,
        0x3F1A36E2E8E94FFCu, 0x3F50624DD191D1FDu, 0x3F847AE145F6467Du, 0x3FB999999773D81Cu, 0x3FEFFFFFFD50CE23u,
        0x4023FFFFFE5280D6u, 0x4058FFFFFDE7210Bu, 0x408F3FFFFD60E94Eu, 0x40C387FFFE5C91D1u, 0x40F869FFFDF3B645u,
        0x412E847FFD70A3D7u, 0x416312CFFE666666u, 0x4197D783FDFFFFFFu, 0x3F1A36E2EAE3F7A7u, 0x3F50624DD2CE7AC8u,
        0x3F847AE14782197Bu, 0x3FB9999999629FD9u, 0x3FEFFFFFFFBB47D0u, 0x4023FFFFFFD50CE2u, 0x4058FFFFFFCA501Au,
        0x408F3FFFFFBCE421u, 0x40C387FFFFD60E94u, 0x40F869FFFFCB923Au, 0x412E847FFFBE76C8u, 0x416312CFFFD70A3Du,
        0x4197D783FFCCCCCCu, 0x41CDCD64FFBFFFFFu, 0x3F1A36E2EB16A205u, 0x3F50624DD2EE2543u, 0x3F847AE147A9AE94u,
        0x3FB9999999941A39u, 0x3FEFFFFFFFF920C8u, 0x4023FFFFFFFBB47Du, 0x4058FFFFFFFAA19Cu, 0x408F3FFFFFF94A03u,
        0x40C387FFFFFBCE42u, 0x40F869FFFFFAC1D2u, 0x412E847FFFF97247u, 0x416312CFFFFBE76Cu, 0x4197D783FFFAE147u,
        0x41CDCD64FFF99999u, 0x4202A05F1FFBFFFFu, 0x3F1A36E2EB1BB30Fu, 0x3F50624DD2F14FE9u, 0x3F847AE147ADA3E3u,
        0x3FB9999999990CDCu, 0x3FEFFFFFFFFF5014u, 0x4023FFFFFFFF920Cu, 0x4058FFFFFFFF768Fu, 0x408F3FFFFFFF5433u,
        0x40C387FFFFFF94A0u, 0x40F869FFFFFF79C8u, 0x412E847FFFFF583Au, 0x416312CFFFFF9724u, 0x4197D783FFFF7CEDu,
        0x41CDCD64FFFF5C28u, 0x4202A05F1FFF9999u, 0x42374876E7FF7FFFu, 0x3F1A36E2EB1C34C3u, 0x3F50624DD2F1A0FAu,
        0x3F847AE147AE0938u, 0x3FB9999999998B86u, 0x3FEFFFFFFFFFEE68u, 0x4023FFFFFFFFF501u, 0x4058FFFFFFFFF241u,
        0x408F3FFFFFFFEED1u, 0x40C387FFFFFFF543u, 0x40F869FFFFFFF294u, 0x412E847FFFFFEF39u, 0x416312CFFFFFF583u,
        0x4197D783FFFFF2E4u, 0x41CDCD64FFFFEF9Du, 0x4202A05F1FFFF5C2u, 0x42374876E7FFF333u, 0x426D1A94A1FFEFFFu,
        0x3F1A36E2EB1C41BBu, 0x3F50624DD2F1A915u, 0x3F847AE147AE135Au, 0x3FB9999999999831u, 0x3FEFFFFFFFFFFE3Du,
        0x4023FFFFFFFFFEE6u, 0x4058FFFFFFFFFEA0u, 0x408F3FFFFFFFFE48u, 0x40C387FFFFFFFEEDu, 0x40F869FFFFFFFEA8u,
        0x412E847FFFFFFE52u, 0x416312CFFFFFFEF3u, 0x4197D783FFFFFEB0u, 0x41CDCD64FFFFFE5Cu, 0x4202A05F1FFFFEF9u,
        0x42374876E7FFFEB8u, 0x426D1A94A1FFFE66u, 0x42A2309CE53FFEFFu, 0x3F1A36E2EB1C4307u, 0x3F50624DD2F1A9E4u,
        0x3F847AE147AE145Eu, 0x3FB9999999999975u, 0x3FEFFFFFFFFFFFD2u, 0x4023FFFFFFFFFFE3u, 0x4058FFFFFFFFFFDCu,
        0x408F3FFFFFFFFFD4u, 0x40C387FFFFFFFFE4u, 0x40F869FFFFFFFFDDu, 0x412E847FFFFFFFD5u, 0x416312CFFFFFFFE5u,
        0x4197D783FFFFFFDEu, 0x41CDCD64FFFFFFD6u, 0x4202A05F1FFFFFE5u, 0x42374876E7FFFFDFu, 0x426D1A94A1FFFFD7u,
        0x42A2309CE53FFFE6u, 0x42D6BCC41E8FFFDFu, 0x3F1A36E2EB1C4328u, 0x3F50624DD2F1A9F9u, 0x3F847AE147AE1477u,
        0x3FB9999999999995u, 0x3FEFFFFFFFFFFFFBu, 0x4023FFFFFFFFFFFDu, 0x4058FFFFFFFFFFFCu, 0x408F3FFFFFFFFFFBu,
        0x40C387FFFFFFFFFDu, 0x40F869FFFFFFFFFCu, 0x412E847FFFFFFFFBu, 0x416312CFFFFFFFFDu, 0x4197D783FFFFFFFCu,
        0x41CDCD64FFFFFFFBu, 0x4202A05F1FFFFFFDu, 0x42374876E7FFFFFCu, 0x426D1A94A1FFFFFBu, 0x42A2309CE53FFFFDu,
        0x42D6BCC41E8FFFFCu, 0x430C6BF52633FFFBu};

    static constexpr int _Max_P = 309;

    static constexpr uint64_t _Ordinary_X_table[314] = {0x3F1A36E2EB1C432Cu, 0x3F50624DD2F1A9FBu, 0x3F847AE147AE147Au,
        0x3FB9999999999999u, 0x3FEFFFFFFFFFFFFFu, 0x4023FFFFFFFFFFFFu, 0x4058FFFFFFFFFFFFu, 0x408F3FFFFFFFFFFFu,
        0x40C387FFFFFFFFFFu, 0x40F869FFFFFFFFFFu, 0x412E847FFFFFFFFFu, 0x416312CFFFFFFFFFu, 0x4197D783FFFFFFFFu,
        0x41CDCD64FFFFFFFFu, 0x4202A05F1FFFFFFFu, 0x42374876E7FFFFFFu, 0x426D1A94A1FFFFFFu, 0x42A2309CE53FFFFFu,
        0x42D6BCC41E8FFFFFu, 0x430C6BF52633FFFFu, 0x4341C37937E07FFFu, 0x4376345785D89FFFu, 0x43ABC16D674EC7FFu,
        0x43E158E460913CFFu, 0x4415AF1D78B58C3Fu, 0x444B1AE4D6E2EF4Fu, 0x4480F0CF064DD591u, 0x44B52D02C7E14AF6u,
        0x44EA784379D99DB4u, 0x45208B2A2C280290u, 0x4554ADF4B7320334u, 0x4589D971E4FE8401u, 0x45C027E72F1F1281u,
        0x45F431E0FAE6D721u, 0x46293E5939A08CE9u, 0x465F8DEF8808B024u, 0x4693B8B5B5056E16u, 0x46C8A6E32246C99Cu,
        0x46FED09BEAD87C03u, 0x4733426172C74D82u, 0x476812F9CF7920E2u, 0x479E17B84357691Bu, 0x47D2CED32A16A1B1u,
        0x48078287F49C4A1Du, 0x483D6329F1C35CA4u, 0x48725DFA371A19E6u, 0x48A6F578C4E0A060u, 0x48DCB2D6F618C878u,
        0x4911EFC659CF7D4Bu, 0x49466BB7F0435C9Eu, 0x497C06A5EC5433C6u, 0x49B18427B3B4A05Bu, 0x49E5E531A0A1C872u,
        0x4A1B5E7E08CA3A8Fu, 0x4A511B0EC57E6499u, 0x4A8561D276DDFDC0u, 0x4ABABA4714957D30u, 0x4AF0B46C6CDD6E3Eu,
        0x4B24E1878814C9CDu, 0x4B5A19E96A19FC40u, 0x4B905031E2503DA8u, 0x4BC4643E5AE44D12u, 0x4BF97D4DF19D6057u,
        0x4C2FDCA16E04B86Du, 0x4C63E9E4E4C2F344u, 0x4C98E45E1DF3B015u, 0x4CCF1D75A5709C1Au, 0x4D03726987666190u,
        0x4D384F03E93FF9F4u, 0x4D6E62C4E38FF872u, 0x4DA2FDBB0E39FB47u, 0x4DD7BD29D1C87A19u, 0x4E0DAC74463A989Fu,
        0x4E428BC8ABE49F63u, 0x4E772EBAD6DDC73Cu, 0x4EACFA698C95390Bu, 0x4EE21C81F7DD43A7u, 0x4F16A3A275D49491u,
        0x4F4C4C8B1349B9B5u, 0x4F81AFD6EC0E1411u, 0x4FB61BCCA7119915u, 0x4FEBA2BFD0D5FF5Bu, 0x502145B7E285BF98u,
        0x50559725DB272F7Fu, 0x508AFCEF51F0FB5Eu, 0x50C0DE1593369D1Bu, 0x50F5159AF8044462u, 0x512A5B01B605557Au,
        0x516078E111C3556Cu, 0x5194971956342AC7u, 0x51C9BCDFABC13579u, 0x5200160BCB58C16Cu, 0x52341B8EBE2EF1C7u,
        0x526922726DBAAE39u, 0x529F6B0F092959C7u, 0x52D3A2E965B9D81Cu, 0x53088BA3BF284E23u, 0x533EAE8CAEF261ACu,
        0x53732D17ED577D0Bu, 0x53A7F85DE8AD5C4Eu, 0x53DDF67562D8B362u, 0x5412BA095DC7701Du, 0x5447688BB5394C25u,
        0x547D42AEA2879F2Eu, 0x54B249AD2594C37Cu, 0x54E6DC186EF9F45Cu, 0x551C931E8AB87173u, 0x5551DBF316B346E7u,
        0x558652EFDC6018A1u, 0x55BBE7ABD3781ECAu, 0x55F170CB642B133Eu, 0x5625CCFE3D35D80Eu, 0x565B403DCC834E11u,
        0x569108269FD210CBu, 0x56C54A3047C694FDu, 0x56FA9CBC59B83A3Du, 0x5730A1F5B8132466u, 0x5764CA732617ED7Fu,
        0x5799FD0FEF9DE8DFu, 0x57D03E29F5C2B18Bu, 0x58044DB473335DEEu, 0x583961219000356Au, 0x586FB969F40042C5u,
        0x58A3D3E2388029BBu, 0x58D8C8DAC6A0342Au, 0x590EFB1178484134u, 0x59435CEAEB2D28C0u, 0x59783425A5F872F1u,
        0x59AE412F0F768FADu, 0x59E2E8BD69AA19CCu, 0x5A17A2ECC414A03Fu, 0x5A4D8BA7F519C84Fu, 0x5A827748F9301D31u,
        0x5AB7151B377C247Eu, 0x5AECDA62055B2D9Du, 0x5B22087D4358FC82u, 0x5B568A9C942F3BA3u, 0x5B8C2D43B93B0A8Bu,
        0x5BC19C4A53C4E697u, 0x5BF6035CE8B6203Du, 0x5C2B843422E3A84Cu, 0x5C6132A095CE492Fu, 0x5C957F48BB41DB7Bu,
        0x5CCADF1AEA12525Au, 0x5D00CB70D24B7378u, 0x5D34FE4D06DE5056u, 0x5D6A3DE04895E46Cu, 0x5DA066AC2D5DAEC3u,
        0x5DD4805738B51A74u, 0x5E09A06D06E26112u, 0x5E400444244D7CABu, 0x5E7405552D60DBD6u, 0x5EA906AA78B912CBu,
        0x5EDF485516E7577Eu, 0x5F138D352E5096AFu, 0x5F48708279E4BC5Au, 0x5F7E8CA3185DEB71u, 0x5FB317E5EF3AB327u,
        0x5FE7DDDF6B095FF0u, 0x601DD55745CBB7ECu, 0x6052A5568B9F52F4u, 0x60874EAC2E8727B1u, 0x60BD22573A28F19Du,
        0x60F2357684599702u, 0x6126C2D4256FFCC2u, 0x615C73892ECBFBF3u, 0x6191C835BD3F7D78u, 0x61C63A432C8F5CD6u,
        0x61FBC8D3F7B3340Bu, 0x62315D847AD00087u, 0x6265B4E5998400A9u, 0x629B221EFFE500D3u, 0x62D0F5535FEF2084u,
        0x630532A837EAE8A5u, 0x633A7F5245E5A2CEu, 0x63708F936BAF85C1u, 0x63A4B378469B6731u, 0x63D9E056584240FDu,
        0x64102C35F729689Eu, 0x6444374374F3C2C6u, 0x647945145230B377u, 0x64AF965966BCE055u, 0x64E3BDF7E0360C35u,
        0x6518AD75D8438F43u, 0x654ED8D34E547313u, 0x6583478410F4C7ECu, 0x65B819651531F9E7u, 0x65EE1FBE5A7E7861u,
        0x6622D3D6F88F0B3Cu, 0x665788CCB6B2CE0Cu, 0x668D6AFFE45F818Fu, 0x66C262DFEEBBB0F9u, 0x66F6FB97EA6A9D37u,
        0x672CBA7DE5054485u, 0x6761F48EAF234AD3u, 0x679671B25AEC1D88u, 0x67CC0E1EF1A724EAu, 0x680188D357087712u,
        0x6835EB082CCA94D7u, 0x686B65CA37FD3A0Du, 0x68A11F9E62FE4448u, 0x68D56785FBBDD55Au, 0x690AC1677AAD4AB0u,
        0x6940B8E0ACAC4EAEu, 0x6974E718D7D7625Au, 0x69AA20DF0DCD3AF0u, 0x69E0548B68A044D6u, 0x6A1469AE42C8560Cu,
        0x6A498419D37A6B8Fu, 0x6A7FE52048590672u, 0x6AB3EF342D37A407u, 0x6AE8EB0138858D09u, 0x6B1F25C186A6F04Cu,
        0x6B537798F428562Fu, 0x6B88557F31326BBBu, 0x6BBE6ADEFD7F06AAu, 0x6BF302CB5E6F642Au, 0x6C27C37E360B3D35u,
        0x6C5DB45DC38E0C82u, 0x6C9290BA9A38C7D1u, 0x6CC734E940C6F9C5u, 0x6CFD022390F8B837u, 0x6D3221563A9B7322u,
        0x6D66A9ABC9424FEBu, 0x6D9C5416BB92E3E6u, 0x6DD1B48E353BCE6Fu, 0x6E0621B1C28AC20Bu, 0x6E3BAA1E332D728Eu,
        0x6E714A52DFFC6799u, 0x6EA59CE797FB817Fu, 0x6EDB04217DFA61DFu, 0x6F10E294EEBC7D2Bu, 0x6F451B3A2A6B9C76u,
        0x6F7A6208B5068394u, 0x6FB07D457124123Cu, 0x6FE49C96CD6D16CBu, 0x7019C3BC80C85C7Eu, 0x70501A55D07D39CFu,
        0x708420EB449C8842u, 0x70B9292615C3AA53u, 0x70EF736F9B3494E8u, 0x7123A825C100DD11u, 0x7158922F31411455u,
        0x718EB6BAFD91596Bu, 0x71C33234DE7AD7E2u, 0x71F7FEC216198DDBu, 0x722DFE729B9FF152u, 0x7262BF07A143F6D3u,
        0x72976EC98994F488u, 0x72CD4A7BEBFA31AAu, 0x73024E8D737C5F0Au, 0x7336E230D05B76CDu, 0x736C9ABD04725480u,
        0x73A1E0B622C774D0u, 0x73D658E3AB795204u, 0x740BEF1C9657A685u, 0x74417571DDF6C813u, 0x7475D2CE55747A18u,
        0x74AB4781EAD1989Eu, 0x74E10CB132C2FF63u, 0x75154FDD7F73BF3Bu, 0x754AA3D4DF50AF0Au, 0x7580A6650B926D66u,
        0x75B4CFFE4E7708C0u, 0x75EA03FDE214CAF0u, 0x7620427EAD4CFED6u, 0x7654531E58A03E8Bu, 0x768967E5EEC84E2Eu,
        0x76BFC1DF6A7A61BAu, 0x76F3D92BA28C7D14u, 0x7728CF768B2F9C59u, 0x775F03542DFB8370u, 0x779362149CBD3226u,
        0x77C83A99C3EC7EAFu, 0x77FE494034E79E5Bu, 0x7832EDC82110C2F9u, 0x7867A93A2954F3B7u, 0x789D9388B3AA30A5u,
        0x78D27C35704A5E67u, 0x79071B42CC5CF601u, 0x793CE2137F743381u, 0x79720D4C2FA8A030u, 0x79A6909F3B92C83Du,
        0x79DC34C70A777A4Cu, 0x7A11A0FC668AAC6Fu, 0x7A46093B802D578Bu, 0x7A7B8B8A6038AD6Eu, 0x7AB137367C236C65u,
        0x7AE585041B2C477Eu, 0x7B1AE64521F7595Eu, 0x7B50CFEB353A97DAu, 0x7B8503E602893DD1u, 0x7BBA44DF832B8D45u,
        0x7BF06B0BB1FB384Bu, 0x7C2485CE9E7A065Eu, 0x7C59A742461887F6u, 0x7C9008896BCF54F9u, 0x7CC40AABC6C32A38u,
        0x7CF90D56B873F4C6u, 0x7D2F50AC6690F1F8u, 0x7D63926BC01A973Bu, 0x7D987706B0213D09u, 0x7DCE94C85C298C4Cu,
        0x7E031CFD3999F7AFu, 0x7E37E43C8800759Bu, 0x7E6DDD4BAA009302u, 0x7EA2AA4F4A405BE1u, 0x7ED754E31CD072D9u,
        0x7F0D2A1BE4048F90u, 0x7F423A516E82D9BAu, 0x7F76C8E5CA239028u, 0x7FAC7B1F3CAC7433u, 0x7FE1CCF385EBC89Fu,
        0x7FEFFFFFFFFFFFFFu};
};

template <class _Floating>
[[nodiscard]] inline to_chars_result _Floating_to_chars_general_precision(
    char* _First, char* const _Last, const _Floating _Value, int _Precision) noexcept {

    using _Traits    = _Floating_type_traits<_Floating>;
    using _Uint_type = typename _Traits::_Uint_type;

    const _Uint_type _Uint_value = bit_cast<_Uint_type>(_Value);

    if (_Uint_value == 0) { // zero detected; write "0" and return; _Precision is irrelevant due to zero-trimming
        if (_First == _Last) {
            return {_Last, std::errc::value_too_large};
        }

        *_First++ = '0';

        return {_First, std::errc{}};
    }

    // C11 7.21.6.1 "The fprintf function"/5:
    // "A negative precision argument is taken as if the precision were omitted."
    // /8: "g,G [...] Let P equal the precision if nonzero, 6 if the precision is omitted,
    // or 1 if the precision is zero."

    // Performance note: It's possible to rewrite this for branchless codegen,
    // but profiling will be necessary to determine whether that's faster.
    if (_Precision < 0) {
        _Precision = 6;
    } else if (_Precision == 0) {
        _Precision = 1;
    } else if (_Precision < 1'000'000) {
        // _Precision is ok.
    } else {
        // Avoid integer overflow.
        // Due to general notation's zero-trimming behavior, we can simply clamp _Precision.
        // This is further clamped below.
        _Precision = 1'000'000;
    }

    // _Precision is now the Standard's P.

    // /8: "Then, if a conversion with style E would have an exponent of X:
    // - if P > X >= -4, the conversion is with style f (or F) and precision P - (X + 1).
    // - otherwise, the conversion is with style e (or E) and precision P - 1."

    // /8: "Finally, [...] any trailing zeros are removed from the fractional portion of the result
    // and the decimal-point character is removed if there is no fractional portion remaining."

    using _Tables = _General_precision_tables<_Floating>;

    const _Uint_type* _Table_begin;
    const _Uint_type* _Table_end;

    if (_Precision <= _Tables::_Max_special_P) {
        _Table_begin = _Tables::_Special_X_table + (_Precision - 1) * (_Precision + 10) / 2;
        _Table_end   = _Table_begin + _Precision + 5;
    } else {
        _Table_begin = _Tables::_Ordinary_X_table;
        _Table_end   = _Table_begin + (std:: min)(_Precision, _Tables::_Max_P) + 5;
    }

    // Profiling indicates that linear search is faster than binary search for small tables.
    // Performance note: lambda captures may have a small performance cost.
    const _Uint_type* const _Table_lower_bound = [=] {
        if constexpr (!std::is_same_v<_Floating, float>) {
            if (_Precision > 155) { // threshold determined via profiling
                return std:: lower_bound(_Table_begin, _Table_end, _Uint_value, std::less{});
            }
        }

        return std:: find_if(_Table_begin, _Table_end, [=](const _Uint_type _Elem) { return _Uint_value <= _Elem; });
    }();

    const ptrdiff_t _Table_index     = _Table_lower_bound - _Table_begin;
    const int _Scientific_exponent_X = static_cast<int>(_Table_index - 5);
    const bool _Use_fixed_notation   = _Precision > _Scientific_exponent_X && _Scientific_exponent_X >= -4;

    // Performance note: it might (or might not) be faster to modify Ryu Printf to perform zero-trimming.
    // Such modifications would involve a fairly complicated state machine (notably, both '0' and '9' digits would
    // need to be buffered, due to rounding), and that would have performance costs due to increased branching.
    // Here, we're using a simpler approach: writing into a local buffer, manually zero-trimming, and then copying into
    // the output range. The necessary buffer size is reasonably small, the zero-trimming logic is simple and fast,
    // and the final copying is also fast.

    constexpr int _Max_output_length =
        std::is_same_v<_Floating, float> ? 117 : 773; // cases: 0x1.fffffep-126f and 0x1.fffffffffffffp-1022
    constexpr int _Max_fixed_precision =
        std::is_same_v<_Floating, float> ? 37 : 66; // cases: 0x1.fffffep-14f and 0x1.fffffffffffffp-14
    constexpr int _Max_scientific_precision =
        std::is_same_v<_Floating, float> ? 111 : 766; // cases: 0x1.fffffep-126f and 0x1.fffffffffffffp-1022

    // Note that _Max_output_length is determined by scientific notation and is more than enough for fixed notation.
    // 0x1.fffffep+127f is 39 digits, plus 1 for '.', plus _Max_fixed_precision for '0' digits, equals 77.
    // 0x1.fffffffffffffp+1023 is 309 digits, plus 1 for '.', plus _Max_fixed_precision for '0' digits, equals 376.

    char _Buffer[_Max_output_length];
    const char* const _Significand_first = _Buffer; // e.g. "1.234"
    const char* _Significand_last        = nullptr;
    const char* _Exponent_first          = nullptr; // e.g. "e-05"
    const char* _Exponent_last           = nullptr;
    int _Effective_precision; // number of digits printed after the decimal point, before trimming

    // Write into the local buffer.
    // Clamping _Effective_precision allows _Buffer to be as small as possible, and increases efficiency.
    if (_Use_fixed_notation) {
        _Effective_precision = (std:: min)(_Precision - (_Scientific_exponent_X + 1), _Max_fixed_precision);
        const to_chars_result _Buf_result =
            _Floating_to_chars_fixed_precision(_Buffer, std:: end(_Buffer), _Value, _Effective_precision);
        assert(_Buf_result.ec == std::errc{});
        _Significand_last = _Buf_result.ptr;
    } else {
        _Effective_precision = (std:: min)(_Precision - 1, _Max_scientific_precision);
        const to_chars_result _Buf_result =
            _Floating_to_chars_scientific_precision(_Buffer, std:: end(_Buffer), _Value, _Effective_precision);
        assert(_Buf_result.ec == std::errc{});
        _Significand_last = std:: find(_Buffer, _Buf_result.ptr, 'e');
        _Exponent_first   = _Significand_last;
        _Exponent_last    = _Buf_result.ptr;
    }

    // If we printed a decimal point followed by digits, perform zero-trimming.
    if (_Effective_precision > 0) {
        while (_Significand_last[-1] == '0') { // will stop at '.' or a nonzero digit
            --_Significand_last;
        }

        if (_Significand_last[-1] == '.') {
            --_Significand_last;
        }
    }

    // Copy the significand to the output range.
    const ptrdiff_t _Significand_distance = _Significand_last - _Significand_first;
    if (_Last - _First < _Significand_distance) {
        return {_Last, std::errc::value_too_large};
    }
    std:: memcpy(_First, _Significand_first, static_cast<size_t>(_Significand_distance));
    _First += _Significand_distance;

    // Copy the exponent to the output range.
    if (!_Use_fixed_notation) {
        const ptrdiff_t _Exponent_distance = _Exponent_last - _Exponent_first;
        if (_Last - _First < _Exponent_distance) {
            return {_Last, std::errc::value_too_large};
        }
        std:: memcpy(_First, _Exponent_first, static_cast<size_t>(_Exponent_distance));
        _First += _Exponent_distance;
    }

    return {_First, std::errc{}};
}

enum class _Floating_to_chars_overload { _Plain, _Format_only, _Format_precision };

template <_Floating_to_chars_overload _Overload, class _Floating>
[[nodiscard]] to_chars_result _Floating_to_chars(
    char* _First, char* const _Last, _Floating _Value, const chars_format _Fmt, const int _Precision) noexcept {
    MSCHARCONV_VERIFY_RANGE(_First, _Last);

    if constexpr (_Overload == _Floating_to_chars_overload::_Plain) {
        assert(_Fmt == chars_format{}); // plain overload must pass chars_format{} internally
    } else {
        MSCHARCONV_ASSERT_MSG(_Fmt == chars_format::general || _Fmt == chars_format::scientific || _Fmt == chars_format::fixed
                        || _Fmt == chars_format::hex,
            "invalid format in to_chars()");
    }

    using _Traits    = _Floating_type_traits<_Floating>;
    using _Uint_type = typename _Traits::_Uint_type;

    _Uint_type _Uint_value = bit_cast<_Uint_type>(_Value);

    const bool _Was_negative = (_Uint_value & _Traits::_Shifted_sign_mask) != 0;

    if (_Was_negative) { // sign bit detected; write minus sign and clear sign bit
        if (_First == _Last) {
            return {_Last, std::errc::value_too_large};
        }

        *_First++ = '-';

        _Uint_value &= ~_Traits::_Shifted_sign_mask;
        _Value = bit_cast<_Floating>(_Uint_value);
    }

    if ((_Uint_value & _Traits::_Shifted_exponent_mask) == _Traits::_Shifted_exponent_mask) {
        // inf/nan detected; write appropriate string and return
        const char* _Str;
        size_t _Len;

        const _Uint_type _Mantissa = _Uint_value & _Traits::_Denormal_mantissa_mask;

        if (_Mantissa == 0) {
            _Str = "inf";
            _Len = 3;
        } else if (_Was_negative && _Mantissa == _Traits::_Special_nan_mantissa_mask) {
            // When a NaN value has the sign bit set, the quiet bit set, and all other mantissa bits cleared,
            // the UCRT interprets it to mean "indeterminate", and indicates this by printing "-nan(ind)".
            _Str = "nan(ind)";
            _Len = 8;
        } else if ((_Mantissa & _Traits::_Special_nan_mantissa_mask) != 0) {
            _Str = "nan";
            _Len = 3;
        } else {
            _Str = "nan(snan)";
            _Len = 9;
        }

        if (_Last - _First < static_cast<ptrdiff_t>(_Len)) {
            return {_Last, std::errc::value_too_large};
        }

        std:: memcpy(_First, _Str, _Len);

        return {_First + _Len, std::errc{}};
    }

    if constexpr (_Overload == _Floating_to_chars_overload::_Plain) {
        return _Floating_to_chars_ryu(_First, _Last, _Value, chars_format{});
    } else if constexpr (_Overload == _Floating_to_chars_overload::_Format_only) {
        if (_Fmt == chars_format::hex) {
            return _Floating_to_chars_hex_shortest(_First, _Last, _Value);
        }

        return _Floating_to_chars_ryu(_First, _Last, _Value, _Fmt);
    } else if constexpr (_Overload == _Floating_to_chars_overload::_Format_precision) {
        switch (_Fmt) {
        case chars_format::scientific:
            return _Floating_to_chars_scientific_precision(_First, _Last, _Value, _Precision);
        case chars_format::fixed:
            return _Floating_to_chars_fixed_precision(_First, _Last, _Value, _Precision);
        case chars_format::general:
            return _Floating_to_chars_general_precision(_First, _Last, _Value, _Precision);
        case chars_format::hex:
        default: // avoid warning C4715: not all control paths return a value
            return _Floating_to_chars_hex_precision(_First, _Last, _Value, _Precision);
        }
    }
}

inline to_chars_result to_chars(char* const _First, char* const _Last, const float _Value) noexcept /* strengthened */ {
    return _Floating_to_chars<_Floating_to_chars_overload::_Plain>(_First, _Last, _Value, chars_format{}, 0);
}
inline to_chars_result to_chars(char* const _First, char* const _Last, const double _Value) noexcept
/* strengthened */ {
    return _Floating_to_chars<_Floating_to_chars_overload::_Plain>(_First, _Last, _Value, chars_format{}, 0);
}
inline to_chars_result to_chars(char* const _First, char* const _Last, const long double _Value) noexcept
/* strengthened */ {
    return _Floating_to_chars<_Floating_to_chars_overload::_Plain>(
        _First, _Last, static_cast<double>(_Value), chars_format{}, 0);
}
inline to_chars_result to_chars(
    char* const _First, char* const _Last, const float _Value, const chars_format _Fmt) noexcept /* strengthened */ {
    return _Floating_to_chars<_Floating_to_chars_overload::_Format_only>(_First, _Last, _Value, _Fmt, 0);
}
inline to_chars_result to_chars(
    char* const _First, char* const _Last, const double _Value, const chars_format _Fmt) noexcept /* strengthened */ {
    return _Floating_to_chars<_Floating_to_chars_overload::_Format_only>(_First, _Last, _Value, _Fmt, 0);
}
inline to_chars_result to_chars(char* const _First, char* const _Last, const long double _Value,
    const chars_format _Fmt) noexcept /* strengthened */ {
    return _Floating_to_chars<_Floating_to_chars_overload::_Format_only>(
        _First, _Last, static_cast<double>(_Value), _Fmt, 0);
}
inline to_chars_result to_chars(char* const _First, char* const _Last, const float _Value, const chars_format _Fmt,
    const int _Precision) noexcept /* strengthened */ {
    return _Floating_to_chars<_Floating_to_chars_overload::_Format_precision>(_First, _Last, _Value, _Fmt, _Precision);
}
inline to_chars_result to_chars(char* const _First, char* const _Last, const double _Value, const chars_format _Fmt,
    const int _Precision) noexcept /* strengthened */ {
    return _Floating_to_chars<_Floating_to_chars_overload::_Format_precision>(_First, _Last, _Value, _Fmt, _Precision);
}
inline to_chars_result to_chars(char* const _First, char* const _Last, const long double _Value,
    const chars_format _Fmt, const int _Precision) noexcept /* strengthened */ {
    return _Floating_to_chars<_Floating_to_chars_overload::_Format_precision>(
        _First, _Last, static_cast<double>(_Value), _Fmt, _Precision);
}

//~ _STD_END

//~ #pragma pop_macro("new")
//~ _STL_RESTORE_CLANG_WARNINGS
//~ #pragma warning(pop)
//~ #pragma pack(pop)
#endif // _HAS_CXX17
//~ #endif // _STL_COMPILER_PREPROCESSOR
#endif // _CHARCONV_
