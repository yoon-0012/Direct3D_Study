//------------------------------------------------------- ----------------------
// File: Object.h
//-----------------------------------------------------------------------------

#pragma once

#include "Mesh.h"
#include "Camera.h"

#define DIR_FORWARD					0x01
#define DIR_BACKWARD				0x02
#define DIR_LEFT					0x04
#define DIR_RIGHT					0x08
#define DIR_UP						0x10
#define DIR_DOWN					0x20
#define DIR_RUN						0x40

class CShader;
class CStandardShader;
class CScene;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05

class CGameObject;


struct MATERIAL
{
	XMFLOAT4						m_xmf4Ambient;
	XMFLOAT4						m_xmf4Diffuse;
	XMFLOAT4						m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4						m_xmf4Emissive;
};

struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4						m_xmf4x4World;
	UINT							m_nMaterial;
};




struct CB_GRASSOBJECT_INFO//GrassObjectInfo
{
	UINT							m_nType{ 0x00 };
	XMFLOAT3						texMat{ XMFLOAT3(0.f, 0.f, 0.f) };
};

//============================================================

class CTexture
{
public:

	CTexture(int nTextureResources, UINT nResourceType, int nSamplers, int nRootParameters, int nRows = 1, int nCols = 1);

	virtual ~CTexture();

private:
	int								m_nReferences{};

	UINT							m_nTextureType;


	_TCHAR(*m_ppstrTextureNames)[64] {NULL};
	ID3D12Resource** m_ppd3dTextures{ NULL };
	ID3D12Resource** m_ppd3dTextureUploadBuffers;

	UINT* m_pnResourceTypes{ NULL };

	DXGI_FORMAT* m_pdxgiBufferFormats{ NULL };
	int* m_pnBufferElements{ NULL };

	int								m_nRootParameters{};
	int* m_pnRootParameterIndices{ NULL };
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSrvGpuDescriptorHandles{ NULL };

	int								m_nSamplers{};
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSamplerGpuDescriptorHandles{ NULL };

public:
	void AddRef() { m_nReferences++; }
	void Release() { if ( 0>= --m_nReferences ) delete this; }

	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	void LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex);
	void LoadBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT ndxgiFormat, UINT nIndex);
	ID3D12Resource* CreateTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nIndex, UINT nResourceType, UINT nWidth, UINT nHeight, UINT nElements, UINT nMipLevels, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue);

	int LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, CShader* pShader, UINT nIndex);

	void SetRootParameterIndex(int nIndex, UINT nRootParameterIndex);
	void SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle);

	int GetRootParameters() { return(m_nRootParameters); }
	int GetTextures() { return(m_nTextures); }
	_TCHAR* GetTextureName(int nIndex) { return(m_ppstrTextureNames[nIndex]); }
	ID3D12Resource* GetResource(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(int nIndex) { return(m_pd3dSrvGpuDescriptorHandles[nIndex]); }
	int GetRootParameter(int nIndex) { return(m_pnRootParameterIndices[nIndex]); }

	UINT GetTextureType() { return(m_nTextureType); }
	UINT GetTextureType(int nIndex) { return(m_pnResourceTypes[nIndex]); }
	DXGI_FORMAT GetBufferFormat(int nIndex) { return(m_pdxgiBufferFormats[nIndex]); }
	int GetBufferElements(int nIndex) { return(m_pnBufferElements[nIndex]); }

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(int nIndex);

	void ReleaseUploadBuffers();

	int 							m_nRows{ 1 };
	int 							m_nCols{ 1 };
	int 							m_nRow{};
	int 							m_nCol{};
	XMFLOAT4X4						m_xmf4x4Texture;
	void AnimateRowColumn(float fTime = 0.0f);


	XMFLOAT3 texMat = { 0.0f,0.0f,0.0f };
	bool							m_bActive{ false };
	int								m_nTextures{};
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

class CGameObject;

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

private:
	int								m_nReferences{};

public:
	void AddRef() { m_nReferences++; }
	void Release() { if ( 0 >=--m_nReferences ) delete this; }

public:
	CShader* m_pShader{ NULL };
	CTexture* m_pTexture{ NULL };

	MATERIAL* m_pReflection{ NULL };

	XMFLOAT4						m_xmf4AlbedoColor{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) };
	XMFLOAT4						m_xmf4EmissiveColor{ XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	XMFLOAT4						m_xmf4SpecularColor{ XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	XMFLOAT4						m_xmf4AmbientColor{ XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };

	void SetShader(CShader* pShader);
	void SetMaterialType(UINT nType) { m_nType |= nType; }
	void SetTexture(CTexture* pTexture);



	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

public:
	UINT							m_nType{ 0x00 };

	float							m_fGlossiness{ 0.0f };
	float							m_fSmoothness{ 0.0f };
	float							m_fSpecularHighlight{ 0.0f };
	float							m_fMetallic{ 0.0f };
	float							m_fGlossyReflection{ 0.0f };
	UINT							m_nReflection{};

};



class CGameObject
{
public:
	CGameObject();
	CGameObject(int nMeshes, int nMaterials);
	CGameObject(int nMeshes);
	virtual ~CGameObject();

public:
	void AddRef();
	void Release();


	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) {
		m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr;
	}

	void SetLookAt(XMFLOAT3& xmf3Target, XMFLOAT3& xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f));

	D3D12_GPU_DESCRIPTOR_HANDLE getGPUDescriptorHandle() { return m_d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }

	virtual void SetMesh(int nIndex, CMesh* pMesh);
	void SetShader(int nMaterial, CShader* pShader);
	void SetMaterial(int nMaterial, CMaterial* pMaterial);

	void SetChild(CGameObject* pChild);

	virtual void BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) { }

	virtual void PrepareAnimate() { }
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);

	virtual void OnPrepareRender() { }
	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, CScene* pScene) { }
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	void Render2(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, CMaterial* pMaterial);

	virtual void ReleaseUploadBuffers();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetScale(float x, float y, float z);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT4* pxmf4Quaternion);

	CGameObject* GetParent() { return(m_pParent); }
	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);
	CGameObject* FindFrame(char* pstrFrameName);

	int FindReplicatedTexture(_TCHAR* pstrTextureName, D3D12_GPU_DESCRIPTOR_HANDLE* pd3dSrvGpuDescriptorHandle);

	UINT GetMeshType(UINT nIndex) { return((m_ppMeshes[nIndex]) ? m_ppMeshes[nIndex]->GetType() : 0x00); }


	void LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, CShader* pShader);

	static CGameObject* LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObject* pParent, FILE* pInFile, CShader* pShader);
	static CGameObject* LoadGeometryFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, CShader* pShader);

	static void PrintFrameInfo(CGameObject* pGameObject, CGameObject* pParent);
	void	Boundingbox_Transform();

