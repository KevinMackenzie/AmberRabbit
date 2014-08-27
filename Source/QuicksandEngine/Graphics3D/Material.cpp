#include "../Stdafx.hpp"
#include "Material.hpp"
#include "../ResourceCache/ResCache.hpp"
#include "SceneNode.hpp"
#include <png.h>
#include <jpeglib.h>
//#include "../GLI/gli.hpp"


////////////////////////////////////////////////////
// class GLMaterial						- Chapter 14, page 486
////////////////////////////////////////////////////

GLMaterial::GLMaterial()
{
	m_Diffuse = g_White;
	m_Ambient = Color(0.10f, 0.10f, 0.10f, 1.0f);
	m_Specular = g_White;
	m_Emissive = g_Black;
}

void GLMaterial::SetAmbient(const Color &color)
{
	m_Ambient = color;
}

void GLMaterial::SetDiffuse(const Color &color)
{
	m_Diffuse = color;
}

void GLMaterial::SetSpecular(const Color &color, const float power)
{
	m_Specular = color;
	m_Power = power;
}

void GLMaterial::SetEmissive(const Color &color)
{
	m_Emissive = color;
}

void GLMaterial::SetAlpha(float alpha)
{
	m_Diffuse.a = glm::u8(glm::clamp(alpha * 255.0f, 0.0f, 255.0f));
}

void GLMaterial::GLUse()
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
	virtual bool VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
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
	virtual bool VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
};

shared_ptr<IResourceLoader> CreatePNGResourceLoader()
{
	return shared_ptr<IResourceLoader>(QSE_NEW PngResourceLoader());
}


GLTextureResourceExtraData::GLTextureResourceExtraData()
	: m_pTexture(0)
{
}

GLTextureResourceExtraData::~GLTextureResourceExtraData()
{
	glDeleteBuffers(1, &m_pTexture);
}


unsigned int TextureResourceLoader::VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize)
{
	// This will keep the resource cache from allocating memory for the texture, so DirectX can manage it on it's own.
	return 0;
}

//
// TextureResourceLoader::VLoadResource				- Chapter 14, page 492
//

bool PngResourceLoader::VLoadResource(char* rawData, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	auto extra = shared_ptr<GLTextureResourceExtraData>(QSE_NEW GLTextureResourceExtraData());

	//header for testing if it is a png
	png_byte header[8];

	//open file as binary
	FILE *fp = fopen("DO_NOT_DELETE", "rb");
	if (!fp) 
	{
		LOG_ERROR("Dummy File Deleted!")
		return false;
	}

	//my hack
	setbuf(fp, rawData);

	//read the header
	fread(header, 1, 8, fp);

	//test if png
	int is_png = !png_sig_cmp(header, 0, 8);
	if (!is_png) 
	{
		fclose(fp);
		return false;
	}

	//create png struct
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
		NULL, NULL);
	if (!png_ptr) 
	{
		fclose(fp);
		return false;
	}

	//create png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) 
	{
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(fp);
		return false;
	}

	//create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info) 
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		return false;
	}

	//png error stuff, not sure libpng man suggests this.
	if (setjmp(png_jmpbuf(png_ptr))) 
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return false;
	}

	//init png reading
	png_init_io(png_ptr, fp);

	//let libpng know you already read the first 8 bytes
	png_set_sig_bytes(png_ptr, 8);

	// read all the info up to the image data
	png_read_info(png_ptr, info_ptr);

	//variables to pass to get info
	int bit_depth, color_type;
	png_uint_32 twidth, theight;

	// get info about png
	png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
		NULL, NULL, NULL);

	//update width and height based on png info
	//width = twidth;
	//height = theight;

	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);

	// Row size in bytes.
	int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	// Allocate the image_data as a big block, to be given to opengl
	png_byte *image_data = new png_byte[rowbytes * theight];
	if (!image_data) 
	{
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return false;
	}

	//row_pointers is for pointing to image_data for reading the png with libpng
	png_bytep *row_pointers = new png_bytep[theight];
	if (!row_pointers) {
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		delete[] image_data;
		fclose(fp);
		return false;
	}
	// set the individual row_pointers to point at the correct offsets of image_data
	for (unsigned int i = 0; i < theight; ++i)
		row_pointers[theight - 1 - i] = image_data + i * rowbytes;

	//read the png into image_data through row_pointers
	png_read_image(png_ptr, row_pointers);

	//Now generate the OpenGL texture object
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, twidth, theight, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//clean up memory and close stuff
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	delete[] image_data;
	delete[] row_pointers;
	fclose(fp);

	extra->m_pTexture = texture;

	handle->SetExtra(extra);

	/* That's it */
	return true;
}

bool JpgResourceLoader::VLoadResource(char* rawData, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	auto extra = shared_ptr<GLTextureResourceExtraData>(QSE_NEW GLTextureResourceExtraData());
	GLuint texture;

	FILE *fd;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	if (0 == (fd = fopen("DO_NOT_DELETE", "rb")))
	{
		LOG_ERROR("Dummy File Deleted!")
		return false;
	}

	//a little hack
	setbuf(fd, rawData);

	jpeg_stdio_src(&cinfo, fd);
	jpeg_read_header(&cinfo, TRUE);
	
	if (!jpeg_start_decompress(&cinfo))
	{
		LOG_ERROR("Failed to start jpeg decompression");
		return false;
	}


	//JPG can be grayscale OR RGB
	unsigned int offset = cinfo.out_color_space == JCS_GRAYSCALE ? 1 : 3;

	//the bitmap data
	unsigned char* bitmap = (unsigned char*)malloc(cinfo.output_width * cinfo.output_height * offset);

	//loop through and get each line
	unsigned char* line;
	while (cinfo.output_scanline < cinfo.output_height)
	{
		line = bitmap +
			offset * cinfo.output_width * cinfo.output_scanline;
		jpeg_read_scanlines(&cinfo, &line, 1);
	}
	
	if (!jpeg_finish_decompress(&cinfo))
	{
		LOG_ERROR("Failed to finish jpeg decompression");
		return false;
	}

	//buffer the data
	glGenBuffers(1, &texture);
	glBindBuffer(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0,
		offset == 1 ? GL_LUMINANCE : GL_RGB, 
		cinfo.output_width,
		cinfo.output_height, 0, 
		offset == 1 ? GL_LUMINANCE : GL_RGB, 
		GL_UNSIGNED_BYTE, 
		bitmap);

	//clean up
	free(bitmap);
	fclose(fd);
	line = nullptr;


	extra->m_pTexture = texture;

	handle->SetExtra(extra);

	/* That's it */
	return true;
}

bool DdsResourceLoader::VLoadResource(char* rawData, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	auto extra = shared_ptr<GLTextureResourceExtraData>(QSE_NEW GLTextureResourceExtraData());
	//GLI only does .dds's

	extra->m_pTexture = GLUF::LoadTextureFromMemory(rawData, rawSize, GLUF::TFF_DDS);

	handle->SetExtra(extra);
	return true;
}

