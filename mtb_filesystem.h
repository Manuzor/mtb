/*
    mtb filesystem (mfs)

    Identifiers marked with double-underscores (e.g. MFS__WIN32) are intended
    for internal use of this library. Public facing identifiers start with
    mfs_ (types and functions both) or MFS_ (preprocessor stuff).

    Strings are usually provided as a ptr+len pair. These strings are always
    null-terminated but len never includes the terminating null-character.

    Functions may temporarily allocate memory internally for result values.
    Result pointers are only valid until the next API call. So it's best to
    copy any results on the caller side if that result is needed further.
    Ownership is never passed to the called.

    Error messages are formatted using a dedicated internal buffer. This buffer is re-used for every subsequent error messages. If the test of an error message is required to live longer, it should be copied.

    Example:
        .str = "foo", // points to `char const[4]{'f', 'o', 'o', '\0'}`
        .len = 3,

    Public API section index:
        [Section] Error codes
        [Section] Public API

    Implementation index:
        [Section] Implementation
        [Section] Backend configuration
        [Section] Internal helpers
        [Section] POSIX Backend
        [Section] WIN32 Backend
        [Section] Public API Implementation
*/

// ReSharper disable CppClangTidyClangDiagnosticReservedIdentifier
// ReSharper disable CppClangTidyClangDiagnosticReservedMacroIdentifier
// ReSharper disable CppClangTidyBugproneReservedIdentifier
// ReSharper disable CppInconsistentNaming
// ReSharper disable CppParameterMayBeConstPtrOrRef
// ReSharper disable CppIfCanBeReplacedByConstexprIf
// ReSharper disable CppClangTidyModernizeUseAuto
// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr
#ifndef MFS_INCLUDED
#define MFS_INCLUDED

#include <stdint.h>

// ReSharper disable once CppUnusedIncludeDirective
#include <stdbool.h>

#if !defined(MFS_FN)
#if defined(__cplusplus)
#define MFS_FN extern "C"
#else
#define MFS_FN
#endif
#endif

#if !defined(MFS_ZERO_INIT)
#if defined(__cplusplus)
/* clang-format off */
#define MFS_ZERO_INIT() {}
/* clang-format on */
#else
/* clang-format off */
#define MFS_ZERO_INIT() { 0 }
/* clang-format on */
#endif
#endif

/*
    [Section] Error codes
*/
typedef enum mfs_ErrorCode {
    mfs_ErrorCode_None = 0,

    /*
        Internal errors.
    */
    mfs_ErrorCode_InvalidOperation,
    mfs_ErrorCode_OutOfMemory,

    /*
    File system related errors.
    */
    mfs_ErrorCode_Unkown,
    mfs_ErrorCode_NotFound,
    mfs_ErrorCode_PermissionDenied,
    mfs_ErrorCode_InvalidFileName,
} mfs_ErrorCode;

typedef struct mfs_Error {
    mfs_ErrorCode code;
    char const* message;  // null-terminated.
} mfs_Error;

/*
    Pointer + Length pair. String data is expected to be encoded as UTF-8.
*/
#if defined(MTB_INCLUDED)
using mfs_String = ::mtb::tSlice<char const>;
#else
typedef struct mfs_String {
    char const* ptr;
    ptrdiff_t len;
} mfs_String;
#endif

typedef enum mfs_PathType {
    mfs_PathType_Auto,
    mfs_PathType_Windows,
    mfs_PathType_Posix,
} mfs_PathType;

typedef struct mfs_ResolvedPath {
    mfs_Error error;
    char const* ptr;
    ptrdiff_t len;
} mfs_ResolvedPath;

typedef struct mfs_EntireFile {
    mfs_Error error;
    uint8_t* data;
    size_t size;
} mfs_EntireFile;

typedef struct mfs_CreateDirectoriesResult {
    mfs_Error error;
    bool already_exists;
} mfs_CreateDirectoriesResult;

typedef struct mfs_FileIterator {
    mfs_Error error;

    mfs_String file_path;
    mfs_String base_name;

    bool is_file;
    bool is_dir;
    bool is_symlink;

    bool read_only_flag;
    bool hidden_flag;
    bool system_flag;

    size_t file_size;

    void* internals;
} mfs_FileIterator;

typedef struct mfs_Allocator {
    void* (*realloc_cb)(void* user_data, void* old_ptr, size_t old_size, size_t new_size);
    void* user_data;
} mfs_Allocator;

typedef struct mfs_SetupDesc {
    mfs_PathType path_type;

    /*
        Custom allocator function.
    */
    mfs_Allocator allocator;
} mfs_SetupDesc;

/*
    [Section] Public API
*/

/*
    Initialize mfs internal state.
*/
MFS_FN void mfs_Setup(mfs_SetupDesc const* setup_desc);

/*
    Reset the internal mfs state. This frees all allocated memory, etc.
    Can be called multiple times in succession and is ready for use as if mfs_Setup was called on it the first time.
*/
MFS_FN void mfs_Reset(void);

MFS_FN char const* mfs_ErrorCodeToString(mfs_ErrorCode error_code);

/*
    Construct a mfs_String from a zero-terminated string.
*/
MFS_FN mfs_String mfs_StringZ(char const* zero_terminated_string);

/*
    Drive letter, if any, with trailing colon. For non-windows paths this
    always returns an empty string.

    "C:\Program Files\" -> "C:"
    "\Program Files\"   -> ""
    "C:/Program Files/" -> "C:"
    "/Program Files/"   -> ""
    "/"                 -> ""
*/
MFS_FN mfs_String mfs_Drive(mfs_String path);
MFS_FN mfs_String mfs_DriveZ(char const* path_cstr);

/*
    Local or global root of the given `path`.

    "C:\Program Files\" -> "\"
    "\Program Files\" -> "\"
    "C:Program Files\" -> ""
    "/etc/foo" -> "/"
*/
MFS_FN mfs_String mfs_Root(mfs_String path);
MFS_FN mfs_String mfs_RootZ(char const* path_cstr);

/*
    The combination drive + root.

    "C:\Program Files\" -> "C:\"
    "\Program Files\"   -> "\"
    "C:Program Files\"  -> "C:"
    "/etc/foo"          -> "/"
*/
MFS_FN mfs_String mfs_Anchor(mfs_String path);
MFS_FN mfs_String mfs_AnchorZ(char const* path_cstr);

