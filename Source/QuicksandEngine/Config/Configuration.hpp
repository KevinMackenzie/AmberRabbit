#ifndef AW_CONFIGURATION_HPP
#define AW_CONFIGURATION_HPP

#include <map>
#include <string>

//this class contains all of the configuration options.  It stores a map that is 
//just a wrapper around an xml config file

typedef  std::map<string, string>::iterator ConfigIterator;

class Configuration
{
	std::map<string, string> mConfigOptions;
	friend class StringsManager;

	Configuration(){};

public:

	//some useful helper methods for geting configuration options
	//ALL of them will throw an exception in the case of an invalid config key

	string GetConfigElement(string key);
	
};

#define INVALID_CONFIG_KEY "INVALID KEY"


#endif