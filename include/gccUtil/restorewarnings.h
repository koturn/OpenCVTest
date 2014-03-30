#ifdef NOWARNING_H
#undef NOWARNING_H


#define GNUC_PREREQ(major, minor)  \
  (defined(__GNUC__) && __GNUC__ >= (major) && __GNUC_MINOR__ >= (minor))


#if GNUC_PREREQ(4, 6)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif




#undef GNUC_PREREQ


#endif  // NOWARNING_H