/*
    The parent path.

    "C:\foo\bar\baz.txt" -> "C:\foo\bar"
    "C:\foo\bar\"        -> "C:\foo"
    "C:\foo\bar"         -> "C:\foo"
    "C:\"                -> "C:\"
*/
MFS_FN mfs_String mfs_DirName(mfs_String path);
MFS_FN mfs_String mfs_DirNameZ(char const* path_cstr);

/*
    The final part of the path.

    "C:\foo\bar\baz.txt" -> "baz.txt"
    "C:\foo\bar\"        -> "bar"
    "C:\foo\bar"         -> "bar"
    "C:\"                -> ""
*/
MFS_FN mfs_String mfs_BaseName(mfs_String path);
MFS_FN mfs_String mfs_BaseNameZ(char const* path_cstr);

/*
    The suffix of the base name of the path. Also known as "file extension".
    Includes a leading dot, if a suffix is found.

    "C:\foo\bar\baz.txt"     -> ".txt"
    "C:\foo\bar\baz.txt.zip" -> ".zip"
    "C:\foo\bar\"            -> ""
    "C:\foo\bar"             -> ""
    "C:\"                    -> ""
*/
MFS_FN mfs_String mfs_Suffix(mfs_String path);
MFS_FN mfs_String mfs_SuffixZ(char const* path_cstr);

/*
    The final part of the path without the suffix.

    "C:\foo\bar\baz.txt"     -> "baz"
    "C:\foo\bar\baz.txt.zip" -> "baz.txt"
    "C:\foo\bar\"            -> "bar"
    "C:\foo\bar"             -> "bar"
    "C:\"                    -> ""
*/
MFS_FN mfs_String mfs_BaseNameWithoutSuffix(mfs_String path);
MFS_FN mfs_String mfs_BaseNameWithoutSuffixZ(char const* path_cstr);

/*
    The full path without suffix.

    "C:\foo\bar\baz.txt"     -> "C:\foo\bar\baz"
    "C:\foo\bar\baz.txt.zip" -> "C:\foo\bar\baz.txt"
    "C:\foo\bar\"            -> "C:\foo\bar"
    "C:\foo\bar"             -> "C:\foo\bar"
    "C:\"                    -> "C:\"
*/
MFS_FN mfs_String mfs_WithoutSuffix(mfs_String path);
MFS_FN mfs_String mfs_WithoutSuffixZ(char const* path_cstr);

/*
    Produces a fully qualified and canonicalized path.
*/
MFS_FN mfs_ResolvedPath mfs_ResolvePath(mfs_String path_utf8);
MFS_FN mfs_ResolvedPath mfs_ResolvePathZ(char const* path_utf8);

/*
    Try to read the entire contents of a file. The result data will remain
    valid until mfs_Reset() is called.
*/
MFS_FN mfs_EntireFile mfs_ReadEntireFile(mfs_String path_utf8);
MFS_FN mfs_EntireFile mfs_ReadEntireFileZ(char const* path_utf8);

/*
    Create all directories. This will fail if `path_utf8` points to an existing file.
*/
MFS_FN mfs_CreateDirectoriesResult mfs_CreateDirectories(mfs_String path_utf8);
MFS_FN mfs_CreateDirectoriesResult mfs_CreateDirectoriesZ(char const* path_utf8);

/*
    Iterate files under the given directory.
 */
MFS_FN mfs_FileIterator mfs_OpenFileIterator(mfs_String path_utf8);
MFS_FN mfs_FileIterator mfs_OpenFileIteratorZ(char const* path_utf8);

/*
    Close the given file iterator.
 */
void mfs_CloseFileIterator(mfs_FileIterator* iter);

/*
    Advance the given file iterator. May not be NULL.
 */
MFS_FN bool mfs_AdvanceFileIterator(mfs_FileIterator* iter);

#endif /* MFS_INCLUDED */

/*
    [Section] Implementation
*/
#ifdef MFS_IMPLEMENTATION

/* Guard against mutli-includes */
#ifdef MFS__IMPLEMENTATION_GUARD
#error "IMPLEMENTATION of mtb_filesystem included multiple times!"
#endif /* MFS__IMPLEMENTATION_GUARD */
#define MFS__IMPLEMENTATION_GUARD

#include <stdlib.h>
#include <string.h>
#include <stdio.h>   // snprintf
#include <stdarg.h>  // va_list, va_start, va_end

/*
    [Section] Backend configuration
*/
#if defined(MFS_BACKEND_WIN32)
#define MFS__WIN32 1
#else
#define MFS__WIN32 0
#endif

#if defined(MFS_BACKEND_POSIX)
#define MFS__POSIX 1
#else
#define MFS__POSIX 0
#endif

#if(MFS__WIN32 + MFS__POSIX) > 1
#error "Multiple backends are not allowed, please define only one of: MFS_BACKEND_POSIX, MFS_BACKEND_WIN32"
#endif

#if(MFS__WIN32 + MFS__POSIX) == 0
/* Try to determine a backend automatically */
#if defined(_WIN32)
#undef MFS__WIN32
#define MFS__WIN32 1
#endif
#endif

#if(MFS__POSIX + MFS__WIN32) == 0
#error "Please select a backend: MFS_BACKEND_POSIX, MFS_BACKEND_WIN32"
#endif

/*
    [Section] Internal helpers
*/

#define MFS__STRINGIFY(THING) MFS__STRINGIRY_MORE(THING)
#define MFS__STRINGIRY_MORE(THING) #THING

#if !defined(MFS_ASSERT)
#define MFS_ASSERT(CONDITION) \
    do {                      \
        if(!(CONDITION)) {    \
            *(int*)0 = 0;     \
        }                     \
    } while(0)
#endif

#if !defined(MFS_FATAL)
#define MFS_FATAL() /* Trigger an access violation on most platforms. */ *(int*)0 = 0
#endif

#if !defined(MFS_MAKE_ERROR)
#define MFS_MAKE_ERROR mfs__MakeError

static char mfs__error_buf[2 * 1024];

static mfs_Error mfs__MakeError(mfs_ErrorCode ec, char const* format, ...) {
    mfs_Error result;
    result.code = ec;
    va_list args;
    va_start(args, format);
    (void)vsnprintf(mfs__error_buf, sizeof(mfs__error_buf), format, args);  // NOLINT(clang-diagnostic-format-nonliteral)
    va_end(args);
    result.message = mfs__error_buf;
    return result;
}
#endif  // MFS_MAKE_ERROR

