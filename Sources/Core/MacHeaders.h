#if defined(macintosh) || defined (__APPLE__)
#if defined(USE_UMBRELLA_HEADERS) && USE_UMBRELLA_HEADERS
    #include <CoreServices/CoreServices.h>
#else
    #include <MacTypes.h>
    #include <Files.h>
    #include <Finder.h>
#endif
#else
#include "MacEmu.h"
#endif

