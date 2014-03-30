#ifndef DEBUGGER_H
#define DEBUGGER_H


#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

#if defined(_MSC_VER)
#  define __DBG_FUNCTION__  __FUNCSIG__
#elif defined(__GNUC__)
#  define __DBG_FUNCTION__  __PRETTY_FUNCTION__
#else
#  define __DBG_FUNCTION__  __func__
#endif


#ifdef NDEBUG
#  define $DEBUG                   1 ? (void) 0 :
#  define SET_STREAM(stream)       ((void) 0)
#  define STATIC_ASSERT(expr)      ((void) 0)
#  define SET_SIGNALS()            ((void) 0)
#  define _SET_SIGNALS()           ((void) 0)
#  define SPEEDUP_COUT()           ((void) 0)

#  define STDOUT_ASSERT(expr)      ((void) 0)
#  define STDERR_ASSERT(expr)      ((void) 0)
#  define STDOUT_ASSERT(expr)      ((void) 0)

#  define STDOUT_TRACE(...)        ((void) 0)
#  define STDERR_TRACE(...)        ((void) 0)
#  define TRACE(...)               ((void) 0)

#  define STDOUT_DBG_LOG(message)  ((void) 0)
#  define STDERR_DBG_LOG(message)  ((void) 0)
#  define DBG_LOG(message)         ((void) 0)

#  define STDOUT_DUMP(...)         ((void) 0)
#  define STDERR_DUMP(...)         ((void) 0)
#  define DUMP(...)                ((void) 0)
#else
#  define $DEBUG
#  define SET_STREAM(stream)   dbg::setStream(stream)
#  define STATIC_ASSERT(expr)  typedef char __STATIC_ASSERT_TYPE__[(expr) ? 1 : -1]
#  define SET_SIGNALS()         dbg::setSignals(__FILE__)
#  define _SET_SIGNALS()        dbg::setSignals()
#  define SPEEDUP_COUT()        (std::cin.tie(0), std::ios::sync_with_stdio(false))

