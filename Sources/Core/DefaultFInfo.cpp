#include "DefaultFInfo.h"

void DefaultFInfo (FInfo	&theInfo, FXInfo &extendedInfo)
{
	FInfo def = {0}; 
	FXInfo xdef = {0};
	theInfo = def;
	extendedInfo = xdef;

	theInfo.fdCreator = '\?\?\?\?';
	theInfo.fdType = '\?\?\?\?' ;
}