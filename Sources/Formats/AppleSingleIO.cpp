#include "AppleSingleIO.h"

#ifdef macintosh
#include <Endian.h>
#else
#include "Core/MacEmu.h"
#endif

#include <fstream>
#include <memory>

#include "AppleSingle.h"
#include "Core/CommandLine.h"
#include "Core/WriteFork.h"
#include "Core/DefaultFInfo.h"

#include "Core/FileUtils.h"


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


ASFileInput::ASFileInput (const std::string	&path)
: mStream(path.c_str(), ios::in | ios::binary)
{
	mDataSize	= 0;
	mResSize	= 0;
	mHasComment = false;
	mFInfoSize	= 0;

	AppleSingleHeader	header;
	if (!mStream.good())
	{
		throw CL::BasicException ("Bad stream passed to mStream!\n");
	}
	mStream.read(reinterpret_cast<char*>(&header),sizeof(header));
	if (!mStream.good())
	{
		throw CL::BasicException ("Cannot read the header\n");
	}
	
        if (EndianU32_BtoN(header.fMagicNumber) != kAppleSingleMagicNumber)
		throw CL::BasicException ("Bad apple single header!\n");

	if (EndianU32_BtoN(header.fVersion) != kAppleSingleVersion)
		throw CL::BasicException ("Bad apple single version!\n");
	
	int i;
	for (i = 0; i < EndianU16_BtoN(header.fEntries); i++)
	{
		AppleSingleEntry entry;
		mStream.read(reinterpret_cast<char*>(&entry),sizeof(entry));
		
		UInt32 size = EndianU32_BtoN (entry.fLength);
		UInt32 offset = EndianU32_BtoN (entry.fOffset);
		
		switch (EndianU32_BtoN(entry.fEntryID))
		{
		case kAppleSingleDataFork:
			mDataSize = size;
			mDataOffset = offset;
			break;
		case kAppleSingleResFork:
			mResSize = size;
			mResOffset = offset;
			break;
		case kAppleSingleComment:
			break;
		case kAppleSingleMacFInfo:
			mFInfoSize = size;
			mFInfoOffset = offset;
			break;
		}
	}
	
	// Determine name:
	mFileName = FileUtils::GetFileNameFromPath(path);
}

ASFileInput::~ASFileInput()
{
}


MacForkInputStream*	 
ASFileInput::openDF ()
{
	if (mDataSize == 0)
		return NULL;
	else
		return new SubIStream(mStream,mDataOffset,mDataSize);
}

MacForkInputStream*
ASFileInput::openRF ()
{
	if (mResSize == 0)
		return NULL;
	else
		return new SubIStream(mStream,mResOffset,mResSize);

}
bool ASFileInput::getInfo (	FInfo&		finderInfo,
				FXInfo&		extendedInfo ) 
{
	DefaultFInfo (finderInfo,extendedInfo);
	if (mFInfoSize == 0)
		return false;

	if ((mFInfoSize != sizeof(finderInfo)+sizeof(extendedInfo)) &&
		(mFInfoSize != sizeof(finderInfo)))
		throw CL::BasicException ("Bad size of the FInfo enty!\n");

	mStream.seekg (mFInfoOffset,ios::beg);
	mStream.read (reinterpret_cast<char*>(&finderInfo), sizeof(finderInfo));
	if (mFInfoSize == sizeof(finderInfo)+sizeof(extendedInfo))
		mStream.read (reinterpret_cast<char*>(&extendedInfo), sizeof (extendedInfo));

	return true;
}

std::string ASFileInput::getFileName()
{
	// ToDo if this info present in AS return that value!!!
	return mFileName;
}

bool ASFileInput::getComment ( std::string&	thecomment )
{
	return false;
}


static void writeAppleSingleEntry(ostream &output, const AppleSingleEntry& entry)
{
	AppleSingleEntry	entryBig;

	entryBig.fEntryID = EndianU32_NtoB (entry.fEntryID);
	entryBig.fLength = EndianU32_NtoB (entry.fLength);
	entryBig.fOffset = EndianU32_NtoB (entry.fOffset);

	output.write ((char*)&entryBig, sizeof(entryBig));
}


