#include "../Stdafx.hpp"
#include "Material.hpp"
#include "../ResourceCache/ResCache.hpp"
#include "SceneNode.hpp"
#include "../GLI/gli.hpp"


////////////////////////////////////////////////////
// class Material						- Chapter 14, page 486
////////////////////////////////////////////////////

Material::Material()
{
	m_Diffuse = g_White;
	m_Ambient = Color(0.10f, 0.10f, 0.10f, 1.0f);
	m_Specular = g_White;
	m_Emissive = g_Black;
}

void Material::SetAmbient(const Color &color)
{
	m_Ambient = color;
}

void Material::SetDiffuse(const Color &color)
{
	m_Diffuse = color;
}

void Material::SetSpecular(const Color &color, const float power)
{
	m_Specular = color;
	m_Power = power;
}

void Material::SetEmissive(const Color &color)
{
	m_Emissive = color;
}

void Material::SetAlpha(float alpha)
{
	m_Diffuse.a = alpha;
}

void Material::GLUse()
{
	//TODO;
}


//
// class DdsResourceLoader					- creates an interface with the Resource cache to load DDS files
//
class DdsResourceLoader : public TextureResourceLoader
{
public:
	virtual std::string VGetPattern() { return "*.dds"; }
	virtual bool VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
};

shared_ptr<IResourceLoader> CreateDDSResourceLoader()
{
	return shared_ptr<IResourceLoader>(QSE_NEW DdsResourceLoader());
}

//
// class JpgResourceLoader					- creates an interface with the Resource cache to load JPG files
//
class JpgResourceLoader : public TextureResourceLoader
{
public:
	virtual std::string VGetPattern() { return "*.jpg"; }
};

shared_ptr<IResourceLoader> CreateJPGResourceLoader()
{
	return shared_ptr<IResourceLoader>(QSE_NEW JpgResourceLoader());
}

//
// class PngResourceLoader					- creates an interface with the Resource cache to load PNG files
//
class PngResourceLoader : public TextureResourceLoader
{
public:
	virtual std::string VGetPattern() { return "*.png"; }
};

shared_ptr<IResourceLoader> CreatePNGResourceLoader()
{
	return shared_ptr<IResourceLoader>(QSE_NEW PngResourceLoader());
}


GLTextureResourceExtraData::GLTextureResourceExtraData()
	: m_Texture(NULL)
{
}

GLTextureResourceExtraData::~GLTextureResourceExtraData()
{
	glDeleteTextures(1, &m_Texture);
}

ILenum GetTextureType(std::string pattern)
{
	if (pattern == "*.dds")
		return IL_DDS;
	else if (pattern == "*.png")
		return IL_PNG;
	else if (pattern == "*.jpg")
		return IL_JPG;
	else
		return UNSUPPORTED_IMAGE_FORMAT;
}

unsigned int TextureResourceLoader::VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize)
{
	// This will keep the resource cache from allocating memory for the texture, so DirectX can manage it on it's own.
	return 0;
}

//
// TextureResourceLoader::VLoadResource				- Chapter 14, page 492
//
bool TextureResourceLoader::VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	shared_ptr<GLTextureResourceExtraData> extra = shared_ptr<GLTextureResourceExtraData>(QSE_NEW GLTextureResourceExtraData());

	// Load the Texture

		//everything else can just have standard flat images with no mipmaps
		
		
	//------ Generate an image name to use.
	ILuint ImgId = 0;
	ilGenImages(1, &ImgId);

	//------ Bind this image name.
	ilBindImage(ImgId);


	if (!ilLoadL(GetTextureType(VGetPattern()), rawBuffer, rawSize))
	{
		LOG_ERROR("Loading image failed. Invalid filename.");
		return false;
	}

	unsigned int texWidth = ilGetInteger(IL_IMAGE_WIDTH);
	unsigned int texHeight = ilGetInteger(IL_IMAGE_HEIGHT);

	int bytesPerPixel = ilGetInteger(IL_IMAGE_BPP);
	if (bytesPerPixel != 3)
	{
		LOG_ERROR("Loading image failed. Must be 24 bits/pixels.");
		return false;
	}

	//------ Allocate a buffer to store the bitmap data.

	unsigned char* data;


	//------ Copy data from DevIL to our pixmap.
	ilCopyPixels(0, 0, 0, texHeight, texWidth, 1, IL_RGBA/*ALWAYS load in RGBA space, becuase everything but JPG and a couple others support it*/, IL_UNSIGNED_BYTE, data);

	//------ Unbound image name and frees DevIL image memory.  I am only using DevIL to load to memory, I am loading to OpenGL Manuall
	ilBindImage(0);
	ilDeleteImage(ImgId);

	// Create the sampler state?
	//NO, there will be ONE... ONE sampler that is in the renderer

	//make this openGL
	glGenTextures(1, &extra->m_Texture);
	glBindTexture(GL_TEXTURE_2D, extra->m_Texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, texWidth, texHeight);

	glBindTexture(GL_TEXTURE_2D, 0);
	

	handle->SetExtra(shared_ptr<GLTextureResourceExtraData>(extra));
	return true;
}

bool DdsResourceLoader::VLoadResource(char* rawData, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	shared_ptr<GLTextureResourceExtraData> extra = shared_ptr<GLTextureResourceExtraData>(QSE_NEW GLTextureResourceExtraData());
	//GLI only does .dds's

	gli::texture2D Texture(gli::load_dds_memory(rawData, rawSize));
	assert(!Texture.empty());
	glBindTexture(GL_TEXTURE_2D, extra->m_Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
	glTexStorage2D(GL_TEXTURE_2D,
		GLint(Texture.levels()),
		GLenum(gli::internal_format(Texture.format())),
		GLsizei(Texture.dimensions().x),
		GLsizei(Texture.dimensions().y));
	if (gli::is_compressed(Texture.format()))
	{
		for (gli::texture2D::size_type Level = 0; Level < Texture.levels(); ++Level)
		{
			glCompressedTexSubImage2D(GL_TEXTURE_2D,
				GLint(Level),
				0, 0,
				GLsizei(Texture[Level].dimensions().x),
				GLsizei(Texture[Level].dimensions().y),
				GLenum(gli::internal_format(Texture.format())),
				GLsizei(Texture[Level].size()),
				Texture[Level].data());
		}
	}
	else
	{
		for (gli::texture2D::size_type Level = 0; Level < Texture.levels(); ++Level)
		{
			glTexSubImage2D(GL_TEXTURE_2D,
				GLint(Level),
				0, 0,
				GLsizei(Texture[Level].dimensions().x),
				GLsizei(Texture[Level].dimensions().y),
				GLenum(gli::external_format(Texture.format())),
				GLenum(gli::type_format(Texture.format())),
				Texture[Level].data());
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	handle->SetExtra(extra);
	return true;
}