private:
	int								m_nReferences{};
public:
	CB_GAMEOBJECT_INFO* m_pcbMappedGameObject{ NULL };
	CB_GRASSOBJECT_INFO* grassObj{ NULL };
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;
	BoundingBox aabb;

	char							m_pstrFrameName[64];

	int								m_nMeshes{};
	CMesh** m_ppMeshes{ NULL };

	int								m_nMaterials{};
	CMaterial** m_ppMaterials{};

	bool shadow{};
	ID3D12Resource* m_pd3dcbGameObject{ NULL };


	XMFLOAT4X4						m_xmf4x4Transform;
	XMFLOAT4X4						m_xmf4x4World;

	CGameObject* m_pParent{ NULL };
	CGameObject* m_pChild{ NULL };
	CGameObject* m_pSibling{ NULL };

};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSuperCobraObject : public CGameObject
{
public:
	CSuperCobraObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CSuperCobraObject();

private:
	CGameObject* m_pMainRotorFrame{ NULL };
	CGameObject* m_pTailRotorFrame{ NULL };

public:
	virtual void PrepareAnimate();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
};

class CGunshipObject : public CGameObject
{
public:
	CGunshipObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CGunshipObject();

private:
	CGameObject* m_pMainRotorFrame{ NULL };
	CGameObject* m_pTailRotorFrame{ NULL };

public:
	virtual void PrepareAnimate();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
};

class CMi24Object : public CGameObject
{
public:
	CMi24Object(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CMi24Object();

private:
	CGameObject* m_pMainRotorFrame{ NULL };
	CGameObject* m_pTailRotorFrame{ NULL };

public:
	virtual void PrepareAnimate();
	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkyBox : public CGameObject
{
public:
	CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CSkyBox();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
};


class CGrassObject : public CGameObject
{
public:
	CGrassObject();
	virtual ~CGrassObject();

	virtual void Animate(float fTimeElapsed);

	float m_fRotationAngle{ 0.0f };
	float m_fRotationDelta{ 1.0f };


};
//===================================================

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~CHeightMapTerrain();

private:
	CHeightMapImage* m_pHeightMapImage;

	int							m_nWidth;
	int							m_nLength;

	XMFLOAT3					m_xmf3Scale;
public:
	int m_nObjects{ 1 };

public:
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMapImage->GetHeight(x, z, bReverseQuad) * m_xmf3Scale.y); } //World
	XMFLOAT3 GetNormal(float x, float z) { return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z))); }

	int GetRawImageWidth() { return(m_pHeightMapImage->GetRawImageWidth()); }
	int GetRawImageLength() { return(m_pHeightMapImage->GetRawImageLength()); }

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }
};

class CMultiSpriteObject : public CGameObject
{
public:
	CMultiSpriteObject();
	virtual ~CMultiSpriteObject();

	float m_fSpeed{ 0.1f };
	float m_fTime{ 0.0f };

	virtual void Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent = NULL);
};

class CRippleWater : public CGameObject
{
public:
	CRippleWater(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~CRippleWater();

private:
	int								m_nWidth;
	int								m_nLength;

	XMFLOAT3						m_xmf3Scale;


public:
	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	float GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float GetLength() { return(m_nLength * m_xmf3Scale.z); }

};

//=====================================================================
class CDynamicCubeMappingObject : public CGameObject
{
public:
	CDynamicCubeMappingObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, LONG nCubeMapSize, D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle, D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle, CShader* pShader);
	virtual ~CDynamicCubeMappingObject();

	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, CScene* pScene);

	CCamera* m_ppCameras[6];

	D3D12_CPU_DESCRIPTOR_HANDLE		m_pd3dRtvCPUDescriptorHandles[6];

	ID3D12Resource* m_pd3dDepthStencilBuffer = NULL;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dDsvCPUDescriptorHandle;
};
