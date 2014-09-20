#include "../Stdafx.hpp"
#include "Mesh.hpp"
#include <iterator>

#include "Raycast.hpp"
#include <assimp/Importer.hpp>
#include <assimp/types.h>
#include <assimp/mesh.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Material.hpp"

//#include <IL/ilut.h>

unsigned int GLObjMeshResourceLoader::VGetLoadedResourceSize(char* rawBuffer, unsigned int rawSize)
{
	return 0;
}

bool GLObjMeshResourceLoader::VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	//since we are only loading one object, only load the first of all
	//shared_ptr<GLObjMeshResourceExtraData> extraData = static_pointer_cast<GLObjMeshResourceExtraData>(handle->GetExtra());
	auto extraData = shared_ptr<GLObjMeshResourceExtraData>(QSE_NEW GLObjMeshResourceExtraData());

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFileFromMemory(rawBuffer, rawSize, aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_SortByPType, "");
	
	//create the vertex array pointer (empty)
	//extraData->mData = GLUFBUFFERMANAGER.CreateVertexArray();

	if (scene->HasMeshes() == true && scene->HasMaterials() == true)
	{
		

		extraData->m_pArray = LoadVertexArrayFromScene(scene);

		//now load the material (singular)
		aiMaterial* pMat = scene->mMaterials[0];
		extraData->m_pMaterial = new GLMaterial();

		//load the file name of the texture if it exists
		aiString texPath;
		if (AI_SUCCESS == pMat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath))
		{

			//load the texture from the file
			Resource resource(texPath.C_Str());
			shared_ptr<ResHandle> pResourceHandle = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&resource);
			extraData->m_pMaterial->SetTexture(pResourceHandle);
		}

		aiColor4D tempColor;

		//DIFFUSE
		if (AI_SUCCESS == aiGetMaterialColor(pMat, AI_MATKEY_COLOR_DIFFUSE, &tempColor))
			extraData->m_pMaterial->SetDiffuse(AssimpToGlm4_3u8(tempColor));

		//AMBIENT
		if (AI_SUCCESS == aiGetMaterialColor(pMat, AI_MATKEY_COLOR_AMBIENT, &tempColor))
			extraData->m_pMaterial->SetAmbient(AssimpToGlm4_3u8(tempColor));

		//EMMISIVE
		if (AI_SUCCESS == aiGetMaterialColor(pMat, AI_MATKEY_COLOR_EMISSIVE, &tempColor))
			extraData->m_pMaterial->SetEmissive(AssimpToGlm4_3u8(tempColor));

		//POWER
		float power;
		unsigned int max;
		aiGetMaterialFloatArray(pMat, AI_MATKEY_SHININESS, &power, &max);

		//SPECULAR
		if (AI_SUCCESS == aiGetMaterialColor(pMat, AI_MATKEY_COLOR_SPECULAR, &tempColor))
			extraData->m_pMaterial->SetSpecular(AssimpToGlm4_3u8(tempColor), power);


		/*
		if (scene->mNumMeshes > 1)
		{
			LOG_WARNING("Loaded File with more than one Mesh, Skipping all other meshes");
		}
		if (scene->mNumMaterials > 1)
		{
			LOG_WARNING("Loaded File with more than one GLMaterial, Skipping all other materials");
		}

		aiMesh* pMesh = scene->mMeshes[0];
		if (!pMesh->HasPositions() || !pMesh->HasNormals() || !pMesh->HasTextureCoords(0) || !pMesh->HasFaces())
		{
			LOG_ERROR("Loading mesh without required components");
		}

		GLushort *faceArray;
		faceArray = (GLushort *)malloc(sizeof(GLushort) * pMesh->mNumFaces * 3);
		unsigned int faceIndex = 0;

		for (GLushort t = 0; t < pMesh->mNumFaces; ++t) {
			const aiFace* face = &pMesh->mFaces[t];

			memcpy(&faceArray[faceIndex], face->mIndices, 3 * sizeof(GLushort));
			faceIndex += 3;
		}

		GLUFVAOData dat = GLUFBUFFERMANAGER.MapVertexArray(extraData->mData);

		//get the data into the memory
		for (unsigned int i = 0; i < pMesh->mNumVertices; ++i)
		{
			dat.mPositions->push_back(Aivec3Tovec3(pMesh->mVertices[i]));
			dat.mNormals->push_back(Aivec3Tovec3(pMesh->mNormals[i]));
			dat.mUVCoords->push_back(Aivec3ToVec2(pMesh->mTextureCoords[0][i]));
		}

		*dat.mIndicies = ArrToVec<GLushort>(faceArray, pMesh->mNumFaces*3);
		
		GLUFBUFFERMANAGER.UnMapVertexArray(extraData->mData);

		//now put it in openGL,  (its already there this is gone)
		/*
		glGenVertexArrays(1, &meshHandles->mVAO);
		glBindVertexArray(meshHandles->mVAO);

		//load the indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshHandles->mIndexArray);
		glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLint) * 3 * pMesh->mNumFaces, faceArray, GL_STATIC_DRAW);


		//load the vertices
		glBindBuffer(GL_ARRAY_BUFFER, meshHandles->mVertexArray);
		glBufferStorage(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * pMesh->mNumVertices, pMesh->mVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(meshHandles->mVertexArray);
		glVertexAttribPointer(meshHandles->mVertexArray, 3, GL_FLOAT, 0, 0, 0);


		//load the Normals
		glBindBuffer(GL_ARRAY_BUFFER, meshHandles->mNormalArray);
		glBufferStorage(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * pMesh->mNumVertices, pMesh->mNormals, GL_STATIC_DRAW);
		glEnableVertexAttribArray(meshHandles->mNormalArray);
		glVertexAttribPointer(meshHandles->mNormalArray, 3, GL_FLOAT, 0, 0, 0);


		//load the texture coordinates
		glBindBuffer(GL_ARRAY_BUFFER, meshHandles->mUVArray);
		glBufferStorage(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * pMesh->mNumVertices, pMesh->mTextureCoords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(meshHandles->mUVArray);
		glVertexAttribPointer(meshHandles->mUVArray, 2, GL_FLOAT, 0, 0, 0);


		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		

		


		

		//now load the material into the uniform buffer
		GLUFBUFFERMANAGER.ModifyUniformMaterial(extraData->mUniforms, mat);
		*/
		//delete scene;
		handle->SetExtra(extraData);
		return true;
	}
	else
	{
		//delete scene;
		LOG_ERROR("Mesh File Has no Meshes");
		return false;
	}
}

