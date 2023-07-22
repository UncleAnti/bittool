#include <stdio.h>
#include <stdint.h>

#define BIT_BUF_SIZE (4096)

typedef void (*fptr)(FILE*, FILE*, const char *);

typedef struct engines_s{
	const char * name;
	const char mode;
	fptr func;
	const char * help;
} engines_t;

#ifdef __cplusplus 
	extern "C" { 
#endif

int has_char(const char * str, const char c);

#ifdef __cplusplus 
	} 
#endif

#define ENGINE_ENTRY(name, mode, func, help) { name, mode, func, help }
#define ENGINE_EXPORT(...) static __attribute__((section("engine"),aligned(4),used)) engines_t engines[] = {  __VA_ARGS__ }
