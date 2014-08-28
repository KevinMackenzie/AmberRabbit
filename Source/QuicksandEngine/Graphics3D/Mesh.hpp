#ifndef QSE_MESH_HPP
#define QSE_MESH_HPP


#include "Geometry.hpp"
#include "../ResourceCache/ResCache.hpp"
#include "SceneNode.hpp"

/*
struct GLMeshData
{
	Vec3Array mVertexArray;
	Vec3Array mNormalArray;
	Vec2Array mUVArray;
	IndexArray mIndexArray;
};

struct GLMeshHandles
{
	GLuint mVAO;
	GLuint mVertexArray;
	GLuint mNormalArray;
	GLuint mUVArray;
	GLuint mIndexArray;

	GLMaterial mMaterial;
	//todo: support bones and animation

	GLuint* operator*()
	{
		GLuint arr[4] = {mVertexArray, mNormalArray, mUVArray, mIndexArray};
		return arr;
	}

	~GLMeshHandles(){ glDeleteBuffers(4, **this); glDeleteVertexArrays(1, &mVAO); }
};*/

//NOTE: THIS IS DESIGNED TO ONLY LOAD ONE OBJECT, NOT a whole scene
//there will be a seperate loader for generating a whole tree MAYBE

class GLObjMeshResourceExtraData : public IResourceExtraData
{
	friend class GLObjMeshResourceLoader;

public:
	GLObjMeshResourceExtraData() : m_pArray(nullptr), m_pMaterial(nullptr){};
	virtual ~GLObjMeshResourceExtraData(){}
	virtual std::string VToString() { return "GLObjMeshResourceExtraData"; }

	GLMaterial* GetMaterial(){ return m_pMaterial; }
	GLUFVertexArray* GetVertexArray(){ return m_pArray; }

protected:

	GLUFVertexArray* m_pArray;

	GLMaterial *m_pMaterial;

	// m_pTextureResource;

	//GLUFVertexArrayPtr mData;
	//GLUFUniformBufferPtr mUniforms;
};

class GLObjMeshResourceLoader : public IResourceLoader
{
public:
	virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
	virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize);
	virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
	virtual std::string VGetPattern() { return "*.obj.model"; }
};

//////////////////////////////////////////////////////////////////////////////////

/*
//this recursively loads 
class GLobjSceneResourceExtraData : public IResourceExtraData
{
	friend class _objSceneResourceLoader;

public:
	GLobjSceneResourceExtraData(){};
	virtual ~GLobjSceneResourceExtraData(){}
	virtual std::string VToString() { return "GLobjSceneResourceExtraData"; }

	//store a list of meshExtra data
	GLObjMeshResourceExtraData m_Nodes;
};*/
/*
struct Vec3Array
{
	Vec3Array mVertexArray;
	IndexArray mIndexArray;

	Vec3Array(GLMeshData mesh) : mVertexArray(mesh.mVertexArray), mIndexArray(mesh.mIndexArray){}
};*/


class GLMeshNode : public SceneNode
{
public:
	GLMeshNode(const ActorId actorId,
		WeakBaseRenderComponentPtr renderComponent,
		std::string FileName,
		RenderPass renderPass,
		const glm::mat4 *t);

	virtual HRESULT VOnRestore(Scene *pScene);
	virtual HRESULT VOnLostDevice(Scene *pScene) { return S_OK; }
	virtual HRESULT VPreRender(Scene* pScene);
	virtual HRESULT VRender(Scene *pScene);
	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast);

protected:
	shared_ptr<ResHandle> m_Data;

	//GLUFProgramPtr m_ShadingProgram;

	//for a default, just have a single shader that does basic lighting
	shared_ptr<ResHandle> m_pBasicShading;

	float CalcBoundingSphere();
};

#endif