#ifndef ASMACFILEINPUT_H
#define ASMACFILEINPUT_H

#include <ostream>
#include <fstream>

using namespace std;

#include "Core/MacFileInput.h"

class ASFileInput : public MacFileInput	// Services For Macitnosh
{
public:
		ASFileInput (const std::string &path);
virtual	MacForkInputStream*	openDF () ;
virtual	MacForkInputStream*	openRF () ;

virtual ~ASFileInput ();

virtual bool getInfo (	FInfo&		finderInfo,
						FXInfo&		extendedInfo ) ;

virtual bool getComment ( std::string&	thecomment );
	 
private:
	UInt32			mDataSize;	// 0 if not available
	UInt32			mDataOffset;
	
	UInt32			mResSize;	// 0 if not available
	UInt32			mResOffset;	

	bool			mHasComment; 
	std::string		mComment;

	UInt32			mFInfoSize;
	UInt32			mFInfoOffset;

	ifstream		mStream;
};

extern void RegisterAppleSingleIO ();


#endif ASMACFILEINPUT_H