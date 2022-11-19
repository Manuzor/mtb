/*
    mtb filesystem (mfs)

    Identifiers marked with double-underscores (e.g. MFS__WIN32) are intended
    for internal use of this library. Public facing identifiers start with
    mfs_ (types and functions both) or MFS_ (preprocessor stuff).

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

#ifndef MFS_INCLUDED
#define MFS_INCLUDED

#include <stdint.h>
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
    char const* message;
} mfs_Error;

/*
    Pointer + Length pair. String data is expected to be encoded as UTF-8.
*/
typedef struct mfs_String {
    char const* ptr;
    size_t len;
} mfs_String;

typedef enum mfs_PathType {
    mfs_PathType_Auto,
    mfs_PathType_Windows,
    mfs_PathType_Posix,
} mfs_PathType;

typedef struct mfs_EntireFile {
    mfs_Error error;
    uint8_t* data;
    size_t size;
} mfs_EntireFile;

typedef struct mfs_CreateDirectoriesResult {
    mfs_Error error;
    bool already_exists;
} mfs_CreateDirectoriesResult;

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
MFS_FN void mfs_Setup(mfs_SetupDesc* setup);

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
#define MFS_MAKE_ERROR(ERROR_CODE, ERROR_MESSAGE) mfs__MakeError((ERROR_CODE), (ERROR_MESSAGE))

static mfs_Error mfs__MakeError(mfs_ErrorCode ec, char const* message) {
    mfs_Error result;
    result.code = ec;
    result.message = message;
    return result;
}
#endif  // MFS_MAKE_ERROR

static mfs_Error mfs_NoError() {
    mfs_Error result = MFS_ZERO_INIT();
    return result;
}

struct mfs_Allocation {
    uint8_t* ptr;
    size_t allocation_size;
    size_t fill;
    mfs_Allocation* next;
};

#define MFS__FILE_NAME_LIMIT 4096

struct mfs__State {
    bool ready;
    mfs_SetupDesc desc;
    mfs_PathType path_type;

    mfs_Allocator allocator;
    mfs_Allocation* current_allocation;

    uint8_t buf[16 * 1024];
};

static mfs__State mfs__state;

static void* mfs__DefaultReallocProc(void* user_data, void* old_ptr, size_t old_size, size_t new_size) {
    (void)user_data;
    (void)old_size;
    return realloc(old_ptr, new_size);
}

struct mfs__ReallocResult {
    mfs_Error error;
    void* new_ptr;
};

static mfs__ReallocResult mfs__Realloc(void* old_ptr, size_t old_size, size_t new_size) {
    MFS_ASSERT(mfs__state.ready);

    mfs__ReallocResult result = MFS_ZERO_INIT();

    uint8_t* cur_ptr;
    size_t cur_len;
    size_t cur_cap;
    if(mfs__state.current_allocation) {
        cur_ptr = mfs__state.current_allocation->ptr;
        cur_len = mfs__state.current_allocation->fill;
        cur_cap = mfs__state.current_allocation->allocation_size - mfs__state.current_allocation->fill;
    }else {
        cur_ptr = NULL;
        cur_len = 0;
        cur_cap = 0;
    }

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

        MFS_ASSERT(mfs__state.current_allocation);
        if(!mfs__state.current_allocation) {
            result.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidOperation, "Bad realloc: old_ptr is non-NULL but no current allocation exists.");
            return result;
        }

        if(old_size < new_size) {
            size_t delta = new_size - old_size;
            if(old_ptr == cur_ptr + cur_len - old_size && cur_len + delta <= cur_cap) {
                /* Resize previous allocation */
                cur_len += delta;
                result.new_ptr = old_ptr;
                return result;
            }
        } else if(old_size > new_size) {
            size_t delta = new_size - old_size;
        } else {
            /* Unchanged size requirements. Weird. */
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

    if(cur_len + new_size <= cur_cap) {
        /*
            Try sub-allocating from the current allocation
        */
        result.new_ptr = cur_ptr + cur_len;
        cur_len += new_size;
    } else {
        /*
            Create a new allocation.
        */
        size_t allocation_size = sizeof(mfs_Allocation) + new_size;
        if(allocation_size < 4096) {
            allocation_size = 4096;
        }
        mfs_Allocation* new_a = (mfs_Allocation*)mfs__state.allocator.realloc_cb(mfs__state.allocator.user_data, mfs__state.current_allocation, cur_cap, allocation_size);
        if(!new_a) {
            result.error = MFS_MAKE_ERROR(mfs_ErrorCode_OutOfMemory, "Out of memory: realloc_cb returned NULL.");
            return result;
        }

        new_a->ptr = (uint8_t*)(new_a + 1);
        new_a->allocation_size = allocation_size;
        new_a->fill = new_size;
        new_a->next = mfs__state.current_allocation;

        result.new_ptr = new_a->ptr;
        mfs__state.current_allocation = new_a;
    }

    if(old_ptr) {
        memcpy(result.new_ptr, old_ptr, old_size);
    }

    return result;
}

static bool mfs__IsDirSeparator(char c) {
    switch(mfs__state.path_type) {
        case mfs_PathType_Windows: return c == '\\' || c == '/';
        case mfs_PathType_Posix: return c == '/';
        default: MFS_FATAL(); return false;
    }
}