static mfs_Error mfs_NoError() {
    mfs_Error result = MFS_ZERO_INIT();
    return result;
}

// ReSharper disable once CppDefaultInitializationWithNoUserConstructor
static const mfs_Allocator mfs__no_allocator;

static void* mfs__LibcReallocProc(void* user_data, void* old_ptr, size_t old_size, size_t new_size) {
    (void)user_data;
    (void)old_size;
    return realloc(old_ptr, new_size);
}

typedef struct mfs__Arena {
    uint8_t* ptr;
    size_t cap;
    size_t fill;
    mfs__Arena* prev;
} mfs__Arena;

static const size_t arena_header_size = sizeof(mfs__Arena);

static mfs__Arena* mfs__EmbedArena(void* ptr, size_t size, size_t fill) {
    MFS_ASSERT(size >= arena_header_size);
    mfs__Arena* arena = (mfs__Arena*)ptr;
    arena->ptr = (uint8_t*)ptr + arena_header_size;
    arena->cap = size - arena_header_size;
    arena->fill = fill;
    arena->prev = NULL;
    return arena;
}

struct mfs__ReallocResult {
    mfs_Error error;
    void* new_ptr;
};

static mfs__ReallocResult mfs__ArenaRealloc(mfs__Arena** arena_chain, mfs_Allocator allocator, void* old_ptr, size_t old_size, size_t new_size) {
    mfs__ReallocResult result = MFS_ZERO_INIT();

    MFS_ASSERT(arena_chain);
    if(!arena_chain) {
        result.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidOperation, "Bad realloc: arena_chain argument is NULL.");
        return result;
    }

    mfs__Arena* arena = *arena_chain;

    MFS_ASSERT(old_ptr || new_size);
    if(!(old_ptr || new_size)) {
        result.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidOperation, "Bad realloc: old_ptr is NULL and new_size is zero.");
        return result;
    }

    if(old_ptr) {
        MFS_ASSERT(old_size);
        if(!old_size) {
            result.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidOperation, "Bad realloc: old_ptr is non-NULL but old_size is zero.");
            return result;
        }

        MFS_ASSERT(arena);
        if(!arena) {
            result.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidOperation, "Bad realloc: old_ptr is non-NULL but no current allocation exists.");
            return result;
        }

        if(old_size < new_size) {
            size_t delta = new_size - old_size;
            if(old_ptr == arena->ptr + arena->fill - old_size && arena->fill + delta <= arena->cap - arena->fill) {
                /* Resize previous allocation */
                arena->fill += delta;
                result.new_ptr = old_ptr;
                return result;
            }
        } else if(old_size > new_size) {
            size_t delta = new_size - old_size;
            if(old_ptr == arena->ptr + arena->fill - old_size) {
                arena->fill -= delta;
            }
            result.new_ptr = old_ptr;
            return result;
        } else {
            /* Unchanged size requirements. Weird but okay. */
            result.new_ptr = old_ptr;
            return result;
        }
    } else {
        MFS_ASSERT(!old_size);
        if(old_size) {
            result.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidOperation, "Bad realloc: old_ptr is NULL but old_size is non-zero.");
            return result;
        }
    }

    if(arena && arena->fill + new_size <= arena->cap) {
        /*
            Try sub-allocating from the current allocation
        */
        result.new_ptr = arena->ptr + arena->fill;
        arena->fill += new_size;
    } else {
        /*
            Create a new allocation.
        */
        size_t new_allocation_size = arena_header_size + new_size;
        if(new_allocation_size < 4096) {
            new_allocation_size = 4096;
        }
        if(!allocator.realloc_cb) {
            result.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidOperation, "Bad realloc: realloc_cb is NULL.");
            return result;
        }
        mfs__Arena* new_arena = (mfs__Arena*)allocator.realloc_cb(allocator.user_data, NULL, 0, new_allocation_size);
        if(!new_arena) {
            result.error = MFS_MAKE_ERROR(mfs_ErrorCode_OutOfMemory, "Bad realloc: realloc_cb returned NULL - Out of memory.");
            return result;
        }

        new_arena->ptr = (uint8_t*)new_arena + arena_header_size;
        MFS_ASSERT(new_arena->ptr == (uint8_t*)(new_arena + 1));
        new_arena->cap = new_allocation_size - arena_header_size;
        new_arena->fill = new_size;
        new_arena->prev = arena;

        result.new_ptr = new_arena->ptr;
        *arena_chain = new_arena;
    }

    if(old_ptr) {
        MFS_ASSERT(old_size);
        memcpy(result.new_ptr, old_ptr, old_size);
    }

    return result;
}

static void mfs__FreeArenaChain(mfs__Arena* arena, mfs_Allocator allocator) {
    MFS_ASSERT(allocator.realloc_cb);
    if(!allocator.realloc_cb) {
        return;
    }
    while(arena) {
        mfs__Arena* to_free = arena;
        arena = arena->prev;
        void* free_result = allocator.realloc_cb(allocator.user_data, to_free, arena_header_size + to_free->cap, 0);
        MFS_ASSERT(free_result == NULL);
    }
}

// TODO: Is there a *good* reason to have this limit?
#define MFS__FILE_NAME_LIMIT 4096

typedef struct mfs__State {
    bool ready;
    mfs_SetupDesc desc;
    mfs_PathType path_type;

    mfs_Allocator allocator;
    mfs__Arena* arena;

    uint8_t buf[14 * 1024];
} mfs__State;

static mfs__State mfs__state;

static mfs__ReallocResult mfs__GlobalRealloc(void* old_ptr, size_t old_size, size_t new_size) {
    MFS_ASSERT(mfs__state.ready);
    if(!mfs__state.ready) {
        mfs__ReallocResult result = MFS_ZERO_INIT();
        result.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidOperation, "Not initialized. Did you forget to call mfs_Setup?");
        return result;
    }
    return mfs__ArenaRealloc(&mfs__state.arena, mfs__state.allocator, old_ptr, old_size, new_size);
}

static bool mfs__IsDirSeparator(char c) {
    switch(mfs__state.path_type) {
        case mfs_PathType_Windows: return c == '\\' || c == '/';
        case mfs_PathType_Posix: return c == '/';
        case mfs_PathType_Auto:
        default:
            MFS_FATAL();
            return false;
    }
}

