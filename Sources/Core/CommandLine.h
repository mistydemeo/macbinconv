#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include "MacFileInput.h"

#include <ostream>

namespace CL {

typedef MacFileInput* (*MacFileInputFromCommandLine) (int argc, const char** argv, int &processedargs );

typedef void (*MacFileOutputFromCommandLine) (MacFileInput *input,int argc, const char**argv, int &processedargs);

void DoExecute(int argc,const char **argv);
void PrintUsage (const char* prgname);
void PrintInputUsage ();
void PrintOutputUsage ();
void RegisterMacFileInput (const std::string &id, const std::string &usage, MacFileInputFromCommandLine creator);
void RegisterMacFileOutput (const std::string &id, const std::string &usage, MacFileOutputFromCommandLine creator);

MacFileInput* ParseInputPart (int argc, const char **argv,  int &processedargs );

void ProcessOutputPart (MacFileInput *input,int argc, const char**argv,int &processedargs);


class BasicException : public std::exception {
public:
		BasicException (const char* message);
		const char*	what ();
private:
		char	mStr[256];
};
class FormatException : public BasicException {
public:
		FormatException (const char* message);
};

};

#endif