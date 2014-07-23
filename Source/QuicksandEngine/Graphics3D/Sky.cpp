#include "../Stdafx.hpp"
#include "Sky.hpp"


#include "GLRenderer.hpp"
#include "Geometry.hpp"
#include "SceneNode.hpp"
#include "Scene.hpp"
#include "Shaders.hpp"
#include "Sky.hpp"

////////////////////////////////////////////////////
// SkyNode Implementation
////////////////////////////////////////////////////

//
// SkyNode::SkyNode						- Chapter 16, page 554
//
SkyNode::SkyNode(const char *pTextureBaseName)
	: SceneNode(INVALID_ACTOR_ID, WeakBaseRenderComponentPtr(), RenderPass_Sky, &mat4())
	, m_bActive(true)
{
	m_textureBaseName = pTextureBaseName;
}

//
// SkyNode::VPreRender				- Chapter 14, page 502
//
HRESULT SkyNode::VPreRender(Scene *pScene)
{
	vec3 cameraPos = m_camera->VGet()->ToWorld().GetPosition();
	mat4 mat = m_Props.ToWorld();
	SetPosition(mat,cameraPos);
	VSetTransform(&mat);

	return SceneNode::VPreRender(pScene);
}





//
// GLSkyNode::GLSkyNode					- Chapter 16, page 555
//
GLSkyNode::GLSkyNode(const char *pTextureBaseName)
	: SkyNode(pTextureBaseName)
{
	m_IndexBuffer = 0;
	m_VertexBuffer = 0;
	m_TextureVertBuffer = 0;
}

//
// GLSkyNode::~GLSkyNode					- Chapter 16, page 555
//
GLSkyNode::~GLSkyNode()
{
	glDeleteBuffers(1, &m_IndexBuffer);
	glDeleteBuffers(1, &m_VertexBuffer);
	glDeleteBuffers(1, &m_TextureVertBuffer);
}


