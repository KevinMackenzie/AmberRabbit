#include "../Stdafx.hpp"
#include "Sky.hpp"


#include "GLRenderer.hpp"
#include "Geometry.hpp"
#include "SceneNode.hpp"
#include "Scene.hpp"
#include "Sky.hpp"
#include "ShaderResource.hpp"


shared_ptr<ResHandle> GLSkyNode::m_pSkyProgram(nullptr);


////////////////////////////////////////////////////
// SkyNode Implementation
////////////////////////////////////////////////////

//
// SkyNode::SkyNode						- Chapter 16, page 554
//
SkyNode::SkyNode(shared_ptr<ResHandle> cubemap)
	: SceneNode(INVALID_ACTOR_ID, WeakBaseRenderComponentPtr(), RenderPass_Sky, &glm::mat4())
	, m_bActive(true), m_Cubemap(cubemap)
{
}

//
// SkyNode::VPreRender				- Chapter 14, page 502
//
HRESULT SkyNode::VPreRender(Scene *pScene)
{
	glm::vec3 cameraPos = ::GetPosition(m_camera->VGet()->ToWorld());
	glm::mat4 mat = m_Props.ToWorld();
	::SetPosition(mat,cameraPos);
	VSetTransform(&mat);

	return SceneNode::VPreRender(pScene);
}





//
// GLSkyNode::GLSkyNode					- Chapter 16, page 555
//
GLSkyNode::GLSkyNode(shared_ptr<ResHandle> cubemap)
	: SkyNode(cubemap), m_pVertexArray(GL_TRIANGLES, GL_STATIC_DRAW)
{
	if (!m_pSkyProgram)
	{
		Resource res("Shaders\\Sky.prog");
		m_pSkyProgram = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&res);
	}

	SetShader(m_pSkyProgram);


	mPositionLoc = mUVLoc = 0;
	GLUFVariableLocMap attribLocations = GLUFSHADERMANAGER.GetShaderAttribLocations(static_pointer_cast<GLProgramResourceExtraData>(GetShader()->GetExtra())->GetProgram());

	GLUFVariableLocMap::iterator it;

	it = attribLocations.find("_position");
	if (it != attribLocations.end())
	{
		m_pVertexArray.AddVertexAttrib(GLUFVertAttrib(it->second, 4, 3, GL_FLOAT));
		mPositionLoc = it->second;
	}

}

//
// GLSkyNode::~GLSkyNode					- Chapter 16, page 555
//
GLSkyNode::~GLSkyNode()
{
}


//
// GLSkyNode::VOnRestore						- Chapter 16, page 556
//


