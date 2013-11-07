#include "colorprint.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>

// port from GTest JerryZhou@outlook.com
// This header defines the following utilities:
//
// Macros indicating the current platform (defined to 1 if compiled on
// the given platform; otherwise undefined):
//   GTEST_OS_AIX      - IBM AIX
//   GTEST_OS_CYGWIN   - Cygwin
//   GTEST_OS_HPUX     - HP-UX
//   GTEST_OS_LINUX    - Linux
//     GTEST_OS_LINUX_ANDROID - Google Android
//   GTEST_OS_MAC      - Mac OS X
//     GTEST_OS_IOS    - iOS
//       GTEST_OS_IOS_SIMULATOR - iOS simulator
//   GTEST_OS_NACL     - Google Native Client (NaCl)
//   GTEST_OS_OPENBSD  - OpenBSD
//   GTEST_OS_QNX      - QNX
//   GTEST_OS_SOLARIS  - Sun Solaris
//   GTEST_OS_SYMBIAN  - Symbian
//   GTEST_OS_WINDOWS  - Windows (Desktop, MinGW, or Mobile)
//     GTEST_OS_WINDOWS_DESKTOP  - Windows Desktop
//     GTEST_OS_WINDOWS_MINGW    - MinGW
//     GTEST_OS_WINDOWS_MOBILE   - Windows Mobile
//   GTEST_OS_ZOS      - z/OS

// Determines the platform on which Google Test is compiled.
#ifdef __CYGWIN__
# define GTEST_OS_CYGWIN 1
#elif defined __SYMBIAN32__
# define GTEST_OS_SYMBIAN 1
#elif defined _WIN32
# define GTEST_OS_WINDOWS 1
# ifdef _WIN32_WCE
#  define GTEST_OS_WINDOWS_MOBILE 1
# elif defined(__MINGW__) || defined(__MINGW32__)
#  define GTEST_OS_WINDOWS_MINGW 1
# else
#  define GTEST_OS_WINDOWS_DESKTOP 1
# endif  // _WIN32_WCE
#elif defined __APPLE__
# define GTEST_OS_MAC 1
# if TARGET_OS_IPHONE
#  define GTEST_OS_IOS 1
#  if TARGET_IPHONE_SIMULATOR
#   define GTEST_OS_IOS_SIMULATOR 1
#  endif
# endif
#elif defined __linux__
# define GTEST_OS_LINUX 1
# if defined __ANDROID__
#  define GTEST_OS_LINUX_ANDROID 1
# endif
#elif defined __MVS__
# define GTEST_OS_ZOS 1
#elif defined(__sun) && defined(__SVR4)
# define GTEST_OS_SOLARIS 1
#elif defined(_AIX)
# define GTEST_OS_AIX 1
#elif defined(__hpux)
# define GTEST_OS_HPUX 1
#elif defined __native_client__
# define GTEST_OS_NACL 1
#elif defined __OpenBSD__
# define GTEST_OS_OPENBSD 1
#elif defined __QNX__
# define GTEST_OS_QNX 1
#endif  // __CYGWIN__


#if GTEST_OS_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace posix{
const char* GetEnv(const char* name) {
#if GTEST_OS_WINDOWS_MOBILE
    // We are on Windows CE, which has no environment variables.
    return NULL;
#elif defined(__BORLANDC__) || defined(__SunOS_5_8) || defined(__SunOS_5_9)
    // Environment variables which we programmatically clear will be set to the
    // empty string rather than unset (NULL).  Handle that case.
    const char* const env = getenv(name);
    return (env != NULL && env[0] != '\0') ? env : NULL;
#else
    return getenv(name);
#endif
}
    
#if GTEST_OS_WINDOWS
# ifdef __BORLANDC__
    inline int IsATTY(int fd) { return isatty(fd); }
    inline int StrCaseCmp(const char* s1, const char* s2) {
        return stricmp(s1, s2);
    }
# else  // !__BORLANDC__
#  if GTEST_OS_WINDOWS_MOBILE
    inline int IsATTY(int /* fd */) { return 0; }
#  else
    inline int IsATTY(int fd) { return _isatty(fd); }
#  endif  // GTEST_OS_WINDOWS_MOBILE
    inline int StrCaseCmp(const char* s1, const char* s2) {
        return _stricmp(s1, s2);
    }
# endif  // __BORLANDC__
# if GTEST_OS_WINDOWS_MOBILE
    inline int FileNo(FILE* file) { return reinterpret_cast<int>(_fileno(file)); }
    // Stat(), RmDir(), and IsDir() are not needed on Windows CE at this
    // time and thus not defined there.
# else
    inline int FileNo(FILE* file) { return _fileno(file); }
# endif  // GTEST_OS_WINDOWS_MOBILE
#else
    inline int StrCaseCmp(const char* s1, const char* s2) {
        return strcasecmp(s1, s2);
    }
    inline int FileNo(FILE* file) { return fileno(file); }
    inline int IsATTY(int fd) { return isatty(fd); }
