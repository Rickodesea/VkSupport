//Copyright (C) 2024 Alrick Grandison, Algodal™

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define VK_SET(ptr, val) do{ if(ptr) *ptr = val; }while(0)
#define VK_REQSET(ptr, val) do{ assert(ptr); *ptr = val; }while(0)
#define VK_FREE(ptr) do{ if(ptr) free(ptr); ptr=0; }while(0)

#define VK_ENUMERATE(T, F, P, N, B, S) T B[S] = {}; uint32_t N = 0; do { \
    F(P, &N, 0); \
    N = (N < S) ? N : S; \
    F(P, &N, B); \
} while(0);

#define VK_ENUMERATE2(T, F, P, Q, N, B, S) T B[S] = {}; uint32_t N = 0; do { \
    F(P, Q, &N, 0); \
    N = (N < S) ? N : S; \
    F(P, Q, &N, B); \
} while(0);

#ifdef __cplusplus
}
#endif










