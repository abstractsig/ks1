/*
 *
 * configure compiler for io build
 *
 */
#ifndef configure_io_build_H_
#define configure_io_build_H_
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <alloca.h>

#define STVM 						0
#define USE_LIBC

#define EVENT_THREAD_INTERRUPT			PendSV_IRQn

#define EVENT_DATA const
#define DBP_FIELD1				14
#define DBP_FIELD2				20

#define IO_PRINT_BUFFER_SIZE					80
#define TEXT_ENCODING_INITIAL_SIZE			IO_PRINT_BUFFER_SIZE
#define TEXT_ENCODING_GROWTH_INCREMENT		IO_PRINT_BUFFER_SIZE
#define STB_SPRINTF_MIN							IO_PRINT_BUFFER_SIZE	// characters per callback

#define IO_PRINTF_SOCKET 						0
#define CONTROL_UART_SPEED						115200

//
// compiler specific stuff
//
#if defined ( __GNUC__ )
# define PACK_STRUCTURE			__attribute__((__packed__))
# define ALLOCATE_ALIGN(size)	__attribute__((__aligned__(size)))
# define UNUSED(v)				(void)(v)
# define SIZEOF(a)				(sizeof(a)/sizeof(a[0]))
# define OPTIMIZE_O2(fn)		fn __attribute__((optimize("-O2")));
#define INLINE_FUNCTION			static inline
  typedef float  float32_t;
  typedef double float64_t;
#else
# error "this compiler is not supported"
#endif

#define IO_GRAPHICS_FLOAT_IS_FLOAT32

#define MIN(a,b) 				(((a)<(b))?(a):(b))
#define plural(v)				(((v) > 1)? "s" : "")
#define KB(n)					((n) * 1024)

//
// a method of producing compile-time size assertions
//
// combine arguments (after expanding arguments)
#define GLUE(a,b) __GLUE(a,b)
#define __GLUE(a,b) a ## b
#define CVERIFY(expr, msg) typedef char GLUE (compiler_verify_, msg) [(expr) ? (+1) : (-1)]
#define COMPILER_VERIFY(exp) CVERIFY (exp, __LINE__)

#define IO_VALUE_ENCODING_EPOCH {\
	0xa4,0x83,0x5f,0x18,0x16,0xd3,0xb7,0x9b,\
	0x24,0xfd,0x53,0x52,0x00,0x75,0x69,0x14,\
	0x1a,0x22,0x36,0xb1,0x21,0x71,0x14,0x23,\
	0xaf,0xeb,0x23,0x34,0x11,0x70,0xf3,0x68,}


#endif /* configure_io_build_H_ */