#endif  // GTEST_OS_WINDOWS

}
namespace String{
// Compares two C strings, ignoring case.  Returns true iff they have
// the same content.
//
// Unlike strcasecmp(), this function can handle NULL argument(s).  A
// NULL C string is considered different to any non-NULL C string,
// including the empty string.
bool CaseInsensitiveCStringEquals(const char * lhs, const char * rhs) {
    if (lhs == NULL)
        return rhs == NULL;
    if (rhs == NULL)
        return false;
    return posix::StrCaseCmp(lhs, rhs) == 0;
}
    
// Compares two C strings.  Returns true iff they have the same content.
//
// Unlike strcmp(), this function can handle NULL argument(s).  A NULL
// C string is considered different to any non-NULL C string,
// including the empty string.
bool CStringEquals(const char * lhs, const char * rhs) {
    if ( lhs == NULL ) return rhs == NULL;
    
    if ( rhs == NULL ) return false;
    
    return strcmp(lhs, rhs) == 0;
}
}

#if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE

// Returns the character attribute for the given color.
WORD GetColorAttribute(GTestColor color) {
    switch (color) {
        case COLOR_RED:    return FOREGROUND_RED;
        case COLOR_GREEN:  return FOREGROUND_GREEN;
        case COLOR_YELLOW: return FOREGROUND_RED | FOREGROUND_GREEN;
        case COLOR_BLUE:   return FOREGROUND_BLUE;
        case COLOR_MAGENTA:return FOREGROUND_RED | FOREGROUND_BLUE;
        default:           return 0;
    }
}

#else

// Returns the ANSI color code for the given color.  COLOR_DEFAULT is
// an invalid input.
const char* GetAnsiColorCode(GTestColor color) {
    switch (color) {
        case COLOR_RED:     return "1";
        case COLOR_GREEN:   return "2";
        case COLOR_YELLOW:  return "3";
        case COLOR_BLUE:    return "4";
        case COLOR_MAGENTA: return "5";
        default:            return NULL;
    };
}

#endif  // GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE

// Returns true iff Google Test should use colors in the output.
bool ShouldUseColor(bool stdout_is_tty) {
    const char* const gtest_color = "auto";
    
    if (String::CaseInsensitiveCStringEquals(gtest_color, "auto")) {
#if GTEST_OS_WINDOWS
        // On Windows the TERM variable is usually not set, but the
        // console there does support colors.
        return stdout_is_tty;
#else
        // On non-Windows platforms, we rely on the TERM variable.
        const char* const term = posix::GetEnv("TERM");
        const bool term_supports_color =
        String::CStringEquals(term, "xterm") ||
        String::CStringEquals(term, "xterm-color") ||
        String::CStringEquals(term, "xterm-256color") ||
        String::CStringEquals(term, "screen") ||
        String::CStringEquals(term, "screen-256color") ||
        String::CStringEquals(term, "linux") ||
        String::CStringEquals(term, "cygwin");
        return stdout_is_tty && term_supports_color;
#endif  // GTEST_OS_WINDOWS
    }
    
    return String::CaseInsensitiveCStringEquals(gtest_color, "yes") ||
    String::CaseInsensitiveCStringEquals(gtest_color, "true") ||
    String::CaseInsensitiveCStringEquals(gtest_color, "t") ||
    String::CStringEquals(gtest_color, "1");
    // We take "yes", "true", "t", and "1" as meaning "yes".  If the
    // value is neither one of these nor "auto", we treat it as "no" to
    // be conservative.
}

// Helpers for printing colored strings to stdout. Note that on Windows, we
// cannot simply emit special characters and have the terminal change colors.
// This routine must actually emit the characters rather than return a string
// that would be colored when printed, as can be done on Linux.
void ColoredPrintf(GTestColor color, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
#if GTEST_OS_WINDOWS_MOBILE || GTEST_OS_SYMBIAN || GTEST_OS_ZOS || GTEST_OS_IOS
    const bool use_color = false;
#else
    static const bool in_color_mode =
    ShouldUseColor(posix::IsATTY(posix::FileNo(stdout)) != 0);
    const bool use_color = in_color_mode && (color != COLOR_DEFAULT);
#endif  // GTEST_OS_WINDOWS_MOBILE || GTEST_OS_SYMBIAN || GTEST_OS_ZOS
    // The '!= 0' comparison is necessary to satisfy MSVC 7.1.
    
    if (!use_color) {
        vprintf(fmt, args);
        va_end(args);
        return;
    }
    
#if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE
    const HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    
    // Gets the current text color.
    CONSOLE_SCREEN_BUFFER_INFO buffer_info;
    GetConsoleScreenBufferInfo(stdout_handle, &buffer_info);
    const WORD old_color_attrs = buffer_info.wAttributes;
    
    // We need to flush the stream buffers into the console before each
    // SetConsoleTextAttribute call lest it affect the text that is already
    // printed but has not yet reached the console.
    fflush(stdout);
    SetConsoleTextAttribute(stdout_handle,
                            GetColorAttribute(color) | FOREGROUND_INTENSITY);
    vprintf(fmt, args);
    
    fflush(stdout);
    // Restores the text color.
    SetConsoleTextAttribute(stdout_handle, old_color_attrs);
#else
    printf("\033[0;3%sm", GetAnsiColorCode(color));
    vprintf(fmt, args);
    printf("\033[m");  // Resets the terminal to default.
#endif  // GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE
    va_end(args);
}