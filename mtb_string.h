/*
    mtb string builder (mstr)
*/

#if !defined(MSTR_INCLUDED)
#define MSTR_INCLUDED

#include <stdint.h>

#if defined(STB_SPRINTF_H_INCLUDE)
#include <stdarg.h>
#endif

typedef struct mstr_Allocator {
    void* user;
    void* (*realloc_cb)(void* user, void* old_ptr, size_t old_size, size_t new_size);
} mstr_Allocator;

#if defined(MTB_INCLUDED)
namespace mstr {
    mstr_Allocator WrapMtbAllocator(mtb::tAllocator& allocator);
}
#endif

bool mstr_IsWhiteChar(char c);
bool mstr_IsDigit(char c);

typedef struct mstr_Builder {
    mstr_Allocator allocator;
    char* ptr;
    size_t len;
    size_t cap;
} mstr_Builder;

void mstr_ShrinkAllocation(mstr_Builder* b);
void mstr_Clear(mstr_Builder* b);
void mstr_ClearAllocation(mstr_Builder* b);
void mstr_EnsureCapacity(mstr_Builder* b, size_t n);
void mstr_AppendChar(mstr_Builder* b, char c);
void mstr_AppendString(mstr_Builder* b, char const* str, size_t str_len);
void mstr_AppendStringZ(mstr_Builder* b, char const* str);

void mstr_TrimEnd(mstr_Builder* b);
char* mstr_Finish(mstr_Builder* b);

#if defined(STB_SPRINTF_H_INCLUDE)
/* NOTE: If you want to use mstr_AppendFormat, include stb_sprintf.h first! */
void mstr_AppendFormat(mstr_Builder* b, char const* format, ...);
void mstr_AppendFormatV(mstr_Builder* b, char const* format, va_list args);
#endif

#if defined(__cplusplus)
#include <new>

namespace mstr {
    struct Builder : mstr_Builder {
        Builder() = default;
        Builder(Builder const&) = default;
        ~Builder() = default;
        Builder& operator=(Builder const&) = default;

        explicit Builder(mstr_Allocator in_allocator) {
            (void)new(this) Builder{};
            allocator = in_allocator;
        }

        /* clang-format off */
        Builder& ShrinkAllocation() { mstr_ShrinkAllocation(this); return *this; }
        Builder& Clear() { mstr_Clear(this); return *this; }
        Builder& ClearAllocation() { mstr_ClearAllocation(this); return *this; }
        Builder& EnsureCapacity(size_t n) { mstr_EnsureCapacity(this, n); return *this; }
        Builder& AppendChar(char c) { mstr_AppendChar(this, c); return *this; }
        Builder& AppendString(char const* str, size_t str_len) { mstr_AppendString(this, str, str_len); return *this; }
        Builder& AppendStringZ(char const* str) { mstr_AppendStringZ(this, str); return *this; }
        Builder& TrimEnd() { mstr_TrimEnd(this); return *this; }
        Builder& Finish() { (void)mstr_Finish(this); return *this; }

        /* clang-format on */

#if defined(MTB_INCLUDED)
        ::mtb::tSlice<char> AsSlice() { return {ptr, (ptrdiff_t)len}; }
#endif

#if defined(STB_SPRINTF_H_INCLUDE)
        /* NOTE: If you want to use AppendFormat, include stb_sprintf.h first! */
        Builder& AppendFormat(char const* format, ...);
        Builder& AppendFormatV(char const* format, va_list args);
#endif /* defined(STB_SPRINTF_H_INCLUDE) */
    };
} /* namespace mstr */
#endif /* defined(__cplusplus) */

typedef union mstr_Number {
    double f64;
    uint64_t u64;
    int64_t i64;
} mstr_number;

typedef struct mstr_ParseOptions {
    mstr_Number fallback_value;
} mstr_ParseOptions;

mstr_ParseOptions mstr_DefaultParseOptions();

typedef struct mstr_ParseResult {
    bool success;
    char const* tail_ptr;
    size_t tail_len;
    mstr_Number value;
#if defined(MTB_INCLUDED)
    ::mtb::tSlice<char const> Tail() const { return {tail_ptr, (ptrdiff_t)tail_len}; }
#endif
} mstr_ParseResult;

mstr_ParseResult mstr_ParseFloat(char const* str_ptr, size_t str_len, mstr_ParseOptions options);
mstr_ParseResult mstr_ParseUnsignedInteger(char const* str_ptr, size_t str_len, mstr_ParseOptions options);
mstr_ParseResult mstr_ParseSignedInteger(char const* str_ptr, size_t str_len, mstr_ParseOptions options);