static ptrdiff_t mfs__TrimTrailingDirSeparators(char const* ptr, ptrdiff_t len) {
    while(len > 0) {
        if(!mfs__IsDirSeparator(ptr[len - 1])) {
            break;
        }
        --len;
    }
    return len;
}

/*
    [Section] POSIX Backend
*/

#if MFS__POSIX
#include <stdlib.h>
#include <stdio.h>

static mfs_EntireFile mfs__posix_ReadEntireFile(mfs_String path_utf8) {
    mfs_EntireFile result = MFS_ZERO_INIT();

    if(!path_utf8.len) {
        result.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidFileName, "ReadEntireFile: The given file name is empty.");
        return result;
    }

    if(path_utf8.len > MFS__FILE_NAME_LIMIT) {
        result.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidFileName, "ReadEntireFile: The given file name is too large (must be <= " MFS__STRINGIFY(MFS__FILE_NAME_LIMIT) ").");
        return result;
    }

    memcpy(&mfs__state.buf[0], path_utf8.ptr, path_utf8.len);
    mfs__state.buf[path_utf8.len] = 0;
    char const* file_name_z = (char const*)&mfs__state.buf[0];

    FILE* file;
    errno_t file_error = fopen_s(&file, file_name_z, "rb");
    if(file_error != 0) {
        result.error = MFS_MAKE_ERROR(mfs_ErrorCode_Unkown, "ReadEntireFile: Unable to open file.");
        return result;
    }

    if(fseek(file, 0, SEEK_END) != 0) {
        result.error = MFS_MAKE_ERROR(mfs_ErrorCode_Unkown, "ReadEntireFile: Unable to determine file size.");
        return result;
    }

    size_t file_size = (size_t)ftell(file);

    if(fseek(file, 0, SEEK_SET) != 0) {
        result.error = MFS_MAKE_ERROR(mfs_ErrorCode_Unkown, "ReadEntireFile: Unable to determine file size...");
        return result;
    }

    mfs__ReallocResult alloc_result = mfs__GlobalRealloc(NULL, 0, file_size);
    if(alloc_result.error.code) {
        result.error = alloc_result.error;
        return result;
    }
    uint8_t* data = (uint8_t*)alloc_result.new_ptr;

    size_t num_read = fread(data, 1, file_size, file);
    if(num_read != file_size) {
        result.error = MFS_MAKE_ERROR(mfs_ErrorCode_Unkown, "ReadEntireFile: Unable to read file contents...");
        return result;
    }

    if(fclose(file) != 0) {
        result.error = MFS_MAKE_ERROR(mfs_ErrorCode_Unkown, "ReadEntireFile: Unable to close file?!");
        return result;
    }

    result.data = data;
    result.size = file_size;
    return result;
}

static mfs_CreateDirectoriesResult mfs__posix_CreateDirectories(mfs_String path_utf8) {
    mfs_CreateDirectoriesResult result = MFS_ZERO_INIT();
    result.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidOperation, "NOT IMPLEMENTED");
    return result;
}

static mfs_FileIterator mfs__posix_OpenFileIterator(mfs_String path_utf8) {
    mfs_FileIterator result = MFS_ZERO_INIT();
    result.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidOperation, "NOT IMPLEMENTED");
    return result;
}

#endif  // MFS__POSIX

/*
    [Section] WIN32 Backend
*/

#if MFS__WIN32

#include <Windows.h>

typedef struct mfs__win32_WideStringResult {
    mfs_Error error;
    wchar_t* ptr;
    size_t allocation_size;
    ptrdiff_t len;  // Number of characters.
} mfs__win32_WideStringResult;

static mfs__win32_WideStringResult mfs__win32_ConvertToWideString(mfs__Arena** arena, mfs_Allocator allocator, char const* str_ptr, ptrdiff_t str_len) {
    mfs__win32_WideStringResult result = MFS_ZERO_INIT();

    ptrdiff_t required_len = (ptrdiff_t)MultiByteToWideChar(
        CP_UTF8,       // UINT                              CodePage,
        0,             // DWORD                             dwFlags,
        str_ptr,       // _In_NLS_string_(cbMultiByte)LPCCH lpMultiByteStr,
        (int)str_len,  // int                               cbMultiByte,
        NULL,          // LPWSTR                            lpWideCharStr,
        0              // int                               cchWideChar
    );

    if(str_len < 0) {
        result.len = required_len > 0 ? required_len - 1 : 0;
    } else {
        result.len = required_len;
    }

    result.allocation_size = (result.len + 1) * sizeof(wchar_t);
    mfs__ReallocResult alloc_result = mfs__ArenaRealloc(arena, allocator, NULL, 0, result.allocation_size);
    if(alloc_result.error.code) {
        result.error = alloc_result.error;
        return result;
    }

    result.ptr = (wchar_t*)alloc_result.new_ptr;

    (void)MultiByteToWideChar(
        CP_UTF8,         // UINT                              CodePage,
        0,               // DWORD                             dwFlags,
        str_ptr,         // _In_NLS_string_(cbMultiByte)LPCCH lpMultiByteStr,
        (int)str_len,    // int                               cbMultiByte,
        result.ptr,      // LPWSTR                            lpWideCharStr,
        (int)result.len  // int                               cchWideChar
    );

    result.ptr[result.len] = 0;
    return result;
}

typedef struct mfs__win32_Utf8Result {
    mfs_Error error;
    char* ptr;
    size_t allocation_size;
    ptrdiff_t len;  // Number of characters.
} mfs__win32_Utf8Result;

