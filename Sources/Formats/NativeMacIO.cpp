#include "Formats/NativeMacIO.h"

#include "Core/CommandLine.h"
#include "Core/FileUtils.h"
#include "Core/WriteFork.h"
#include "Core/DefaultFInfo.h"

#include <sstream>

#if defined(USE_UMBRELLA_HEADERS) && USE_UMBRELLA_HEADERS
    #include <CoreServices/CoreServices.h>
#else
    #include <Files.h>
    #include <Aliases.h>
#endif

#if TARGET_API_MAC_CARBON
template <class T> class AutoCFType {
public:
        AutoCFType (const T& p) : mP (p) {}
        AutoCFType () : mP (NULL) {}
        
        AutoCFType(AutoCFType<T> &r) : mP (r.relase ()) {}
        
        ~AutoCFType () { if (mP != NULL) CFRelease (mP); }
    
    
    AutoCFType<T>& operator= (AutoCFType<T>& r) { reset( r.release ()); return *this; }
    T&		operator= (const T& p) { reset (p); return mP;  }
    operator	T& () { return mP; }
    T 		get () { return mP; }
    T		release () { T result = mP; mP = NULL; return result; }
    void 	reset (const T& p) { if ((p != mP) && (mP != NULL)) CFRelease (mP); mP = const_cast<T>(p);   }

private:
    T	mP;
};
#endif
NativeMacFileInput::NativeMacFileInput (const std::string &path)
{
	mFileName = FileUtils::GetFileNameFromPath(path);

#if TARGET_API_MAC_CARBON && USE_HFS_PLUS
        AutoCFType<CFStringRef>	pathstr (NULL);
        AutoCFType<CFURLRef>	url (NULL);
#if TARGET_RT_MAC_MACHO
        CFURLPathStyle	pathstyle = kCFURLPOSIXPathStyle;
#else
        CFURLPathStyle 	pathstyle = kCFURLHFSPathStyle;
#endif
        pathstr = CFStringCreateWithCString (
                    kCFAllocatorDefault,
                    path.c_str (),
                    CFStringGetSystemEncoding ());
        if (pathstr.get () == NULL)
            throw std::bad_alloc ();
            
        url = CFURLCreateWithFileSystemPath (
                    kCFAllocatorDefault,
                    pathstr.get (),
                    pathstyle,
                    false
                );
        if (url.get () == NULL)
            throw std::bad_alloc ();
        
        if (!CFURLGetFSRef (url.get (), &mFileRef)) {
            std::string error = std::string("No such file:\"") + path + std::string("\"");
            throw CL::BasicException (error.c_str ());
        }
        
#else    
	AliasHandle	alias;
	Boolean		wasChanged;
	Str255		nullString;
	OSErr		result;
        
        nullString[0] = 0;	/* null string to indicate no zone or server name */
	result = NewAliasMinimalFromFullPath(path.length(), path.c_str(), nullString, nullString, &alias);

	if (result != noErr) 
		throw CL::BasicException ("Unable to resolve path!\n");
#if USE_HFS_PLUS	
	result = FSResolveAlias(NULL, alias, &mFileRef, &wasChanged);	
#else
	result = ResolveAlias(NULL, alias, &mSpec, &wasChanged);
#endif
	DisposeHandle ((Handle)alias);

	if (result != noErr) 
		throw CL::BasicException ("Unable to resolve path!\n");	
#endif	
}
NativeMacFileInput::~NativeMacFileInput ()
{
}

class NativeMacForkInputStream : public MacForkInputStream
{
public:
	NativeMacForkInputStream (const short	iRefNum) : mRefNum (iRefNum) 
	{ }
	UInt32	getSize ()
	{
		long	theEOF;
		if (GetEOF (mRefNum, &theEOF) != noErr)
			throw CL::BasicException ("Cannot get the eof mark!\n");
		return theEOF;
	}
	bool	read (void *buffer, UInt32& size) 
	{
		OSErr	result;
		long	theSize = size;
		result = FSRead (mRefNum, &theSize, buffer);
		size = theSize;
		return (result == noErr);
	}
	virtual ~NativeMacForkInputStream () { FSClose (mRefNum); } ;
private:
	short	mRefNum;
};