#if defined(MTB_INCLUDED)
namespace mtb {
    inline mstr_ParseResult mstr_ParseFloat(tSlice<char const> str, mstr_ParseOptions options) { return mstr_ParseFloat(str.ptr, (size_t)str.len, options); }

    inline mstr_ParseResult mstr_ParseUnsignedInteger(tSlice<char const> str, mstr_ParseOptions options) { return mstr_ParseUnsignedInteger(str.ptr, (size_t)str.len, options); }

    inline mstr_ParseResult mstr_ParseSignedInteger(tSlice<char const> str, mstr_ParseOptions options) { return mstr_ParseSignedInteger(str.ptr, (size_t)str.len, options); }
}  // namespace mtb
#endif

#endif /* !defined(MSTR_INCLUDED) */

#if defined(MSTR_IMPLEMENTATION)
#undef MSTR_IMPLEMENTATION
#define MSTR_IMPLEMENTATION 1
#else
#define MSTR_IMPLEMENTATION 0
#endif

#if MSTR_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#define MSTR_ASSERT(CONDITION) assert(CONDITION)

static void* mstr__LibCReallocCallback(void* user, void* old_ptr, size_t old_size, size_t new_size) {
    (void)user;
    (void)old_size;
    void* new_ptr = realloc(old_ptr, new_size);
    MSTR_ASSERT(new_ptr);
    return new_ptr;
}

static mstr_Allocator mstr__LibCAllocator() {
    mstr_Allocator result;
    result.realloc_cb = mstr__LibCReallocCallback;
    result.user = NULL;
    return result;
}

static mstr_Allocator mstr__GetAllocator(mstr_Builder* b) {
    mstr_Allocator result = b->allocator;
    if(!result.realloc_cb) {
        result = mstr__LibCAllocator();
    }
    return result;
}

#if defined(MTB_INCLUDED)
namespace mstr::impl {
    static void* MtbReallocCallback(void* user, void* old_ptr, size_t old_size, size_t new_size) {
        MFS_ASSERT(user);
        mtb::tAllocator& a = *(mtb::tAllocator*)user;
        mtb::tSlice<void> new_mem = a.ResizeArray(mtb::PtrSlice(old_ptr, old_size), new_size, mtb::kNoInit);
        MSTR_ASSERT((size_t)new_mem.len == new_size);
        return new_mem.ptr;
    }
}  // namespace mstr::impl

mstr_Allocator mstr::WrapMtbAllocator(mtb::tAllocator& mtb_allocator) {
    mstr_Allocator result = {};
    result.user = &mtb_allocator;
    result.realloc_cb = &impl::MtbReallocCallback;
    return result;
}
#endif

bool mstr_IsWhiteChar(char c) {
    switch(c) {
        case ' ':
        case '\n':
        case '\r':
        case '\t':
        case '\v':
        case '\b':
            return true;
        default:
            return false;
    }
}

bool mstr_IsDigit(char c) {
    switch(c) {
        case '0':  // fallthrough
        case '1':  // fallthrough
        case '2':  // fallthrough
        case '3':  // fallthrough
        case '4':  // fallthrough
        case '5':  // fallthrough
        case '6':  // fallthrough
        case '7':  // fallthrough
        case '8':  // fallthrough
        case '9': return true;
        default: return false;
    }
}

void mstr_ShrinkAllocation(mstr_Builder* b) {
    mstr_Allocator a = mstr__GetAllocator(b);
    b->ptr = (char*)a.realloc_cb(a.user, b->ptr, b->cap, b->len);
    b->cap = b->len;
}

void mstr_Clear(mstr_Builder* b) {
    b->len = 0;
}

void mstr_ClearAllocation(mstr_Builder* b) {
    mstr_Clear(b);
    mstr_ShrinkAllocation(b);
}

void mstr_EnsureCapacity(mstr_Builder* b, size_t n) {
    if(n <= b->cap) {
        return;
    }
    mstr_Allocator a = mstr__GetAllocator(b);
    size_t const default_cap = 4096;
    size_t new_cap = b->cap < default_cap ? default_cap : b->cap;
    while(new_cap < n) {
        new_cap = (3 * new_cap) / 2;
    }
    char* new_ptr = (char*)a.realloc_cb(a.user, b->ptr, b->cap, new_cap);
    MSTR_ASSERT(new_ptr);
    b->ptr = new_ptr;
    b->cap = new_cap;
}

void mstr_AppendChar(mstr_Builder* b, char c) {
    mstr_EnsureCapacity(b, b->len + 1);
    b->ptr[b->len] = c;
    b->len += 1;
}

