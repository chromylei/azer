#pragma once

#define AZER_ALIGN_128(n)    ((((uintptr_t)(n)) + 127) & ~127)
#define AZER_ALIGN_64(n)     ((((uintptr_t)(n)) + 63 ) & ~63 )
#define AZER_ALIGN_32(n)     ((((uintptr_t)(n)) + 31 ) & ~31 )
#define AZER_ALIGN_16(n)     ((((uintptr_t)(n)) + 15 ) & ~15 )
#define AZER_ALIGN_8(n)      ((((uintptr_t)(n)) + 7  ) & ~7  )
#define AZER_ALIGN_4(n)      ((((uintptr_t)(n)) + 3  ) & ~3  )
#define AZER_ALIGN_2(n)      ((((uintptr_t)(n)) + 1  ) & ~1  )
#define AZER_ALIGN_ARB(n, a) ((((uintptr_t)(n)) + ((a)-1)) & ~((a)-1))
// 'a' needs to be a power of 2

#ifdef _MSC_VER
# define AZER_STRUCT_ALIGN( bytes ) __declspec( align( bytes ) )
#else
# define AZER_STRUCT_ALIGN( bytes ) __attribute__ (( aligned ( bytes ) ))
#endif

#define  AZER_GPU_VAR  AZER_STRUCT_ALIGN(16)

// reference
// [1] Examples of Structure Alignment
// http://msdn.microsoft.com/en-us/library/71kf49f1.aspx
// [2] Packing Rules for Constant Variables
// http://msdn.microsoft.com/en-us/library/windows/desktop/bb509632(v=vs.85).aspx
// [3] Pack: http://msdn.microsoft.com/en-us/library/2e70t5y1.aspx
// [4] std::align http://www.cplusplus.com/reference/memory/align/
