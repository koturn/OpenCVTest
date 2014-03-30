#ifndef NOWARNING_H
#define NOWARNING_H


#define GNUC_PREREQ(major, minor)  \
  (defined(__GNUC__) && __GNUC__ >= (major) && __GNUC_MINOR__ >= (minor))

#if GNUC_PREREQ(4, 6)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-pedantic"
#  pragma GCC diagnostic ignored "-Wall"
#  pragma GCC diagnostic ignored "-Wextra"
#  pragma GCC diagnostic ignored "-Weffc++"
#  pragma GCC diagnostic ignored "-Wcast-align"
#  pragma GCC diagnostic ignored "-Wcast-qual"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#  pragma GCC diagnostic ignored "-Wformat"
#  pragma GCC diagnostic ignored "-Wlong-long"
#  pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#  pragma GCC diagnostic ignored "-Woverloaded-virtual"
#  pragma GCC diagnostic ignored "-Wpointer-arith"
#  pragma GCC diagnostic ignored "-Wstrict-aliasing"
#  pragma GCC diagnostic ignored "-Wswitch-enum"
#  pragma GCC diagnostic ignored "-Wunused-function"
#  pragma GCC diagnostic ignored "-Wvariadic-macros"
#  pragma GCC diagnostic ignored "-Wwrite-strings"
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4245)
#endif




#undef GNUC_PREREQ


#endif  // NOWARNING_H
