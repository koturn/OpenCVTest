/*!
 * @brief A header file for compatibility
 *
 * This header enables keep compatibility between compilers.
 * (but not perfectly)
 *
 * @author    koturn 0;
 * @date      2013 05/23
 * @file      compatibility.h
 * @version   0.2.4
 * @attention Be careful to dangerous substitution.
 */
#ifndef COMPATIBILITY_H
#define COMPATIBILITY_H

// In gcc/clang and others
#ifndef _MSC_VER
typedef int errno_t;
//! Dummy error number for no error
#define __DUMMY_ERRNO_OK__   ((errno_t) 0)
//! Dummy error number for error
#define __DUMMY_ERRNO_ERR__  ((errno_t) 1)

#define _countof(array)  (sizeof(array) / sizeof((array)[0]))

/* -------------------------------------------------------------------------
 * functions for IO
 * ------------------------------------------------------------------------- */
#define printf_s(fp, fmt, ...)              \
  printf(fp, fmt, ##__VA_ARGS__)

#define fprintf_s(fp, fmt, ...)             \
  fprintf(fp, fmt, ##__VA_ARGS__)

#define fopen_s(fpp, filename, mode)        \
  (*(fpp) = fopen(filename, mode), *(fpp) == NULL ? __DUMMY_ERRNO_ERR__ : __DUMMY_ERRNO_OK__)

#if !(defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)
#  define gets_s(dst, dst_size)             \
     fgets(dst, dst_size, stdin)
#endif

#define sprintf_s(dst, dst_size, fmt, ...)  \
  sprintf(dst, fmt, ##__VA_ARGS__)

#define getenv_s(ret, dst, dst_size, varname)  \
  (strcpy(dst, getenv(varname)), __DUMMY_ERRNO_OK__)


/* -------------------------------------------------------------------------
 * For scanf() family.
 * You don't have to specify destination size, define macro: AUTO_SECURE
 * previously.
 * ------------------------------------------------------------------------- */
#define SCAN_S_ARG(arg)   (arg)


#ifdef AUTO_UNSECURE
#  define fscanf_s(fp, fmt, ...)       \
     fscanf(fp, fmt, ##__VA_ARGS__)

#  define scanf_s(fmt, dst, dst_size)  \
     scanf(fmt, dst)

#  define sscanf_s(str, fmt, ...)      \
     sscanf(str, fmt, ##__VA_ARGS__)
#endif


/* -------------------------------------------------------------------------
 * Functions for string
 * ------------------------------------------------------------------------- */
#define memcpy_s(dst, dst_size, src, count)   \
  (memcpy(dst, src, count), __DUMMY_ERRNO_OK__)

#define memmove_s(dst, dst_size, src, count)  \
  (memmove(dst, src, count), __DUMMY_ERRNO_OK__)

#define strcat_s(dst, dst_size, src)          \
  (strcat(dst, src), __DUMMY_ERRNO_OK__)

#define strcpy_s(dst, dst_size, src)          \
  (strcpy(dst, src), __DUMMY_ERRNO_OK__)

#define strncat_s(dst, dst_size, src, count)  \
  (strncat(dst, src, count), __DUMMY_ERRNO_OK__)

#define strncpy_s(dst, dst_size, src, count)  \
  (strncpy(dst, src), __DUMMY_ERRNO_OK__)

#define strtok_s(str, demimiter, next_token)  \
  strtok(str, delimiter)

#define _strlwr_s(str, dst_size)              \
  (strlwr(str), __DUMMY_ERRNO_OK__)

#define _strupr_s(str, dst_size)              \
  (strupr(str), __DUMMY_ERRNO_OK__)


/* -------------------------------------------------------------------------
 * Random
 * ------------------------------------------------------------------------- */
#ifndef _CRT_RAND_S
#  define _CRT_RAND_S
#endif
#define rand_s(rand_val)  \
  (rand(rand_val), *(rand_val))

#endif




// In Windows (Visual C++ / MinGW-gcc, MinGW-g++)
#if defined(WIN16) || defined(_WIN16) || defined(__WIN16) || defined(__WIN16__)   \
  || defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)  \
  || defined(WIN64) || defined(_WIN64) || defined(__WIN64) || defined(__WIN64__)
   // require <Windows.h>
   //! sleep() for Windows
#  define sleep(sec)  Sleep((sec) * 1000)
#endif



// for function name and its signature
#if defined(_MSC_VER)
#  define __PRETTY_FUNCTION__  __FUNCSIG__
#elif defined(__GNUC__)
#  define __FUNCSIG__          __PRETTY_FUNCTION__
#endif

// If '__attribute__' isn't available, remove '__attribute__'.
#ifndef __GNUC__
#  define __attribute__(attr)
#endif


// If 'declspec's aren't available, remove 'declspec's.
#ifndef _MSC_VER
#  define _declspec(attr)
#endif


// for dllimport
#if defined(_MSC_VER)
#  define DLLIMPORT  __declspec(dllimport)
#elif defined(__GNUC__)
#  define DLLIMPORT  __attribute__((dllimport))
#else
#  define DLLIMPORT
#endif

// for dllexport
#if defined(_MSC_VER)
#  define DLLEXPORT  __declspec(dllexport)
#elif defined(__GNUC__)
#  define DLLEXPORT  __attribute__((dllexport))
#else
#  define DLLEXPORT
#endif




#if !defined(__GNUC__) || defined(__MINGW32__)
# define __GNUC_PREREQ(major, minor)  \
   (__GNUC__ > (major) || (__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)))
#endif

#define GNUC_PREREQ(major, minor)  \
  (defined(__GNUC__) && __GNUC_PREREQ(major, minor))
#define CLANG_PREREQ(major, minor)  \
  (defined(__clang__) && (__clang_major__ > (major) || (__clang_major__ == (major) && __clang_minor__ >= (minor))))
#define SUPPORT_C99  \
  (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
#define SUPPORT_C11  \
  (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)
#define SUPPORT_CXX11  \
  (__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#define SUPPORT_CXX14  \
  (__cplusplus >= 201300L)


// FORCEINLINE
#if (defined _MSC_VER) || (defined __INTEL_COMPILER)
#  define FORCEINLINE  __forceinline
#else
#  define FORCEINLINE  inline
#endif

// ALWAYSINLINE
#if GNUC_PREREQ(4, 0)
#  define ALWAYSINLINE  __attribute__((always_inline)) inline
#else
#  define ALWAYSINLINE  FORCEINLINE
#endif

// NOINLINE
#if defined(__GNUC__)
#  define NOINLINE  __attribute__((noinline))
#elif defined(_MSC_VER)
#  define NOINLINE  __declspec(noinline)
#else
#  define NOINLINE
#endif




/* -------------------------------------------------------------------------
 * C99 compatibility
 * ------------------------------------------------------------------------- */
#if defined(_MSC_VER) || SUPPORT_C99
#  define __func__  __FUNCTION__
#endif


// Macros for inline-directive under various environmets
#ifndef __cplusplus  // In language-C
#  if defined(_MSC_VER)           // In Visual C++,
#    define inline      __inline  // 'inline' and '__inline__' aren't available
#    define __inline__  __inline  // but '__inline' is.
#  elif !defined(__GNUC__) && SUPPORT_C99
#    define inline                // If 'inline' isn't available, remove 'inline'
#    define __inline              // as well as '__inline'.
#  endif
#endif


// Macros for restrict-qualifier under various environmets
#if _MSC_VER >= 1400                // In Visual C++ (after Visual C++ 2005),
#  define restrict      __restrict  // 'restrict' and '__inline__' aren't available
#  define __restrict__  __restrict  // but '__restrict' is.
#elif __cplusplus                   // In language C++ (but not Visual C++),
#  define restrict      __restrict  // 'restrict' isn't available but '__restrict' is.
#elif !SUPPORT_C99
#  if defined(__GNUC__)             // In gcc,
#    define restrict    __restrict  // 'restrict' isn't available but '__restrict' is.
#  else
#    define restrict                // If 'restrict' isn't available, remove 'restrict'
#    define __restrict              // as well as '__restrict'
#    define __restrict__            // and '__restrict__'.
#  endif
#endif




/* -------------------------------------------------------------------------
 * C11 compatibility
 * ------------------------------------------------------------------------- */
// for static_assert
#if (defined(_MSC_VER) && _MSC_VER >= 1600) ||  \
    !SUPPORT_C11 &&                             \
      ((SUPPORT_CXX11 && GNUC_PREREQ(4, 3)) ||  \
       (SUPPORT_CXX11 && CLANG_PREREQ(2, 9)))
#  define _Static_assert(expr, msg)  static_assert(expr, msg)
#else
#  define _Static_assert(expr, msg)  \
     typedef char __STATIC_ASSERT_ARRAY__[(expr) ? 1 : -1]
#endif


// for _Noreturn
#if SUPPORT_CXX11 && (GNUC_PREREQ(4, 8) || CLANG_PREREQ(3, 3))
#  define _Noreturn  [[noreturn]]
#elif defined(_MSC_VER) && _MSC_VER >= 1300
#  define _Noreturn  __declspec(noreturn)
#elif defined(__GNUC__)
#  define _Noreturn  __attribute__((noreturn))
#else
#  define _Noreturn
#endif


// for _Alignas
#if !((SUPPORT_CXX11 && GNUC_PREREQ(4, 8)) ||  \
      (SUPPORT_CXX11 && CLANG_PREREQ(3, 3)))
#  if defined(_MSC_VER) && (1300 <= _MSC_VER && _MSC_VER < 1800)
#    define _Alignas(n)  __declspec(align(n))
#  elif defined(__GNUC__)
#    define _Alignas(n)  __attribute__((aligned(n)))
#  endif
#endif


// for _Thread_local
#if !SUPPORT_CXX11 ||                       \
  (SUPPORT_CXX11 && !GNUC_PREREQ(4, 8)) &&  \
  (SUPPORT_CXX11 && !CLANG_PREREQ(3, 3))
#  if defined(_MSC_VER) && _MSC_VER >= 1600
#    define _Thread_local  __declspec(thread)
#  elif defined(__GNUC__)
#    define _Thread_local  __thread
#  endif
#endif




/* -------------------------------------------------------------------------
 * C++11 compatibility
 * ------------------------------------------------------------------------- */
// for decltype
#if !SUPPORT_CXX11 && defined(__GNUC__)
#  define decltype(expr)  typeof(expr)
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1600
#  define typeof(expr)  decltype(expr)
#endif

// for static_assert
#if SUPPORT_C11
#  define  static_assert(expr, msg)  _Static_assert(expr, msg)
#elif !((defined(_MSC_VER) && _MSC_VER >= 1600) ||  \
        (SUPPORT_CXX11 && GNUC_PREREQ(4, 3)) ||     \
        (SUPPORT_CXX11 && CLANG_PREREQ(2, 9)))
#  define static_assert(expr, msg)  \
     typedef char __STATIC_ASSERT_ARRAY__[(expr) ? 1 : -1]
#endif

// for nullptr
#if !((defined(_MSC_VER) && _MSC_VER >= 1600) ||  \
      (SUPPORT_CXX11 && GNUC_PREREQ(4, 6)) ||     \
      (SUPPORT_CXX11 && CLANG_PREREQ(3, 0)))
#  define nullptr  NULL
#endif

// for final, override and explicit
#if !SUPPORT_CXX11 || (defined(_MSC_VER) && _MSC_VER < 1700)
#  define final
#  define override
#  define explicit
#endif

// for noexcept
#if !((defined(_MSC_VER) && _MSC_VER > 1800) ||  \
      (SUPPORT_CXX11 && GNUC_PREREQ(4, 6)))
#  define noexcept
#endif

// for constexpr
#if !((defined(_MSC_VER) && _MSC_VER > 1800) ||  \
      (SUPPORT_CXX11 && GNUC_PREREQ(4, 6)) ||    \
      (SUPPORT_CXX11 && CLANG_PREREQ(3, 1)))