void mstr_AppendString(mstr_Builder* b, char const* str, size_t str_len) {
    mstr_EnsureCapacity(b, b->len + str_len);
    memmove(b->ptr + b->len, str, str_len);
    b->len += str_len;
}

void mstr_AppendStringZ(mstr_Builder* b, char const* str) {
    mstr_AppendString(b, str, strlen(str));
}

void mstr_TrimEnd(mstr_Builder* b) {
    while(b->len > 0) {
        if(!isspace(b->ptr[b->len - 1])) {
            break;
        }
        b->len -= 1;
    }
}

char* mstr_Finish(mstr_Builder* b) {
    mstr_EnsureCapacity(b, b->len + 1);
    b->ptr[b->len] = 0;
    b->len += 1;
    mstr_ShrinkAllocation(b);
    b->len -= 1;
    return b->ptr;
}

#if defined(STB_SPRINTF_H_INCLUDE)

static char* mstr__StbSprintfCallback(char const* buf, void* user, int len) {
    mstr_Builder* b = (mstr_Builder*)user;
    mstr_EnsureCapacity(b, b->len + len);
    memcpy(b->ptr + b->len, buf, len);
    b->len += len;
    return (char*)buf;
}

void mstr_AppendFormat(mstr_Builder* b, char const* format, ...) {
    va_list args;
    va_start(args, format);
    mstr_AppendFormatV(b, format, args);
    va_end(args);
}

void mstr_AppendFormatV(mstr_Builder* b, char const* format, va_list args) {
    char buf[STB_SPRINTF_MIN];
    stbsp_vsprintfcb(&mstr__StbSprintfCallback, b, buf, format, args);
}
#endif /* defined(STB_SPRINTF_H_INCLUDE) */

#if defined(STB_SPRINTF_H_INCLUDE) && defined(__cplusplus)
mstr::Builder& mstr::Builder::AppendFormat(char const* format, ...) {
    va_list args;
    va_start(args, format);
    mstr_AppendFormatV(this, format, args);
    va_end(args);
    return *this;
}

mstr::Builder& mstr::Builder::AppendFormatV(char const* format, va_list args) {
    mstr_AppendFormatV(this, format, args);
    return *this;
}
#endif  // defined(STB_SPRINTF_H_INCLUDE) && defined(__cplusplus)

static size_t mstr__TrimWhitespaceFront(char const** inout_ptr, size_t* inout_len) {
    char const* ptr = *inout_ptr;
    size_t len = *inout_len;

    while(len && mstr_IsWhiteChar(ptr[0])) {
        ++ptr;
        --len;
    }

    size_t num_trimmed = *inout_len - len;
    *inout_ptr = ptr;
    *inout_len = len;
    return num_trimmed;
}

/// If a '+' was consumed, `1` is returned and \a SourcePtr is advanced by 1,
/// else, if a '-' was consumed, `-1` is returned and \a SourcePtr is advanced by 1,
/// otherwise 0 is returned and \a SourcePtr will not be modified.
static int mstr__ConsumeSign(char const** inout_ptr, size_t* inout_len) {
    int result{};
    char const* ptr = *inout_ptr;
    size_t len = *inout_len;

    if(len) {
        if(ptr[0] == '+') {
            ++ptr;
            --len;
            result = 1;
        } else if(ptr[0] == '-') {
            ++ptr;
            --len;
            result = -1;
        }
    }

    *inout_ptr = ptr;
    *inout_len = len;
    return result;
}

mstr_ParseOptions mstr_DefaultParseOptions() {
    static mstr_ParseOptions mstr_no_parse_options;
    return mstr_no_parse_options;
}

