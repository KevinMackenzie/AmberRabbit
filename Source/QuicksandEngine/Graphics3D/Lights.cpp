#include "../Stdafx.hpp"
#include "Lights.hpp"

#include "../Actor/RenderComponent.hpp"


LightNode::LightNode(const ActorId actorId, WeakBaseRenderComponentPtr renderComponent, const LightProperties &props, const mat4 *t)
	: SceneNode(actorId, renderComponent, RenderPass_NotRendered, t)
{
	m_LightProps = props;
}

HRESULT GLLightNode::VOnUpdate(Scene *, DWORD const elapsedMs)
{
	// light color can change anytime! Check the BaseRenderComponent!
	LightRenderComponent* lrc = static_cast<LightRenderComponent*>(m_RenderComponent);
	m_Props.GetMaterial().SetDiffuse(lrc->GetColor());
	return S_OK;
}


//
// LightManager::CalcLighting					- Chapter 16, page 554
//
void LightManager::CalcLighting(Scene *pScene)
{
	// FUTURE WORK: There might be all kinds of things you'd want to do here for optimization, especially turning off lights on actors that can't be seen, etc.
	pScene->GetRenderer()->VCalcLighting(&m_Lights, MAXIMUM_LIGHTS_SUPPORTED);

	int count = 0;

	LOG_ASSERT(m_Lights.size() < MAXIMUM_LIGHTS_SUPPORTED);
	for (Lights::iterator i = m_Lights.begin(); i != m_Lights.end(); ++i, ++count)
	{
		shared_ptr<LightNode> light = *i;

		if (count == 0)
		{
			// Light 0 is the only one we use for ambient lighting. The rest are ignored in the simple shaders used for GameCode4.
			Color ambient = light->VGet()->GetMaterial().GetAmbient();
			m_vLightAmbient = vec4(ambient.r, ambient.g, ambient.b, 1.0f);
		}

		vec3 lightDir = light->GetDirection();
		m_vLightDir[count] = vec4(lightDir.x, lightDir.y, lightDir.z, 1.0f);
		m_vLightDiffuse[count] = light->VGet()->GetMaterial().GetDiffuse();
	}
}


void LightManager::CalcLighting(ConstantBuffer_Lighting* pLighting, SceneNode *pNode)
{
	int count = GetLightCount(pNode);
	if (count)
	{
		pLighting->m_vLightAmbient = *GetLightAmbient(pNode);
		memcpy(pLighting->m_vLightDir, GetLightDirection(pNode), sizeof(vec4) * count);
		memcpy(pLighting->m_vLightDiffuse, GetLightDiffuse(pNode), sizeof(vec4) * count);
		pLighting->m_nNumLights = count;
	}
}

