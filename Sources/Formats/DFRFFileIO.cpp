#include "DFRFFileIO.h"

#include <fstream>
#include <istream>
#include <memory>

#include "Core/CommandLine.h"
#include "Core/WriteFork.h"
#include "Core/DefaultFInfo.h"

#include "Core/FileUtils.h"

using namespace std;

class StdInputStream : public MacForkInputStream  {
public:
	StdInputStream (istream* input) : 
	  mStream (input)
	  {}
	virtual ~StdInputStream ()
	{ delete mStream; }

	virtual UInt32 getSize ()
	{
		long oldpos = mStream->tellg ();
		mStream->seekg (0,ios::end);
		long endpos = mStream->tellg ();
		mStream->seekg (oldpos);

		return endpos;
	}
	virtual bool read (void *buffer, UInt32 &size)
	{
		mStream->read (reinterpret_cast<char*>(buffer),size);
		if (mStream->eof())
			return false;
		else
			return true;
	}
private:
	istream*		mStream;
};


DFRFFileInput::DFRFFileInput(
					const string&	dfPath, 
					const string&	rfPath)
					: mDFFile (dfPath), mRFFile (rfPath)
{
	std::string dfname;
	std::string rfname;
	dfname = FileUtils::GetFileNameFromPath(dfPath);
	rfname = FileUtils::GetFileNameFromPath(rfPath);
	if (dfname.empty()) {
		mFileName = rfname;
	} else {
		mFileName = dfname;
	} 
}
DFRFFileInput::~DFRFFileInput()
{}
bool DFRFFileInput::getComment ( std::string&	thecomment )
{
	return false;
}
bool DFRFFileInput::getInfo ( FInfo&		finderInfo,
							  FXInfo&		extendedInfo)
{
	DefaultFInfo (finderInfo,extendedInfo);
	return false;
}

MacForkInputStream*	DFRFFileInput::openDF ()
{
	if (mDFFile == "none")
		return NULL;
	return new StdInputStream(new ifstream ( mDFFile.c_str(), ios::in | ios::binary));
}

MacForkInputStream*	DFRFFileInput::openRF ()
{
	if (mRFFile == "none")
		return NULL;
	return new StdInputStream(new ifstream ( mRFFile.c_str(), ios::in | ios::binary));
}

void EncodeDFRF (MacFileInput &theInput, const char* dataFile, const char* resourceFile)
{
	// Read DF size
	std::auto_ptr<MacForkInputStream> df (theInput.openDF());
	//UInt32 dataForkLen = (df.get()  ? df->getSize() : 0);
	// Read RF size
	std::auto_ptr<MacForkInputStream> rf (theInput.openRF());
	//UInt32 resourceForkLen = (rf.get() ? rf->getSize() : 0);
	
	// Write data fork
	{
		ofstream output (dataFile, ios::out | ios::binary);
		StdOStream	theOutput (output);
		writeFork (*df, theOutput);
	}	
	// Write resource fork
	{
		ofstream output (resourceFile, ios::out | ios::binary);
		StdOStream	theOutput(output);
		
		writeFork (*rf, theOutput);
	}

}


// Register to command line:
namespace {

static MacFileInput *create (int argc,const char** argv, int &processed)
{
	if (argc < 3)
		throw CL::FormatException("dfrf input: needs two filename\n");
	
	processed=3;
	return new DFRFFileInput(argv[1],argv[2]);
}
static void process (MacFileInput *input, int argc,const char **argv, int &processed)
{
	processed = 3;
	if (argc < 3)
		throw CL::FormatException ("dfrf output requires two filename\n");
	EncodeDFRF (*input,argv[1],argv[2]);
}
}
void RegisterDFRFFileIO() 
{
	CL::RegisterMacFileInput ("dfrf","-dfrf <datafile> <resfile> : where datafile is the data fork of the file and resfile is the resource fork. Any of them can be none.",create);
	CL::RegisterMacFileOutput ("dfrf","-dfrf <datafile> <resfile>  : files data fork will be encoded to datafile while file resource fork will be encoded to resfile.",process);
}
