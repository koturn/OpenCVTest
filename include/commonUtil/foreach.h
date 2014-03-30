#ifndef FOREACH_H
#define FOREACH_H


#define LENGTH(array)        (sizeof(array) / sizeof((array)[0]))
#define FOR_I(i, from, to)   for (int i = (from); i < (to); ++i)
#define FOR_U(i, from, to)   for (unsigned int i = (from); i < (to); ++i)
#define FOR(i, from, to)     FOR_I(i, from, to)
#define RFOR_I(i, from, to)  for (int i = (from); i >= (to); --i)
#define RFOR_U(i, from, to)  for (unsigned int i = (from); i >= (to); --i)
#define RFOR(i, from, to)    RFOR_I(i, from, to)
#define REP_I(i, n)          for (int i = 0; i < (n); ++i)
#define REP_U(i, n)          for (unsigned int i = 0; i < (n); ++i)
#define REP(i, n)            for (size_t i = 0; i < (n); ++i)
#define LOOP(n)              REP(__loop_tmp_var__, n)


#if !defined(__GNUC__) || defined(__MINGW32__)
# define __GNUC_PREREQ(major, minor)  \
   (__GNUC__ > (major) || (__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)))
#endif
#define GNUC_PREREQ(major, minor)  \
  (defined(__GNUC__) && __GNUC_PREREQ(major, minor))
#define CLANG_PREREQ(major, minor)  \
  (defined(__clang__) && (__clang_major__ > (major) || (__clang_major__ == (major) && __clang_minor__ >= (minor))))
#define SUPPORT_C11  \
  (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)


#if (defined(_MSC_VER) && _MSC_VER >= 1600) ||   \
    SUPPORT_CPP11 &&                             \
      ((SUPPORT_CPP11 && !GNUC_PREREQ(4, 4)) ||  \
       (SUPPORT_CPP11 && !CLANG_PREREQ(2, 9)))
#  define FOREACH(elm, v)  \
     for (auto elm = (v).begin(); elm != (v).end(); ++elm)
#  define FOREACH_SA(elm, array)  \
     for (auto elm = (array); elm < ((array) + LENGTH(array)); ++elm)
#elif defined(__GNUC__)
#  define FOREACH(elm, v)  \
     for (typeof((v).begin()) elm = (v).begin(); elm != (v).end(); ++elm)
#  define FOREACH_SA(elm, array)  \
     for (typeof(&array[0]) elm = (array); elm < ((array) + LENGTH(array)); ++elm)
#endif


#define FOREACH_STL(CONTAINER, TYPE, elm, v)  \
  for (CONTAINER<TYPE>::iterator elm = (v).begin(); elm != (v).end(); ++elm)

#define FOREACH_VECTOR(TYPE, elm, v)  \
  FOREACH_STL(std::vector, TYPE, elm, v)
#define FOREACH_LIST(TYPE, elm, v)  \
  FOREACH_STL(std::list, TYPE, elm, v)
#define FOREACH_QUEUE(TYPE, elm, v)  \
  FOREACH_STL(std::queue, TYPE, elm, v)
#define FOREACH_DEQUE(TYPE, elm, v)  \
  FOREACH_STL(std::deque, TYPE, elm, v)
#define FOREACH_PRIORITY_QUEUE(TYPE, elm, v)  \
  FOREACH_STL(std::priority_queue, TYPE, elm, v)
#define FOREACH_STACK(TYPE, elm, v)  \
  FOREACH_STL(std::stack, TYPE, elm, v)
#define FOREACH_MAP(TYPE, elm, v)  \
  FOREACH_STL(std::map, TYPE, elm, v)
#define FOREACH_MULTIMAP(TYPE, elm, v)  \
  FOREACH_STL(std::multimap, TYPE, elm, v)
#define FOREACH_SET(TYPE, elm, v)  \
  FOREACH_STL(std::set, TYPE, elm, v)
#define FOREACH_MULTISET(TYPE, elm, v)  \
  FOREACH_STL(std::multiset, TYPE, elm, v)
#define FOREACH_BITSET(TYPE, elm, v)  \
  FOREACH_STL(std::bitset, TYPE, elm, v)


#if !defined(__GNUC__) || defined(__MINGW32__)
#  undef __GNUC_PREREQ
#endif
#undef SUPPORT_CPP11
#undef GNUC_PREREQ
#undef CLANG_PREREQ


#endif  // FOREACH_H
