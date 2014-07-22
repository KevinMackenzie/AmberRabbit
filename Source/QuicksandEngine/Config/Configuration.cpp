#include "../Stdafx.hpp"
#include "Configuration.hpp"

string Configuration::GetConfigElement(string key)
{
	ConfigIterator it = mConfigOptions.find(key);
	if (it != mConfigOptions.end())
	{
		return it->second;
	}
	else
	{
		//throw an exception, this should be cought by the class accessing this key
		throw INVALID_CONFIG_KEY;
	}
}