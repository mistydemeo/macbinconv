#include "InfoOut.h"

#include "Core/MacFileInput.h"
#include "Core/CommandLine.h"

#include <memory>
#include <string.h>


/*
 * Specification of checksum:
 *	1. If checkSumFork(a) != checkSumFork(b) => a != b
 *  2. P ((a != b) && (checkSumFork(a) == checkSumFork(b))) < 2^-20
 *	3. checkSumFork(empty) = 0
 *  4. chechSumFork(a0) != checSumFork(a)
 */
UInt32 checkSumFork (MacForkInputStream &theFork)
{
	UInt32 sum = 0;
	UInt32 p = 16776899 ;

	UInt32 bytesToGet	= theFork.getSize() ;
	while (bytesToGet > 0)
	{
		char buffer[1024];
		UInt32 copyLen = bytesToGet;
		if (copyLen > sizeof (buffer))
			copyLen = sizeof (buffer);
		if (!theFork.read(buffer,copyLen))
			throw CL::BasicException ("Unexpected EOF in fork!\n");
		
		int i;
		for (i = 0; i < copyLen; i++)
		{
			sum = (sum*256+(buffer[i]+1))%p;
		}

		bytesToGet -= copyLen;
	}
	return sum;
}

void printFileInfo (MacFileInput &theInput, bool checkSum = true)
{
	FInfo		finderInfo;
	FXInfo		extendedInfo;
	// Print file name
	printf ("FileName = <%s",theInput.getFileName().c_str());
	printf (">\n");
	// Print file info
	if (!theInput.getInfo(finderInfo,extendedInfo))
		printf ("No Finder Info!\n");
	else
	{
		int i;
		// Print Creator
		OSType theType = finderInfo.fdCreator;
		char *data = (char*)(&theType);
		printf ("Creator = <");
		for (i = 0; i < 4; i++)
			printf ("%c",data[i]);
		printf (">\n");
		// Print Type
		theType = finderInfo.fdType;
		printf ("Type = <");
		for (i = 0; i < 4; i++)
			printf ("%c",data[i]);
		printf (">\n");
	}
	// Print DataFork/ResourceFork size
	{
		UInt32 size = 0;
		// Print DF size
		std::auto_ptr<MacForkInputStream> df (theInput.openDF());

		size = (df.get()  ? df->getSize() : 0);
		printf ("Data fork size = %d\n",size);
		if (checkSum)
		{
			printf ("Data fork chk = %d\n",(df.get() ? checkSumFork(*df) : 0));
		}
		// Print RF size
		std::auto_ptr<MacForkInputStream> rf (theInput.openRF());
		size = (rf.get() ? rf->getSize() : 0);
		printf ("Resource fork size = %d\n",size);
		if (checkSum)
		{
			printf ("Resource fork chk = %d\n",(rf.get() ? checkSumFork(*rf) : 0));
		}
	}
}
static void processInfo (MacFileInput *input, int argc,const char **argv, int &processed)
{
	processed = 1;
	if ((argc >= 2) && (strcmp(argv[1],"chk") == 0))
		processed++;
	printFileInfo (*input,processed == 2);
}
void RegisterInfoOut ()
{
	CL::RegisterMacFileOutput ("info","-info [chk] : prints information to the stdout, if chk is defined the checksum is also printed",processInfo);
}