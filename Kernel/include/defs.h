/***************************************************
  Defs.h
****************************************************/

#ifndef _defs_
#define _defs_

/* Flags for access rights to segments */
#define ACS_PRESENT 0x80 /* segment present in memory */
#define ACS_CSEG    0x18 /* code segment */
#define ACS_DSEG    0x10 /* data segment */
#define ACS_READ    0x02 /* readable segment */
#define ACS_WRITE   0x02 /* writeable segment */
#define ACS_IDT     ACS_DSEG
#define ACS_INT_386 0x0E /* 32-bit interrupt gate */
#define ACS_INT     (ACS_PRESENT | ACS_INT_386)

#define ACS_CODE  (ACS_PRESENT | ACS_CSEG | ACS_READ)
#define ACS_DATA  (ACS_PRESENT | ACS_DSEG | ACS_WRITE)
#define ACS_STACK (ACS_PRESENT | ACS_DSEG | ACS_WRITE)

#endif