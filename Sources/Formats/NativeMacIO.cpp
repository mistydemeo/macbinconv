#include "Formats/NativeMacIO.h"

#include "Core/CommandLine.h"

#include "DefaultFInfo.h"

#include <Files.h>
#include <Aliases.h>

NativeMacFileInput::NativeMacFileInput (const std::string &path)
{
	AliasHandle	alias;
	Boolean		wasChanged;
	Str255		nullString;
	OSErr		result;
	
	nullString[0] = 0;	/* null string to indicate no zone or server name */
	result = NewAliasMinimalFromFullPath(path.length(), path.c_str(), nullString, nullString, &alias);

	if (result != noErr)
		throw CL::BasicException ("Unable to resolve path!\n");
		
	result = ResolveAlias(NULL, alias, &mSpec, &wasChanged);
	DisposeHandle ((Handle)alias);
}
NativeMacFileInput::~NativeMacFileInput ()
{
}

class NativeMacForkInputStream : public MacForkInputStream
{
public:
	NativeMacForkInputStream (const short	iRefNum) : mRefNum (iRefNum) 
	{ }
	UInt32	getSize ()
	{
		long	theEOF;
		if (GetEOF (mRefNum, &theEOF) != noErr)
			throw CL::BasicException ("Cannot get the eof mark!\n");
		return theEOF;
	}
	bool	read (void *buffer, UInt32& size) 
	{
		OSErr	result;
		long	theSize = size;
		result = FSRead (mRefNum, &theSize, buffer);
		size = theSize;
		return (result == noErr);
	}
	virtual ~NativeMacForkInputStream () { FSClose (mRefNum); } ;
private:
	short	mRefNum;
};

MacForkInputStream *NativeMacFileInput::openDF ()
{
	OSErr theErr;
	short theRefNum;
	theErr = FSpOpenDF (&mSpec,fsRdPerm,&theRefNum);
	if (theErr != noErr)
		throw CL::BasicException ("Cannot get open data fork!\n");
	return new NativeMacForkInputStream (theRefNum);
}
MacForkInputStream *NativeMacFileInput::openRF ()
{
	OSErr theErr;
	short theRefNum;
	theErr = FSpOpenRF (&mSpec,fsRdPerm,&theRefNum);
	if (theErr != noErr)
		throw CL::BasicException ("Cannot get resource fork!\n");
	return new NativeMacForkInputStream (theRefNum);
}
bool NativeMacFileInput::getInfo (	FInfo&		finderInfo,
								FXInfo&		extendedInfo)
{
	DefaultFInfo (finderInfo,extendedInfo);
	return true;
}
bool  NativeMacFileInput::getComment ( std::string&	thecomment )
{
	return false;
}
// Register to command line:
namespace {
static MacFileInput *create (int argc,const char** argv, int &processed)
{
	if (argc == 1)
		throw CL::FormatException("MacBinary input: expected filename\n");	

	processed=2;
	return new NativeMacFileInput(argv[1]);	
}

}
void RegisterNativeMacIO ()
{
  CL::RegisterMacFileInput ("mac","-mac <filename> : where filename is a MacOS native file!",create);

}