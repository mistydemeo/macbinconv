#ifndef NATIVEMACFILEIO_H
#define NATIVEMACFILEIO_H

#include <ostream>
#include <fstream>

using namespace std;

#include "Core/MacFileInput.h"

class NativeMacFileInput : public MacFileInput	// Native macos
{
public:
		NativeMacFileInput (const std::string &path);
virtual	MacForkInputStream*	openDF () ;
virtual	MacForkInputStream*	openRF () ;

virtual ~NativeMacFileInput ();

virtual bool getInfo (	FInfo&		finderInfo,
						FXInfo&		extendedInfo ) ;

virtual bool getComment ( std::string&	thecomment );
	 
private:
	FSSpec 		mSpec;
	SInt16		mResFRefNum;
	SInt16		mDataFRefNum;
};

extern void RegisterNativeMacIO ();


#endif ASMACFILEINPUT_H