//
// GLSkyNode::VOnRestore						- Chapter 16, page 556
//
HRESULT GLSkyNode::VOnRestore(Scene *pScene)
{
	HRESULT hr;

	V_RETURN(SceneNode::VOnRestore(pScene));

	m_camera = pScene->GetCamera();

	//TODO: onRestore for shader classes
	V_RETURN(m_pShaderProgram.OnRestore(pScene));
	V_RETURN(m_PixelShader.OnRestore(pScene));

	m_numVerts = 20;

	// Fill the vertex buffer. We are setting the tu and tv texture
	// coordinates, which range from 0.0 to 1.0
	GLVertexTexturedList pVertices;
	pVertices.resize(m_numVerts);
	LOG_ASSERT(pVertices.size() != m_numVerts && "Out of memory in GLSkyNode::VOnRestore()");
	if (pVertices.size() != m_numVerts)
		return E_FAIL;

	// Loop through the grid squares and calc the values
	// of each index. Each grid square has two triangles:
	//
	//		A - B
	//		| / |
	//		C - D

	Color skyVertColor = g_White;
	float dim = 50.0f;

	m_VertexBufferData.resize(4); m_TextureVertBufferData.resize(4);
	m_VertexBufferData[0] = vec3(dim, dim, dim);   m_TextureVertBufferData[0] = vec2(1.0f, 0.0f);
	m_VertexBufferData[1] = vec3(-dim, dim, dim);  m_TextureVertBufferData[1] = vec2(0.0f, 0.0f);
	m_VertexBufferData[2] = vec3(dim, -dim, dim);  m_TextureVertBufferData[2] = vec2(1.0f, 1.0f);
	m_VertexBufferData[3] = vec3(-dim, -dim, dim); m_TextureVertBufferData[3] = vec2(0.0f, 1.0f);

	vec3 triangle[3];
	triangle[0] = vec3(0.f, 0.f, 0.f);
	triangle[1] = vec3(5.f, 0.f, 0.f);
	triangle[2] = vec3(5.f, 5.f, 0.f);

	vec3 edge1 = triangle[1] - triangle[0];
	vec3 edge2 = triangle[2] - triangle[0];

	vec3 normal;
	normal = cross(edge1,edge2);
	normal.Normalize();

	mat4 rotY;
	rotY.BuildRotationY(AR_PI / 2.0f);
	mat4 rotX;
	rotX.BuildRotationX(-AR_PI / 2.0f);

	m_sides = 5;

	for (DWORD side = 0; side < m_sides; side++)
	{
		for (DWORD v = 0; v < 4; v++)
		{
			vec4 temp;
			if (side < m_sides - 1)
			{
				temp = Xform(rotY,vec3(skyVerts[v].Pos));
			}
			else
			{
				skyVerts[0].Uv = vec2(1.0f, 1.0f);
				skyVerts[1].Uv = vec2(1.0f, 1.0f);
				skyVerts[2].Uv = vec2(1.0f, 1.0f);
				skyVerts[3].Uv = vec2(1.0f, 1.0f);

				temp = Xform(rotX, vec3(skyVerts[v].Pos));
			}
			skyVerts[v].Pos = vec3(temp);
		}
		memcpy(&pVertices[side * 4], skyVerts, sizeof(skyVerts));
	}

	//TODO: start here
	//create the buffers
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	glGenBuffers(1, &m_VertexBuffer);
	glGenBuffers(1, &m_IndexBuffer);
	glGenBuffers(1, &m_TextureVertBuffer);

	//verts
	glBindBuffer(GL_VERTEX_ARRAY, m_VertexBuffer);
	glBufferStorage(GL_VERTEX_ARRAY, m_VertexBufferData.size() * 3 * sizeof(GLfloat), m_VertexBufferData.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(m_VertexBuffer);
	glVertexAttribPointer(m_VertexBuffer, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//UV
	glBindBuffer(GL_VERTEX_ARRAY, m_TextureVertBuffer);
	glBufferStorage(GL_VERTEX_ARRAY, m_TextureVertBufferData.size() * 2 * sizeof(GLfloat), m_TextureVertBufferData.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_VERTEX_ARRAY, 0);

	// Loop through the grid squares and calc the values
	// of each index. Each grid square has two triangles:
	//
	//		A - B
	//		| / |
	//		C - D

	WORD *pIndices = QSE_NEW WORD[m_sides * 2 * 3];

	WORD *current = pIndices;
	for (DWORD i = 0; i<m_sides; i++)
	{
		// Triangle #1  ACB
		*(current) = WORD(i * 4);
		*(current + 1) = WORD(i * 4 + 2);
		*(current + 2) = WORD(i * 4 + 1);

		// Triangle #2  BCD
		*(current + 3) = WORD(i * 4 + 1);
		*(current + 4) = WORD(i * 4 + 2);
		*(current + 5) = WORD(i * 4 + 3);
		current += 6;
	}

	m_IndexBufferData = ArrToVec(pIndices, m_sides * 2 * 3);

	//now the indicies
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, m_sides * 2 * 3 * sizeof(GLfloat), m_IndexBufferData.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	SAFE_DELETE_ARRAY(pIndices);


	return S_OK;
}

//
// GLSkyNode::VRender						- Chapter 16, page 559
//
HRESULT GLSkyNode::VRender(Scene *pScene)
{
	HRESULT hr;

	//TODO:
	V_RETURN(m_VertexShader.SetupRender(pScene, this));
	V_RETURN(m_PixelShader.SetupRender(pScene, this));

	// Set vertex buffer
	UINT stride = sizeof(D3D11Vertex_UnlitTextured);
	UINT offset = 0;
	DXUTGetD3D11DeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set index buffer
	DXUTGetD3D11DeviceContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	DXUTGetD3D11DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (DWORD side = 0; side < m_sides; side++)
	{
		// FUTURTE WORK: A good optimization would be to transform the camera's
		// world look vector into local space, and do a dot product. If the
		// result is positive, we shouldn't draw the side since it has to be
		// behind the camera!

		// Sky boxes aren't culled by the normal mechanism

		/***
		// [mrmike] - This was slightly changed post press, look at the lines below this commented out code
		const char *suffix[] = { "_n.jpg", "_e.jpg",  "_s.jpg",  "_w.jpg",  "_u.jpg" };
		name += suffix[side];
		****/

		std::string name = GetTextureName(side);
		m_PixelShader.SetTexture(name.c_str());

		DXUTGetD3D11DeviceContext()->DrawIndexed(6, side * 6, 0);
	}
	return S_OK;
}


std::string SkyNode::GetTextureName(const int side)
{
	std::string name = m_textureBaseName;
	char *letters[] = { "n", "e", "s", "w", "u" };
	unsigned int index = name.find_first_of("_");
	LOG_ASSERT(index >= 0 && index < name.length() - 1);
	if (index >= 0 && index < name.length() - 1)
	{
		name[index + 1] = *letters[side];
	}
	return name;
}