#  define STDOUT_ASSERT(expr)                                                       \
     (                                                                              \
       dbg::printWhere(std::cout, "ASSERT", __FILE__, __LINE__, __DBG_FUNCTION__),  \
       dbg::d_assert(std::cout, #expr, (expr))                                      \
     )
#  define STDERR_ASSERT(expr)                                                       \
     (                                                                              \
       dbg::printWhere(std::cerr, "ASSERT", __FILE__, __LINE__, __DBG_FUNCTION__),  \
       dbg::d_assert(std::cerr, #expr, (expr))                                      \
     )
#  define ASSERT(expr)                                                                     \
     (                                                                                     \
       dbg::printWhere(*dbg::stream_ptr, "ASSERT", __FILE__, __LINE__, __DBG_FUNCTION__),  \
       dbg::d_assert(*dbg::stream_ptr, #expr, (expr))                                      \
     )


#  define STDOUT_TRACE()  \
     dbg::printWhere(std::cout, "TRACE", __FILE__, __LINE__, __DBG_FUNCTION__)
#  define STDERR_TRACE()  \
     dbg::printWhere(std::cerr, "TRACE", __FILE__, __LINE__, __DBG_FUNCTION__)
#  define TRACE(...)                                                                     \
     (                                                                                   \
       dbg::setStream(__VA_ARGS__),                                                      \
       dbg::printWhere(*dbg::stream_ptr, "TRACE", __FILE__, __LINE__, __DBG_FUNCTION__)  \
     )


#  define STDOUT_DBG_LOG(message)                                                \
     (                                                                           \
       dbg::printWhere(std::cout, "LOG", __FILE__, __LINE__, __DBG_FUNCTION__),  \
       dbg::log(std::cout, message)                                              \
     )
#  define STDERR_DBG_LOG(message)                                                \
     (                                                                           \
       dbg::printWhere(std::cerr, "LOG", __FILE__, __LINE__, __DBG_FUNCTION__),  \
       dbg::log(std::cerr, message)                                              \
     )
#  define DBG_LOG(message)                                                              \
     (                                                                                  \
       dbg::printWhere(*dbg::stream_ptr, "LOG", __FILE__, __LINE__, __DBG_FUNCTION__),  \
       dbg::log(*dbg::stream_ptr, message)                                              \
     )


#  define STDOUT_DUMP(var, ...)                                                   \
     (                                                                            \
       dbg::setStream(__VA_ARGS__),                                               \
       dbg::printWhere(std::cout, "DUMP", __FILE__, __LINE__, __DBG_FUNCTION__),  \
       (std::cout << "    " #var " = " << (var) << std::endl)                     \
     )
#  define STDERR_DUMP(var, ...)                                                   \
     (                                                                            \
       dbg::setStream(__VA_ARGS__),                                               \
       dbg::printWhere(std::cerr, "DUMP", __FILE__, __LINE__, __DBG_FUNCTION__),  \
       (std::cerr << "    " #var " = " << (var) << std::endl)                     \
     )
#  define DUMP(var, ...)                                                                 \
     (                                                                                   \
       dbg::setStream(__VA_ARGS__),                                                      \
       dbg::printWhere(*dbg::stream_ptr, "DUMP", __FILE__, __LINE__, __DBG_FUNCTION__),  \
       (*dbg::stream_ptr << "    " #var " = " << (var) << std::endl)                     \
     )
#endif  // NDEBUG


namespace dbg {
// ---------- prototype ---------- //
static inline void printWhere(
    std::ostream &stream,
    const char   *label,
    const char   *filename,
    int           linenr,
    const char   *funcname);

static inline void d_assert(
    std::ostream &stream,
    const char   *expr_str,
    int           expr);

static inline void log(
    std::ostream &stream,
    const char   *message);
static inline void log(
    std::ostream      &stream,
    const std::string &message);


static inline void setStream(void);
static inline void setStream(std::ostream *stream_p);
static inline void setStream(const std::string &filename);
static inline void setStream(const char *filename);
static inline std::string currentTimeString(void);

static std::ostream *stream_ptr = &std::cout;
static std::map<const char *, std::ostream *> file_map;

static void setSignals(void);
static void setSignals(const char *filename);
static void setSignals(const char *filename);
static void sigAbrtHandler(int sig);
static void sigFpeHandler(int sig);
static void sigIntHandler(int sig);
static void sigIllHandler(int sig);
static void sigSegvHandler(int sig);
static void sigTermHandler(int sig);


static inline void printWhere(
    std::ostream &stream,
    const char   *label,
    const char   *filename,
    int           linenr,
    const char   *funcname)
{
  stream << label << ": "
                  << filename
                  << " at line " << linenr
                  << " in " << funcname
                  << std::endl;
}


static inline void d_assert(
    std::ostream &stream,
    const char   *expr_str,
    int           expr)
{
  if (!expr) {
    stream << "    FAILE: " << expr_str << std::endl
           << ">>>>>>>>>>>>>>>>>>>> Exit this program <<<<<<<<<<<<<<<<<<<<"
           << std::endl;
    exit(1);
  } else {
    stream << "    SUCCESS: " << expr_str << std::endl;
  }
}

static inline void log(
    std::ostream &stream,
    const char   *message)
{
  stream << "    > " << message << std::endl;
}

static inline void log(
    std::ostream      &stream,
    const std::string &message)
{
  stream << "    > " << message << std::endl;
}


static inline void setStream(void) {
  // No Effect
}

static inline void setStream(std::ostream *stream_p) {
  stream_ptr = stream_p;
}

static inline void setStream(const std::string &filename) {
  setStream(filename.c_str());
}

static inline void setStream(const char *filename) {
  if (file_map.count(filename) == 0) {
    stream_ptr = new std::ofstream(filename, std::ios::out | std::ios::app);
    file_map.insert(std::map<const char *, std::ostream *>::value_type(filename, stream_ptr));
    if (!*stream_ptr) {
      std::cout << "Cannot open " << filename << std::endl;
      stream_ptr = &std::cerr;
    }
    *stream_ptr << "============================== "
                << currentTimeString()
                << " =============================="
                << std::endl;
  } else {
    stream_ptr = file_map[filename];
  }
}


static inline std::string currentTimeString(void) {
  time_t     now = time(0);
  struct tm  tstruct;
  char       buf[80];
  tstruct = *localtime(&now);
  strftime(buf, sizeof(buf), "%Y %m/%d %X", &tstruct);
  return std::string(buf);
}

static inline void closeStream() {
  delete stream_ptr;
  stream_ptr = &std::cout;
}

static std::string partOfSignalMessage = "";
static void setSignals(const char *filename) {
  partOfSignalMessage = " in " + std::string(filename);
  setSignals();
}

static void setSignals(void) {
  signal(SIGABRT, sigAbrtHandler);
  signal(SIGFPE,  sigFpeHandler);
  signal(SIGILL,  sigIllHandler);
  signal(SIGINT,  sigIntHandler);
  signal(SIGSEGV, sigSegvHandler);
  signal(SIGTERM, sigTermHandler);
}

static void sigAbrtHandler(int sig) {
  std::cerr << "SIGABRT was catched" << partOfSignalMessage << std::endl;
  std::exit(sig);
}

static void sigFpeHandler(int sig) {
  std::cerr << "SIGFPE was catched" << partOfSignalMessage << std::endl;
  std::exit(sig);
}

static void sigIllHandler(int sig) {
  std::cerr << "SIGILL was catched" << partOfSignalMessage << std::endl;
  std::exit(sig);
}

static void sigIntHandler(int sig) {
  std::cerr << "SIGINT was catched" << partOfSignalMessage << std::endl;
  std::exit(sig);
}

static void sigSegvHandler(int sig) {
  std::cerr << "SIGSEGV was catched" << partOfSignalMessage << std::endl;
  std::exit(sig);
}

static void sigTermHandler(int sig) {
  std::cerr << "SIGTERM was catched" << partOfSignalMessage << std::endl;
  std::exit(sig);
}


}  // namespace dbg


#endif  // DEBUGGER_H
