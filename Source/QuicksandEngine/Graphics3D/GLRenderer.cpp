#include "../Stdafx.hpp"
#include "GLRenderer.hpp"

#include "GLRenderer.hpp"
#include "Lights.hpp"
#include "../UserInterface/HumanView.hpp"


GLUFDialogResourceManager *GLRenderer_Base::g_pDialogResourceManager;
GLUFTextHelper *GLRenderer_Base::g_pTextHelper;

shared_ptr<ResHandle> GLLineDrawer::m_LineDrawerShader(nullptr);

//
// class D3DRendererAlphaPass9				- not described in the book, see class GLRenderPass 
//
class GLRendererAlphaPass : public IRenderState
{
protected:
	GLUFMatrixStack m_oldWorld;
	DWORD m_oldZWriteEnable;

public:
	GLRendererAlphaPass();
	~GLRendererAlphaPass();
	std::string VToString() { return "GLRendererAlphaPass"; }
};

GLRendererAlphaPass::GLRendererAlphaPass()
{
	m_oldWorld = *g_pMatrixStack;
	//DXUTGetD3D9Device()->GetTransform(D3DTS_WORLD, &m_oldWorld);
	//DXUTGetD3D9Device()->GetRenderState(D3DRS_ZWRITEENABLE, &m_oldZWriteEnable);
	//DXUTGetD3D9Device()->SetRenderState(D3DRS_ZWRITEENABLE, false);
	
	//TODO: is this correct?
	//glDisable(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//DXUTGetD3D9Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	//DXUTGetD3D9Device()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//DXUTGetD3D9Device()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

GLRendererAlphaPass::~GLRendererAlphaPass()
{
	/*DXUTGetD3D9Device()->SetRenderState(D3DRS_COLORVERTEX, false);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	DXUTGetD3D9Device()->SetRenderState(D3DRS_ZWRITEENABLE, m_oldZWriteEnable);
	DXUTGetD3D9Device()->SetTransform(D3DTS_WORLD, &m_oldWorld);*/


	*g_pMatrixStack = m_oldWorld;
	//glEnable(GL_DEPTH_BUFFER_BIT);
}


class GLRenderPass : public IRenderState
{
protected:
	//ID3D11BlendState* m_pOldBlendState;
	FLOAT m_OldBlendFactor[4];
	UINT m_OldSampleMask;

	//ID3D11BlendState* m_pCurrentBlendState;

public:
	GLRenderPass();
	~GLRenderPass();
	std::string VToString() { return "GLRenderPass"; }
};

//
// GLRenderPass::GLRenderPass			- Chapter 16, page 544
//
GLRenderPass::GLRenderPass()
{
	
	//DXUTGetD3D11DeviceContext()->OMGetBlendState(&m_pOldBlendState, m_OldBlendFactor, &m_OldSampleMask);
	//m_pCurrentBlendState = NULL;

	//D3D11_BLEND_DESC BlendState;
	//ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	

	/*BlendState.AlphaToCoverageEnable = false;
	BlendState.IndependentBlendEnable = false;
	BlendState.RenderTarget[0].BlendEnable = TRUE;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;*/

	//DXUTGetD3D11Device()->CreateBlendState(&BlendState, &m_pCurrentBlendState);
	//DXUTGetD3D11DeviceContext()->OMSetBlendState(m_pCurrentBlendState, 0, 0xffffffff);
}

//
// GLRenderPass::~GLRenderPass			- Chapter 16, page 544
//
GLRenderPass::~GLRenderPass()
{
	//DXUTGetD3D11DeviceContext()->OMSetBlendState(m_pOldBlendState, m_OldBlendFactor, m_OldSampleMask);
	/*SAFE_RELEASE(m_pCurrentBlendState);
	SAFE_RELEASE(m_pOldBlendState);*/
}

/*
//
// class GLRenderSkyBoxPass			(was 9)		- not described in the book - see GLRenderSkyBox 
//
class GLRenderSkyBoxPass : public IRenderState
{
protected:

public:
	GLRenderSkyBoxPass();
	~GLRenderSkyBoxPass();
	std::string VToString() { return "GLRenderSkyBoxPass"; }
};

GLRenderSkyBoxPass::GLRenderSkyBoxPass()
{
	//DXUTGetD3D9Device()->GetRenderState(D3DRS_ZWRITEENABLE, &m_oldZWriteEnable);
	//DXUTGetD3D9Device()->SetRenderState(D3DRS_ZWRITEENABLE, false);
	//DXUTGetD3D9Device()->GetRenderState(D3DRS_LIGHTING, &m_oldLightMode);
	//DXUTGetD3D9Device()->SetRenderState(D3DRS_LIGHTING, FALSE);
	//DXUTGetD3D9Device()->GetRenderState(D3DRS_CULLMODE, &m_oldCullMode);
	//DXUTGetD3D9Device()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	glDisable(GL_DEPTH_TEST);
}


GLRenderSkyBoxPass::~GLRenderSkyBoxPass()
{
	//DXUTGetD3D9Device()->SetRenderState(D3DRS_LIGHTING, m_oldLightMode);
	//DXUTGetD3D9Device()->SetRenderState(D3DRS_CULLMODE, m_oldCullMode);
	//DXUTGetD3D9Device()->SetRenderState(D3DRS_ZWRITEENABLE, m_oldZWriteEnable);

	glEnable(GL_DEPTH_TEST);
}*/





//
// class GLRenderSkyBox							- Chapter 16, page 548
//
class GLRenderSkyBox : public IRenderState
{
protected:
	//ID3D11DepthStencilState* m_pOldDepthStencilState;
	//ID3D11DepthStencilState* m_pSkyboxDepthStencilState;

public:
	GLRenderSkyBox();
	~GLRenderSkyBox();
	std::string VToString() { return "GLRenderSkyBox"; }
};

//
// GLRenderSkyBox::GLRenderSkyBox()		- Chapter 16, page 548
//
GLRenderSkyBox::GLRenderSkyBox()
{
	// Depth stencil state
	/*D3D11_DEPTH_STENCIL_DESC DSDesc;
	ZeroMemory(&DSDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	DSDesc.DepthEnable = TRUE;
	DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DSDesc.DepthFunc = D3D11_COMPARISON_LESS;
	DSDesc.StencilEnable = FALSE;
	DXUTGetD3D11Device()->CreateDepthStencilState(&DSDesc, &m_pSkyboxDepthStencilState);
	DXUT_SetDebugName(m_pSkyboxDepthStencilState, "SkyboxDepthStencil");*/

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	/*
	UINT StencilRef;
	DXUTGetD3D11DeviceContext()->OMGetDepthStencilState(&m_pOldDepthStencilState, &StencilRef);
	DXUTGetD3D11DeviceContext()->OMSetDepthStencilState(m_pSkyboxDepthStencilState, 0);*/
}

//
// GLRenderSkyBox::~GLRenderSkyBox()		- Chapter 16, page 548
//
GLRenderSkyBox::~GLRenderSkyBox()
{
	//DXUTGetD3D11DeviceContext()->OMSetDepthStencilState(m_pOldDepthStencilState, 0);
	//SAFE_RELEASE(m_pOldDepthStencilState);
	//SAFE_RELEASE(m_pSkyboxDepthStencilState);

}


GLLineDrawer::GLLineDrawer() : m_pVertexBuffer(GL_LINES, GL_STREAM_DRAW, false) 
{ 
	if (m_LineDrawerShader == nullptr)
	{
		Resource res("Shaders/LineDrawer.prog");
		m_LineDrawerShader = QuicksandEngine::g_pApp->m_ResCache->GetHandle(&res);
	}

	GLUFVariableLocMap attribLocations = GLUFSHADERMANAGER.GetShaderAttribLocations(static_pointer_cast<GLProgramResourceExtraData>(m_LineDrawerShader->GetExtra())->GetProgram());
	GLUFVariableLocMap::iterator it;
	
	it = attribLocations.find("_position");
	if (it != attribLocations.end())
	{
		m_pVertexBuffer.AddVertexAttrib(GLUFVertAttrib(it->second, 4, 3, GL_FLOAT));
		m_PositionLocation = it->second;
	}
	it = attribLocations.find("_color0");
	if (it != attribLocations.end())
	{
		m_pVertexBuffer.AddVertexAttrib(GLUFVertAttrib(it->second, 4, 4, GL_FLOAT));
		m_ColorLocation = it->second;
	}
}


HRESULT GLLineDrawer::OnRestore()
{
	/*D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(D3D11Vertex_PositionColored) * 2;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &m_Verts;
	HRESULT hr;

	V_RETURN(DXUTGetD3D11Device()->CreateBuffer(&bd, &InitData, &m_pVertexBuffer));

	shared_ptr<Scene> pScene = QuicksandEngine::g_pApp->GetHumanView()->m_pScene;

	V_RETURN(m_LineDrawerShader.OnRestore(&(*pScene)));*/

	return S_OK;
}



void GLLineDrawer::DrawLine(const glm::vec3& from, const glm::vec3& to, const Color& color)
{
	//HRESULT hr;

	shared_ptr<Scene> pScene = QuicksandEngine::g_pApp->GetHumanView()->m_pScene;
	shared_ptr<IRenderer> pRenderer = pScene->GetRenderer();


	//if (FAILED(m_LineDrawerShader.SetupRender(&(*pScene))))
	//	return;

	//m_LineDrawerShader.SetDiffuse("art\\grid.dds", color);

	// Set vertex buffer
	/*UINT stride = sizeof(glm::vec3);
	UINT offset = 0;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	V(DXUTGetD3D11DeviceContext()->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));

	glm::vec3 * pVerts = (glm::vec3*)MappedResource.pData;
	pVerts[0] = from;
	pVerts[1] = to;
	DXUTGetD3D11DeviceContext()->Unmap(m_pVertexBuffer, 0);*/

	glm::vec3 two[] = { from, to };
	m_pVertexBuffer.BufferData(m_PositionLocation, 2, two);

	//DXUTGetD3D11DeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	GLUFSHADERMANAGER.UseProgram(static_pointer_cast<GLProgramResourceExtraData>(m_LineDrawerShader->GetExtra())->GetProgram());

	// Set primitive topology
	//DXUTGetD3D11DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	m_pVertexBuffer.Draw();

	//DXUTGetD3D11DeviceContext()->Draw(2, 0);
}



////////////////////////////////////////////////////
// GLRenderer Implementation
//
//    Not described in the book - but it abstracts 
//    some of the calls to get the game engine to run
//    under DX11.
////////////////////////////////////////////////////

HRESULT GLRenderer::VOnRestore()
{
	HRESULT hr;
	V_RETURN(GLRenderer_Base::VOnRestore());
	//SAFE_DELETE(GLRenderer_Base::g_pTextHelper);
	//GLRenderer_Base::g_pTextHelper = QSE_NEW CGLUTTextHelper(&g_pDialogResourceManager, 15);

	return S_OK;
}


bool GLRenderer::VPreRender()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, QuicksandEngine::g_pApp->GetScreenSize().x, QuicksandEngine::g_pApp->GetScreenSize().y);

	Color4f col = GLUFColorToFloat(m_BackgroundColor);
	glClearBufferfv(GL_COLOR, 0, &col[0]);
	
	return true;
}

bool GLRenderer::VPostRender(void)
{
	return true;
}

void GLRenderer::VDrawLine(const glm::vec3& from, const glm::vec3& to, const Color& color)
{
	if (!m_pLineDrawer)
	{
		m_pLineDrawer = QSE_NEW GLLineDrawer();
		m_pLineDrawer->OnRestore();
	}

	m_pLineDrawer->DrawLine(from, to, color);
}




shared_ptr<IRenderState> GLRenderer::VPrepareAlphaPass()
{
	return shared_ptr<IRenderState>(QSE_NEW GLRenderPass());
}


shared_ptr<IRenderState> GLRenderer::VPrepareSkyBoxPass()
{
	return shared_ptr<IRenderState>(QSE_NEW GLRenderSkyBox());
}