static size_t mfs__TrimTrailingDirSeparators(char const* ptr, size_t len) {
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

mfs_EntireFile mfs__posix_ReadEntireFile(mfs_String path_utf8) {
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

    mfs__ReallocResult alloc_result = mfs__Realloc(NULL, 0, file_size);
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

mfs_CreateDirectoriesResult mfs__posix_CreateDirectories(mfs_String path_utf8) {
    mfs_CreateDirectoriesResult result = MFS_ZERO_INIT();
    result.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidOperation, "NOT IMPLEMENTED");
    return result;
}

#endif // MFS__POSIX

/*
    [Section] WIN32 Backend
*/

#if MFS__WIN32

#include <Windows.h>

/*
    TODO: Convert input UTF-8 string to Windows' wide string, create file, read everything.
*/

mfs_EntireFile mfs__win32_ReadEntireFile(mfs_String path_utf8) {
    mfs_EntireFile result = MFS_ZERO_INIT();

    if(!mfs__state.ready) {
        result.error = MFS_MAKE_ERROR(mfs_ErrorCode_InvalidOperation, "Not initialized. Did you forget to call mfs_Setup?");
        return result;
    }

    wchar_t* wname_ptr = (wchar_t*)&mfs__state.buf[0];
    size_t wbuf_cap = sizeof(mfs__state.buf) / sizeof(wchar_t);

    int wname_len = MultiByteToWideChar(
        CP_UTF8,                  // UINT                              CodePage,
        0,                        // DWORD                             dwFlags,
        path_utf8.ptr,       // _In_NLS_string_(cbMultiByte)LPCCH lpMultiByteStr,
        (int)path_utf8.len,  // int                               cbMultiByte,
        wname_ptr,                // LPWSTR                            lpWideCharStr,
        (int)wbuf_cap - 1         // int                               cchWideChar
    );
    wname_ptr[wname_len] = 0;

    HANDLE file = CreateFileW(
        wname_ptr,        // [in]           LPCWSTR               lpFileName,
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

    size_t size = file_size.QuadPart;
    mfs__ReallocResult alloc_result = mfs__Realloc(NULL, 0, size);
    if(alloc_result.error.code) {
        result.error = alloc_result.error;
        CloseHandle(file);
        return result;
    }

    uint8_t* ptr = (uint8_t*)(uintptr_t)alloc_result.new_ptr;
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

mfs_CreateDirectoriesResult mfs__win32_CreateDirectories(mfs_String path_utf8) {
    mfs_CreateDirectoriesResult result = MFS_ZERO_INIT();

    char const* name_ptr = path_utf8.ptr;
    size_t name_len = mfs__TrimTrailingDirSeparators(path_utf8.ptr, path_utf8.len);
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

    result.already_exists = true;

    size_t end = 0;
    while(end < name_len) {
        while(end < name_len) {
            if(mfs__IsDirSeparator(name_ptr[end])) {
                break;
            }
            ++end;
        }

        wchar_t* wname_ptr = (wchar_t*)&mfs__state.buf[0];
        size_t wbuf_cap = sizeof(mfs__state.buf) / sizeof(wchar_t);

        int wname_len = MultiByteToWideChar(
            CP_UTF8,           // UINT                              CodePage,
            0,                 // DWORD                             dwFlags,
            name_ptr,          // _In_NLS_string_(cbMultiByte)LPCCH lpMultiByteStr,
            (int)end,          // int                               cbMultiByte,
            wname_ptr,         // LPWSTR                            lpWideCharStr,
            (int)wbuf_cap - 1  // int                               cchWideChar
        );
        wname_ptr[wname_len] = 0;

        bool ok = CreateDirectoryW(wname_ptr, NULL) != 0;
        if(ok) {
            result.already_exists = false;
        }
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

#endif /* MFS__WIN32 */


/*
    [Section] Public API Implementation
*/

void mfs_Setup(mfs_SetupDesc* setup_desc) {
    MFS_ASSERT(!mfs__state.ready);
    MFS_ASSERT(setup_desc);
    mfs__state.desc = *setup_desc;
    if(setup_desc->path_type == mfs_PathType_Auto) {
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
        mfs__state.allocator.realloc_cb = &mfs__DefaultReallocProc;
        mfs__state.allocator.user_data = NULL;
    }
    mfs__state.ready = true;
}

void mfs_Reset(void) {
    if(!mfs__state.ready) {
        return;
    }

    mfs_Allocation* a = mfs__state.current_allocation;
    while(a) {
        mfs_Allocation* to_free = a;
        a = a->next;
        void* free_result = mfs__state.allocator.realloc_cb(mfs__state.allocator.user_data, to_free, to_free->allocation_size, 0);
        MFS_ASSERT(free_result == NULL);
    }
    mfs__state.current_allocation = NULL;
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
        default: MFS_FATAL(); return "";
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
    size_t dir_end = mfs__TrimTrailingDirSeparators(path.ptr, path.len);
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
    size_t base_end = mfs__TrimTrailingDirSeparators(path.ptr, path.len);
    size_t base_start = base_end;
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
    size_t start = path.len;
    bool dot = false;
    while(start > 0) {
        if(mfs__IsDirSeparator(path.ptr[start - 1])) {
            break;
        }
        if(path.ptr[start -1] == '.') {
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


#endif /* MFS_IMPLEMENTATION */
