
#ifndef MACOSXIO_H
#define MACOSXIO_H

#include <ostream>
#include <fstream>

using namespace std;

#include "AppleSingleIO.h"

namespace MacOSX {
class Input : public MacFileInput	// Services For Macitnosh
{
public:
		Input (const std::string &path);
virtual	MacForkInputStream*	openDF () ;
virtual	MacForkInputStream*	openRF () ;

virtual ~Input ();

virtual bool getInfo (	FInfo&		finderInfo,
						FXInfo&		extendedInfo ) ;
virtual string getFileName ();
virtual bool getComment ( std::string&	thecomment );
	 
private:
	AppleDouble::Input	mImplementation;
	std::string			mFileName;
};

extern void Write (MacFileInput& theInput, const char* path);


extern void RegisterForCommandLine ();
}

#endif