#include "MacBinaryIO.h"

#include "Core/MacHeaders.h"

#include <fstream>
#include <memory>

#include "MacBinary.h"
#include "Core/CommandLine.h"
#include "Core/WriteFork.h"
#include "Core/DefaultFInfo.h"

// CRC 16 table lookup array
static UInt16 CRC16Table[256] =
	{0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
	0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
	0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
	0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
	0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
	0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
	0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
	0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
	0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
	0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
	0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
	0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
	0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
	0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
	0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
	0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
	0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
	0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
	0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
	0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
	0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
	0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
	0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0};

// CalculateCRC 
static UInt16 CalculateCRC(UInt16 CRC, const char* dataBlock, UInt32 dataSize)
{	
	while (dataSize)
	{
		CRC = (CRC << 8) ^ CRC16Table[((*dataBlock) ^ (CRC >> 8)) & 0x00FF];
		dataBlock++;
		dataSize--;
	}

	return CRC;
}


bool IsValidMacBinary(char *theHeader, MacHeaderVersion *theVersion)
{
	bool isValid;
	MacHeaderVersion internalVersion;
	int i;
	
	isValid = false;
	internalVersion = kMacBinary;
	
	/*
		MacBinary, MacBinary II, and MacBinary III have the following minimum requirements:
			MBOldVersion must be zero
			MBZeroFill1 must be zero
			MBZeroFill2 must be zero
			MBComputerOS must be zero
			MBFilenameLength must be between 1 and 63
			
			MacBinary III restricts MBFilenameLength to 31, but that's checked for later
	*/
	
	if ((MBOldVersion(theHeader) == 0) && (MBZeroFill1(theHeader) == 0) &&
		(MBZeroFill2(theHeader) == 0) && (MBComputerOS(theHeader) == 0) &&
		(MBFilenameLength(theHeader) >= 1) && (MBFilenameLength(theHeader) <= 63))
	{
		/*
			MacBinary II and MacBinary III have the following requirements:
				MBSecondaryHeader must be zero
				MBMinVersion must be kVersion2 (129)
				MBChecksum must match the CRC of bytes 0 to kChecksumEnd (124)
		*/
		
		if ((MBSecondaryHeader(theHeader) == 0) && (MBMinVersion(theHeader) == kVersion2) &&
			(MBChecksum(theHeader) == EndianU16_NtoB(CalculateCRC(0, theHeader, kChecksumEnd))))
		{
			// Check for the specific MacBinary II version number
			if (MBVersion2(theHeader) == kVersion2)
			{
				isValid = true;
				internalVersion = kMacBinaryII;
			}
			
			/*
				MacBinary III has the following requirements:
				MBVersion2 must be kVersion3 (130)
				MBSignature must be kMacBinarySignature ('mBIN')
				MBFilenameLength must be between 1 and 31
			*/
			
			// Check for the specific MacBinary III version and tags
			else if ((MBVersion2(theHeader) == kVersion3) &&
				(EndianU32_BtoN(MBSignature(theHeader)) == kMacBinarySignature) &&
				(MBFilenameLength(theHeader) <= 31))
			{
				isValid = true;
				internalVersion = kMacBinaryIII;
			}
		}
		
		if (internalVersion == kMacBinary)
		{
			/*
				MacBinary has the following additional requirements:
					MBDataFork must not be bigger than 0x007FFFFF
					MBResourceFork must not be bigger than 0x007FFFFF
					
					Unused bytes in the header must also be zero
			*/
			
			// Test for valid ranges in the data fork and resource fork sizes
			if (!((EndianU32_BtoN(MBDataFork(theHeader)) & ~0x007FFFFF) && (EndianU32_BtoN(MBResourceFork(theHeader)) & ~0x007FFFFF)))
				isValid = true;
			
			// Test for the large field of zeros
			i = 0;
			if (0)
			{
				while (isValid && (i < kVersion1ZeroFillLength))
				{
					if (MBFindByte(theHeader, kVersion1ZeroFill + i) != 0)
						isValid = false;
				
					i++;
				}
			}
		}
	}
	
	if (theVersion)
		*theVersion = internalVersion;
	
	return isValid;
}



