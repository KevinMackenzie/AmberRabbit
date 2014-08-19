#include "../Stdafx.hpp"
#include "ConfigManager.hpp"
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

	gConfig.pDoc = new tinyxml2::XMLDocument();
	gConfig.pDoc->LoadFile(pathToXml.c_str());
	//TODO: make username/password a config element but secure it in a good way

	tinyxml2::tinyxml2::XMLElement *root = gConfig.pDoc->FirstChildElement("ROOT");//this is manditory
	
	//now loop through all of the iterators
	//this is a map, so to MYSQL->PORT will be MYSQL_PORT and so forth

	//now recursively load the elements
	LoadConfigElements(root->FirstChildElement());

	return true;
}

void StringsManager::LoadConfigElements(tinyxml2::tinyxml2::XMLElement *root)
{
	
	//since we are using a map, make a prefix ie MYSQL_ and this is append with each new node
	//as long as it is not the root element
	if (root->GetText() != "ROOT")
	{
		mCurrPrefix += root->GetText();
		mCurrPrefix += "_";
	}

	for (tinyxml2::tinyxml2::XMLElement* child = root->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
	{
		//is there a child element?
		if (child->FirstChildElement() != nullptr)
		{
			//recurse
			LoadConfigElements(child);
		}

		std::stringstream childName;
		//first the prefix, then the actual name
		childName << mCurrPrefix;
		childName << child->Name();

		//add each one to the config options AS LONG AS THE KEY DOESN"T ALREADY EXIST
		if (gConfig.mConfigOptions.find(child->Name()) != gConfig.mConfigOptions.end())
		{
			//log this as an error, becuase this will be important to fix to ensure there are no conflicting configurations.
			std::stringstream ss;
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

string StringsManager::GetElementStr(string key)
{
	string ret = "";
	try
	{
		ret = gConfig.GetElement(key);
	}
	catch (string exception)
	{
		if (exception == INVALID_CONFIG_KEY)
		{
			std::stringstream ss;
			ss << "Invalid configuration key, no value for key \'" << key << "\'";
			LOG_ERROR(ss.str());
		}
		else
		{
			std::stringstream ss;
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

char StringsManager::GetElementc(string key)
{
	return static_cast<char>(std::stoi(GetElementStr(key)));
}

unsigned char StringsManager::GetElementuc(string key)
{
	return static_cast<unsigned char>(std::stoi(GetElementStr(key)));
}

short StringsManager::GetElements(string key)
{
	return static_cast<short>(std::stoi(GetElementStr(key)));
}

unsigned short StringsManager::GetElementus(string key)
{
	return static_cast<unsigned short>(std::stoi(GetElementStr(key)));
}

int StringsManager::GetElementi(string key)
{
	return std::stoi(GetElementStr(key));
}

unsigned int StringsManager::GetElementui(string key)
{
	return static_cast<unsigned int>(std::stoi(GetElementStr(key)));
}

long StringsManager::GetElementl(string key)
{
	return std::stol(GetElementStr(key));
}

unsigned long StringsManager::GetElementul(string key)
{
	return std::stoul(GetElementStr(key));
}

long long StringsManager::GetElementll(string key)
{
	return std::stoll(GetElementStr(key));
}

unsigned long long StringsManager::GetElementull(string key)
{
	return std::stoull(GetElementStr(key));
}

float StringsManager::GetElementf(string key)
{
	return std::stof(GetElementStr(key));
}

double StringsManager::GetElementd(string key)
{
	return std::stod(GetElementStr(key));
}

long double StringsManager::GetElementld(string key)
{
	return std::stold(GetElementStr(key));
}