/*****************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\*****************************************************************************/
 /** @file       swDef.h
  *
  *  @brief      Define Standard Types not already defined.
  *
 ******************************************************************************/

#ifndef _SWDEF_H_
#define _SWDEF_H_

#ifndef CHAR
#define CHAR   char
#endif

#ifndef SINT
#define SINT   int
#endif

#ifndef UINT
#define UINT   unsigned int
#endif

/* These are required in 4.2 Builds */
#ifndef SINT8
#define SINT8   signed char
#endif

#ifndef UINT8
#define UINT8   unsigned char
#endif

#ifndef SINT16
#define SINT16  short
#endif

#ifndef UINT16
#define UINT16  unsigned short
#endif

#ifndef SINT32
#define SINT32  int
#endif

#ifndef UINT32
#define UINT32  unsigned int
#endif

#ifndef BOOL
#define BOOL    int
#endif

#ifndef TRUE
#define TRUE    (1U)
#endif

#ifndef FALSE
#define FALSE   (!TRUE)
#endif

#ifndef SUCCESS
#define SUCCESS (1U)
#endif

#ifndef FAIL
#define FAIL    (!SUCCESS)
#endif

#define PRIVATE         static
#define VOLATILE        volatile
#define CONST           const

#define DIM(array)      (sizeof(array)/sizeof(array[0]))

#define FIELDOFFSET(type,field)     ((size_t)&(((type*)0)->field))

#define MAX(a,b)        ((a) > (b) ? (a) : (b))
#define MIN(a,b)        ((a) < (b) ? (a) : (b))

#ifndef INT16
#define INT16 short
#endif

#endif  /* _SWDEF_H_ */


