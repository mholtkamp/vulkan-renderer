#pragma once

#ifdef _DEBUG
#define LogDebug(...) printf( __VA_ARGS__ ); printf("\n");
#else
#define LogDebug(...) 1;
#endif

#define LogWarning(...) printf( __VA_ARGS__ ); printf("\n");
#define LogError(...) printf( __VA_ARGS__ ); printf("\n");