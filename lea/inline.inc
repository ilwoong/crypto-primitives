#if _MSC_VER && !__INTEL_COMPILER
    #define FORCE_INLINE __force_inline
#elif __GNUC__ | __clang__
    #define FORCE_INLINE __attribute__((always_inline)) inline
#else
    #define FORCE_INLINE inline
#endif