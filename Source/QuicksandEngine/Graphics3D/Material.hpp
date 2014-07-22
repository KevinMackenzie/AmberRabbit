#ifndef QSE_MATERIAL_HPP
#define QSE_MATERIAL_HPP


#include "Geometry.hpp"
#include "../ResourceCache/ResCache.hpp"

class Material
{
	Color m_Diffuse, m_Ambient, m_Specular, m_Emissive;			// This structure stores diffuse, ambient, specular, emissive, and power.
	float m_Power;
public:
	Material();
	void SetAmbient(const Color &color);
	const Color GetAmbient() { return m_Ambient; }

	void SetDiffuse(const Color &color);
	const Color GetDiffuse() { return m_Diffuse; }

	void SetSpecular(const Color &color, const float power);
	void GetSpecular(Color &_color, float &_power)
	{
		_color = m_Specular; _power = m_Power;
	}

	void SetEmissive(const Color &color);
	const Color GetEmissive() { return m_Emissive; }

	void SetAlpha(const float alpha);
	bool HasAlpha() const { return GetAlpha() != fOPAQUE; }
	float GetAlpha() const { return m_Diffuse.a; }

	void D3DUse9();
};

/*
// 
//  class D3DTextureResourceExtraData11				- Chapter 14, page 492
//
class GLTextureResourceExtraData : public IResourceExtraData
{
	friend class TextureResourceLoader;

public:
	GLTextureResourceExtraData();
	virtual ~GLTextureResourceExtraData() { SAFE_RELEASE(m_pTexture); SAFE_RELEASE(m_pSamplerLinear); }
	virtual std::string VToString() { return "GLTextureResourceExtraData"; }

	GLuint * const *GetTextureUniformLocation() { return &m_pTexture; }
	GLint  * const *GetSamplerUniformLocation() { return &m_pSamplerLinear; }

protected:
	GLuint *m_pTexture;
	GLint  *m_pSamplerLinear;
};*/



// 
//  class TextureResourceLoader				- Chapter 14, page 492
//
class TextureResourceLoader : public IResourceLoader
{
public:
	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize);
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
};


#endif