static mfs__win32_Utf8Result mfs__win32_ConvertToUtf8(mfs__Arena** arena, mfs_Allocator allocator, wchar_t* str_ptr, ptrdiff_t str_len) {
    mfs__win32_Utf8Result result = MFS_ZERO_INIT();

    ptrdiff_t required_len = (ptrdiff_t)WideCharToMultiByte(
        CP_UTF8,       // [in]            UINT                               CodePage,
        0,             // [in]            DWORD                              dwFlags,
        str_ptr,       // [in]            _In_NLS_string_(cchWideChar)LPCWCH lpWideCharStr,
        (int)str_len,  // [in]            int                                cchWideChar,
        NULL,          // [out, optional] LPSTR                              lpMultiByteStr,
        0,             // [in]            int                                cbMultiByte,
        NULL,          // [in, optional]  LPCCH                              lpDefaultChar,
        NULL           // [out, optional] LPBOOL                             lpUsedDefaultChar
    );

    // NOTE: When we pass -1 as the input length, the returned length includes the trailing null-char.
    if(str_len < 0) {
        result.len = required_len > 0 ? required_len - 1 : 0;
    } else {
        result.len = required_len;
    }

    result.allocation_size = result.len + 1;
    mfs__ReallocResult alloc_result = mfs__ArenaRealloc(arena, allocator, NULL, 0, result.allocation_size);
    if(alloc_result.error.code) {
        result.error = alloc_result.error;
        return result;
    }

    result.ptr = (char*)alloc_result.new_ptr;

    (void)WideCharToMultiByte(
        CP_UTF8,          // [in]            UINT                               CodePage,
        0,                // [in]            DWORD                              dwFlags,
        str_ptr,          // [in]            _In_NLS_string_(cchWideChar)LPCWCH lpWideCharStr,
        (int)str_len,     // [in]            int                                cchWideChar,
        result.ptr,       // [out, optional] LPSTR                              lpMultiByteStr,
        (int)result.len,  // [in]            int                                cbMultiByte,
        NULL,             // [in, optional]  LPCCH                              lpDefaultChar,
        NULL              // [out, optional] LPBOOL                             lpUsedDefaultChar
    );

    result.ptr[result.len] = 0;
    return result;
}

static mfs_ResolvedPath mfs__win32_ResolvePath(mfs_String path_utf8) {
    mfs_ResolvedPath result{};
    if(!mfs__state.ready) {
        result.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidOperation, "Not initialized. Did you forget to call mfs_Setup?");
        return result;
    }

    mfs__Arena* temp_arena = mfs__EmbedArena(mfs__state.buf, sizeof(mfs__state.buf), 0);
    mfs__win32_WideStringResult path_win32 = mfs__win32_ConvertToWideString(&temp_arena, mfs__no_allocator, path_utf8.ptr, path_utf8.len);
    if(path_win32.error.code) {
        result.error = path_win32.error;
        return result;
    }

    // NOTE: GetFullPathNameW returns a length in wchar_t's. The terminating null-character is only included in that length if the input buffer is NULL.

    DWORD full_path_len_with_nullchar = GetFullPathNameW(path_win32.ptr, 0, NULL, NULL);

    mfs__ReallocResult dest_buffer_result = mfs__ArenaRealloc(&temp_arena, mfs__no_allocator, NULL, 0, sizeof(wchar_t) * full_path_len_with_nullchar);
    if(dest_buffer_result.error.code) {
        result.error = dest_buffer_result.error;
        return result;
    }
    wchar_t* full_path_ptr = (wchar_t*)dest_buffer_result.new_ptr;
    DWORD full_path_len = GetFullPathNameW(path_win32.ptr, full_path_len_with_nullchar, full_path_ptr, NULL);
    MFS_ASSERT(full_path_len + 1 == full_path_len_with_nullchar);

    mfs__win32_Utf8Result full_path_utf8 = mfs__win32_ConvertToUtf8(&temp_arena, mfs__no_allocator, full_path_ptr, full_path_len);
    if(full_path_utf8.error.code) {
        result.error = full_path_utf8.error;
        return result;
    }

    result.ptr = full_path_utf8.ptr;
    result.len = full_path_utf8.len;
    return result;
}

static mfs_EntireFile mfs__win32_ReadEntireFile(mfs_String path_utf8) {
    mfs_EntireFile result = MFS_ZERO_INIT();

    if(!mfs__state.ready) {
        result.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidOperation, "Not initialized. Did you forget to call mfs_Setup?");
        return result;
    }

    mfs__Arena* temp_arena = mfs__EmbedArena(mfs__state.buf, sizeof(mfs__state.buf), 0);
    mfs__win32_WideStringResult path_win32 = mfs__win32_ConvertToWideString(&temp_arena, mfs__no_allocator, path_utf8.ptr, path_utf8.len);
    if(path_win32.error.code) {
        result.error = path_win32.error;
        return result;
    }

    HANDLE file = CreateFileW(
        path_win32.ptr,   // [in]           LPCWSTR               lpFileName,
        GENERIC_READ,     // [in]           DWORD                 dwDesiredAccess,
        FILE_SHARE_READ,  // [in]           DWORD                 dwShareMode,
        NULL,             // [in, optional] LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        OPEN_EXISTING,    // [in]           DWORD                 dwCreationDisposition,
        0,                // [in]           DWORD                 dwFlagsAndAttributes,
        NULL              // [in, optional] HANDLE                hTemplateFile
    );
    if(file == INVALID_HANDLE_VALUE) {
        result.error = MFS_MAKE_ERROR(mfs_ErrorCode_Unkown, "ReadEntireFile: Unable to open file for reading.");
        return result;
    }

    LARGE_INTEGER file_size;
    if(!GetFileSizeEx(file, &file_size)) {
        result.error = MFS_MAKE_ERROR(mfs_ErrorCode_Unkown, "ReadEntireFile: Unable to determine file size.");
        CloseHandle(file);
        return result;
    }

    uint8_t* ptr = nullptr;
    size_t size = file_size.QuadPart;
    if(size) {
        mfs__ReallocResult alloc_result = mfs__GlobalRealloc(NULL, 0, size);
        if(alloc_result.error.code) {
            result.error = alloc_result.error;
            CloseHandle(file);
            return result;
        }
        ptr = (uint8_t*)alloc_result.new_ptr;
    }

    size_t remain = size;
    while(remain > 0) {
        DWORD part = remain > UINT32_MAX ? (DWORD)UINT32_MAX : (DWORD)remain;
        DWORD bytes_read;
        bool ok = ReadFile(
            file,                   // [in]                HANDLE       hFile,
            ptr + (size - remain),  // [out]               LPVOID       lpBuffer,
            part,                   // [in]                DWORD        nNumberOfBytesToRead,
            &bytes_read,            // [out, optional]     LPDWORD      lpNumberOfBytesRead,
            NULL                    // [in, out, optional] LPOVERLAPPED lpOverlapped
        );
        if(!ok || bytes_read != part) {
            result.error = MFS_MAKE_ERROR(mfs_ErrorCode_Unkown, "ReadEntireFile: Unable to read file contents.");
            CloseHandle(file);
            return result;
        }
        remain -= bytes_read;
    }

    result.data = ptr;
    result.size = size;
    CloseHandle(file);
    return result;
}

