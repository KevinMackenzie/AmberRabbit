#ifndef AW_CONFIGMANAGER_HPP
#define AW_CONFIGMANAGER_HPP
#include "Configuration.hpp"
#include <tinyxml2.h>

//this loads configuration from an xml, and puts it into the global config 
//NOTE:LOADING ELEMENTS MUST ONLY BE CALLED BY MAIN THREAD
class StringsManager
{
	Configuration gConfig;

	//this is a recursive method that goes through and loads all of the elements in the given root
	void LoadConfigElements(tinyxml2::XMLElement *root);
	
	//this is used by LoadConfigElements only
	string mCurrPrefix;

public:
	StringsManager(){}

	XMLDocument* GetDocument(){ return gConfig.pDoc; }

	//the three below functions are NOT MUTABLE IN ANY WAY

	//how to add elements
	bool AddConfigElements(string pathToXml);

	//this will not be called unless a config refresh is in order (which will be complicated)
	void FlushConfig();

	//explicitly typed helper functions
	string GetElementStr(string key);

	void ModifyElement(string key, string element);

	//these functions ARE MUTABLE 

	char GetElementc(string key);//THIS IS THE NUMBER (0-255)
	unsigned char GetElementuc(string key);
	short GetElements(string key);
	unsigned short GetElementus(string key);
	int GetElementi(string key);
	unsigned int GetElementui(string key);
	long GetElementl(string key);
	unsigned long GetElementul(string key);
	long long GetElementll(string key);
	unsigned long long GetElementull(string key);

	float GetElementf(string key);
	double GetElementd(string key);
	long double GetElementld(string key);

};


//preprocessors to help getting configuration elements

#define GET_CONFIG_ELEMENT_STR(key) QuicksandEngine::g_pApp->mConfigManager.GetElementStr(key)

#define GET_CONFIG_ELEMENT_C(key)	QuicksandEngine::g_pApp->mConfigManager.GetElementc(key)
#define GET_CONFIG_ELEMENT_UC(key)	QuicksandEngine::g_pApp->mConfigManager.GetElementuc(key)
#define GET_CONFIG_ELEMENT_S(key)	QuicksandEngine::g_pApp->mConfigManager.GetElements(key)
#define GET_CONFIG_ELEMENT_US(key)	QuicksandEngine::g_pApp->mConfigManager.GetElementus(key)
#define GET_CONFIG_ELEMENT_I(key)	QuicksandEngine::g_pApp->mConfigManager.GetElementi(key)
#define GET_CONFIG_ELEMENT_UI(key)	QuicksandEngine::g_pApp->mConfigManager.GetElementui(key)
#define GET_CONFIG_ELEMENT_L(key)	QuicksandEngine::g_pApp->mConfigManager.GetElementl(key)
#define GET_CONFIG_ELEMENT_UL(key)	QuicksandEngine::g_pApp->mConfigManager.GetElementul(key)
#define GET_CONFIG_ELEMENT_LL(key)	QuicksandEngine::g_pApp->mConfigManager.GetElementll(key)
#define GET_CONFIG_ELEMENT_ULL(key)	QuicksandEngine::g_pApp->mConfigManager.GetElementull(key)

#define GET_CONFIG_ELEMENT_F(key)	QuicksandEngine::g_pApp->mConfigManager.GetElementf(key)
#define GET_CONFIG_ELEMENT_D(key)	QuicksandEngine::g_pApp->mConfigManager.GetElementd(key)
#define GET_CONFIG_ELEMENT_LD(key)	QuicksandEngine::g_pApp->mConfigManager.GetElementld(key)

#define MODIFY_CONFIG_ELEMENT(key, value) QuicksandEngine::g_pApp->mConfigManager.ModifyElement(key, value);


#define GET_STRING(key) QuicksandEngine::g_pApp->mStringsManager.GetElementStr(key)
#endif