#  define constexpr
#endif

// for constexpr
#if ((defined(_MSC_VER) && _MSC_VER > 1800) ||  \
     (SUPPORT_CXX11 && GNUC_PREREQ(4, 6)) ||    \
     (SUPPORT_CXX11 && CLANG_PREREQ(3, 1)))
#  define CONSTEXPR_CXX11  constexpr
#else
#  define CONSTEXPR_CXX11
#endif

// for thread_local
#if !SUPPORT_CXX11 ||                       \
  (SUPPORT_CXX11 && !GNUC_PREREQ(4, 8)) &&  \
  (SUPPORT_CXX11 && !CLANG_PREREQ(3, 3))
#  if defined(_MSC_VER) && _MSC_VER >= 1600
#    define thread_local  __declspec(thread)
#  elif defined(__GNUC__)
#    define thread_local  __thread
#  elif SUPPORT_C11
#    define thread_local  _Thread_local
#  endif
#endif

// for noexcept attribute
#if defined(_MSC_VER)
#  define ATTR_NOTHROW  __declspec(nothrow)
#elif defined(__GNUC__)
#  define ATTR_NOTHROW  __attribute__((nothrow))
#else
#  define ATTR_NOTHROW
#endif

// for noreturn
#if SUPPORT_CXX11 && (GNUC_PREREQ(4, 8) || CLANG_PREREQ(3, 3))
#  define ATTR_NORETURN  [[noreturn]]
#elif defined(_MSC_VER) && _MSC_VER >= 1300
#  define ATTR_NORETURN  __declspec(noreturn)
#elif defined(__GNUC__)
#  define ATTR_NORETURN  __attribute__((noreturn))
#elif SUPPORT_C11
#  define ATTR_NORETURN  _Noreturn
#else
#  define ATTR_NORETURN
#endif

