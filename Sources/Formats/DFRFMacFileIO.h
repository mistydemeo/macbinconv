#ifndef DFRFFILEIO_H
#define DFRFFILEIO_H

#include "MacFileInput.h"

#include <string>

using namespace std;

class DFRFFileInput : public MacFileInput	// RF/DF stored files
{
public:
		DFRFFileInput (const std::string &dfpath, const std::string &rfpath);
virtual	MacForkInputStream*	openDF () ;
virtual	MacForkInputStream*	openRF () ;

virtual ~DFRFFileInput ();

virtual bool getInfo (	FInfo&		finderInfo,
						FXInfo&		extendedInfo ) ;

virtual bool getComment ( std::string&	thecomment );
	 
private:
	string	mDFFile;
	string	mRFFile;
};

extern void RegisterDFRF();


#endif // DFRFFileIO.h