HRESULT GLSkyNode::VOnRestore(Scene *pScene)
{
	/*HRESULT hr;

	V_RETURN(SceneNode::VOnRestore(pScene));

	m_camera = pScene->GetCamera();

	//TODO: onRestore for shader classes
	/*V_RETURN(m_pShaderProgram.OnRestore(pScene));
	V_RETURN(m_PixelShader.OnRestore(pScene));

	m_numVerts = 20;

	// Fill the vertex buffer. We are setting the tu and tv texture
	// coordinates, which range from 0.0 to 1.0
	SkyVertices data;
	data.mPositions.resize(m_numVerts);

	//LOG_ASSERT(data.mPositions.size() != m_numVerts && "Out of memory in GLSkyNode::VOnRestore()");
	if (data.mPositions.size() != (size_t)m_numVerts)
		return E_FAIL;

	// Loop through the grid squares and calc the values
	// of each index. Each grid square has two triangles:
	//
	//		A - B
	//		| / |
	//		C - D

	Color skyVertColor = g_White;
	float dim = 50.0f;


	data.mPositions.resize(4); data.mUVCoords.resize(4);
	data.mPositions[0] = glm::vec3(dim, dim, dim);   data.mUVCoords[0] = glm::vec2(1.0f, 0.0f);
	data.mPositions[1] = glm::vec3(-dim, dim, dim);  data.mUVCoords[1] = glm::vec2(0.0f, 0.0f);
	data.mPositions[2] = glm::vec3(dim, -dim, dim);  data.mUVCoords[2] = glm::vec2(1.0f, 1.0f);
	data.mPositions[3] = glm::vec3(-dim, -dim, dim); data.mUVCoords[3] = glm::vec2(0.0f, 1.0f);

	glm::vec3 triangle[3];
	triangle[0] = glm::vec3(0.f, 0.f, 0.f);
	triangle[1] = glm::vec3(5.f, 0.f, 0.f);
	triangle[2] = glm::vec3(5.f, 5.f, 0.f);

	glm::vec3 edge1 = triangle[1] - triangle[0];
	glm::vec3 edge2 = triangle[2] - triangle[0];

	glm::vec3 normal;
	normal = glm::cross(edge1,edge2);
	normal = glm::normalize(normal);

	glm::mat4 rotY;
	rotY = BuildRotationY(AR_PI / 2.0f);
	glm::mat4 rotX;
	rotX = BuildRotationX(-AR_PI / 2.0f);
	
	m_sides = 5;

	//glm::vec3 skyVerts[4] = {};
	data.mPositions.resize(m_sides * 4);
	data.mUVCoords.resize(m_sides * 4);
	for (DWORD side = 0; side < m_sides; side++)
	{
		for (DWORD v = 0; v < 4; v++)
		{
			DWORD sv = (side * 4) + v;

			glm::vec3 temp;
			if (side < m_sides - 1)
			{
				temp = Xform(rotY,data.mPositions[sv]);
			}
			else
			{
				DWORD ssv = (side * 4);
				data.mUVCoords[ssv]     = glm::vec2(1.0f, 1.0f);
				data.mUVCoords[ssv + 1] = glm::vec2(1.0f, 0.0f);
				data.mUVCoords[ssv + 2] = glm::vec2(0.0f, 1.0f);
				data.mUVCoords[ssv + 3] = glm::vec2(0.0f, 0.0f);

				temp = Xform(rotX, data.mPositions[sv]);
			}
			data.mPositions[sv] = temp;
		}
		//memcpy(&pVertices[side * 4], skyVerts, sizeof(skyVerts));
	}

	/*
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

	GLuint *pIndices = QSE_NEW GLuint[m_sides * 2 * 3];

	GLuint *current = pIndices;
	for (DWORD i = 0; i<m_sides; i++)
	{
		// Triangle #1  ACB
		*(current) = GLuint(i * 4);
		*(current + 1) = GLuint(i * 4 + 2);
		*(current + 2) = GLuint(i * 4 + 1);

		// Triangle #2  BCD
		*(current + 3) = GLuint(i * 4 + 1);
		*(current + 4) = GLuint(i * 4 + 2);
		*(current + 5) = GLuint(i * 4 + 3);
		current += 6;
	}

	data.mIndices = GLUFArrToVec(pIndices, m_sides * 2 * 3);
	
	//now the indicies
	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, m_sides * 2 * 3 * sizeof(GLfloat), m_IndexBufferData.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	SAFE_DELETE_ARRAY(pIndices);

	//buffer the data
	m_pVertexArray.BufferData(mPositionLoc, data.mPositions.size(), &data.mPositions[0]);
	m_pVertexArray.BufferData(mUVLoc, data.mUVCoords.size(), &data.mPositions[0]);
	m_pVertexArray.BufferIndices(pIndices, m_sides * 2 * 3);
	*/

	
	Vec3Array verts;
	float val = 1.0f;


	float depth = val;
	m_sides = 6;

	//float N = 1.0f / sqrt(3.0f);

	//north
	/*verts.push_back(glm::vec3(-val, -val, depth));
	verts.push_back(glm::vec3(val, -val, depth));
	verts.push_back(glm::vec3(val, val, depth));
	verts.push_back(glm::vec3(-val, val, depth));
	
	//uvws.push_back(glm::vec3(-N, -N, N));
	//uvws.push_back(glm::vec3(N, -N, N));
	//uvws.push_back(glm::vec3(N, N, N));
	//uvws.push_back(glm::vec3(-N, N, N));

	//south
	verts.push_back(glm::vec3(val, -val, -depth));
	verts.push_back(glm::vec3(-val, -val, -depth));
	verts.push_back(glm::vec3(-val, val, -depth));
	verts.push_back(glm::vec3(val, val, -depth));

	//uvws.push_back(glm::vec3(N, -N, -N));
	//uvws.push_back(glm::vec3(-N, -N, -N));
	//uvws.push_back(glm::vec3(-N, N, -N));
	//uvws.push_back(glm::vec3(N, N, -N));


	//east
	verts.push_back(glm::vec3(-val, -val, -depth));
	verts.push_back(glm::vec3(-val, -val, depth));
	verts.push_back(glm::vec3(-val, val, depth));
	verts.push_back(glm::vec3(-val, val, -depth));

	//uvws.push_back(glm::vec3(-N, -N, -N));
	//uvws.push_back(glm::vec3(-N, -N, N));
	//uvws.push_back(glm::vec3(-N, N, N));
	//uvws.push_back(glm::vec3(-N, N, -N));

	//west
	verts.push_back(glm::vec3(val, -val, depth));
	verts.push_back(glm::vec3(val, -val, -depth));
	verts.push_back(glm::vec3(val, val, -depth));
	verts.push_back(glm::vec3(val, val, depth));

	//uvws.push_back(glm::vec3(N, -N, N));
	//uvws.push_back(glm::vec3(N, -N, -N));
	//uvws.push_back(glm::vec3(N, N, -N));
	//uvws.push_back(glm::vec3(N, N, N));


	//top
	verts.push_back(glm::vec3(-val, val, depth));
	verts.push_back(glm::vec3(val, val, depth));
	verts.push_back(glm::vec3(val, val, -depth));
	verts.push_back(glm::vec3(-val, val, -depth));

	//uvws.push_back(glm::vec3(-N, N, N));
	//uvws.push_back(glm::vec3(N, N, N));
	//uvws.push_back(glm::vec3(N, N, -N));
	//uvws.push_back(glm::vec3(-N, N, -N));

	//bottom
	verts.push_back(glm::vec3(-val, -val, -depth));
	verts.push_back(glm::vec3(val, -val, -depth));
	verts.push_back(glm::vec3(val, -val, depth));
	verts.push_back(glm::vec3(-val, -val, depth));*/


	//new plan, just 8 vertices

	//back
	verts.push_back(glm::vec3(val, -val, -depth));
	verts.push_back(glm::vec3(-val, -val, -depth));
	verts.push_back(glm::vec3(-val, val, -depth));
	verts.push_back(glm::vec3(val, val, -depth));

	//front
	verts.push_back(glm::vec3(-val, -val, depth));
	verts.push_back(glm::vec3(val, -val, depth));
	verts.push_back(glm::vec3(val, val, depth));
	verts.push_back(glm::vec3(-val, val, depth));

	/*// Loop through the grid squares and calc the values
	// of each index. Each grid square has two triangles:
	//
	//		D - C
	//		| / |
	//		A - B


	//two triangles per face
	for (unsigned int i = 0; i < m_sides; ++i)
	{
		unsigned int base = i * 4;
		//triangle ABC
		mTriangles.push_back(glm::u32vec3(base, base + 1, base + 2));

		//triangle CDA
		mTriangles.push_back(glm::u32vec3(base + 2, base + 3, base));
	}*/

	std::vector<glm::u32vec3> mTriangles;

	//north
	mTriangles.push_back(glm::u32vec3(4, 5, 6));
	mTriangles.push_back(glm::u32vec3(6, 7, 4));

	//south
	mTriangles.push_back(glm::u32vec3(0, 1, 2));
	mTriangles.push_back(glm::u32vec3(2, 3, 0));

	//east
	mTriangles.push_back(glm::u32vec3(1, 4, 7));
	mTriangles.push_back(glm::u32vec3(7, 2, 1));

	//west
	mTriangles.push_back(glm::u32vec3(5, 0, 3));
	mTriangles.push_back(glm::u32vec3(3, 6, 5));

	//top
	mTriangles.push_back(glm::u32vec3(7, 6, 3));
	mTriangles.push_back(glm::u32vec3(3, 2, 7));

	//bottom
	mTriangles.push_back(glm::u32vec3(1, 0, 5));
	mTriangles.push_back(glm::u32vec3(5, 4, 1));

	m_pVertexArray.BufferData(mPositionLoc, verts.size(), &verts[0][0]);
	m_pVertexArray.BufferIndices(mTriangles);


	return S_OK;
}


