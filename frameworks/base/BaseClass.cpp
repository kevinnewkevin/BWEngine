#include "base/BaseClass.h"

BaseMSGClass::BaseMSGClass()
{
}


BaseMSGClass::~BaseMSGClass()
{
}

bool BaseMSGClass::sendMessage(const char * msg, std::vector<std::string>& params)
{
	return false;
}

void BaseMSGClass::receiveMessage(const char * msg)
{
}
