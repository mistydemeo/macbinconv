#include "FileUtils.h"

void FileUtils::SplitFilePath (const std::string &path, std::string *dir, std::string *fname)
{
	std::string::size_type loc;

#if defined(macintosh) 
    #if TARGET_RT_MAC_MACHO
        loc = path.find_last_of("\\/",path.size ());
        if (loc != std::string::npos)
            loc++;
    #else
		loc = path.find_last_of(':',path.size());
		if (loc != std::string::npos)
			loc++;
    #endif
#else
	loc = path.find_last_of("\\/",path.size());
	if (loc != std::string::npos)
		loc++;
#endif	
    if (loc == std::string::npos) {
        if (fname != 0)
            *fname = path;
        #if defined(macintosh) 
            #if TARGET_RT_MAC_MACHO
            if (dir != 0)
                *dir = ".";
            #else
                *dir = ":";
            #endif
        #else
            *dir = ".";
        #endif
    } else {
        if (fname != 0) 
            *fname = path.substr (loc);
        if (dir != 0) 
            *dir = path.substr (0, loc);
    }
}

std::string FileUtils::GetFileNameFromPath (const std::string &path)
{
	std::string::size_type loc;

#if defined(macintosh) 
    #if TARGET_RT_MAC_MACHO
        loc = path.find_last_of("\\/",path.size ());
        if (loc != std::string::npos)
            loc++;
    #else
	loc = path.find_last_of(':',path.size());
	if (loc != std::string::npos)
		loc++;
    #endif
#else
	loc = path.find_last_of("\\/",path.size());
	if (loc != std::string::npos)
		loc++;
#endif	
    if (loc != std::string::npos)
      return (path.substr(loc));
    else
      return path;   	  
}