MacForkInputStream *NativeMacFileInput::openDF ()
{
	OSErr theErr;
	short theRefNum;
#if USE_HFS_PLUS
	HFSUniStr255 forkName;
	theErr = FSGetDataForkName(&forkName);
	if (theErr != noErr)
		throw CL::BasicException ("Cannot get datafork name!\n");
		
	theErr = FSOpenFork (&mFileRef, forkName.length, forkName.unicode, fsRdPerm, &theRefNum);
#else
	theErr = FSpOpenDF (&mSpec,fsRdPerm,&theRefNum);
#endif
	if (theErr != noErr)
		throw CL::BasicException ("Cannot get open data fork!\n");
	return new NativeMacForkInputStream (theRefNum);
}
MacForkInputStream *NativeMacFileInput::openRF ()
{
	OSErr theErr;
	short theRefNum;
#if USE_HFS_PLUS
	HFSUniStr255 forkName;
	theErr = FSGetResourceForkName(&forkName);
	if (theErr != noErr)
		throw CL::BasicException ("Cannot get datafork name!\n");
		
	theErr = FSOpenFork (&mFileRef, forkName.length, forkName.unicode, fsRdPerm, &theRefNum);
#else
	theErr = FSpOpenRF (&mSpec,fsRdPerm,&theRefNum);
#endif
	if (theErr != noErr)
		throw CL::BasicException ("Cannot get resource fork!\n");
	return new NativeMacForkInputStream (theRefNum);
}
bool NativeMacFileInput::getInfo (	FInfo&		finderInfo,
					FXInfo&		extendedInfo)
{
    DefaultFInfo (finderInfo,extendedInfo);
    
    #if USE_HFS_PLUS
        OSErr 		status;
        FSCatalogInfo	info;
    
        status = FSGetCatalogInfo (&mFileRef, kFSCatInfoFinderInfo | kFSCatInfoFinderXInfo, &info, NULL, NULL, NULL);
        if (status == noErr) {
            finderInfo = *(reinterpret_cast<FInfo*>(info.finderInfo));
            extendedInfo = *(reinterpret_cast<FXInfo*>(info.extFinderInfo));
        }
    #endif
    return true;
}
bool  NativeMacFileInput::getComment ( std::string&	thecomment )
{
	return false;
}

/*
    OSErr FSCreateFileUnicode (
    const FSRef *parentRef, 
    UniCharCount nameLength, 
    const UniChar *name, 
    FSCatalogInfoBitmap whichInfo, 
    const FSCatalogInfo *catalogInfo, 
    FSRef newRef, 
    FSSpec newSpec
);
*/
#if TARGET_API_MAC_CARBON
class UniCharBuffer {
public:
                UniCharBuffer (const CFStringRef	&inString) : mData (NULL) {
                    mLength = CFStringGetLength (inString); 
                    mData = reinterpret_cast<UniChar*>(malloc (mLength*sizeof (UniChar*)));
                    CFStringGetCharacters (inString, CFRangeMake (0, mLength), mData);
                }
                ~UniCharBuffer () {
                    if (mData != NULL) {
                        free (mData);
                    }
                }
    operator 	UniChar* () { return mData; }	
    int		length () { return mLength; }
private:
    int 	mLength;
    UniChar*	mData;
};
#endif

