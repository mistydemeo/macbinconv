// Supported Formats  
#if defined (WINDOWS)
#include "Formats/ServicesForMacIO.h"	// Services For Macintosh i/o
#endif
#if defined (macintosh) || defined (__APPLE__)
#include "Formats/NativeMacIO.h"		// Native Mac I/O with FOpen, etc.
#endif
#include "Formats/AppleSingleIO.h"		// AppleSingle and AppleDouble I/O
#include "Formats/DFRFFileIO.h"			// Data Fork/Resource Fork I/O
#include "Formats/MacBinaryIO.h"		// MacBinary File I/O
#include "Formats/InfoOUT.h"			// FileInfo output
#include "Formats/MacOSXIO.h"			// MacOS-X I/O

#include "Core/CommandLine.h"			// Command line helper class.


int main(int argc, const char* argv[])
{

#if defined(WINDOWS)
	RegisterServicesForMacIO ();
#endif

#if defined(macintosh) || defined (__APPLE__)
	RegisterNativeMacIO ();
#endif

	RegisterAppleSingleIO ();
	RegisterAppleDoubleIO ();
	RegisterMacBinaryIO ();
	RegisterDFRFFileIO ();
	RegisterInfoOut ();
	MacOSX::RegisterForCommandLine ();
	
	CL::DoExecute (argc,argv);

	return 0;
}

