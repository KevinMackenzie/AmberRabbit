#include "../Stdafx.hpp"
#include "Mesh.hpp"
#include <iterator>

#include <assimp/Importer.hpp>
#include <assimp/types.h>
#include <assimp/mesh.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Material.hpp"

#include <IL/ilut.h>

//useful for converting aiColor4d to glm::vec4
Color AiColorToColor(aiColor4D color)
{
	return Color(color.r, color.g, color.b, color.a);
}

glm::vec3 Aivec3Tovec3(aiVector3D vec)
{
	return glm::vec3(vec.x, vec.y, vec.z);
}

glm::vec2 Aivec3ToVec2(aiVector3D vec)
{
	return glm::vec2(vec.x, vec.y);
}

template <class M>
std::vector<M> ArrToVec(M* arr, unsigned long size)
{
	return std::vector<M>(std::begin(arr), std::end(arr));
}

unsigned int ObjMeshResourceLoader::VGetLoadedResourceSize(char* rawBuffer, unsigned int rawSize)
{
	return rawSize;
}

bool ObjMeshResourceLoader::VLoadResource(char* rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle)
{
	//since we are only loading one object, only load the first of all
	shared_ptr<GLObjMeshResourceExtraData> extraData = static_pointer_cast<GLObjMeshResourceExtraData>(handle->GetExtra());

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFileFromMemory(rawBuffer, rawSize, aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_SortByPType, "");
	
	//create the vertex array pointer (empty)
	extraData->mData = GLUFBUFFERMANAGER.CreateVertexArray();

	if (scene->HasMeshes() == true && scene->HasMaterials() == true)
	{
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
		*/

		


		//now load the material (singular)
		aiMaterial* pMat = scene->mMaterials[0];
		GLMaterial mat;

		//load the file name of the texture if it exists
		aiString texPath;
		if (AI_SUCCESS == pMat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath))
		{

			//load the texture from the file
			Resource resource(texPath.C_Str());
			shared_ptr<ResHandle> pResourceHandle = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&resource);
			mat.SetTexture(static_pointer_cast<GLTextureResourceExtraData>(pResourceHandle->GetExtra())->GetTexture());
		}

		aiColor4D tempColor;

		//DIFFUSE
		if (AI_SUCCESS == aiGetMaterialColor(pMat, AI_MATKEY_COLOR_DIFFUSE, &tempColor))
			mat.SetDiffuse(AiColorToColor(tempColor));

		//AMBIENT
		if (AI_SUCCESS == aiGetMaterialColor(pMat, AI_MATKEY_COLOR_AMBIENT, &tempColor))
			mat.SetAmbient(AiColorToColor(tempColor));

		//EMMISIVE
		if (AI_SUCCESS == aiGetMaterialColor(pMat, AI_MATKEY_COLOR_EMISSIVE, &tempColor))
			mat.SetEmissive(AiColorToColor(tempColor));

		//POWER
		float power;
		unsigned int max;
		aiGetMaterialFloatArray(pMat, AI_MATKEY_SHININESS, &power, &max);

		//SPECULAR
		if (AI_SUCCESS == aiGetMaterialColor(pMat, AI_MATKEY_COLOR_SPECULAR, &tempColor))
			mat.SetSpecular(AiColorToColor(tempColor), power);

		//now load the material into the uniform buffer
		GLUFBUFFERMANAGER.ModifyUniformMaterial(extraData->mUniforms, mat);

		delete scene;
		return true;
	}
	else
	{
		delete scene;
		LOG_ERROR("Mesh File Has no Meshes");
		return false;
	}
}

