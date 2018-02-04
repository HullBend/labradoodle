
/* ------------------------------------------------------------------------ */
/* Minimal stdint.h declarations for 32-Bit mode HP-UX aCC Compiler on      */
/* systems that don't have stdint.h yet.                                    */
/*                                                                          */
/* Hope, that works ... ;-)                                                 */
/* ------------------------------------------------------------------------ */


#if defined(__hpux) /* && COMPILER VERSION < XXX */


#ifndef __ACC_HPUX_BIT_TYPES_DEFINED__
#define __ACC_HPUX_BIT_TYPES_DEFINED__


typedef signed char          int8_t;
typedef signed short         int16_t;
typedef signed long int      int32_t; /* in aCC 32-Bit mode */
typedef signed long long     int64_t; /* in aCC 32-Bit mode */
typedef unsigned char        uint8_t;
typedef unsigned short       uint16_t;
typedef unsigned long int    uint32_t; /* in aCC 32-Bit mode */
typedef unsigned long long   uint64_t; /* in aCC 32-Bit mode */

typedef uint8_t              u_int8_t;
typedef uint16_t             u_int16_t;
typedef uint32_t             u_int32_t;
typedef uint64_t             u_int64_t;


#endif /* __ACC_HPUX_BIT_TYPES_DEFINED__ */


#endif /* __hpux */
