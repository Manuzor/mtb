/*
    mtb string builder (mstr)
*/

#if defined(MSTR_IMPLEMENTATION)
#undef MSTR_IMPLEMENTATION
#define MSTR_IMPLEMENTATION 1
#else
#define MSTR_IMPLEMENTATION 0
#endif

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
char* mstr_ToString(mstr_Builder* b);

#if defined(STB_SPRINTF_H_INCLUDE)
/* NOTE: If you want to use mstr_AppendFormat, include stb_sprintf.h first! */
void mstr_AppendFormat(mstr_Builder* b, char const* format, ...);
void mstr_AppendFormatV(mstr_Builder* b, char const* format, va_list args);
#endif

#if defined(MTB__INCLUDED)
mstr_Allocator mstr_WrapMtbAllocator(mtb::tAllocator& allocator);
#endif

#if defined(__cplusplus)
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

        /* clang-format on */

        char* ToString() { return mstr_ToString(this); }

#if defined(STB_SPRINTF_H_INCLUDE)
        /* NOTE: If you want to use AppendFormat, include stb_sprintf.h first! */
        Builder& AppendFormat(char const* format, ...);
        Builder& AppendFormatV(char const* format, va_list args);
#endif /* defined(STB_SPRINTF_H_INCLUDE) */
    };
} /* namespace mstr */
#endif /* defined(__cplusplus) */

#endif /* MSTR_INCLUDED */

#if MSTR_IMPLEMENTATION

#include <assert.h>
#define MSTR_ASSERT(CONDITION) assert(CONDITION)

static void* mstr__LibCReallocCallback(void* user, void* old_ptr, size_t old_size, size_t new_size) {
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
    size_t new_cap = b->cap == 0 ? default_cap : (3 * b->cap / 2);
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

char* mstr_ToString(mstr_Builder* b) {
    mstr_EnsureCapacity(b, b->len + 1);
    b->ptr[b->len] = 0;
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

#if defined(MTB__INCLUDED)
static void* mstr__MtbReallocCallback(void* user, void* old_ptr, size_t old_size, size_t new_size) {
    MFS_ASSERT(user);
    mtb::tAllocator& a = *(mtb::tAllocator*)user;
    mtb::tSlice<void> new_mem = a.ResizeArray(mtb::PtrSlice(old_ptr, old_size), new_size, mtb::kNoInit);
    MSTR_ASSERT(new_mem.len == new_size);
    return new_mem.ptr;
}

mstr_Allocator mstr_WrapMtbAllocator(mtb::tAllocator& mtb_allocator) {
    mstr_Allocator result = {};
    result.user = &mtb_allocator;
    result.realloc_cb = &mstr__MtbReallocCallback;
    return result;
}
#endif

#if defined(__cplusplus)

#if defined(STB_SPRINTF_H_INCLUDE)
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
#endif /* defined(STB_SPRINTF_H_INCLUDE) */

#endif /* defined(__cplusplus) */

#endif /* MSTR_IMPLEMENTATION */

#endif /* !defined(MSTR_INCLUDED) */
