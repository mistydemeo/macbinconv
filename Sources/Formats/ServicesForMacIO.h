#ifndef SERVICESFORMACIO_H
#define SERVICESFORMACIO_H

#include "Core/MacFileInput.h"

#include <string>

class SFMFileInput : public MacFileInput	// Services For Macitnosh
{
public:
		SFMFileInput (const std::string &path);
virtual	MacForkInputStream*	openDF () ;
virtual	MacForkInputStream*	openRF () ;

virtual ~SFMFileInput ();

virtual bool getInfo (	FInfo&		finderInfo,
						FXInfo&		extendedInfo ) ;

virtual bool getComment ( std::string&	thecomment );
	 
private:
	std::string mPath;
};

extern void RegisterServicesForMacIO ();

#endif // SERVICESFORMACIO_H