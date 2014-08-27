#ifndef QSE_MATERIAL_HPP
#define QSE_MATERIAL_HPP


#include "Geometry.hpp"
#include "../ResourceCache/ResCache.hpp"

class GLMaterial
{
	Color m_Diffuse, m_Ambient, m_Specular, m_Emissive;			// This structure stores diffuse, ambient, specular, emissive, and power.
	float m_Power;

	std::string m_MaterialName;
	shared_ptr<ResHandle> m_pTexture;
public:
	GLMaterial();
	void SetAmbient(const Color &color);
	const Color GetAmbient() { return m_Ambient; }

	void SetDiffuse(const Color &color);
	const Color GetDiffuse() { return m_Diffuse; }

	bool HasTexture() { return m_pTexture != nullptr; }
	shared_ptr<ResHandle> GetTexture() { return m_pTexture; }
	void SetTexture(shared_ptr<ResHandle> pTex) { m_pTexture = pTex; }

	void SetSpecular(const Color &color, const float power);
	void GetSpecular(Color &_color, float &_power)
	{
		_color = m_Specular; _power = m_Power;
	}

	void SetName(std::string name) { m_MaterialName = name; }
	std::string GetName(void) { return m_MaterialName; }

	void SetEmissive(const Color &color);
	const Color GetEmissive() { return m_Emissive; }

	void SetAlpha(const float alpha);
	bool HasAlpha() const { return GetAlpha() != fOPAQUE; }
	float GetAlpha() const { return m_Diffuse.a; }

	void GLUse();
};


// 
//  class D3DTextureResourceExtraData11				- Chapter 14, page 492
//	
#define UNSUPPORTED_IMAGE_FORMAT -1
class GLTextureResourceExtraData : public IResourceExtraData
{
	friend class TextureResourceLoader;
	friend class DdsResourceLoader;
	friend class PngResourceLoader;
	friend class JpgResourceLoader;

public:
	GLTextureResourceExtraData();
	virtual ~GLTextureResourceExtraData();
	virtual std::string VToString() { return "GLTextureResourceExtraData"; }

	GLuint GetTexture() { return m_pTexture; }

	//NOTE: all sampling is a config element that is accessed by the renderer
protected:
	
	GLuint m_pTexture;
};



// 
//  class TextureResourceLoader				- Chapter 14, page 492
//
class TextureResourceLoader : public IResourceLoader
{
public:
	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize);
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle) = 0;
};




#endif