#ifndef MACFILEINPUT_H
#define MACFILEINPUT_H

#include "MacHeaders.h"

#include <stdio.h>
#include <string>


/**
 * @class MacForkInputStream defines an abstract base class
 *		for reading from a MacOS fork. (Resource or Data).
 */
class MacForkInputStream
{
public:
	virtual ~MacForkInputStream () {} ;
	virtual	UInt32	getSize () = 0;
	virtual	bool	read (void *buffer, UInt32& size) = 0;
};


class MacFileInput 
{
public:
	virtual ~MacFileInput () {} ;

virtual MacForkInputStream *openDF () = 0;
/**
 * @returns true if the info is available, false otherwise
 *
 */
virtual MacForkInputStream *openRF () = 0;

/**
 * @param extendedInfo, returned in big endian format
 * @param finderInfo, returned in big endian format
 * @returns true if the info is available, false otherwise
 *
 */
virtual bool getInfo (	FInfo&		finderInfo,
						FXInfo&		extendedInfo ) = 0;
/**
 * @returns the filename encoded in the current codepage/script.
 */
virtual std::string getFileName () = 0; 
/**
 * @param string
 * @returns true if the info is available, false otherwise.
 */
virtual bool getComment ( std::string&	thecomment ) = 0;

};

#endif // _MAC_FILE_INPUT_H