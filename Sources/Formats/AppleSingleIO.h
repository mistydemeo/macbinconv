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

class ADFileInput : public MacFileInput	// Services For Macitnosh
{
public:
		ADFileInput (const std::string &pathforrf, const std::string &pathfordf);
virtual	MacForkInputStream*	openDF () ;
virtual	MacForkInputStream*	openRF () ;

virtual ~ADFileInput ();

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

extern void RegisterAppleSingleIO ();
extern void RegisterAppleDoubleIO ();


#endif ASMACFILEINPUT_H