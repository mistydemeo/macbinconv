#include "FileUtils.h"

void FileUtils::SplitFilePath (const std::string &path, std::string *dir, std::string *fname)
{
	std::string::size_type loc;

#if defined(macintosh) 
    #if TARGET_RT_MAC_MACHO
        loc = path.find_last_of("\\/",path.size ());
        if (loc != 0)
            loc++;
    #else
	loc = path.find_last_of(':',path.size());
	if (loc != 0)
		loc++;
    #endif
#else
	loc = path.find_last_of("\\/",path.size());
	if (loc != 0)
		loc++;
#endif	
	if (fname != 0) 
		*fname = path.substr (loc);
	if (dir != 0) 
		*dir = path.substr (0, loc-1);
}

std::string FileUtils::GetFileNameFromPath (const std::string &path)
{
	std::string::size_type loc;

#if defined(macintosh) 
    #if TARGET_RT_MAC_MACHO
        loc = path.find_last_of("\\/",path.size ());
        if (loc != 0)
            loc++;
    #else
	loc = path.find_last_of(':',path.size());
	if (loc != 0)
		loc++;
    #endif
#else
	loc = path.find_last_of("\\/",path.size());
	if (loc != 0)
		loc++;
#endif	
	return (path.substr(loc));
}