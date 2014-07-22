#include "../Stdafx.hpp"

#include "..\Utilities\String.hpp"
#include "AudioComponent.hpp"
#include "..\Audio\SoundProcess.hpp"
#include "..\MainLoop\ProcessManager.hpp"
#include "..\UserInterface\HumanView.hpp"

const char* AudioComponent::g_Name = "AudioComponent";

AudioComponent::AudioComponent()
{
	m_audioResource = "";
	m_looping = false;
	m_fadeInTime = 0.0f;
	m_volume = 100;
}

bool AudioComponent::VInit(XMLElement* pData)
{
    XMLElement* pTexture = pData->FirstChildElement("Sound");
    if (pTexture)
	{
		m_audioResource = pTexture->FirstChild()->Value();
	}

	XMLElement* pLooping = pData->FirstChildElement("Looping");
	if (pLooping)
	{
		string value = pLooping->FirstChild()->Value();
		m_looping = (value == "0") ? false : true;
	}

	XMLElement* pFadeIn = pData->FirstChildElement("FadeIn");
	if (pFadeIn)
	{
		string value = pFadeIn->FirstChild()->Value();
		m_fadeInTime = (float)atof(value.c_str());
	}

	XMLElement* pVolume = pData->FirstChildElement("Volume");
	if (pVolume)
	{
		string value = pVolume->FirstChild()->Value();
		m_volume = atoi(value.c_str());
	}

	return true;
}


XMLElement* AudioComponent::VGenerateXml(XMLDocument* pDoc)
{
    XMLElement* pBaseElement = QSE_NEW XMLElement(VGetName());

	XMLElement* pSoundNode = QSE_NEW XMLElement("Sound");
    TiXmlText* pSoundText = QSE_NEW TiXmlText(m_audioResource.c_str());
    pSoundNode->LinkEndChild(pSoundText);
    pBaseElement->LinkEndChild(pSoundNode);

    XMLElement* pLoopingNode = QSE_NEW XMLElement("Looping");
    TiXmlText* pLoopingText = QSE_NEW TiXmlText(m_looping ? "1" : "0" );
    pLoopingNode->LinkEndChild(pLoopingText);
    pBaseElement->LinkEndChild(pLoopingNode);

    XMLElement* pFadeInNode = QSE_NEW XMLElement("FadeIn");
    TiXmlText* pFadeInText = QSE_NEW TiXmlText( ToStr(m_fadeInTime).c_str());
    pFadeInNode->LinkEndChild(pFadeInText);
    pBaseElement->LinkEndChild(pFadeInNode);

    XMLElement* pVolumeNode = QSE_NEW XMLElement("Volume");
    TiXmlText* pVolumeText = QSE_NEW TiXmlText( ToStr(m_volume).c_str());
    pVolumeNode->LinkEndChild(pVolumeText);
    pBaseElement->LinkEndChild(pVolumeNode);

	return pBaseElement;
}

void AudioComponent::VPostInit()
{
	HumanView *humanView = g_pApp->GetHumanView();
	if (!humanView)
	{
		GCC_ERROR("Sounds need a human view to be heard!");
		return;
	}

	ProcessManager *processManager = humanView->GetProcessManager();
	if (!processManager)
	{
		GCC_ERROR("Sounds need a process manager to attach!");
		return;
	}

	if (!g_pApp->IsEditorRunning())
	{
		// The editor can play sounds, but it shouldn't run them when AudioComponents are initialized.

		Resource resource(m_audioResource);
		shared_ptr<ResHandle> rh = g_pApp->m_ResCache->GetHandle(&resource);
		shared_ptr<SoundProcess> sound(QSE_NEW SoundProcess(rh, 0, true));
		processManager->AttachProcess(sound);

		// fade process
		if (m_fadeInTime > 0.0f)
		{
			shared_ptr<FadeProcess> fadeProc(new FadeProcess(sound, 10000, 100)); 
			processManager->AttachProcess(fadeProc);
		}
	}
}