HRESULT GLSkyNode::VPreRender(Scene* pScene)
{
	//the position should always be 0, so offset by the opposit of the camera position
	//doing this in the prerender stage make sure the position is correct BEFORE applying the transforms
	this->SetPosition(pScene->GetCamera()->GetPosition());
	pScene->PushAndSetMatrix(this->GetToWorld());

	GLUFProgramPtr prog = static_pointer_cast<GLProgramResourceExtraData>(GetShader()->GetExtra())->GetProgram();
	//if (m_pShader)
	//{
	//this is always safe to do
	GLUFSHADERMANAGER.UseProgram(prog);
	//}


	//apply the uniforms
	glm::mat4 model = pScene->GetTopMatrix() * glm::scale(glm::mat4(), glm::vec3(40, 40, 40));
	glm::mat4 view = pScene->GetCamera()->GetView();
	glm::mat4 proj = pScene->GetCamera()->GetProjection();

	glm::mat4 model_view_proj = proj * view * model;


	GLUFVariableLocMap uniformLocations = GLUFSHADERMANAGER.GetShaderUniformLocations(prog);
	
	GLUFVariableLocMap::iterator it = uniformLocations.find("_mvp");
	if (it != uniformLocations.end())
		glUniformMatrix4fv(it->second, 1, GL_FALSE, &model_view_proj[0][0]);

	shared_ptr<GLTextureResourceExtraData> cubMap = static_pointer_cast<GLTextureResourceExtraData>(m_Cubemap->GetExtra());

	it = uniformLocations.find("m_tex0");
	if (it != uniformLocations.end())//make sure the there is a sampler, and we have a texture
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubMap->GetTexture());
		glUniform1i(it->second, 0);
	}

	// disable depth test
	//glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	return S_OK;
}

//
// GLSkyNode::VRender						- Chapter 16, page 559
//
HRESULT GLSkyNode::VRender(Scene *pScene)
{

	//TODO:
	//V_RETURN(m_VertexShader.SetupRender(pScene, this));
	//V_RETURN(m_PixelShader.SetupRender(pScene, this));

	// Set vertex buffer
	/*UINT stride = sizeof(D3D11Vertex_UnlitTextured);
	UINT offset = 0;
	DXUTGetD3D11DeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set index buffer
	DXUTGetD3D11DeviceContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	DXUTGetD3D11DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	*/

	
	//for (DWORD side = 0; side < m_sides; side++)
	//{
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
		
		//TODO: setup with cube map
		//bind the texture, but the uniform for the texture is already set up
		m_pVertexArray.Draw();
		/*
		std::string name = GetTextureName(side);
		m_PixelShader.SetTexture(name.c_str());

		DXUTGetD3D11DeviceContext()->DrawIndexed(6, side * 6, 0);*/
	//}
	return S_OK;
}

HRESULT GLSkyNode::VPostRender(Scene* pScene)
{
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	return SceneNode::VPostRender(pScene);
}


/*std::string SkyNode::GetTextureName(const int side)
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
*/


