#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>

namespace FileUtils {

extern std::string GetFileNameFromPath(const std::string &path);
extern void		   SplitFilePath (const std::string &path, std::string *dir, std::string *fname);

}

#endif	// FILEUTILS_H