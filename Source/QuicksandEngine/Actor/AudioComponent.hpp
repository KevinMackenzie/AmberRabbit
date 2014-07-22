#ifndef QSE_AUDIOCOMPONENT_HPP
#define QSE_AUDIOCOMPONENT_HPP

#include "ActorComponent.hpp"

//---------------------------------------------------------------------------------------------------------------------
// AudioComponent class.
// [rez] This component was never directly described anywhere in the book but it's used to allow actors to trigger 
// sound effects.
//---------------------------------------------------------------------------------------------------------------------
class AudioComponent : public ActorComponent
{
	string m_audioResource;
	bool m_looping;
	float m_fadeInTime;
	int m_volume;

public:
	static const char *g_Name;
	virtual const char *VGetName() const { return g_Name; }

	bool Start();

    AudioComponent(void);

    virtual XMLElement* VGenerateXml(XMLDocument* pDoc);

    // ActorComponent interface
    virtual bool VInit(XMLElement* pData) override;
    virtual void VPostInit(void) override;
};

#endif