GLMeshNode::GLMeshNode(const ActorId actorId,
	WeakBaseRenderComponentPtr renderComponent,
	std::string FileName,
	RenderPass renderPass,
	const glm::mat4 *t) : SceneNode(actorId, renderComponent, renderPass, t)
{
	Resource res(FileName);
	m_Data = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&res);

	GLMaterial* mat = static_pointer_cast<GLObjMeshResourceExtraData>(m_Data->GetExtra())->GetMaterial();
	if (mat)
		SetMaterial(*mat);

	Resource shaderResource("Shaders\\BasicLighting.prog");
	m_pBasicShading = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&res);
	SetShader(m_pBasicShading);
}

HRESULT GLMeshNode::VOnRestore(Scene* pScene)
{
	//nothing
	return S_OK;
}

HRESULT GLMeshNode::VPreRender(Scene* pScene)
{
	HRESULT hr = S_OK;
	V_RETURN(SceneNode::VPreRender(pScene));


	//GLUFVariableLocMap uniformLocations = GLUFSHADERMANAGER.GetShaderUniformLocations(static_pointer_cast<GLProgramResourceExtraData>(GetShader()->GetExtra())->GetProgram());

	//auto it = uniformLocations.find("_light_position");
	//if (it != uniformLocations.end())
	//	glUniform3f(it->second, 4.0f, 4.0f, 4.0f);//TODO: set this up with an actual light node


	//setup with the lighting uniform

	return S_OK;
}

HRESULT GLMeshNode::VRender(Scene* pScene)
{
	//prerender does all of the work;
	GLUFVertexArray* data = static_pointer_cast<GLObjMeshResourceExtraData>(m_Data->GetExtra())->GetVertexArray();

	if (data)
		data->Draw();

	return S_OK;
}

HRESULT GLMeshNode::VPick(Scene *pScene, RayCast *pRayCast)
{
	GLUFVertexArray* data = static_pointer_cast<GLObjMeshResourceExtraData>(m_Data->GetExtra())->GetVertexArray();

	if (data)
	{
		GLUFMeshBarebones mesh = data->GetBarebonesMesh();
		return pRayCast->Pick(pScene, this->m_Props.ActorId(), &mesh);
	}

	return E_FAIL;
}

float GLMeshNode::CalcBoundingSphere()
{
	GLUFVertexArray* data = static_pointer_cast<GLObjMeshResourceExtraData>(m_Data->GetExtra())->GetVertexArray();
	if (!data)
		return 0;

	float oldMax = 0.0f;
	float thisLen = 0.0f;

	GLUFMeshBarebones mesh = data->GetBarebonesMesh();
	
	//NOTE: there are better ways to get a more accurate sphere, but that requires a different point of origin
	//this is very simple, it just figures out which one the greatest from this object's origin
	for (auto it : mesh.mVertices)
	{
		thisLen = glm::length(it);
		if (thisLen > oldMax)
			oldMax = thisLen;
	}
	

	return oldMax;
}


shared_ptr<IResourceLoader> CreateObjMeshResourceLoader()
{
	return shared_ptr<IResourceLoader>(QSE_NEW GLObjMeshResourceLoader());
}