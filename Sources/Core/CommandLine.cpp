#include "CommandLine.h"


#include <vector>
#include <exception>
#include <string.h>

struct MacFileInputEntry {
	CL::MacFileInputFromCommandLine				create ;
	std::string									id;
	std::string									usage;
};

struct MacFileOutputEntry {
	CL::MacFileOutputFromCommandLine			process ;
	std::string									id;
	std::string									usage;
};

typedef std::vector<MacFileInputEntry>	InputEntryVector;
typedef std::vector<MacFileOutputEntry>	OutputEntryVector;
static 	InputEntryVector	inputs;
static 	OutputEntryVector	outputs;

void CL::RegisterMacFileInput (const std::string &id, const std::string &usage, MacFileInputFromCommandLine creator)
{
	MacFileInputEntry	entry;
	entry.create = creator;
	entry.id = id;
	entry.usage = usage;

	inputs.push_back (entry);
}
CL::BasicException::BasicException (const char* message)
	:	std::exception ()
{
	strncpy (mStr, message, sizeof (mStr));
	mStr[sizeof(mStr)-1] = 0;
}
const char* CL::BasicException::what ()
{
	return mStr;
}

CL::FormatException::FormatException (const char* message)
	:	CL::BasicException (message)
{
}

MacFileInput* CL::ParseInputPart (int argc, const char **argv,  int &processedargs )
{
	if ((argv[0][0] != '-') || (argc == 0))
		throw FormatException("Every input should begin with -<formatid>!");

	InputEntryVector::iterator i = inputs.begin();
	while (i != inputs.end ())
	{
		if (i->id == &argv[0][1])
			return i->create (argc,argv,processedargs);
		i++;
	}

	throw FormatException ((std::string("No input-id named:")+&argv[0][1]+"!").c_str());
}
void CL::RegisterMacFileOutput (const std::string &id, const std::string &usage, MacFileOutputFromCommandLine process)
{
	MacFileOutputEntry	entry;
	entry.process = process;
	entry.id = id;
	entry.usage = usage;

	outputs.push_back (entry);
}
void CL::ProcessOutputPart (MacFileInput *input,int argc, const char**argv,int &processedargs)
{
	if ((argv[0][0] != '-') || (argc == 0))
		throw FormatException("Every output should begin with -<formatid>!");

	OutputEntryVector::iterator i = outputs.begin();
	while (i != outputs.end ())
	{
		if (i->id == &argv[0][1])
		{
			i->process (input,argc,argv,processedargs);
			return;
		}
		i++;
	}

	throw FormatException ((std::string("No output-id named:")+&argv[0][1]+"!").c_str());
}
void CL::PrintInputUsage ()
{
	InputEntryVector::iterator i = inputs.begin();
	
	while (i != inputs.end ())
	{
		printf ("  %s\n",i->usage.c_str());
		i++;
	}
}
void CL::PrintOutputUsage ()
{
	OutputEntryVector::iterator i = outputs.begin();
	
	while (i != outputs.end ())
	{
		printf ("  %s\n",i->usage.c_str());
		i++;
	}
}
void CL::DoExecute(int argc,const char **argv)
{
	const char* progamname = argv[0];

	if (argc <= 1)
	{
		PrintUsage(progamname);
		return;
	}
	try
	{
		int parsed;
		argc--;
		argv++;
		std::auto_ptr<MacFileInput> input (ParseInputPart (argc,argv,parsed));
		argc-=parsed;
		argv+=parsed;
		ProcessOutputPart (input.get(),argc,argv,parsed);
		argc-=parsed;
		argv+=parsed;
		if (argc != 0)
		{
			printf ("Warning: unparsed options from: %s\n",argv[0]);
		}
	}
	catch (CL::FormatException &e) {
		printf ("Error: bad argument: %s\n",e.what());
		PrintUsage (progamname);
	}
	catch (std::exception &e) {
		printf ("Error: %s!\n",e.what());
	}
	catch (...) {
		printf ("Error: unknown!\n");
	}
}
void CL::PrintUsage (const char* prgname)
{
	printf ("usage: %s <input> <output>\n",prgname);
	printf (" where <input> is one of:\n");
	PrintInputUsage ();
	printf (" and <output> is one of:\n");
	PrintOutputUsage ();
}