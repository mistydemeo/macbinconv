#include "ServicesForMacIO.h"

#include "Core/CommandLine.h"
#include "Core/DefaultFInfo.h"
#include "Core/WriteFork.h"


#include <Windows.h>

#include <exception>
#include <memory>

class SFMInputStream : public MacForkInputStream  {
public:
	SFMInputStream (HANDLE hFile) : mFile (hFile)
	{}
	virtual ~SFMInputStream ()
	{
		if (mFile != INVALID_HANDLE_VALUE)
			CloseHandle (mFile);
	}
	virtual UInt32	getSize ()
	{
		UInt32 forkSize = GetFileSize (mFile, NULL);
		if (forkSize == 0xFFFFFFFF)
			throw std::exception ("Cannot get the size of the fork!\n");
		return forkSize;
	}
	virtual bool read (void *buffer, UInt32 &size)
	{
		DWORD bufferSize = size;
		DWORD readSize = 0;
		if (!ReadFile (mFile, buffer, bufferSize, &readSize, NULL) )
			throw std::exception ("Cannot read from the fork!\n");
			
		size = readSize;	
		if (readSize == 0)
			return false;
		else
			return true;
	}
private:
	HANDLE		mFile;
};

SFMFileInput::SFMFileInput (const std::string &path) : mPath (path)
{

}
MacForkInputStream*	SFMFileInput::openDF ()
{
	char szFileStreamName [MAX_PATH*2] ;

	sprintf (szFileStreamName, "%s", mPath.c_str());	

	HANDLE hSrcFile = CreateFile(szFileStreamName, GENERIC_READ, 0, 
			 NULL, OPEN_EXISTING, 0, 0);

	if (hSrcFile == INVALID_HANDLE_VALUE)
		return NULL;
	else
		return new SFMInputStream(hSrcFile);
}
MacForkInputStream*	SFMFileInput::openRF () 
{
	char szFileStreamName [MAX_PATH*2] ;

	sprintf (szFileStreamName, "%s:AFP_Resource", mPath.c_str());	

	HANDLE hSrcFile = CreateFile(szFileStreamName, GENERIC_READ, 0, 
			 NULL, OPEN_EXISTING, 0, 0);

	if (hSrcFile == INVALID_HANDLE_VALUE)
		return NULL;
	else
		return new SFMInputStream(hSrcFile);
}

bool SFMFileInput::getInfo (FInfo&		finderInfo,
							FXInfo&		extendedInfo ) 
{
	DefaultFInfo (finderInfo,extendedInfo);
	char szFileStreamName [2*MAX_PATH] ;

	sprintf (szFileStreamName, "%s:AFP_AfpInfo", mPath.c_str());	

	HANDLE hFile = CreateFile(szFileStreamName, GENERIC_READ, 0, 
				NULL, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	try {
		char buffer[16+sizeof(FInfo)+sizeof(FXInfo)] ;
		DWORD bufferSize = sizeof(buffer);
		DWORD readSize = 0;

		if (!ReadFile (hFile, buffer, bufferSize, &readSize, NULL) )
			throw std::exception ("Cannot read from AFP_AfpInfo!\n");

		if (readSize != sizeof(buffer))
		{
			throw std::exception ("AFP_AfpInfo is too short!\n");
		}

		finderInfo = *(FInfo*)(&buffer[16]);	
		// extendedInfo = *(FXInfo*)(&buffer[16+sizeof(FInfo)]);
	
		CloseHandle (hFile);
		return true;
	}
	catch (...)
	{
		CloseHandle (hFile);
		throw ;
	}
}
bool SFMFileInput::getComment ( std::string&	thecomment )
{
	return false;
}


SFMFileInput::~SFMFileInput ()
{
}


// Serivces for Macintosh encoder!
void EncodeSFM (MacFileInput &theInput, const char* fileName)
{
	// Create file
	char szFileStreamName [MAX_PATH*2] ;

	sprintf (szFileStreamName, "%s", fileName);
		
	HANDLE hData = CreateFile (szFileStreamName, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, 0, NULL);
	
	if (hData == INVALID_HANDLE_VALUE)
		throw std::exception ("Unable to open NTFS data file for writing!");

	try
	{
		// Read DF size
		std::auto_ptr<MacForkInputStream> df (theInput.openDF());
		UInt32 dataForkLen = (df.get()  ? df->getSize() : 0);
	
		// Write data fork
		writeFork (*df, FileHandleOStream(hData));
		
		// Close data fork
		CloseHandle(hData);
	}
	catch (...)
	{
		CloseHandle (hData);
		throw;
	}
	
	
	// Read RF size
	std::auto_ptr<MacForkInputStream> rf (theInput.openRF());
	UInt32 resourceForkLen = (rf.get() ? rf->getSize() : 0);
	
	if (resourceForkLen > 0)
	{
		
		// Open resource fork
		sprintf (szFileStreamName, "%s:AFP_Resource", fileName);
		
		HANDLE hRes = CreateFile (szFileStreamName, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, 0, NULL);
			
		if (hRes == INVALID_HANDLE_VALUE)
			throw std::exception ("Unable to open NTFS resource file for writing!");
		
		try
		{
			// Write resource fork
			writeFork (*rf, FileHandleOStream(hRes));
			
			// Close resource fork
			CloseHandle(hRes);
		}
		catch (...)
		{
			CloseHandle (hRes);
			throw;
		}
	}

	// Write FInfo
	FInfo finderInfo;
	FXInfo extendedInfo;
	if (theInput.getInfo(finderInfo,extendedInfo))
	{
		sprintf (szFileStreamName, "%s:AFP_AfpInfo", fileName);
		
		HANDLE hFinfo = CreateFile (szFileStreamName, GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, 0, NULL);	

		if (hFinfo == INVALID_HANDLE_VALUE)
			throw std::exception ("Cannot create AFP_AfpInfo!");

		try {
			char buffer[60] = {0};
			
			buffer[0] = 'A';
			buffer[1] = 'F';
			buffer[2] = 'P';
			//buffer[3] = 0;
			buffer[6] = 1;
			//buffer[8] = 0x38;
			//buffer[9] = 0xCF;
			//buffer[56] = 0xC8;
			//buffer[57] = 0x38;
			//buffer[58] = 0xA1;
			//buffer[59] = 0xF5;
			*((FInfo*)&buffer[16]) = finderInfo ;

			UInt32	bytes = sizeof (buffer);
			WriteFile (hFinfo, buffer, bytes, &bytes, NULL); 
			if (bytes != sizeof(buffer))
				throw std::exception ("Cannot write AFP_AfpInfo!");
			CloseHandle (hFinfo);
		} catch (...) {
			CloseHandle (hFinfo);
			throw;		
		}
		
	}
}


// Register to command line:
namespace {

static MacFileInput *create (int argc,const char** argv, int &processed)
{
	if (argc == 1)
		throw CL::FormatException("SFM input: expected fileName\n");
	
	processed=2;
	return new SFMFileInput(argv[1]);
}
static void process (MacFileInput *input, int argc,const char **argv, int &processed)
{
	processed = 2;
	if (argc < 2)
		throw CL::FormatException ("sfm output requires one filename\n");
	EncodeSFM (*input,argv[1]);
}

}
void RegisterSFM() 
{
  CL::RegisterMacFileInput ("sfm","-sfm <filename> : where filename is a Services for Macintosh NTFS file",create);
  CL::RegisterMacFileOutput ("sfm","-sfm <filename> : writes to a file used by Serices for Macintosh should be on an NTFS volume",process);
}