static void EncodeNativeMacFile (MacFileInput *input, const std::string& path) 
{
#if TARGET_API_MAC_CARBON && USE_HFS_PLUS
#if TARGET_RT_MAC_MACHO
    CFURLPathStyle	pathstyle = kCFURLPOSIXPathStyle;
#else
    CFURLPathStyle 	pathstyle = kCFURLHFSPathStyle;
#endif
    
    std::string	parent;
    std::string fname;
    FileUtils::SplitFilePath (path, &parent, &fname);
                
    AutoCFType<CFStringRef>	pathstr (NULL);
    AutoCFType<CFURLRef>	url (NULL);
    
    pathstr = CFStringCreateWithCString (
                kCFAllocatorDefault,
                parent.c_str (),
                CFStringGetSystemEncoding ());
    if (pathstr.get () == NULL)
        throw std::bad_alloc ();
            
    url = CFURLCreateWithFileSystemPath (
                kCFAllocatorDefault,
                pathstr.get (),
                pathstyle,
                false
            );
    if (url.get () == NULL)
        throw std::bad_alloc ();
        
    FSRef	parentRef = {0};
    if (!CFURLGetFSRef (url.get (), &parentRef)) 
        throw CL::BasicException ("No parent directory exists!\n");
    
    AutoCFType<CFStringRef>	filename (NULL);
   
    filename = CFStringCreateWithCString (kCFAllocatorDefault, fname.c_str(),  CFStringGetSystemEncoding());
    if (filename.get () == NULL)
         throw std::bad_alloc ();

    UniCharBuffer		name (filename);
    FSCatalogInfo		catalogInfo = {0} ;
 
    input->getInfo (*reinterpret_cast<FInfo*>(catalogInfo.finderInfo), *reinterpret_cast<FXInfo*>(catalogInfo.extFinderInfo));
    
    FSRef theFile = {0};
    OSErr status = FSCreateFileUnicode (&parentRef, name.length (), name, kFSCatInfoFinderInfo | kFSCatInfoFinderXInfo, &catalogInfo, &theFile, NULL);
    if (status != noErr) {
        std::stringstream err;
        err << std::string("Unable to create file:\"");
        err << path;
        err << std::string("\" (");
        err << status;
        err << std::string(")");
        
        throw CL::BasicException (err.str ().c_str ());
    }
    
    // Read DF size
    std::auto_ptr<MacForkInputStream> df (input->openDF());
    int dataForkLen = (df.get()  ? df->getSize() : 0);
    
    // Read RF size
    std::auto_ptr<MacForkInputStream> rf (input->openRF());
    int resourceForkLen = (rf.get() ? rf->getSize() : 0);

    // HFS+ fork name
    HFSUniStr255 forkName;
        
    if (resourceForkLen > 0) {
        // Write Resource Fork:
        OSErr theErr = FSGetResourceForkName(&forkName);
        if (theErr != noErr)
                throw CL::BasicException ("Cannot get datafork name!\n");
                    
        SInt16 refNum = 0;
        status = FSOpenFork (&theFile, forkName.length, forkName.unicode, fsWrPerm, &refNum);
        if (status != noErr)
            throw CL::BasicException ("Unable to open output file for writing\n");
            
        try {
            MacOSFileRefOStream ostream (refNum);
            writeFork (*rf, ostream);
            
            FSClose ( refNum);
        } 
        catch (...) {
            FSClose ( refNum);
        }
    }
    
    if (dataForkLen > 0) {
        // Write Resource Fork:
        OSErr theErr = FSGetDataForkName(&forkName);
        if (theErr != noErr)
                throw CL::BasicException ("Cannot get datafork name!\n");
                    
        SInt16 refNum = 0;
        status = FSOpenFork (&theFile, forkName.length, forkName.unicode, fsWrPerm, &refNum);
        if (status != noErr)
            throw CL::BasicException ("Unable to open output file for writing\n");
            
        try {
            MacOSFileRefOStream ostream (refNum);
            writeFork (*df, ostream);
            
            FSClose ( refNum);
        } 
        catch (...) {
            FSClose ( refNum);
        }
        
    }
#else
    
#endif
}

// Register to command line:
namespace {
static MacFileInput *create (int argc,const char** argv, int &processed)
{
    processed=2;
    if (argc == 1)
            throw CL::FormatException("MacBinary input: expected filename\n");	

    return new NativeMacFileInput(argv[1]);	
}

static void process (MacFileInput *input, int argc,const char **argv, int &processed)
{
    processed = 2;
    if (argc < 2)
            throw CL::FormatException ("-mac output requires only one filename\n");
    std::string path(argv[1]);
    EncodeNativeMacFile (input, path);
}
}
void RegisterNativeMacIO ()
{
  CL::RegisterMacFileInput ("mac","-mac <filename> : where filename is a MacOS native file.",create);
  CL::RegisterMacFileOutput ("mac","-mac <filename> : where filename is a MacOS native file.",process);
}