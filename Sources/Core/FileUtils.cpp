#include "FileUtils.h"

std::string FileUtils::GetFileNameFromPath(const std::string &path)
{
	int loc;

#if defined(macintosh) 
    #if TARGET_RT_MACH0
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