static mfs_CreateDirectoriesResult mfs__win32_CreateDirectories(mfs_String path_utf8) {
    mfs_CreateDirectoriesResult result = MFS_ZERO_INIT();

    char const* name_ptr = path_utf8.ptr;
    ptrdiff_t name_len = mfs__TrimTrailingDirSeparators(path_utf8.ptr, path_utf8.len);
    if(!name_len) {
        // Assume an essentially empty path is already created.
        result.error = mfs_NoError();
        result.already_exists = true;
        return result;
    }

    if(name_len == 1 && name_ptr[0] == '.') {
        // Assume cwd already exists.
        result.error = mfs_NoError();
        result.already_exists = true;
        return result;
    }

    if(name_len > MFS__FILE_NAME_LIMIT) {
        result.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidFileName, "CreateDirectories: The given file name is too large (must be <= " MFS__STRINGIFY(MFS__FILE_NAME_LIMIT) ").");
        return result;
    }

    ptrdiff_t end = 0;
    while(end < name_len) {
        while(end < name_len) {
            if(mfs__IsDirSeparator(name_ptr[end])) {
                break;
            }
            ++end;
        }

        mfs__Arena* temp_arena = mfs__EmbedArena(mfs__state.buf, sizeof(mfs__state.buf), 0);
        mfs__win32_WideStringResult path_win32 = mfs__win32_ConvertToWideString(&temp_arena, mfs__no_allocator, path_utf8.ptr, path_utf8.len);

        bool ok = CreateDirectoryW(path_win32.ptr, NULL) != 0;
        DWORD error = GetLastError();
        if(!ok && error == ERROR_ALREADY_EXISTS) {
            ok = true;
        }
        if(!ok) {
            result.error = MFS_MAKE_ERROR(mfs_ErrorCode_PermissionDenied, "CreateDirectories: Unable to create directories.");
            break;
        }

        ++end;
        while(end < name_len) {
            if(!mfs__IsDirSeparator(name_ptr[end])) {
                break;
            }
            ++end;
        }
    }

    result.already_exists = GetLastError() == ERROR_ALREADY_EXISTS;
    return result;
}

typedef struct mfs__win32_FileIterator {
    HANDLE win32_handle;
    WIN32_FIND_DATAW win32_data;
    uint8_t buf[12 * 1024];
    ptrdiff_t arena_fill;
    char* dir_ptr;
    size_t dir_len;
} mfs__win32_FileIterator;

static mfs_FileIterator mfs__win32_OpenFileIterator(mfs_String dir_utf8) {
    mfs_FileIterator iter = MFS_ZERO_INIT();
    MFS_ASSERT(iter.error.code == mfs_ErrorCode_None);

    if(!mfs__state.ready) {
        iter.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidOperation, "Not initialized. Did you forget to call mfs_Setup?");
        return iter;
    }

    mfs__Arena* temp_arena = mfs__EmbedArena(mfs__state.buf, sizeof(mfs__state.buf), 0);
    mfs__win32_WideStringResult converted_input_dir = mfs__win32_ConvertToWideString(&temp_arena, mfs__no_allocator, dir_utf8.ptr, dir_utf8.len);
    if(converted_input_dir.error.code) {
        iter.error = converted_input_dir.error;
        return iter;
    }

    // NOTE: GetFullPathNameW returns a length in wchar_t's. The terminating null-character is only included in that length if the input buffer is NULL.

    DWORD full_path_len_with_nullchar = GetFullPathNameW(converted_input_dir.ptr, 0, NULL, NULL);
    if(full_path_len_with_nullchar == 0) {
        iter.error = MFS_MAKE_ERROR(mfs_ErrorCode_Unkown, "GetFullPathNameW failed.");
        return iter;
    }

    // NOTE: Allocate two more chars so we can append "\\*" later.
    DWORD full_path_allocation_size_in_wchars = full_path_len_with_nullchar + 2;
    auto full_path_allocation = mfs__ArenaRealloc(&temp_arena, mfs__no_allocator, NULL, 0, sizeof(wchar_t) * full_path_allocation_size_in_wchars);
    if(full_path_allocation.error.code) {
        iter.error = converted_input_dir.error;
        return iter;
    }
    auto* full_path = (wchar_t*)full_path_allocation.new_ptr;
    DWORD full_path_len = GetFullPathNameW(converted_input_dir.ptr, full_path_allocation_size_in_wchars, full_path, NULL);
    MFS_ASSERT(full_path_len + 1 == full_path_len_with_nullchar);

    full_path[full_path_len + 0] = '\\';
    full_path[full_path_len + 1] = '*';
    full_path[full_path_len + 2] = 0;

    mfs__ReallocResult internals_alloc_result = mfs__GlobalRealloc(NULL, 0, sizeof(mfs__win32_FileIterator));
    if(internals_alloc_result.error.code) {
        iter.error = internals_alloc_result.error;
        return iter;
    }
    mfs__win32_FileIterator* win32_iter = (mfs__win32_FileIterator*)internals_alloc_result.new_ptr;

    win32_iter->win32_handle = FindFirstFileW(full_path, &win32_iter->win32_data);
    if(win32_iter->win32_handle == INVALID_HANDLE_VALUE) {
        if(GetLastError() == ERROR_FILE_NOT_FOUND) {
            iter.error = MFS_MAKE_ERROR(mfs_ErrorCode_NotFound, "Unable to find file '%.*s'", (int)dir_utf8.len, dir_utf8.ptr);
        } else {
            iter.error = MFS_MAKE_ERROR(mfs_ErrorCode_Unkown, "FindFirstFileW failed");  // TODO: Get windows error message as string and embed using a temp buffer?
        }
        mfs__GlobalRealloc(win32_iter, sizeof(mfs__win32_FileIterator), 0);
        return iter;
    }
    mfs__Arena* iter_arena = mfs__EmbedArena(win32_iter->buf, sizeof(win32_iter->buf), 0);

    mfs__win32_Utf8Result path_copy = mfs__win32_ConvertToUtf8(&iter_arena, mfs__no_allocator, full_path, (ptrdiff_t)full_path_len);
    if(path_copy.error.code) {
        // iter.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidFileName, "Input path is too long");
        iter.error = path_copy.error;
        mfs__GlobalRealloc(win32_iter, sizeof(mfs__win32_FileIterator), 0);
        return iter;
    }

    win32_iter->dir_len = path_copy.len;
    win32_iter->dir_ptr = path_copy.ptr;
    win32_iter->arena_fill = iter_arena->fill;
    iter.internals = win32_iter;
    return iter;
}

