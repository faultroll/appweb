/**
 * \file: projectdefs.h
 * \brief: data type used in the project
 */

#ifndef _PROJECTDEFS_H_
#define _PROJECTDEFS_H_
#include "osdep.h"
// -------------------
// often used typedef
/*
typedef int bool;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef signed char int8;
typedef signed short int16;
typedef signed int int32;

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned short ushort;
*/
typedef int TICK; // see also in "tick.h"

typedef union TVariant {
	int intValue;
	int intValues[1];

	unsigned int uintValue;
	unsigned int uintValues[1];

	long longValue;
	long longValues[1];

	unsigned long ulongValue;
	unsigned long ulongValues[1];

	short shortValue;
	short shortValues[1];

	unsigned short ushortValue;
	unsigned short ushortValues[1];

	float floatValue;
	float floatValues[1];

	void *ptrValue;
	void *ptrValues[1];

	char charValue;
	char charValues[1];

	unsigned char ucharValue;
	unsigned char ucharValues[1];

	char chars[sizeof(long)];
	unsigned char bytes[sizeof(long)];

} TVariant;

typedef struct TStringS8 {
	char chars[8];
} TStringS8;

typedef struct TStringS16 {
	char chars[16];
} TStringS16;

typedef struct TStringS32 {
	char chars[32];
} TStringS32;

typedef struct TStringS64 {
	char chars[64];
} TStringS64;

typedef struct TStringS100 {
	char chars[100];
} TStringS100;

typedef struct TStringS128 {
	char chars[128];
} TStringS128;

typedef struct TStringSmall {
	char chars[16];
} TStringSmall;

typedef struct TStringMiddle {
	char chars[64];
} TStringMiddle;

typedef struct TStringLarge {
	char chars[128];
} TStringLarge;

// string for file name
typedef struct TStringPath {
	char chars[64];
} TStringPath;

// string
typedef char * TString;

#endif // _PROJECTDEFS_H_
