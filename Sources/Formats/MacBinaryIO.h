#ifndef MACBINARYIO_H
#define MACBINARYIO_H

#include <string>
#include <fstream>

#include "Core/MacFileInput.h"

using namespace std;



class MacBinFileInput : public MacFileInput	// Services For Macitnosh
{
public:
		MacBinFileInput (const std::string &path);
virtual	MacForkInputStream*	openDF () ;
virtual	MacForkInputStream*	openRF () ;

virtual ~MacBinFileInput ();

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

	bool			mHasFInfo;
	FInfo			mFInfo;
	bool			mHasFXInfo;
	FXInfo			mFXInfo;

	ifstream		mStream;
};

extern void RegisterMacBinaryIO ();

#endif // MACBINARYIO_H