// for alignas
#if !((SUPPORT_CXX11 && GNUC_PREREQ(4, 8)) ||  \
      (SUPPORT_CXX11 && CLANG_PREREQ(3, 3)))
#  if defined(_MSC_VER) && (1300 <= _MSC_VER && _MSC_VER < 1800)
#    define alignas(n)  __declspec(align(n))
#  elif defined(__GNUC__)
#    define alignas(n)  __attribute__((aligned(n)))
#  elif SUPPORT_C11
#    define alignas(n)  _Alignas(n)
#  endif
#endif

// for alignof
#if !SUPPORT_CXX11 &&  \
     ((defined(_MSC_VER) && (1300 <= _MSC_VER && _MSC_VER < 1800)) ||  \
       defined(__GNUC__))
#  define alignof(n)  __alignof(n)
#endif




/* -------------------------------------------------------------------------
 * C++1y compatibility (future)
 * ------------------------------------------------------------------------- */
// for deprecated
#if SUPPORT_CXX14 && (GNUC_PREREQ(4, 9) || CLANG_PREREQ(3, 4))
#  define ATTR_DEPRECATED  [[deprecated]]
#elif defined(_MSC_VER)
#  define ATTR_DEPRECATED  __declspec(deprecated)
#elif defined(__GNUC__)
#  define ATTR_DEPRECATED  __attribute__((deprecated))
#else
#  define ATTR_DEPRECATED
#endif

// for pure function
#if SUPPORT_CXX14 && 0
#  define ATTR_PURE  [[pure]]
#elif defined(__GNUC__)
#  define ATTR_PURE  __attribute__((const))
#else
#  define ATTR_PURE
#endif

// c++14 constexpr
#if SUPPORT_CXX14 && (CLANG_PREREQ(3, 5))
#  define CONSTEXPR_CXX14  constexpr
#else
#  define CONSTEXPR_CXX14
#endif



#if !defined(__GNUC__) || defined(__MINGW32__)
#  undef __GNUC_PREREQ
#endif
#undef SUPPORT_C99
#undef SUPPORT_C11
#undef SUPPORT_CXX11
#undef SUPPORT_CXX14
#undef GNUC_PREREQ
#undef CLANG_PREREQ




#endif  // COMPATIBILITY_H