class SubIStream : public MacForkInputStream {
public:
	SubIStream (istream &inputStream,UInt32 offset,UInt32 size)
		: mStream (inputStream)
	{
		mOffset = offset;
		mCurrentOffset	= offset;
		mSize = size;
	}
	virtual ~SubIStream ()
	{}
	virtual UInt32	getSize ()
	{
		return mSize;
	}
	virtual bool read (void *buffer, UInt32 &size)
	{
		if (mCurrentOffset-mOffset >= mSize)
		{
			size = 0;
			return false;
		}
		mStream.seekg (mCurrentOffset,ios::beg);
		if (mCurrentOffset+size >= mOffset+mSize)
		{
			size = mOffset+mSize-mCurrentOffset;
		}
		mStream.read (reinterpret_cast<char*>(buffer), size);
		mCurrentOffset += size;

		return true;
	}
private:
	istream&		mStream;
	UInt32			mOffset;
	UInt32			mCurrentOffset;
	UInt32			mSize;
};


MacBinFileInput::MacBinFileInput (const std::string	&path)
    : mStream(path.c_str(), ios::in | ios::binary)
{
	mDataSize	= 0;
	mResSize	= 0;
	mHasComment = false;
	mHasFInfo	= 0;
	mHasFXInfo	= 0;

	char macBinHeader[128];
	if (!mStream.good())
	{
        std::string err = std::string("Cannot open:\"") + path + std::string("\" for reading");
		throw CL::BasicException (err.c_str());
	}

	mStream.read(macBinHeader,128);
	if (!mStream.good())
		throw CL::BasicException ("Cannot read the header\n");

	mFileName = std::string((char*)&macBinHeader[kFilenameLengthOffset+1],MBFilenameLength(macBinHeader));
	MacHeaderVersion	theVersion;
	if (!IsValidMacBinary(macBinHeader,&theVersion))
		throw CL::BasicException ("Not a valid mac binary header!\n");



	mDataSize = EndianU32_BtoN(MBDataFork(macBinHeader));
	mDataOffset = (kMacBinaryHeaderSize + EndianU16_BtoN(MBSecondaryHeader(macBinHeader)) + 0x7F) & ~0x7F;
	mResSize = EndianU32_BtoN(MBResourceFork(macBinHeader));
	mResOffset = (mDataSize+mDataOffset+0x7F) & ~0x7F;

	mHasFInfo = true;
	memset(&mFInfo,0,sizeof(mFXInfo)) ;
	mFInfo.fdType = MBFiletype(macBinHeader);
	mFInfo.fdCreator = MBCreator(macBinHeader);
	if (theVersion == kMacBinary)
		mFInfo.fdFlags = EndianU16_NtoB(MBFinderFlagsHigh(macBinHeader) << 8);
	else
		mFInfo.fdFlags = EndianU16_NtoB(MBFinderFlags(macBinHeader));

	mFInfo.fdLocation.v = MBVerticalPos(macBinHeader);
	mFInfo.fdLocation.h = MBHorizontalPos(macBinHeader);

	if (theVersion == kMacBinaryIII)
	{
		mHasFXInfo = true;
		memset(&mFXInfo,0,sizeof(mFXInfo)) ;
		mFXInfo.fdScript = MBScript(macBinHeader);
		mFXInfo.fdXFlags = MBExtendedFlags(macBinHeader);
	}

}

MacBinFileInput::~MacBinFileInput()
{
}


MacForkInputStream*	 
MacBinFileInput::openDF ()
{
	if (mDataSize == 0)
		return NULL;
	else
		return new SubIStream(mStream,mDataOffset,mDataSize);
}

MacForkInputStream*
MacBinFileInput::openRF ()
{
	if (mResSize == 0)
		return NULL;
	else
		return new SubIStream(mStream,mResOffset,mResSize);

}
bool MacBinFileInput::getInfo (	FInfo&		finderInfo,
								FXInfo&		extendedInfo ) 
{
	DefaultFInfo (finderInfo,extendedInfo);
	if (!mHasFInfo)
		return false;
	finderInfo = mFInfo;
	if (mHasFXInfo)
		extendedInfo = mFXInfo;

	return true;
}

bool MacBinFileInput::getComment ( std::string&	thecomment )
{
	return false;
}

