#ifndef WINMAC_H
#define WINMAC_H

#ifdef WINDOWS
#include <Windows.h>
#endif

/* MacTypes.h */
typedef unsigned long	UInt32;
typedef unsigned short	UInt16;
typedef unsigned char	UInt8;

typedef long			SInt32;
typedef short			SInt16;
typedef	char			SInt8;

typedef UInt32			OSType;

typedef SInt16			OSErr;
const	OSErr			noErr = 0;
/* Endian.h */

#define Endian32_Swap(value)                     		\
            (((((UInt32)value)<<24) & 0xFF000000)  | 	\
             ((((UInt32)value)<< 8) & 0x00FF0000)  | 	\
             ((((UInt32)value)>> 8) & 0x0000FF00)  | 	\
             ((((UInt32)value)>>24) & 0x000000FF))

#define Endian16_Swap(value)                 		\
            (((((UInt16)value)<<8) & 0xFF00)   | 	\
             ((((UInt16)value)>>8) & 0x00FF))

inline UInt32 EndianU32_NtoB (UInt32	value)
{
	return Endian32_Swap (value);
}

inline UInt16 EndianU16_NtoB (UInt16	value)
{
	return Endian16_Swap (value);
}
inline UInt32 EndianU32_BtoN (UInt32	value)
{
	return Endian32_Swap (value);
}

inline UInt16 EndianU16_BtoN (UInt16	value)
{
	return Endian16_Swap (value);
}

#pragma pack(2)

struct Point 
{
SInt16	v;
SInt16	h;
};

/* Finder.h */
/* Finder flags (finderFlags, fdFlags and frFlags) */
/* Any flag reserved or not specified should be set to 0. */
/* If a flag applies to a file, but not to a folder, make sure to check */
/* that the item is not a folder by checking ((ParamBlockRec.ioFlAttrib & ioDirMask) == 0) */
const UInt16	kIsOnDesk				= 0x0001;                    /* Files and folders (System 6) */
const UInt16	kColor					= 0x000E;                   /* Files and folders */
                                                                /* bit 0x0020 was kRequireSwitchLaunch, but is now reserved for future use*/
const UInt16	kIsShared				= 0x0040;                   /* Files only (Applications only) */
                                                                /* If clear, the application needs to write to */
                                                                /* its resource fork, and therefore cannot be */
                                                                /* shared on a server */
const UInt16    kHasNoINITs				= 0x0080;                   /* Files only (Extensions/Control Panels only) */
                                                                /* This file contains no INIT resource */
const UInt16    kHasBeenInited			= 0x0100;                   /* Files only */
                                                                /* Clear if the file contains desktop database */
                                                                /* resources ('BNDL', 'FREF', 'open', 'kind'...) */
                                                                /* that have not been added yet. Set only by the Finder */
                                                                /* Reserved for folders - make sure this bit is cleared for folders */
                                                                /* bit 0x0200 was the letter bit for AOCE, but is now reserved for future use */
const UInt16    kHasCustomIcon			= 0x0400;           		/* Files and folders */
const UInt16    kIsStationery          	= 0x0800;           		/* Files only */
const UInt16    kNameLocked            	= 0x1000;                	/* Files and folders */
const UInt16    kHasBundle           	= 0x2000;            		/* Files only */
const UInt16    kIsInvisible        	= 0x4000;              		/* Files and folders */
const UInt16    kIsAlias          		= 0x8000;             		/* Files only */


struct FInfo {
    OSType                          fdType;                     /* The type of the file */
    OSType                          fdCreator;                  /* The file's creator */
    UInt16                          fdFlags;                    /* Flags ex. kHasBundle, kIsInvisible, etc. */
    Point                           fdLocation;                 /* File's location in folder. */
                                                                /* If set to {0, 0}, the Finder will place the item automatically */
    SInt16                          fdFldr;                     /* Reserved (set to 0) */
};
/* Extended file info */
/*
     IMPORTANT:
     In MacOS 8, the fdIconID and fdComment fields were changed
     to become reserved fields for the Finder.
     The fdScript has become an extended flag.
*/

struct FXInfo {
    SInt16                          fdIconID;                   /* Reserved (set to 0) */
    SInt16                          fdReserved[3];              /* Reserved (set to 0) */
    SInt8                           fdScript;                   /* Extended flags. Script code if high-bit is set */
    SInt8                           fdXFlags;                   /* Extended flags */
    SInt16                          fdComment;                  /* Reserved (set to 0). Comment ID if high-bit is clear */
    SInt32                          fdPutAway;                  /* Put away folder ID */
};

#pragma pack()

#endif // WINMAC_H