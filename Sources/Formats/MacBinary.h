#ifndef MACBINARY_H
#define MACBINARY_H

#ifdef macintosh
#include <MacTypes.h>
#else
#include "Core/MacEmu.h"
#endif

enum MacHeaderVersion
	{
		kMacBinary,
		kMacBinaryII,
		kMacBinaryIII
	};


#define MBFindByte(theHeader, theOffset) (*((UInt8 *) (theHeader + theOffset)))
#define MBFindWord(theHeader, theOffset) (*((UInt16 *) (theHeader + theOffset)))
#define MBFindLong(theHeader, theOffset) (*((UInt32 *) (theHeader + theOffset)))
#define MBFindOSType(theHeader, theOffset) (*((OSType *) (theHeader + theOffset)))


enum
	{
		kOldVersionOffset						= 0,
		kFilenameLengthOffset					= 1,
		kFiletypeOffset							= 65,
		kCreatorOffset							= 69,
		kFinderFlagsHighOffset					= 73,
		kZeroFill1Offset						= 74,
		kVerticalPosOffset						= 75,
		kHorizontalPosOffset					= 77,
		kFolderIDOffset							= 79,
		kProtectedOffset						= 81,
		kZeroFill2Offset						= 82,
		kDataForkOffset							= 83,
		kResourceForkOffset						= 87,
		kCreatedDateOffset						= 91,
		kModifiedDateOffset						= 95,
		kCommentOffset							= 99,
		kFinderFlagsLowOffset					= 101,
		kMacBinarySignatureOffset				= 102,
		kScriptOffset							= 106,
		kExtendedFlagsOffset					= 107,
		kUnpackedOffset							= 116,
		kSecondaryHeaderOffset					= 120,
		kVersion2Offset							= 122,
		kMinVersionOffset						= 123,
		kChecksumOffset							= 124,
		kComputerOSOffset						= 126,
		
		kMacBinaryHeaderSize					= 128,
		kChecksumEnd							= kChecksumOffset,
		
		kVersion1ZeroFill						= 99,
		kVersion1ZeroFillLength					= 27,
		
		kVersion2								= 129,
		kVersion3								= 130
	};

#define kMacBinarySignature			'mBIN'

#define MBOldVersion(theHeader) (MBFindByte(theHeader, kOldVersionOffset))
#define MBFilenameLength(theHeader) (MBFindByte(theHeader, kFilenameLengthOffset))
#define MBFiletype(theHeader) (MBFindOSType(theHeader, kFiletypeOffset))
#define MBCreator(theHeader) (MBFindOSType(theHeader, kCreatorOffset))
#define MBFinderFlagsHigh(theHeader) (MBFindByte(theHeader, kFinderFlagsHighOffset))
#define MBZeroFill1(theHeader) (MBFindByte(theHeader, kZeroFill1Offset))
#define MBVerticalPos(theHeader) (MBFindWord(theHeader, kVerticalPosOffset))
#define MBHorizontalPos(theHeader) (MBFindWord(theHeader, kHorizontalPosOffset))
#define MBFolderID(theHeader) (MBFindWord(theHeader, kFolderIDOffset))
#define MBProtected(theHeader) (MBFindByte(theHeader, kProtectedOffset))
#define MBZeroFill2(theHeader) (MBFindByte(theHeader, kZeroFill2Offset))
#define MBDataFork(theHeader) (MBFindLong(theHeader, kDataForkOffset))
#define MBResourceFork(theHeader) (MBFindLong(theHeader, kResourceForkOffset))
#define MBCreatedDate(theHeader) (MBFindLong(theHeader, kCreatedDateOffset))
#define MBModifiedDate(theHeader) (MBFindLong(theHeader, kModifiedDateOffset))
#define MBComment(theHeader) (MBFindWord(theHeader, kCommentOffset))
#define MBFinderFlagsLow(theHeader) (MBFindByte(theHeader, kFinderFlagsHighOffset))
#define MBSignature(theHeader) (MBFindOSType(theHeader, kMacBinarySignatureOffset))
#define MBScript(theHeader) (MBFindByte(theHeader, kScriptOffset))
#define MBExtendedFlags(theHeader) (MBFindByte(theHeader, kExtendedFlagsOffset))
#define MBUnpacked(theHeader) (MBFindLong(theHeader, kUnpackedOffset))
#define MBSecondaryHeader(theHeader) (MBFindWord(theHeader, kSecondaryHeaderOffset))
#define MBVersion2(theHeader) (MBFindByte(theHeader, kVersion2Offset))
#define MBMinVersion(theHeader) (MBFindByte(theHeader, kMinVersionOffset))
#define MBChecksum(theHeader) (MBFindWord(theHeader, kChecksumOffset))
#define MBComputerOS(theHeader) (MBFindWord(theHeader, kComputerOSOffset))

#define MBFinderFlags(theHeader) ((MBFinderFlagsHigh(theHeader) << 8) + MBFinderFlagsLow(theHeader))


#endif // MACBINARY_H