void EncodeMacBinIII (MacFileInput &theInput, std::ostream &output)
{
	int i;
	char macBinHeader [128] = {0};
	char zeroPad [128] = {0};
	const std::string fileName = theInput.getFileName();

	macBinHeader[0] = 0; // old version number
	if ((fileName.size() > 31) || fileName.empty())
		throw CL::BasicException ("The fileName is too long\n");
	macBinHeader[1] = fileName.size();
	strcpy (&macBinHeader[2],fileName.c_str());

	FInfo finderInfo = {0} ;
	FXInfo extendedFinderInfo = {0};
	
	theInput.getInfo (finderInfo, extendedFinderInfo);
	*((FInfo*)(&macBinHeader[65])) = finderInfo;

	macBinHeader[81] = 0; // proteced flag
	macBinHeader[82] = 0; // zero fill

	// Read DF size
	std::auto_ptr<MacForkInputStream> df (theInput.openDF());
	UInt32 dataForkLen = (df.get()  ? df->getSize() : 0);
	// Read RF size
	std::auto_ptr<MacForkInputStream> rf (theInput.openRF());
	UInt32 resourceForkLen = (rf.get() ? rf->getSize() : 0);

	*((UInt32*)(&macBinHeader[83])) = EndianU32_NtoB(dataForkLen);
	*((UInt32*)(&macBinHeader[87])) = EndianU32_NtoB(resourceForkLen);

	*((UInt32*)(&macBinHeader[91])) = 0;	// Creation date
	*((UInt32*)(&macBinHeader[95])) = 0;	// Modification date
	*((UInt16*)(&macBinHeader[99])) = 0;	// Get Info Comment (ToDo)
	
	macBinHeader[101] = macBinHeader[74]; // Finder flags
	macBinHeader[74] = 0;
	macBinHeader[102] = 'm'; // Signature of id purposes ('mBIN')
	macBinHeader[103] = 'B';
	macBinHeader[104] = 'I';
	macBinHeader[105] = 'N';

	macBinHeader[106] = extendedFinderInfo.fdScript; // script of file name
	macBinHeader[107] = extendedFinderInfo.fdXFlags; // extended finder flags
	for (i = 108; i <= 115; i++)
		macBinHeader[i] = 0;
	*((UInt32*)(&macBinHeader[116])) = 0;	// Length of total files when packed files are unpacked
	*((UInt16*)(&macBinHeader[120])) = 0;	// Length of secondary header
	(unsigned char&)macBinHeader[122] = (unsigned char)130; // Version number that the uploading program was written for
	(unsigned char&)macBinHeader[123] = (unsigned char)129; // Minimum version to read
	*((UInt16*)(&macBinHeader[124])) = EndianU16_NtoB(CalculateCRC (0,macBinHeader, 124));

	// Dump header:
	output.write (macBinHeader,128);

	// Dump data fork:
	if (dataForkLen != 0) {
		StdOStream	theOutput (output); 
		writeFork (*df, theOutput); 
	}
	// Pad with 0:
	UInt32 rem = (dataForkLen & 0x7F);
	if (rem != 0)
		output.write (zeroPad, 128-rem );
	
	// Dump resource fork:
	if (resourceForkLen != 0) {
		StdOStream	theOutput (output);
		
		writeFork (*rf, theOutput);
	}
	// Pad with 0
	rem = (resourceForkLen & 0x7F);
	if (rem != 0)
		output.write (zeroPad, 128-rem );	

}



// Register to command line:
namespace {

static MacFileInput *create (int argc,const char** argv, int &processed)
{
	if (argc == 1)
		throw CL::FormatException("MacBinary input: expected filename\n");
	
	processed=2;
	return new MacBinFileInput(argv[1]);
}
static void process (MacFileInput *input, int argc,const char **argv, int &processed)
{
	processed = 2;
	if (argc < 2)
		throw CL::FormatException ("as output requires one filename\n");
	std::ofstream	theOutputStream(argv[1],ios::out | ios::binary);
	EncodeMacBinIII (*input,theOutputStream);
}
}
void RegisterMacBinaryIO () 
{
  CL::RegisterMacFileInput ("mb","-mb <filename> : where filename is a MacBinary I,II or III encoded file!",create);
  CL::RegisterMacFileOutput ("mb","-mb <filename> : encodes the file as MacBinaryIII to filename",process); 
}

