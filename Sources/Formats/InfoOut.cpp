#include "InfoOut.h"

#include "Core/MacFileInput.h"
#include "Core/CommandLine.h"

#include <memory>
#include <string.h>

#include "md5.h"


/*
 * Specification of checksum:
 *	Checksum returns an MD-5 checksum of the given fork.
 */
void checkSumFork (MacForkInputStream &theFork, md5_state_t& state)
{
    UInt32 bytesToGet	= theFork.getSize() ;
    while (bytesToGet > 0)
    {
            char buffer[1024];
            UInt32 copyLen = bytesToGet;
            if (copyLen > sizeof (buffer))
                    copyLen = sizeof (buffer);
            if (!theFork.read(buffer,copyLen))
                    throw CL::BasicException ("Unexpected EOF in fork!\n");
            
            md5_append (&state, reinterpret_cast<md5_byte_t*>(buffer), copyLen);
            bytesToGet -= copyLen;
    }
}

void printFileInfo (MacFileInput &theInput, bool checkSum = true)
{
	FInfo		finderInfo;
	FXInfo		extendedInfo;
        printf ("<info>\n");
	// Print file name
        printf ("\t<filename>%s</filename>\n",theInput.getFileName().c_str());
	// Print file info
	if (!theInput.getInfo(finderInfo,extendedInfo))
		printf ("\t<finderinfo></finderinfo>\n");
	else
	{
                printf ("\t<finderinfo>\n");
		int i;
		// Print Creator
		OSType theType = finderInfo.fdCreator;
		char *data = (char*)(&theType);
		printf ("\t\t<creator>");
		for (i = 0; i < 4; i++)
			printf ("%c",data[i]);
		printf ("</creator>\n");
		// Print Type
		theType = finderInfo.fdType;
		printf ("\t\t<type>");
		for (i = 0; i < 4; i++)
			printf ("%c",data[i]);
		printf ("</type>\n");
                printf ("\t</finderinfo>\n");
	}
	// Print DataFork/ResourceFork size
	{
		UInt32 size = 0;
		// Print DF size
                printf ("\t<datafork>\n");
		std::auto_ptr<MacForkInputStream> df (theInput.openDF());

		size = (df.get()  ? df->getSize() : 0);
		printf ("\t\t<size>%ul</size>\n",size);
		if (checkSum && df.get ())
		{
                    md5_byte_t digest[16];
                    
                    md5_state_t state = {0};
                    md5_init (&state);
                    checkSumFork(*df, state);
                    md5_finish (&state, digest);
                    printf ("\t\t<md5>");
                    for (int i = 0; i < 16; i++) {
                        printf ("%x",digest[i]);
                    }
                    printf ("</md5>\n");
		}
                printf ("\t</datafork>\n");
		// Print RF size
                printf ("\t<resourcefork>\n");
		std::auto_ptr<MacForkInputStream> rf (theInput.openRF());
		size = (rf.get() ? rf->getSize() : 0);
		printf ("\t\t<size>%d</size>\n",size);
		if (checkSum && rf.get ())
		{
                    md5_byte_t digest[16];
                    
                    md5_state_t state = {0};
                    md5_init (&state);
                    checkSumFork(*rf, state);
                    md5_finish (&state, digest);
                    printf ("\t\t<md5>");
                    for (int i = 0; i < 16; i++) {
                        printf ("%x",digest[i]);
                    }
                    printf ("</md5>\n");
		}
                printf ("\t</resourcefork>\n");
	}
        printf ("</info>\n");
}
static void processInfo (MacFileInput *input, int argc,const char **argv, int &processed)
{
	processed = 1;
	if ((argc >= 2) && (strcmp(argv[1],"md5") == 0))
		processed++;
	printFileInfo (*input,processed == 2);
}
void RegisterInfoOut ()
{
	CL::RegisterMacFileOutput ("info","-info [md5] : prints information to the stdout, if md5 is defined prints md5 fingerprint.",processInfo);
}