#ifndef NATIVEMACFILEIO_H
#define NATIVEMACFILEIO_H

#include <ostream>
#include <fstream>

using namespace std;

#include "Core/MacFileInput.h"

#define USE_HFS_PLUS	0

class NativeMacFileInput : public MacFileInput	// Native macos
{
public:
		NativeMacFileInput (const std::string &path);
virtual	MacForkInputStream*	openDF () ;
virtual	MacForkInputStream*	openRF () ;

virtual ~NativeMacFileInput ();

virtual std::string	getFileName() { return mFileName; };

virtual bool getInfo (	FInfo&		finderInfo,
						FXInfo&		extendedInfo ) ;

virtual bool getComment ( std::string&	thecomment );
	 
private:
#if USE_HFS_PLUS
	FSRef		mFileRef;
#else
	FSSpec 		mSpec;
#endif
	std::string	mFileName;
	SInt16		mResFRefNum;
	SInt16		mDataFRefNum;
};

extern void RegisterNativeMacIO ();


#endif ASMACFILEINPUT_H