#ifndef WRITEFORK_H
#define WRITEFORK_H

#include <ostream>

class OStream
{
public:
virtual ~OStream () {} ;
virtual void write (const char* data, UInt32 bytes) = 0;
};

#ifdef WINDOWS
class FileHandleOStream : public OStream
{
public:
		FileHandleOStream (HANDLE file) : mFile(file) {}
virtual ~FileHandleOStream () {};

virtual void write (const char *data, UInt32 bytes)
{
	DWORD dataWritten ;
	
	WriteFile (mFile, data, bytes, &dataWritten, NULL); 
	if (dataWritten != bytes)
		throw std::exception ("Write failed\n");
}

private:
	HANDLE	mFile;
};
#endif

#ifdef macintosh
class MacOSFileRefOStream {
public:
		MacOSFileRefOStream (SInt16 file) : mFile(file) {}
virtual ~MacOSFileRefOStream () {}

virtual void write (const char *data, UInt32 bytes)
{
	OSErr	theErr;
	SInt32 dataWritten = bytes;
	
	theErr = FSWrite (mFile, &dataWritten, data); 
	if ((dataWritten != bytes) || (theErr != noErr))
		throw CL::BasicException ("Write failed\n");
}

private:
	SInt16	mFile;
	
};
#endif

class StdOStream : public OStream
{
public:
	StdOStream (std::ostream &inStream) : mStream(inStream) {}
virtual ~StdOStream () {};

virtual void write (const char *data, UInt32 bytes)
{
	mStream.write (data, bytes);
}
private:
	std::ostream &mStream;
};

inline void writeFork (MacForkInputStream &fork, OStream &output)
{
	UInt32 bytesToGet	= fork.getSize() ;
	while (bytesToGet > 0)
	{
		char buffer[1024];
		UInt32 copyLen = bytesToGet;
		if (copyLen > sizeof (buffer))
			copyLen = sizeof (buffer);
		if (!fork.read(buffer,copyLen))
			throw CL::BasicException ("Unexpected EOF in fork!\n");
		output.write (buffer, copyLen);
		bytesToGet -= copyLen;
	}
}

#endif // WRITEFORK_H