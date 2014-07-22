#include "../Stdafx.hpp"
#include "StringsManager.hpp"
#include "Configuration.hpp"
#include <fstream>
#include <sstream>

//the definition of the static members

bool StringsManager::AddConfigElements(string pathToXml)
{
	
	//make sure the file exists
	std::ifstream file(pathToXml.c_str());
	if (!file)
	{
		return false;
	}

	file.close();

	tinyxml2::XMLDocument doc;
	doc.LoadFile(pathToXml.c_str());
	//TODO: make username/password a config element but secure it in a good way

	tinyxml2::XMLElement *root = doc.FirstChildElement("ROOT");//this is manditory
	
	//now loop through all of the iterators
	//this is a map, so to MYSQL->PORT will be MYSQL_PORT and so forth

	//now recursively load the elements
	LoadConfigElements(root->FirstChildElement());

	return true;
}

void StringsManager::LoadConfigElements(tinyxml2::XMLElement *root)
{
	
	//since we are using a map, make a prefix ie MYSQL_ and this is append with each new node
	//as long as it is not the root element
	if (root->GetText() != "ROOT")
	{
		mCurrPrefix += root->GetText();
		mCurrPrefix += "_";
	}

	for (tinyxml2::XMLElement* child = root->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
	{
		//is there a child element?
		if (child->FirstChildElement() != nullptr)
		{
			//recurse
			LoadConfigElements(child);
		}

		stringstream childName;
		//first the prefix, then the actual name
		childName << mCurrPrefix;
		childName << child->Name();

		//add each one to the config options AS LONG AS THE KEY DOESN"T ALREADY EXIST
		if (gConfig.mConfigOptions.find(child->Name()) != gConfig.mConfigOptions.end())
		{
			//log this as an error, becuase this will be important to fix to ensure there are no conflicting configurations.
			stringstream ss;
			ss << "Redundent Configuration Key, not adding key \'" << childName.str() << "\' and value \'" << child->GetText() << "\'";
			LOG_ERROR(ss.str());
		}

		

		//finally insert the element
		gConfig.mConfigOptions.insert(std::pair<string, string>(childName.str(), child->GetText()));
	}
}

void StringsManager::FlushConfig()
{
	
	gConfig.mConfigOptions.clear();
}

string StringsManager::GetConfigElementStr(string key)
{
	string ret = "";
	try
	{
		ret = gConfig.GetConfigElement(key);
	}
	catch (string exception)
	{
		if (exception == INVALID_CONFIG_KEY)
		{
			stringstream ss;
			ss << "Invalid configuration key, no value for key \'" << key << "\'";
			LOG_ERROR(ss.str());
		}
		else
		{
			stringstream ss;
			ss << "Unknown Error Occured while catching exception processing configuration key: \'" << key << "\'";
			LOG_ERROR(ss.str());
		}
	}

	return ret;
}




///////////////////////////////////////////////
//
//	Configuration Conversion Methods Below
//
//
//

char StringsManager::GetConfigElementc(string key)
{
	return static_cast<char>(std::stoi(GetConfigElementStr(key)));
}

unsigned char StringsManager::GetConfigElementuc(string key)
{
	return static_cast<unsigned char>(std::stoi(GetConfigElementStr(key)));
}

short StringsManager::GetConfigElements(string key)
{
	return static_cast<short>(std::stoi(GetConfigElementStr(key)));
}

unsigned short StringsManager::GetConfigElementus(string key)
{
	return static_cast<unsigned short>(std::stoi(GetConfigElementStr(key)));
}

int StringsManager::GetConfigElementi(string key)
{
	return std::stoi(GetConfigElementStr(key));
}

unsigned int StringsManager::GetConfigElementui(string key)
{
	return static_cast<unsigned int>(std::stoi(GetConfigElementStr(key)));
}

long StringsManager::GetConfigElementl(string key)
{
	return std::stol(GetConfigElementStr(key));
}

unsigned long StringsManager::GetConfigElementul(string key)
{
	return std::stoul(GetConfigElementStr(key));
}

long long StringsManager::GetConfigElementll(string key)
{
	return std::stoll(GetConfigElementStr(key));
}

unsigned long long StringsManager::GetConfigElementull(string key)
{
	return std::stoull(GetConfigElementStr(key));
}

float StringsManager::GetConfigElementf(string key)
{
	return std::stof(GetConfigElementStr(key));
}

double StringsManager::GetConfigElementd(string key)
{
	return std::stod(GetConfigElementStr(key));
}

long double StringsManager::GetConfigElementld(string key)
{
	return std::stold(GetConfigElementStr(key));
}