mstr_ParseResult mstr_ParseFloat(char const* str_ptr, size_t str_len, mstr_ParseOptions options) {
    mstr_ParseResult result;
    result.success = false;
    result.tail_ptr = str_ptr;
    result.tail_len = str_len;
    result.value = options.fallback_value;

    char const* ptr = str_ptr;
    size_t len = str_len;

    (void)mstr__TrimWhitespaceFront(&ptr, &len);
    if(len) {
        bool has_sign = mstr__ConsumeSign(&ptr, &len) < 0;

        // Parse all parts of a floating point number.
        if(len) {
            // Numeric part
            mstr_ParseResult numeric_result = mstr_ParseUnsignedInteger(ptr, len, mstr_DefaultParseOptions());
            len = numeric_result.tail_len;
            ptr = numeric_result.tail_ptr;
            double value = (double)numeric_result.value.u64;

            // Decimal part
            bool has_decimal_part = false;
            uint64_t decimal_value = 0;
            uint64_t decimal_divider = 1;
            if(len && ptr[0] == '.') {
                ++ptr;
                --len;
                while(len && mstr_IsDigit(ptr[0])) {
                    uint64_t NewDigit = (uint64_t)(ptr[0] - '0');
                    decimal_value = (10 * decimal_value) + NewDigit;
                    has_decimal_part = true;
                    decimal_divider *= 10;
                    ++ptr;
                    --len;
                }

                value += (double)decimal_value / (double)decimal_divider;
            }

            if(numeric_result.success || has_decimal_part) {
                // Parse exponent, if any.
                if(len && (ptr[0] == 'e' || ptr[0] == 'E')) {
                    size_t exponent_source_len = len - 1;
                    char const* exponent_source_ptr = ptr + 1;
                    bool exponent_has_sign = mstr__ConsumeSign(&exponent_source_ptr, &exponent_source_len) < 0;

                    mstr_ParseOptions exponent_parse_options = mstr_DefaultParseOptions();
                    exponent_parse_options.fallback_value.u64 = UINT64_MAX;
                    mstr_ParseResult exponent_result = mstr_ParseUnsignedInteger(exponent_source_ptr, exponent_source_len, exponent_parse_options);
                    if(exponent_result.success) {
                        ptr = exponent_result.tail_ptr;
                        len = exponent_result.tail_len;

                        uint64_t exponent_part = exponent_result.value.u64;
                        double exponent_value = 1;
                        while(exponent_part > 0) {
                            exponent_value *= 10;
                            --exponent_part;
                        }

                        if(exponent_has_sign)
                            value /= exponent_value;
                        else
                            value *= exponent_value;
                    }
                }

                result.success = true;
                result.tail_len = len;
                result.tail_ptr = ptr;
                result.value.f64 = has_sign ? -value : value;
            }
        }
    }

    return result;
}

mstr_ParseResult mstr_ParseUnsignedInteger(char const* str_ptr, size_t str_len, mstr_ParseOptions options) {
    mstr_ParseResult result;
    result.success = false;
    result.tail_ptr = str_ptr;
    result.tail_len = str_len;
    result.value = options.fallback_value;

    char const* ptr = str_ptr;
    size_t len = str_len;

    if(len) {
        (void)mstr__TrimWhitespaceFront(&ptr, &len);
        bool has_sign = mstr__ConsumeSign(&ptr, &len) < 0;
        if(!has_sign) {
            uint64_t numerical_part = 0;
            bool has_numerical_part = false;

            while(len && mstr_IsDigit(ptr[0])) {
                numerical_part = (10 * numerical_part) + (*ptr - '0');
                has_numerical_part = true;
                --len;
                ++ptr;
            }

            if(has_numerical_part) {
                result.value.u64 = numerical_part;
                result.success = true;
            }
        }
    }

    if(result.success) {
        result.tail_ptr = ptr;
        result.tail_len = len;
    }

    return result;
}

mstr_ParseResult mstr_ParseSignedInteger(char const* str_ptr, size_t str_len, mstr_ParseOptions options) {
    mstr_ParseResult result;
    result.success = false;
    result.tail_ptr = str_ptr;
    result.tail_len = str_len;
    result.value = options.fallback_value;

    char const* ptr = str_ptr;
    size_t len = str_len;

    if(len) {
        (void)mstr__TrimWhitespaceFront(&ptr, &len);
        bool has_sign = mstr__ConsumeSign(&ptr, &len) < 0;

        uint64_t numerical_part = 0;
        bool has_numerical_part = false;

        while(len && mstr_IsDigit(ptr[0])) {
            numerical_part = (10 * numerical_part) + (*ptr - '0');
            has_numerical_part = true;
            --len;
            ++ptr;
        }

        if(has_numerical_part) {
            if(has_sign) {
                if(numerical_part <= (uint64_t)INT64_MAX + 1) {
                    if(numerical_part == (uint64_t)INT64_MAX + 1) {
                        result.value.i64 = INT64_MIN;
                    } else {
                        result.value.i64 = -(int64_t)numerical_part;
                    }

                    result.success = true;
                }
            } else {
                if(numerical_part <= INT64_MAX) {
                    result.value.i64 = numerical_part;
                    result.success = true;
                }
            }
        }
    }

    if(result.success) {
        result.tail_ptr = ptr;
        result.tail_len = len;
    }

    return result;
}

#endif /* MSTR_IMPLEMENTATION */