static void mfs__win32_CloseFileIterator(mfs_FileIterator* iter) {
    if(!iter) {
        return;
    }

    if(iter->internals) {
        mfs__win32_FileIterator* win32_iter = (mfs__win32_FileIterator*)iter->internals;
        if(win32_iter->win32_handle != INVALID_HANDLE_VALUE) {
            (void)FindClose(win32_iter->win32_handle);
        }

        mfs__GlobalRealloc(iter->internals, sizeof(mfs__win32_FileIterator), 0);
        iter->internals = NULL;
    }
}

static bool mfs__win32_AdvanceFileIterator(mfs_FileIterator* iter) {
    MFS_ASSERT(iter);
    if(!iter) {
        return false;
    }

    iter->error = MFS_ZERO_INIT();

    if(!iter->internals) {
        return false;
    }

    mfs__win32_FileIterator* win32_iter = (mfs__win32_FileIterator*)iter->internals;
    MFS_ASSERT(win32_iter->win32_handle != INVALID_HANDLE_VALUE);
    if(win32_iter->win32_handle == INVALID_HANDLE_VALUE) {
        iter->error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidOperation, "Invalid internal iterator state. (BUG)");
        mfs__win32_CloseFileIterator(iter);
        return false;
    }

    bool skip = true;
    while(skip) {
        skip = false;

        // Skip '.'
        if(win32_iter->win32_data.cFileName[0] == '.' && win32_iter->win32_data.cFileName[1] == 0) {
            skip = true;
        }

        // Skip '..'
        if(win32_iter->win32_data.cFileName[0] == '.' && win32_iter->win32_data.cFileName[1] == '.' && win32_iter->win32_data.cFileName[2] == 0) {
            skip = true;
        }

        if(skip) {
            if(!FindNextFileW(win32_iter->win32_handle, &win32_iter->win32_data)) {
                mfs__win32_CloseFileIterator(iter);
                return false;
            }
        }
    }

    mfs__Arena* arena = mfs__EmbedArena(win32_iter->buf, sizeof(win32_iter->buf), win32_iter->arena_fill);
    mfs__ReallocResult dir_copy_result = mfs__ArenaRealloc(&arena, mfs__no_allocator, NULL, 0, win32_iter->dir_len + 1);
    if(dir_copy_result.error.code) {
        iter->error = MFS_MAKE_ERROR(mfs_ErrorCode_OutOfMemory, "Internal arena exhausted.");
        return false;
    }

    char* full_path = (char*)dir_copy_result.new_ptr;
    memmove(full_path, win32_iter->dir_ptr, win32_iter->dir_len);
    full_path[win32_iter->dir_len] = '\\';

    mfs__win32_Utf8Result file_name = mfs__win32_ConvertToUtf8(&arena, mfs__no_allocator, win32_iter->win32_data.cFileName, -1);
    if(file_name.error.code) {
        iter->error = file_name.error;
        return false;
    }

    iter->file_path.ptr = full_path;
    iter->file_path.len = win32_iter->dir_len + 1 + file_name.len;

    iter->base_name.ptr = file_name.ptr;
    iter->base_name.len = file_name.len;

    iter->is_dir = (win32_iter->win32_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    iter->is_file = (win32_iter->win32_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
    iter->is_symlink = (win32_iter->win32_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0;

    iter->read_only_flag = (win32_iter->win32_data.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
    iter->hidden_flag = (win32_iter->win32_data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0;
    iter->system_flag = (win32_iter->win32_data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != 0;

    iter->file_size = (size_t)win32_iter->win32_data.nFileSizeHigh * ((size_t)MAXDWORD + 1) + (size_t)win32_iter->win32_data.nFileSizeLow;

    // Advance to the next file.
    if(!FindNextFileW(win32_iter->win32_handle, &win32_iter->win32_data)) {
        // There are no more files left so close up shop and free allocated memory.
        mfs__win32_CloseFileIterator(iter);
    }
    return true;
}

#endif /* MFS__WIN32 */

/*
    [Section] Public API Implementation
*/

void mfs_Setup(mfs_SetupDesc const* setup_desc) {
    MFS_ASSERT(!mfs__state.ready);
    MFS_ASSERT(setup_desc);
    mfs__state.desc = *setup_desc;
    if(setup_desc->path_type == mfs_PathType_Auto) {
        // ReSharper disable once CppUnreachableCode
        if(MFS__WIN32) {
            mfs__state.path_type = mfs_PathType_Windows;
        } else {
            mfs__state.path_type = mfs_PathType_Posix;
        }
    } else {
        mfs__state.path_type = setup_desc->path_type;
    }
    if(setup_desc->allocator.realloc_cb) {
        mfs__state.allocator = setup_desc->allocator;
    } else {
        mfs__state.allocator.realloc_cb = &mfs__LibcReallocProc;
        mfs__state.allocator.user_data = NULL;
    }
    mfs__state.ready = true;
}

void mfs_Reset(void) {
    if(!mfs__state.ready) {
        return;
    }

    if(mfs__state.arena) {
        mfs__FreeArenaChain(mfs__state.arena, mfs__state.allocator);
        mfs__state.arena = NULL;
    }
}

char const* mfs_ErrorCodeToString(mfs_ErrorCode error_code) {
    switch(error_code) {
        case mfs_ErrorCode_None: return "None";
        case mfs_ErrorCode_InvalidOperation: return "InvalidOperation";
        case mfs_ErrorCode_OutOfMemory: return "OutOfMemory";
        case mfs_ErrorCode_Unkown: return "Unkown";
        case mfs_ErrorCode_NotFound: return "NotFound";
        case mfs_ErrorCode_PermissionDenied: return "PermissionDenied";
        case mfs_ErrorCode_InvalidFileName: return "InvalidFileName";
        default:
            MFS_FATAL();
            return "";
    }
}

mfs_String mfs_StringZ(char const* zero_terminated_string) {
    mfs_String result;
    result.ptr = zero_terminated_string;
    result.len = strlen(zero_terminated_string);
    return result;
}

MFS_FN mfs_String mfs_Drive(mfs_String path) {
    mfs_String result;
    result.ptr = path.ptr;
    result.len = 0;
    if(mfs__state.path_type == mfs_PathType_Windows) {
        if(path.len >= 2 && path.ptr[1] == ':') {
            result.len = 2;
        }
    }

    return result;
}

MFS_FN mfs_String mfs_DriveZ(char const* path_cstr) {
    return mfs_Drive(mfs_String(path_cstr));
}

MFS_FN mfs_String mfs_Root(mfs_String path) {
    mfs_String drive = mfs_Drive(path);
    mfs_String result;
    result.ptr = path.ptr + drive.len;
    result.len = 0;
    if(path.len > drive.len && mfs__IsDirSeparator(path.ptr[drive.len])) {
        result.len = 1;
    }
    return result;
}

MFS_FN mfs_String mfs_RootZ(char const* path_cstr) {
    return mfs_Root(mfs_String(path_cstr));
}

MFS_FN mfs_String mfs_Anchor(mfs_String path) {
    mfs_String drive = mfs_Drive(path);
    mfs_String root = mfs_Root(path);
    mfs_String result;
    result.ptr = drive.ptr;
    result.len = drive.len + root.len;
    return result;
}

MFS_FN mfs_String mfs_AnchorZ(char const* path_cstr) {
    return mfs_Anchor(mfs_String(path_cstr));
}

MFS_FN mfs_String mfs_DirName(mfs_String path) {
    ptrdiff_t dir_end = mfs__TrimTrailingDirSeparators(path.ptr, path.len);
    while(dir_end > 0) {
        --dir_end;
        if(mfs__IsDirSeparator(path.ptr[dir_end])) {
            break;
        }
    }
    mfs_String result;
    result.ptr = path.ptr;
    result.len = mfs__TrimTrailingDirSeparators(path.ptr, dir_end);
    return result;
}

MFS_FN mfs_String mfs_DirNameZ(char const* path_cstr) {
    return mfs_DirName(mfs_String(path_cstr));
}

MFS_FN mfs_String mfs_BaseName(mfs_String path) {
    ptrdiff_t base_end = mfs__TrimTrailingDirSeparators(path.ptr, path.len);
    ptrdiff_t base_start = base_end;
    while(base_start > 0) {
        if(mfs__IsDirSeparator(path.ptr[base_start - 1])) {
            break;
        }
        --base_start;
    }
    mfs_String result;
    result.ptr = path.ptr + base_start;
    result.len = base_end - base_start;
    return result;
}

MFS_FN mfs_String mfs_BaseNameZ(char const* path_cstr) {
    return mfs_BaseName(mfs_String(path_cstr));
}

MFS_FN mfs_String mfs_Suffix(mfs_String path) {
    ptrdiff_t start = path.len;
    bool dot = false;
    while(start > 0) {
        if(mfs__IsDirSeparator(path.ptr[start - 1])) {
            break;
        }
        if(path.ptr[start - 1] == '.') {
            dot = true;
            --start;
            break;
        }
        --start;
    }
    mfs_String result = MFS_ZERO_INIT();
    if(dot) {
        result.ptr = path.ptr + start;
        result.len = path.len - start;
    }
    return result;
}

MFS_FN mfs_String mfs_SuffixZ(char const* path_cstr) {
    return mfs_Suffix(mfs_String(path_cstr));
}

MFS_FN mfs_String mfs_BaseNameWithoutSuffix(mfs_String path) {
    mfs_String base_name = mfs_BaseName(path);
    mfs_String suffix = mfs_Suffix(base_name);
    mfs_String result = base_name;
    result.len -= suffix.len;
    return result;
}

MFS_FN mfs_String mfs_BaseNameWithoutSuffixZ(char const* path_cstr) {
    return mfs_BaseNameWithoutSuffix(mfs_String(path_cstr));
}

MFS_FN mfs_String mfs_WithoutSuffix(mfs_String path) {
    mfs_String suffix = mfs_Suffix(path);
    mfs_String result = path;
    result.len -= suffix.len;
    return result;
}

MFS_FN mfs_String mfs_WithoutSuffixZ(char const* path_cstr) {
    return mfs_WithoutSuffix(mfs_String(path_cstr));
}

mfs_ResolvedPath mfs_ResolvePath(mfs_String path_utf8) {
#if MFS__POSIX
    return mfs__posix_ResolvePath(path_utf8);
#elif MFS__WIN32
    return mfs__win32_ResolvePath(path_utf8);
#endif
}

mfs_ResolvedPath mfs_ResolvePathZ(char const* path_utf8) {
    return mfs_ResolvePath(mfs_StringZ(path_utf8));
}

mfs_EntireFile mfs_ReadEntireFile(mfs_String path_utf8) {
#if MFS__POSIX
    return mfs__posix_ReadEntireFile(path_utf8);
#elif MFS__WIN32
    return mfs__win32_ReadEntireFile(path_utf8);
#endif
}

mfs_EntireFile mfs_ReadEntireFileZ(char const* path_utf8) {
    return mfs_ReadEntireFile(mfs_StringZ(path_utf8));
}

mfs_CreateDirectoriesResult mfs_CreateDirectories(mfs_String path_utf8) {
#if MFS__POSIX
    return mfs__posix_CreateDirectories(path_utf8);
#elif MFS__WIN32
    return mfs__win32_CreateDirectories(path_utf8);
#endif
}

mfs_CreateDirectoriesResult mfs_CreateDirectoriesZ(char const* path_utf8) {
    return mfs_CreateDirectories(mfs_StringZ(path_utf8));
}

mfs_FileIterator mfs_OpenFileIterator(mfs_String path_utf8) {
#if MFS__POSIX
    return mfs__posix_OpenFileIterator(path_utf8);
#elif MFS__WIN32
    return mfs__win32_OpenFileIterator(path_utf8);
#endif
}

mfs_FileIterator mfs_OpenFileIteratorZ(char const* path_utf8) {
    return mfs_OpenFileIterator(mfs_StringZ(path_utf8));
}

void mfs_CloseFileIterator(mfs_FileIterator* iter) {
#if MFS__POSIX
    return mfs__posix_CloseFileIterator(iter);
#elif MFS__WIN32
    return mfs__win32_CloseFileIterator(iter);
#endif
}

bool mfs_AdvanceFileIterator(mfs_FileIterator* iter) {
    MFS_ASSERT(iter);
    if(!iter) {
        return false;
    }
#if MFS__POSIX
    return mfs__posix_AdvanceFileIterator(iter);
#elif MFS__WIN32
    return mfs__win32_AdvanceFileIterator(iter);
#endif
}

#endif /* MFS_IMPLEMENTATION */
