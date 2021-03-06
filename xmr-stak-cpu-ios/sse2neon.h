#pragma once
#include "TargetConditionals.h"

#if TARGET_OS_SIMULATOR

#ifdef __GNUC__
#include <x86intrin.h>
#else
#include <intrin.h>
#endif // __GNUC__

#else
#define __X86INTRIN_H // suppress including of <x86intrin.h>
#define __INTRIN_H // suppress including of <intrin.h>
#define __IMMINTRIN_H // suppress including of <immintrin.h>
#include "arm_neon.h"
#include "sse2neon/SSE2NEON.h"

#define _MM_HINT_T0 3
#define _MM_HINT_T1 2
#define _MM_HINT_T2 1
#define _MM_HINT_NTA 0

// Loads one cache line of data from address p to a location closer to the processor.
// https://msdn.microsoft.com/en-us/library/84szxsww(v=vs.100).aspx
static inline __attribute__((always_inline)) void _mm_prefetch(const void *p , int i )
{
    __builtin_prefetch(p);
}

// Returns the __m128i structure with its two 64-bit integer values initialized to the values of the two 64-bit integers passed in.
// https://msdn.microsoft.com/en-us/library/dk2sdw0h(v=vs.120).aspx
static inline __attribute__((always_inline)) __m128i _mm_set_epi64x( int64_t i1, int64_t i2)
{
    int64_t __attribute__((aligned(16))) data[2] = { i2, i1 };
    return vreinterpretq_m128i_s64(vld1q_s64(data));
}

// Emits the Streaming SIMD Extensions 2 (SSE2) instruction movq. This instruction extracts the low order 64-bit integer from the parameter.
// https://msdn.microsoft.com/en-us/library/bb531384(v=vs.120).aspx
static inline __attribute__((always_inline)) int64_t _mm_cvtsi128_si64( __m128i a )
{
    return vgetq_lane_s64(vreinterpretq_s64_m128i(a), 0);
}

// Emits the Advanced Encryption Standard (AES) instruction aesenc. This instruction performs one round of AES encryption.
// https://msdn.microsoft.com/en-us/library/cc664810(v=vs.120).aspx
// Since Intel and ARM have different notation of "AES round", this issues additional instructions to maintain the same semantics.
static inline __attribute__((always_inline)) __m128i _mm_aesenc_si128( __m128i v, __m128i rkey )
{
    const __attribute__((aligned(16))) __m128i zero = {0};
    return veorq_u8(vaesmcq_u8(vaeseq_u8(v, zero)), rkey);
}

// Emits the Advanced Encryption Standard (AES) instruction aeskeygenassist. This instruction generates a round key for AES encryption.
// https://msdn.microsoft.com/en-us/library/cc714138(v=vs.120).aspx
static inline __attribute__((always_inline)) __m128i _mm_aeskeygenassist_si128(__m128i key, const int rcon)
{
    static const uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};
    uint32_t X1 = _mm_cvtsi128_si32(_mm_shuffle_epi32(key, 0x55));
    uint32_t X3 = _mm_cvtsi128_si32(_mm_shuffle_epi32(key, 0xFF));
    for( int i = 0; i < 4; ++i  ) {
        ((uint8_t*)&X1)[i] = sbox[ ((uint8_t*)&X1)[i] ];
        ((uint8_t*)&X3)[i] = sbox[ ((uint8_t*)&X3)[i] ];
    }
    return _mm_set_epi32(((X3 >> 8) | (X3 << 24)) ^ rcon, X3, ((X1 >> 8) | (X1 << 24)) ^ rcon, X1);
}

// Move the upper 2 single-precision (32-bit) floating-point elements from b to the lower 2 elements of dst, and copy the upper 2 elements from a to the upper 2 elements of dst.
// https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_movehl_ps&expand=3600
static inline __attribute__((always_inline)) __m128 _mm_movehl_ps(__m128 a, __m128 b)
{
    return vreinterpretq_m128_f32(vcombine_f32(vget_high_f32(vreinterpretq_f32_m128(b)), vget_high_f32(vreinterpretq_f32_m128(a))));    
}

// Add packed 64-bit integers in a and b, and store the results in dst.
// https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_add_epi64&expand=3600,97
static inline __attribute__((always_inline)) __m128i _mm_add_epi64(__m128i a, __m128i b)
{
    return vreinterpretq_s32_s64(vaddq_s64( vreinterpretq_s64_s32(a), vreinterpretq_s64_s32(b)));
}

#endif
