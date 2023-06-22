#ifndef __AMCOMDEF_H__
#define __AMCOMDEF_H__


#if	defined(WINCE) || defined(WIN32)

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif//#ifndef _WCHAR_T_DEFINED

#elif (defined(EKA2) && defined(__GCCE__))
#ifndef _STDDEF_H_
#ifndef __cplusplus
typedef unsigned short wchar_t;
#define __wchar_t_defined
#endif
#endif


#elif defined(__GCCE__) || defined(__GCC32__)
#ifndef _STDDEF_H_
typedef unsigned short wchar_t;
#endif

#endif//#if	defined(WINCE)


#if	defined(__GCC32__)  || defined(__GCCE__) || defined(WINCE) || defined(WIN32)
typedef wchar_t			MWChar;
#else
typedef unsigned short	MWChar;
#endif


typedef long					MLong;
typedef float					MFloat;
typedef double					MDouble;
typedef unsigned char			MByte;
typedef unsigned short			MWord;	
typedef unsigned int 			MDWord;	
typedef void*					MHandle;
typedef char					MChar;
typedef long					MBool;
typedef void					MVoid;
typedef void*					MPVoid;
typedef char*					MPChar;
typedef short					MShort;
typedef const char*				MPCChar;
typedef	MLong					MRESULT;
typedef MDWord					MCOLORREF; 
typedef	signed		char		MInt8;
typedef	unsigned	char		MUInt8;
typedef	signed		short		MInt16;
typedef	unsigned	short		MUInt16;
typedef signed		int			MInt32;
typedef unsigned	int			MUInt32;

#if !defined(M_UNSUPPORT64)
#if defined(_MSC_VER)
typedef signed		__int64		MInt64;
typedef unsigned	__int64		MUInt64;
#else
typedef signed		long long	MInt64;
typedef unsigned	long long	MUInt64;
#endif
#endif

typedef struct __tag_rect
{
	MInt32 left;
	MInt32 top;
	MInt32 right;
	MInt32 bottom;
} MRECT, *PMRECT;

typedef struct __tag_point
{ 
	MInt32 x; 
	MInt32 y; 
} MPOINT, *PMPOINT;


#define MNull		0
#define MFalse		0
#define MTrue		1

#ifndef MAX_PATH
#define MAX_PATH	256
#endif

#ifdef M_WIDE_CHAR
#define MTChar MWChar
#else 
#define MTChar MChar
#endif

#endif
