#include "../Stdafx.hpp"
#include "Configuration.hpp"

string Configuration::GetElement(string key)
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

void Configuration::SetElement(string key, string value)
{
	std::map<string, string>::iterator it = mConfigOptions.find(key);
	if (it != mConfigOptions.end())
	{
		it->second = value;
	}
}