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
		LOG_ERROR("Invalid Configuration Key!" + key);

#ifdef _DEBUG
		//throw this just to have an unhandled exception during debugging, so I can add config elements easier
		throw 0;
#endif
		return "";
	}
}

void Configuration::SetElement(string key, string value)
{
	std::map<string, string>::iterator it = mConfigOptions.find(key);
	if (it != mConfigOptions.end())
	{
		it->second = value;
	}
	else
	{
		mConfigOptions.insert(std::pair<string, string>(key, value));
	}
}