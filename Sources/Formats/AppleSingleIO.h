#ifndef ASMACFILEINPUT_H
#define ASMACFILEINPUT_H

#include <ostream>
#include <fstream>

using namespace std;

#include "Core/MacFileInput.h"

namespace AppleSingle {
class Input : public MacFileInput	// AppleSingle file input
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
	UInt32			mDataSize;	// 0 if not available
	UInt32			mDataOffset;
	
	UInt32			mResSize;	// 0 if not available
	UInt32			mResOffset;	

	bool			mHasComment; 
	std::string		mComment;
	std::string		mFileName;

	UInt32			mFInfoSize;
	UInt32			mFInfoOffset;

	ifstream		mStream;
};

extern void Write (MacFileInput& theInput, std::ostream &output);
}

namespace AppleDouble {
class Input : public MacFileInput	// AppleDouble file input
{
public:
		Input (const std::string &pathforrf, const std::string &pathfordf);

virtual	MacForkInputStream*	openDF () ;
virtual	MacForkInputStream*	openRF () ;

virtual ~Input ();

virtual bool getInfo (	FInfo&		finderInfo,
						FXInfo&		extendedInfo ) ;
virtual string getFileName ();
virtual bool getComment ( std::string&	thecomment );
	 
private:
	UInt32			mDataSize;	// 0 if not available
	UInt32			mDataOffset;
	
	UInt32			mResSize;	// 0 if not available
	UInt32			mResOffset;	

	bool			mHasComment; 
	std::string		mComment;
	std::string		mFileName;

	UInt32			mFInfoSize;
	UInt32			mFInfoOffset;

	ifstream		mDFStream;
	ifstream		mRFStream;
};

extern void Write (MacFileInput& theInput, std::ostream &doutput, std::ostream &routput);
}



extern void RegisterAppleSingleIO ();
extern void RegisterAppleDoubleIO ();


#endif ASMACFILEINPUT_H