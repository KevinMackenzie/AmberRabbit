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

bool AudioComponent::VInit(tinyxml2::XMLElement* pData)
{
    tinyxml2::XMLElement* pTexture = pData->FirstChildElement("Sound");
    if (pTexture)
	{
		m_audioResource = pTexture->FirstChild()->Value();
	}

	tinyxml2::XMLElement* pLooping = pData->FirstChildElement("Looping");
	if (pLooping)
	{
		string value = pLooping->FirstChild()->Value();
		m_looping = (value == "0") ? false : true;
	}

	tinyxml2::XMLElement* pFadeIn = pData->FirstChildElement("FadeIn");
	if (pFadeIn)
	{
		string value = pFadeIn->FirstChild()->Value();
		m_fadeInTime = (float)atof(value.c_str());
	}

	tinyxml2::XMLElement* pVolume = pData->FirstChildElement("Volume");
	if (pVolume)
	{
		string value = pVolume->FirstChild()->Value();
		m_volume = atoi(value.c_str());
	}

	return true;
}


tinyxml2::XMLElement* AudioComponent::VGenerateXml(tinyxml2::XMLDocument* pDoc)
{
    tinyxml2::XMLElement* pBaseElement = pDoc->NewElement(VGetName());

	tinyxml2::XMLElement* pSoundNode = pDoc->NewElement("Sound");
    tinyxml2::XMLText* pSoundText = pDoc->NewText(m_audioResource.c_str());
    pSoundNode->LinkEndChild(pSoundText);
    pBaseElement->LinkEndChild(pSoundNode);

	tinyxml2::XMLElement* pLoopingNode = pDoc->NewElement("Looping");
	tinyxml2::XMLText* pLoopingText = pDoc->NewText(m_looping ? "1" : "0");
    pLoopingNode->LinkEndChild(pLoopingText);
    pBaseElement->LinkEndChild(pLoopingNode);

	tinyxml2::XMLElement* pFadeInNode = pDoc->NewElement("FadeIn");
	tinyxml2::XMLText* pFadeInText = pDoc->NewText(ToStr(m_fadeInTime).c_str());
    pFadeInNode->LinkEndChild(pFadeInText);
    pBaseElement->LinkEndChild(pFadeInNode);

	tinyxml2::XMLElement* pVolumeNode = pDoc->NewElement("Volume");
	tinyxml2::XMLText* pVolumeText = pDoc->NewText(ToStr(m_volume).c_str());
    pVolumeNode->LinkEndChild(pVolumeText);
    pBaseElement->LinkEndChild(pVolumeNode);

	return pBaseElement;
}

void AudioComponent::VPostInit()
{
	HumanView *humanView = QuicksandEngine::g_pApp->GetHumanView();
	if (!humanView)
	{
		LOG_ERROR("Sounds need a human view to be heard!");
		return;
	}

	ProcessManager *processManager = humanView->GetProcessManager();
	if (!processManager)
	{
		LOG_ERROR("Sounds need a process manager to attach!");
		return;
	}

	if (!QuicksandEngine::g_pApp->IsEditorRunning())
	{
		// The editor can play sounds, but it shouldn't run them when AudioComponents are initialized.

		Resource resource(m_audioResource);
		shared_ptr<ResHandle> rh = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&resource);
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
