#ifndef APPLESINGLE_H
#define APPLESINGLE_H

#include "Core/MacHeaders.h"

#if PRAGMA_STRUCT_ALIGN
#pragma options align=mac68k
#endif

#ifdef _MSC_VER
#pragma pack (2)
#endif

struct AppleSingleHeader
	{
	
		UInt32		fMagicNumber;
		UInt32		fVersion;
		UInt32		fFiller1;
		UInt32		fFiller2;
		UInt32		fFiller3;
		UInt32		fFiller4;
		UInt16		fEntries;
	
	};

struct AppleSingleEntry
	{
	
		UInt32		fEntryID;
		UInt32		fOffset;
		UInt32		fLength;
		
	};

#ifdef _MSC_VER
#pragma pack ()
#endif

#if PRAGMA_STRUCT_ALIGN
#pragma options align=reset
#endif

const UInt32 kAppleSingleMagicNumber				= 0x00051600;
const UInt32 kAppleDoubleMagicNumber				= 0x00051607;
const UInt32 kAppleSingleVersion					= 0x00020000;
const UInt32 kWeirdoAppleDoubleVersion				= 0x00010001;


const UInt32 kAppleSingleDataFork					= 1;
const UInt32 kAppleSingleResFork					= 2;
const UInt32 kAppleSingleComment					= 4;
const UInt32 kAppleSingleDate						= 8;
const UInt32 kAppleSingleMacFInfo					= 9;
const UInt32 kDefaultBufferSize						= 131072;

#endif // APPLESINGLE_H