static void EncodeAS (MacFileInput &theInput, std::ostream &output)
{
	AppleSingleHeader	encodeFileHeader = {0} ;
	AppleSingleEntry	dataForkEntry = {0} ;
	AppleSingleEntry	resForkEntry = {0} ;
	AppleSingleEntry	finderInfoEntry = {0} ;
	
	FInfo				finderInfo = {0};
	FXInfo				extendedInfo = {0};

	UInt32				curOffset = 0;

	UInt32				dataForkLen = 0;
	UInt32				resourceForkLen = 0;
	
	// Read DF size
	std::auto_ptr<MacForkInputStream> df (theInput.openDF());
	dataForkLen = (df.get()  ? df->getSize() : 0);
	// Read RF size
	std::auto_ptr<MacForkInputStream> rf (theInput.openRF());
	resourceForkLen = (rf.get() ? rf->getSize() : 0);

	encodeFileHeader.fMagicNumber	= kAppleSingleMagicNumber;
	encodeFileHeader.fVersion 		= kAppleSingleVersion;
	encodeFileHeader.fFiller1 		= 0;
	encodeFileHeader.fFiller2 		= 0;
	encodeFileHeader.fFiller3 		= 0;
	encodeFileHeader.fFiller4 		= 0;
	encodeFileHeader.fEntries 		= 2;

	curOffset = sizeof(AppleSingleHeader) + 2 * sizeof(AppleSingleEntry);

	if (resourceForkLen > 0)
		curOffset += sizeof(AppleSingleEntry);

	dataForkEntry.fEntryID = kAppleSingleDataFork;
	dataForkEntry.fOffset =	curOffset;
	dataForkEntry.fLength = dataForkLen;

	curOffset += dataForkEntry.fLength;

	if (resourceForkLen > 0)
	{
		encodeFileHeader.fEntries++;

		resForkEntry.fEntryID = kAppleSingleResFork;
		resForkEntry.fOffset = curOffset;
		resForkEntry.fLength = resourceForkLen;	
		
		curOffset += resForkEntry.fLength;
	}

	finderInfoEntry.fEntryID = kAppleSingleMacFInfo;
	finderInfoEntry.fOffset = curOffset;
	finderInfoEntry.fLength = sizeof(FInfo) + sizeof(FXInfo);

	curOffset += finderInfoEntry.fLength;

	// Conver header
	encodeFileHeader.fMagicNumber	= EndianU32_NtoB(encodeFileHeader.fMagicNumber);
	encodeFileHeader.fVersion 		= EndianU32_NtoB(encodeFileHeader.fVersion);
	encodeFileHeader.fFiller1 		= EndianU32_NtoB(encodeFileHeader.fFiller1);
	encodeFileHeader.fFiller2 		= EndianU32_NtoB(encodeFileHeader.fFiller2);
	encodeFileHeader.fFiller3 		= EndianU32_NtoB(encodeFileHeader.fFiller3);
	encodeFileHeader.fFiller4 		= EndianU32_NtoB(encodeFileHeader.fFiller4);
	encodeFileHeader.fEntries 		= EndianU16_NtoB(encodeFileHeader.fEntries);

	// Write header
	output.write ((char*)&encodeFileHeader,sizeof(encodeFileHeader));
	
	// Write data fork header 
	writeAppleSingleEntry (output, dataForkEntry);

	// Write resource fork header
	if (resourceForkLen > 0)
	{
		writeAppleSingleEntry (output, resForkEntry);
	}

	// Write finder info header
	writeAppleSingleEntry (output, finderInfoEntry) ;
	
	// Write data fork
	StdOStream	outputStream (output);
	writeFork (*df,outputStream);

	// Write resource fork
	writeFork (*rf,outputStream);

	// Write finder info
	theInput.getInfo(finderInfo,extendedInfo);
	output.write ((char*)&finderInfo,sizeof(finderInfo));
	output.write ((char*)&extendedInfo,sizeof(extendedInfo));
}




// Register to command line:
namespace {

static MacFileInput *create (int argc,const char** argv, int &processed)
{
	if (argc == 1)
		throw CL::FormatException("AppleSingle input: expected fileName\n");
	
	processed=2;
	return new ASFileInput(argv[1]);
}
static void process (MacFileInput *input, int argc,const char **argv, int &processed)
{
	processed = 2;
	if (argc != 2)
		throw CL::FormatException ("as output requires one filename\n");
	
	std::ofstream	outputStream (argv[1],ios::out | ios::binary);
		
	EncodeAS (*input,outputStream);
}
}
void RegisterAppleSingleIO() 
{
  CL::RegisterMacFileInput ("as","-as <filename> : where filename is an AppleSingle encoded file",create);
  CL::RegisterMacFileOutput ("as","-as <filename> : file will be encoded to filename AppleSingle file",process);

}

