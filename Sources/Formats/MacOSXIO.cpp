/* MacOS-X uses the Apple double file format internally,
  with the following naming conventions: the appledouble is in file ._file
*/

#include "MacOSXIO.h"
#include "AppleSingleIO.h"
#include "Core/FileUtils.h"

namespace {

std::string GetMacOSXDFPath (const std::string &ipath)
{
	std::string dir, fname;
	FileUtils::SplitFilePath (ipath, &dir, &fname);
	
	return dir+"._"+fname;
}

}

MacOSX::Input::Input (const std::string &path) :
	mImplementation (path, GetMacOSXDFPath (path)),
	mFileName (FileUtils::GetFileNameFromPath (path))
{
}
MacForkInputStream*	MacOSX::Input::openDF ()
{
	return mImplementation.openDF ();
}
MacForkInputStream*	MacOSX::Input::openRF ()
{
	return mImplementation.openRF ();
}
MacOSX::Input::~Input ()
{}

bool MacOSX::Input::getInfo (	FInfo&		finderInfo,
						FXInfo&		extendedInfo ) 
{
	return mImplementation.getInfo (finderInfo, extendedInfo);
}
string MacOSX::Input::getFileName ()
{
	return mFileName;
}
bool MacOSX::Input::getComment ( std::string&	thecomment )
{
	return mImplementation.getComment (thecomment);
}

void  MacOSX::Write (MacFileInput& theInput, const char* path)
{
	std::ofstream	outputStream_DF (path, ios::out | ios::binary);
	std::ofstream	outputStream_RF (GetMacOSXDFPath (path).c_str (), ios::out | ios::binary);
	
	AppleDouble::Write (theInput, outputStream_DF, outputStream_RF);
}

namespace {

static MacFileInput *createAD (int argc,const char** argv, int &processed)
{
	if (argc <= 2)
		throw CL::FormatException("MacOSX input: expected one fileName\n");
	
	processed=2;
	return new MacOSX::Input(argv[1]);
}
static void processAD (MacFileInput *input, int argc,const char **argv, int &processed)
{
	processed = 2;
	if (argc != 2)
		throw CL::FormatException ("AppleDouble output requires one filename\n");
			
	MacOSX::Write (*input, argv[1]);
}

}

void MacOSX::RegisterForCommandLine ()
{
  CL::RegisterMacFileInput ("osx","-osx <filename> : where filename is a MacOSX encoded file for non HFS volumes",createOSX);
  CL::RegisterMacFileOutput ("osx","-osx <filename> : where filename is a MacOSX encoded file for non HFS volumes",procesOSX);	
}