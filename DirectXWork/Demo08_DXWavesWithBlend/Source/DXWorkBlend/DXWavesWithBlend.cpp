/**
 * DXWavesWithBlend.cpp
 * Demo08_DXWavesWithBlend示例项目主实现文件
 **/

#include "stdafx.h"
#include "DXWavesWithBlend.h"
#include "IMGuiWavesWithBlend.h"
#include "DirectXBaseWork/GeometryGenerator.h"
#include "DirectXBaseWork/DDSTextureLoader.h"

/// <summary>
/// 帧资源个数
/// </summary>
const int kNumFrameResource = 3;

DXWavesWithBlend::DXWavesWithBlend(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
	m_AssetPath += L"\\DirectXWork\\Demo08_DXWavesWithBlend\\Source\\DXWorkBlend\\";
}

DXWavesWithBlend::~DXWavesWithBlend()
{
}

void DXWavesWithBlend::OnResize(UINT width, UINT height)
{
	DirectXBaseWork::OnResize(width, height);
	// 计算投影变换矩阵，Fov = 45，屏幕纵横比，nearZ = 1，farZ = 1000
	DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, GetAspectRatio(), m_NearZ, m_FarZ);
	DirectX::XMStoreFloat4x4(&m_ProjMatrix, projMatrix);
}

void DXWavesWithBlend::OnMouseDown(UINT8 keyCode, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void DXWavesWithBlend::OnMouseUp(UINT8 keyCode, int x, int y)
{
}

void DXWavesWithBlend::OnMouseMove(UINT8 keyCode, int x, int y)
{
	if ((keyCode & MK_LBUTTON) != 0)
	{
		// 左键旋转
		float dx = DirectX::XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos.x));
		float dy = DirectX::XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos.y));
		m_CameraPhi += dx;
		m_CameraTheta += dy;
		m_CameraTheta = MathUtil::Clamp(m_CameraTheta, 0.1f, MathUtil::Pi - 0.1f);
	}
	else if ((keyCode & MK_RBUTTON) != 0)
	{
		// 右键缩放
		float dx = 0.05f * static_cast<float>(x - m_LastMousePos.x);
		float dy = 0.05f * static_cast<float>(y - m_LastMousePos.y);
		m_CameraDistance += dx - dy;
		m_CameraDistance = MathUtil::Clamp(m_CameraDistance, 50.f, 300.f);
	}
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

bool DXWavesWithBlend::OnInit()
{
	// 等待基类相关的命令列表指令执行完成
	FlushCommandQueue();
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));
	// 实例程序相关的初始化
	BuildTerrainMesh();
	BuildWavesMesh();
	BuildFenceCubeMesh();
	LoadTextures();
	BuildStaticSampler();
	BuildSrvHeap();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResource();
	BuildInputLayout();
	CompileShaderFiles();
	BuildRootSignature();
	BuildPSOs(IMGuiWavesWithBlend::GetInstance()->GetEnableFog(), IMGuiWavesWithBlend::GetInstance()->GetFillMode());
	// 执行实例初始化指令
	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(1, cmdLists);
	FlushCommandQueue();

	IMGuiWavesWithBlend::GetInstance()->SetWavesWithBlendDemo(this);

	return true;
}

void DXWavesWithBlend::OnUpdate(float deltaTime, float totalTime)
{
	m_CurrentFrameResourceIndex = (m_CurrentFrameResourceIndex + 1) % kNumFrameResource;
	m_CurrentFrameResource = m_FrameResources[m_CurrentFrameResourceIndex].get();
	if (m_CurrentFrameResource != 0 && m_CurrentFrameResource->fenceValue > m_Fence->GetCompletedValue())
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrentFrameResource->fenceValue, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
	UpdateCamera(deltaTime, totalTime);
	WaveFlow(deltaTime, totalTime);
	UpdateObjectConstant(deltaTime, totalTime);
	UpdateMaterialConstant(deltaTime, totalTime);
	UpdatePassConstant(deltaTime, totalTime);
	UpdateWave(deltaTime, totalTime);
}

void DXWavesWithBlend::OnRender()
{
	// 记录本帧的渲染指令
	PopulateCommandList();
	// 提交本帧的渲染指令
	ID3D12CommandList* cmdList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(1, cmdList);
	// 交互后台缓冲区
	ThrowIfFailed(m_SwapChain->Present(1, 0));
	// 标记围栏
	m_CurrentFrameResource->fenceValue = ++m_FenceValue;
	m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue);
}

void DXWavesWithBlend::OnDestroy()
{
}

void DXWavesWithBlend::BuildTerrainMesh()
{
	GeometryGenerator::MeshData terrainMeshData = GeometryGenerator::CreateGrid(160.f, 160.f, 50, 50);
	UINT totalVerticeSize = (UINT)terrainMeshData.Vertices.size();
	// 拷贝顶点数据
	std::vector<Vertex> terrainVertices(totalVerticeSize);
	for (UINT i = 0; i < totalVerticeSize; ++i)
	{
		DirectX::XMFLOAT3 position = terrainMeshData.Vertices[i].Position;
		terrainVertices[i].position = position;
		terrainVertices[i].position.y = GetHillsHeight(position.x, position.z);
		terrainVertices[i].normal = GetHillsNormal(position.x, position.z);
		// 直接获取纹理坐标
		terrainVertices[i].texCoord = terrainMeshData.Vertices[i].TexCoord;
	}
	UINT totalIndicesSize = (UINT)terrainMeshData.GetIndices16().size();
	// 拷贝索引数据
	std::vector<std::uint16_t> terrainIndices{};
	terrainIndices.assign(terrainMeshData.GetIndices16().cbegin(), terrainMeshData.GetIndices16().cend());
	// 顶点缓冲区和索引缓冲区的大小
	UINT vertexBufferSize = sizeof(Vertex) * totalVerticeSize;
	UINT indexBufferSize = sizeof(std::uint16_t) * totalIndicesSize;
	// 构建几何体
	std::unique_ptr<MeshGeometry> terrainMesh = std::make_unique<MeshGeometry>();
	terrainMesh->m_Name = "HillTerrain";
	terrainMesh->m_VertexBufferCPU = nullptr;   // 不做碰撞检测等，暂时不需要用到CPU上的顶点缓冲区
	terrainMesh->m_VertexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), terrainVertices.data(), vertexBufferSize, terrainMesh->m_VertexBufferUploader);
	terrainMesh->m_VertexSize = vertexBufferSize;
	terrainMesh->m_VertexStride = sizeof(Vertex);
	terrainMesh->m_IndexBufferCPU = nullptr;	// 不做碰撞检测等，暂时不需要用到CPU上的索引缓冲区
	terrainMesh->m_IndexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), terrainIndices.data(), indexBufferSize, terrainMesh->m_IndexBufferUploader);
	terrainMesh->m_IndexSize = indexBufferSize;
	terrainMesh->m_IndexFormat = DXGI_FORMAT_R16_UINT;
	SubMeshGeometry subMesh{};
	subMesh.m_IndexCount = totalIndicesSize;
	subMesh.m_StartIndexLocation = 0U;
	subMesh.m_BaseVertexLocation = 0U;
	terrainMesh->m_SubMeshGeometrys["Terrain"] = subMesh;
	// 添加到场景对象集合中
	m_SceneObjects[terrainMesh->m_Name] = std::move(terrainMesh);
}

float DXWavesWithBlend::GetHillsHeight(float x, float z) const
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

DirectX::XMFLOAT3 DXWavesWithBlend::GetHillsNormal(float x, float z)const
{
	DirectX::XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

	DirectX::XMVECTOR unitNormal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}

void DXWavesWithBlend::BuildWavesMesh()
{
	// 创建波浪
	m_Wave = std::make_unique<Wave>(128, 128, 1.f, 0.03f, 4.f, 0.2f);
	UINT totalIndexCount = static_cast<UINT>(m_Wave->GetIndices().size());
	UINT indiceBufferSize = sizeof(std::uint16_t) * totalIndexCount;
	// 生成波浪对象几何体数据
	std::unique_ptr<MeshGeometry> pWaveMeshGeo = std::make_unique<MeshGeometry>();
	pWaveMeshGeo->m_Name = "Wave";
	pWaveMeshGeo->m_VertexBufferCPU = nullptr;
	pWaveMeshGeo->m_VertexBufferGPU = nullptr;		// 波浪顶点随时间会变化，适用放在上传堆，不适用默认堆	
	pWaveMeshGeo->m_VertexBufferUploader = nullptr;
	pWaveMeshGeo->m_VertexSize = sizeof(Vertex) * m_Wave->GetVertexCount();
	pWaveMeshGeo->m_VertexStride = sizeof(Vertex);
	pWaveMeshGeo->m_IndexBufferCPU = nullptr;
	pWaveMeshGeo->m_IndexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), m_Wave->GetIndices().data(), indiceBufferSize, pWaveMeshGeo->m_IndexBufferUploader);
	pWaveMeshGeo->m_IndexSize = indiceBufferSize;
	pWaveMeshGeo->m_IndexFormat = DXGI_FORMAT_R16_UINT;
	SubMeshGeometry subGeoMesh{};
	subGeoMesh.m_IndexCount = static_cast<UINT>(m_Wave->GetIndices().size());
	subGeoMesh.m_StartIndexLocation = 0U;
	subGeoMesh.m_BaseVertexLocation = 0U;
	pWaveMeshGeo->m_SubMeshGeometrys["Wave"] = subGeoMesh;
	// 保存到场景物件集合中
	m_SceneObjects[pWaveMeshGeo->m_Name] = std::move(pWaveMeshGeo);
}

void DXWavesWithBlend::BuildFenceCubeMesh()
{
	GeometryGenerator::MeshData cube = GeometryGenerator::CreateCube(10.f, 10.f, 10.f, 0);
	// 拷贝顶点数据
	int vertexCount = static_cast<int>(cube.Vertices.size());
	std::vector<Vertex> vertices{};
	vertices.resize(vertexCount);
	for (int i = 0; i < vertexCount; ++i)
	{
		vertices[i].position = cube.Vertices[i].Position;
		vertices[i].normal = cube.Vertices[i].Normal;
		vertices[i].texCoord = cube.Vertices[i].TexCoord;
	}
	// 拷贝索引数据
	int indexCount = static_cast<int>(cube.GetIndices16().size());
	std::vector<std::uint16_t> indices{};
	indices.insert(indices.cend(), cube.GetIndices16().begin(), cube.GetIndices16().end());
	// 创建几何体对象
	int vertexBufferSize = sizeof(Vertex) * vertexCount;
	int indexBufferSize = sizeof(std::uint16_t) * indexCount;
	std::unique_ptr<MeshGeometry> pCubeGeo = std::make_unique<MeshGeometry>();
	pCubeGeo->m_Name = "FenceCube";
	pCubeGeo->m_VertexBufferCPU = nullptr;
	pCubeGeo->m_VertexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), vertices.data(), vertexBufferSize, pCubeGeo->m_VertexBufferUploader);
	pCubeGeo->m_VertexSize = vertexBufferSize;
	pCubeGeo->m_VertexStride = sizeof(Vertex);
	pCubeGeo->m_IndexBufferCPU = nullptr;
	pCubeGeo->m_IndexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), indices.data(), indexBufferSize, pCubeGeo->m_IndexBufferUploader);
	pCubeGeo->m_IndexSize = indexBufferSize;
	pCubeGeo->m_IndexFormat = DXGI_FORMAT_R16_UINT;
	SubMeshGeometry subMesh{};
	subMesh.m_IndexCount = indexCount;
	subMesh.m_StartIndexLocation = 0U;
	subMesh.m_BaseVertexLocation = 0U;
	pCubeGeo->m_SubMeshGeometrys["Cube"] = subMesh;
	m_SceneObjects[pCubeGeo->m_Name] = std::move(pCubeGeo);
}

void DXWavesWithBlend::LoadTextures()
{
	// 加载山地地形贴图
	std::unique_ptr<Texture> pTerrainTexture = std::make_unique<Texture>();
	pTerrainTexture->name = "Terrain";
	pTerrainTexture->fileName = m_AssetRootPath + L"\\Assets\\Textures\\grass.dds";
	DirectX::CreateDDSTextureFromFile12(m_Device.Get(), m_CommandList.Get(), pTerrainTexture->fileName.c_str(), pTerrainTexture->m_TextureGPU, pTerrainTexture->m_TextureUpload);
	m_AllTextures[pTerrainTexture->name] = std::move(pTerrainTexture);
	// 加载围栏贴图
	std::unique_ptr<Texture> pFenceTexture = std::make_unique<Texture>();
	pFenceTexture->name = "FenceBox";
	pFenceTexture->fileName = m_AssetRootPath + L"\\Assets\\Textures\\WireFence.dds";
	DirectX::CreateDDSTextureFromFile12(m_Device.Get(), m_CommandList.Get(), pFenceTexture->fileName.c_str(), pFenceTexture->m_TextureGPU, pFenceTexture->m_TextureUpload);
	m_AllTextures[pFenceTexture->name] = std::move(pFenceTexture);
	// 加载水波贴图
	std::unique_ptr<Texture> pWaterTexture = std::make_unique<Texture>();
	pWaterTexture->name = "Water";
	pWaterTexture->fileName = m_AssetRootPath + L"\\Assets\\Textures\\water1.dds";
	DirectX::CreateDDSTextureFromFile12(m_Device.Get(), m_CommandList.Get(), pWaterTexture->fileName.c_str(), pWaterTexture->m_TextureGPU, pWaterTexture->m_TextureUpload);
	m_AllTextures[pWaterTexture->name] = std::move(pWaterTexture);
}

void DXWavesWithBlend::BuildSrvHeap()
{
	// 创建着色器资源描述符堆
	D3D12_DESCRIPTOR_HEAP_DESC srvheapDesc{};
	srvheapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvheapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvheapDesc.NumDescriptors = 3U;	// 本例中有三张贴图
	srvheapDesc.NodeMask = 0U;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&srvheapDesc, IID_PPV_ARGS(m_SrvDescriptorHeap.GetAddressOf())));
	// 第一个着色器资源描述符，绑定地形纹理贴图
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	Texture* pTerrainTexture = m_AllTextures["Terrain"].get();
	D3D12_SHADER_RESOURCE_VIEW_DESC terrainSrvDesc{};
	terrainSrvDesc.Format = pTerrainTexture->m_TextureGPU->GetDesc().Format;
	terrainSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	terrainSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	terrainSrvDesc.Texture2D.MipLevels = pTerrainTexture->m_TextureGPU->GetDesc().MipLevels;
	terrainSrvDesc.Texture2D.MostDetailedMip = 0U;
	terrainSrvDesc.Texture2D.PlaneSlice = 0U;
	terrainSrvDesc.Texture2D.ResourceMinLODClamp = 0.f;
	m_Device->CreateShaderResourceView(pTerrainTexture->m_TextureGPU.Get(), &terrainSrvDesc, handle);
	// 偏移到下一个着色器资源描述符，绑定围栏纹理贴图
	handle.Offset(m_CBVUAVDescriptorSize);
	Texture* pFenceTexture = m_AllTextures["FenceBox"].get();
	D3D12_SHADER_RESOURCE_VIEW_DESC fenceSrvDesc{};
	fenceSrvDesc.Format = pFenceTexture->m_TextureGPU->GetDesc().Format;
	fenceSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	fenceSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	fenceSrvDesc.Texture2D.MipLevels = pFenceTexture->m_TextureGPU->GetDesc().MipLevels;
	fenceSrvDesc.Texture2D.MostDetailedMip = 0;
	fenceSrvDesc.Texture2D.PlaneSlice = 0;
	fenceSrvDesc.Texture2D.ResourceMinLODClamp = 0.f;
	m_Device->CreateShaderResourceView(pFenceTexture->m_TextureGPU.Get(), &fenceSrvDesc, handle);
	// 偏移到第三个着色器资源描述符，绑定水纹理贴图
	handle.Offset(m_CBVUAVDescriptorSize);
	Texture* pWaterTexture = m_AllTextures["Water"].get();
	D3D12_SHADER_RESOURCE_VIEW_DESC waterSrvDesc{};
	waterSrvDesc.Format = pWaterTexture->m_TextureGPU->GetDesc().Format;
	waterSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	waterSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	waterSrvDesc.Texture2D.MipLevels = pWaterTexture->m_TextureGPU->GetDesc().MipLevels;
	waterSrvDesc.Texture2D.MostDetailedMip = 0U;
	waterSrvDesc.Texture2D.PlaneSlice = 0U;
	waterSrvDesc.Texture2D.ResourceMinLODClamp = 0.f;
	m_Device->CreateShaderResourceView(pWaterTexture->m_TextureGPU.Get(), &waterSrvDesc, handle);
}

void DXWavesWithBlend::BuildStaticSampler()
{
	// 点过滤 + 重复寻址模式
	m_StaticSamplerDescs[0].Init(0U
		, D3D12_FILTER_MIN_MAG_MIP_POINT
		, D3D12_TEXTURE_ADDRESS_MODE_WRAP
		, D3D12_TEXTURE_ADDRESS_MODE_WRAP
		, D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);
	// 点过滤 + clamp寻址模式
	m_StaticSamplerDescs[1].Init(1U
		, D3D12_FILTER_MIN_MAG_MIP_POINT
		, D3D12_TEXTURE_ADDRESS_MODE_CLAMP
		, D3D12_TEXTURE_ADDRESS_MODE_CLAMP
		, D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	);
	// 线性过滤 + 重复寻址模式
	m_StaticSamplerDescs[2].Init(2U
		, D3D12_FILTER_MIN_MAG_MIP_LINEAR
		, D3D12_TEXTURE_ADDRESS_MODE_WRAP
		, D3D12_TEXTURE_ADDRESS_MODE_WRAP
		, D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);
	// 线性过滤 + clamp寻址模式
	m_StaticSamplerDescs[3].Init(3U
		, D3D12_FILTER_MIN_MAG_MIP_LINEAR
		, D3D12_TEXTURE_ADDRESS_MODE_CLAMP
		, D3D12_TEXTURE_ADDRESS_MODE_CLAMP
		, D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	);
	// 各项异性过滤 + 重复寻址模式
	m_StaticSamplerDescs[4].Init(4U
		, D3D12_FILTER_ANISOTROPIC
		, D3D12_TEXTURE_ADDRESS_MODE_WRAP
		, D3D12_TEXTURE_ADDRESS_MODE_WRAP
		, D3D12_TEXTURE_ADDRESS_MODE_WRAP
		, 0.f
		, 8U
	);
	// 各项异性过滤 + clamp寻址模式
	m_StaticSamplerDescs[5].Init(5U
		, D3D12_FILTER_ANISOTROPIC
		, D3D12_TEXTURE_ADDRESS_MODE_CLAMP
		, D3D12_TEXTURE_ADDRESS_MODE_CLAMP
		, D3D12_TEXTURE_ADDRESS_MODE_CLAMP
		, 0.f
		, 8U
	);
}

void DXWavesWithBlend::BuildMaterials()
{
	// 创建地形的材质
	std::unique_ptr<WavesWithBlendMaterial> pTerrainMaterial = std::make_unique<WavesWithBlendMaterial>();
	pTerrainMaterial->name = "Terrain";
	pTerrainMaterial->diffuseAlbedo = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	pTerrainMaterial->fresnelR0 = DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f);
	pTerrainMaterial->rougness = 0.125f;
	pTerrainMaterial->matConstantBufferIndex = 0;
	pTerrainMaterial->diffuseMapIndex = 0;
	m_AllMaterials[pTerrainMaterial->name] = std::move(pTerrainMaterial);
	// 创建围栏材质
	std::unique_ptr<WavesWithBlendMaterial> pFenceMaterial = std::make_unique<WavesWithBlendMaterial>();
	pFenceMaterial->name = "FenceBox";
	pFenceMaterial->diffuseAlbedo = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	pFenceMaterial->fresnelR0 = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
	pFenceMaterial->rougness = 0.25f;
	pFenceMaterial->matConstantBufferIndex = 1;
	pFenceMaterial->diffuseMapIndex = 1;
	m_AllMaterials[pFenceMaterial->name] = std::move(pFenceMaterial);
	// 创建水材质
	std::unique_ptr<WavesWithBlendMaterial> pWaterMaterial = std::make_unique<WavesWithBlendMaterial>();
	pWaterMaterial->name = "Water";
	pWaterMaterial->diffuseAlbedo = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.5f);
	pWaterMaterial->fresnelR0 = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
	pWaterMaterial->rougness = 0.f;
	pWaterMaterial->matConstantBufferIndex = 2;
	pWaterMaterial->diffuseMapIndex = 2;
	m_AllMaterials[pWaterMaterial->name] = std::move(pWaterMaterial);
}

void DXWavesWithBlend::BuildRenderItems()
{
	// 构建地形渲染项，使用不透明PSO
	std::unique_ptr<WavesWithBlendRenderItem> pTerrainRenderItem = std::make_unique<WavesWithBlendRenderItem>();
	pTerrainRenderItem->pMeshData = m_SceneObjects["HillTerrain"].get();
	pTerrainRenderItem->pMaterial = m_AllMaterials["Terrain"].get();
	pTerrainRenderItem->objConstantBufferIndex = 0;
	DirectX::XMFLOAT3 worldPos = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	DirectX::XMStoreFloat4x4(&pTerrainRenderItem->worldMatrix, DirectX::XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z));
	DirectX::XMStoreFloat4x4(&pTerrainRenderItem->texMatrix, DirectX::XMMatrixScaling(5.f, 5.f, 1.f));
	pTerrainRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pTerrainRenderItem->indexCount = pTerrainRenderItem->pMeshData->m_SubMeshGeometrys["Terrain"].m_IndexCount;
	pTerrainRenderItem->startIndexLocation = pTerrainRenderItem->pMeshData->m_SubMeshGeometrys["Terrain"].m_StartIndexLocation;
	pTerrainRenderItem->startVertexLocation = pTerrainRenderItem->pMeshData->m_SubMeshGeometrys["Terrain"].m_BaseVertexLocation;
	m_AllRenderItems.push_back(std::move(pTerrainRenderItem));
	m_RenderItemLayers[static_cast<int>(EnumRenderLayer::LayerOpaque)].push_back(m_AllRenderItems.back().get());
	// 构建水渲染项，使用透明PSO
	std::unique_ptr<WavesWithBlendRenderItem> pWaveRenderItem = std::make_unique<WavesWithBlendRenderItem>();
	pWaveRenderItem->pMeshData = m_SceneObjects["Wave"].get();
	pWaveRenderItem->pMaterial = m_AllMaterials["Water"].get();
	pWaveRenderItem->objConstantBufferIndex = 1;
	DirectX::XMStoreFloat4x4(&pWaveRenderItem->worldMatrix, DirectX::XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z));
	DirectX::XMStoreFloat4x4(&pWaveRenderItem->texMatrix, DirectX::XMMatrixScaling(4.f, 4.f, 1.f));
	pWaveRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pWaveRenderItem->indexCount = pWaveRenderItem->pMeshData->m_SubMeshGeometrys["Wave"].m_IndexCount;
	pWaveRenderItem->startIndexLocation = pWaveRenderItem->pMeshData->m_SubMeshGeometrys["Wave"].m_StartIndexLocation;
	pWaveRenderItem->startVertexLocation = pWaveRenderItem->pMeshData->m_SubMeshGeometrys["Wave"].m_BaseVertexLocation;
	m_AllRenderItems.push_back(std::move(pWaveRenderItem));
	m_RenderItemLayers[static_cast<int>(EnumRenderLayer::LayerTransparent)].push_back(m_AllRenderItems.back().get());
	// 构建围栏渲染项，围栏有的像素是完全透明的，可以直接丢弃，使用透明度测试PSO
	std::unique_ptr<WavesWithBlendRenderItem> pFenceRenderItem = std::make_unique<WavesWithBlendRenderItem>();
	pFenceRenderItem->pMeshData = m_SceneObjects["FenceCube"].get();
	pFenceRenderItem->pMaterial = m_AllMaterials["FenceBox"].get();
	pFenceRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pFenceRenderItem->objConstantBufferIndex = 2;
	DirectX::XMFLOAT3 fenceCubeWorldPos = DirectX::XMFLOAT3(0.f, 5.f, -7.f);
	DirectX::XMStoreFloat4x4(&pFenceRenderItem->worldMatrix, DirectX::XMMatrixTranslation(fenceCubeWorldPos.x, fenceCubeWorldPos.y, fenceCubeWorldPos.z));
	pFenceRenderItem->indexCount = pFenceRenderItem->pMeshData->m_SubMeshGeometrys["Cube"].m_IndexCount;
	pFenceRenderItem->startIndexLocation = pFenceRenderItem->pMeshData->m_SubMeshGeometrys["Cube"].m_StartIndexLocation;
	pFenceRenderItem->startVertexLocation = pFenceRenderItem->pMeshData->m_SubMeshGeometrys["Cube"].m_BaseVertexLocation;
	m_AllRenderItems.push_back(std::move(pFenceRenderItem));
	m_RenderItemLayers[static_cast<int>(EnumRenderLayer::LayerAlphaTest)].push_back(m_AllRenderItems.back().get());
}

void DXWavesWithBlend::BuildInputLayout()
{
	m_InputElementDescs = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 3 * sizeof(float), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 3 * sizeof(float) * 2, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
}

void DXWavesWithBlend::CompileShaderFiles()
{
	// 标准顶点着色器
	m_VSByteCode = CompileShader(m_AssetPath + L"WavesWithBlend.hlsl", nullptr, "VSMain", "vs_5_0");
	// 标准像素着色器
	m_StandardPSByteCode = CompileShader(m_AssetPath + L"WavesWithBlend.hlsl", nullptr, "PSMain", "ps_5_0");
	// 开启雾效的宏
	D3D_SHADER_MACRO fogEnableMacros[] = {
		{ "FOG_ENABLE", "1" },
		{ NULL, NULL }
	};
	m_EnableFogPSByteCode = CompileShader(m_AssetPath + L"WavesWithBlend.hlsl", fogEnableMacros, "PSMain", "ps_5_0");
	// 开启Alpha_Test的像素着色器
	D3D_SHADER_MACRO enaleAlphaTestMacros[] =
	{
		{ "ALPHA_TEST", "1" },
		// 最后需要定义一个NULL，指定宏定义数组结束标识
		{ NULL, NULL },
	};
	m_PSWithAlphaTestByteCode = CompileShader(m_AssetPath + L"WavesWithBlend.hlsl", enaleAlphaTestMacros, "PSMain", "ps_5_0");
	// 开启Alpha_Test和Fog的像素着色器
	D3D_SHADER_MACRO enaleAlphaTestAndFogMacros[] = 
	{
		{ "FOG_ENABLE", "1" },
		// 宏名称, 宏定义值
		{ "ALPHA_TEST", "1" },
		// 最后需要定义一个NULL，指定宏定义数组结束标识
		{ NULL, NULL },
	};
	m_EnableFogPSWithAlphaTestByteCode = CompileShader(m_AssetPath + L"WavesWithBlend.hlsl", enaleAlphaTestAndFogMacros, "PSMain", "ps_5_0");
}

void DXWavesWithBlend::BuildRootSignature()
{
	// 描述符表
	CD3DX12_DESCRIPTOR_RANGE ranges[1];
	ranges->Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	// 构造根参数
	CD3DX12_ROOT_PARAMETER parameters[4]{};
	parameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL); // 着色器资源描述符表
	parameters[1].InitAsConstantBufferView(0); // 物体常量缓冲区
	parameters[2].InitAsConstantBufferView(1); // 材质常量缓冲区
	parameters[3].InitAsConstantBufferView(2); // 渲染过程常量缓冲区
	// 根签名描述(根参数+静态采样器描述)
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Init(4, parameters, static_cast<UINT>(m_StaticSamplerDescs.size()), m_StaticSamplerDescs.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	// 序列化根签名参数
	Microsoft::WRL::ComPtr<ID3DBlob> m_SerializeRootSignature{ nullptr };
	Microsoft::WRL::ComPtr<ID3DBlob> m_ErrorMessage{ nullptr };
	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, m_SerializeRootSignature.GetAddressOf(), m_ErrorMessage.GetAddressOf());
	if (hr != S_OK)
	{
		::OutputDebugStringA((const char*)m_ErrorMessage->GetBufferPointer());
	}
	ThrowIfFailed(hr);
	// 创建根签名
	ThrowIfFailed(m_Device->CreateRootSignature(0U, m_SerializeRootSignature->GetBufferPointer(), m_SerializeRootSignature->GetBufferSize(), IID_PPV_ARGS(m_RootSignture.GetAddressOf())));
}

void DXWavesWithBlend::BuildPSOs(bool enableFog, D3D12_FILL_MODE fillMode)
{
	m_EnableFog = enableFog;
	m_FillMode = fillMode;
	// 不透明物件的PSO配置
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePSODesc{};
	opaquePSODesc.NodeMask = 0U;
	opaquePSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePSODesc.pRootSignature = m_RootSignture.Get();
	opaquePSODesc.NumRenderTargets = 1U;
	opaquePSODesc.RTVFormats[0] = GetBackBufferFormat();
	opaquePSODesc.DSVFormat = GetDepthStencilBufferFormat();
	opaquePSODesc.SampleDesc.Count = Get4XMSAAEnable() ? 4 : 1;
	opaquePSODesc.SampleDesc.Quality = Get4XMSAAEnable() ? m_4XMSAAQualityLevel - 1 : 0;
	opaquePSODesc.SampleMask = UINT_MAX;
	opaquePSODesc.InputLayout = { m_InputElementDescs.data(), static_cast<UINT>(m_InputElementDescs.size()) };
	opaquePSODesc.VS = { reinterpret_cast<BYTE*>(m_VSByteCode->GetBufferPointer()), m_VSByteCode->GetBufferSize() };
	opaquePSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePSODesc.RasterizerState.FillMode = fillMode;
	if (enableFog)
	{
		opaquePSODesc.PS = { reinterpret_cast<BYTE*>(m_EnableFogPSByteCode->GetBufferPointer()), m_EnableFogPSByteCode->GetBufferSize() };
	}
	else
	{
		opaquePSODesc.PS = { reinterpret_cast<BYTE*>(m_StandardPSByteCode->GetBufferPointer()), m_StandardPSByteCode->GetBufferSize() };
	}
	opaquePSODesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	// 创建Opaque渲染层级的PSO
	ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&opaquePSODesc, IID_PPV_ARGS(m_PSOs[static_cast<int>(EnumRenderLayer::LayerOpaque)].GetAddressOf())));
	// 透明物件的PSO配置
	D3D12_GRAPHICS_PIPELINE_STATE_DESC transparentPSODesc{ opaquePSODesc };
	CD3DX12_BLEND_DESC transparentBlendDesc{};
	transparentBlendDesc.AlphaToCoverageEnable = false;
	transparentBlendDesc.IndependentBlendEnable = true;
	transparentBlendDesc.RenderTarget[0].BlendEnable = true;
	transparentBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	transparentBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_BLEND_FACTOR;
	transparentBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_BLEND_FACTOR;
	transparentBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	transparentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	transparentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	transparentBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	transparentBlendDesc.RenderTarget[0].LogicOpEnable = false;
	transparentBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	// 使用半透明物件的混合配置
	transparentPSODesc.BlendState = transparentBlendDesc;
	ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&transparentPSODesc, IID_PPV_ARGS(m_PSOs[static_cast<int>(EnumRenderLayer::LayerTransparent)].GetAddressOf())));
	// AlphaTest的PSO配置
	D3D12_GRAPHICS_PIPELINE_STATE_DESC alphaTestPSODesc{ opaquePSODesc };
	// 使用带有ALPHA_TEST宏开关的PS
	if (enableFog)
	{
		alphaTestPSODesc.PS = { reinterpret_cast<BYTE*>(m_EnableFogPSWithAlphaTestByteCode->GetBufferPointer()), m_EnableFogPSWithAlphaTestByteCode->GetBufferSize() };
	}
	else
	{
		alphaTestPSODesc.PS = { reinterpret_cast<BYTE*>(m_PSWithAlphaTestByteCode->GetBufferPointer()), m_PSWithAlphaTestByteCode->GetBufferSize() };
	}
	// 不剔除，避免裁剪alpha=0的部分，穿透看到背后穿帮
	alphaTestPSODesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&alphaTestPSODesc, IID_PPV_ARGS(m_PSOs[static_cast<int>(EnumRenderLayer::LayerAlphaTest)].GetAddressOf())));
}

void DXWavesWithBlend::BuildFrameResource()
{
	for (int i = 0; i < kNumFrameResource; ++i)
	{
		std::unique_ptr<WavesWithBlendFrameResource> pFrameResource = std::make_unique<WavesWithBlendFrameResource>(m_Device.Get(), (int)m_SceneObjects.size(), (int)m_AllMaterials.size(), m_Wave->GetVertexCount(), 1);
		m_FrameResources.push_back(std::move(pFrameResource));
	}
	m_CurrentFrameResourceIndex = 0;
	m_CurrentFrameResource = m_FrameResources[m_CurrentFrameResourceIndex].get();
}

void DXWavesWithBlend::UpdateObjectConstant(float deltaTime, float totalTime)
{
	for (size_t i = 0; i < m_AllRenderItems.size(); ++i)
	{
		WavesWithBlendRenderItem* pRenderItem = m_AllRenderItems[i].get();
		if (pRenderItem->dirty > 0)
		{
			// DirectX数学库使用行矩阵
			DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&pRenderItem->worldMatrix);
			DirectX::XMMATRIX texMatrix = DirectX::XMLoadFloat4x4(&pRenderItem->texMatrix);
			PerObjectConstants objCBData{};
			// HLSL使用列矩阵，因此拷贝到常量缓冲区需要对矩阵进行一次转置
			DirectX::XMStoreFloat4x4(&objCBData.worldMatrix, DirectX::XMMatrixTranspose(worldMatrix));
			DirectX::XMStoreFloat4x4(&objCBData.texMatrix, DirectX::XMMatrixTranspose(texMatrix));
			m_CurrentFrameResource->pObjCBuffer->CopyData(pRenderItem->objConstantBufferIndex, objCBData);
			--pRenderItem->dirty;
		}
	}
}

void DXWavesWithBlend::UpdateMaterialConstant(float deltaTime, float totalTime)
{
	for (const auto& iter : m_AllMaterials)
	{
		WavesWithBlendMaterial* pMaterial = iter.second.get();
		if (pMaterial->dirty > 0)
		{
			PerMaterialConstants matCBData{};
			matCBData.albedo = pMaterial->diffuseAlbedo;
			matCBData.fresnelR0 = pMaterial->fresnelR0;
			matCBData.roughness = pMaterial->rougness;
			DirectX::XMMATRIX matMatrix = DirectX::XMLoadFloat4x4(&pMaterial->matMatrix);
			DirectX::XMStoreFloat4x4(&matCBData.uvMatrix, DirectX::XMMatrixTranspose(matMatrix));
			m_CurrentFrameResource->pMatCBuffer->CopyData(pMaterial->matConstantBufferIndex, matCBData);
			--pMaterial->dirty;
		}
	}
}

void DXWavesWithBlend::UpdatePassConstant(float deltaTime, float totalTime)
{
	DirectX::XMMATRIX viewMatrix = DirectX::XMLoadFloat4x4(&m_ViewMatrix);
	DirectX::XMMATRIX invViewMatrix = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(viewMatrix), viewMatrix);
	DirectX::XMMATRIX projMatrix = DirectX::XMLoadFloat4x4(&m_ProjMatrix);
	DirectX::XMMATRIX invProjMatrix = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(projMatrix), projMatrix);
	DirectX::XMMATRIX viewProjMatrix = DirectX::XMMatrixMultiply(viewMatrix, projMatrix);
	DirectX::XMMATRIX invViewProjMatrix = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(viewProjMatrix), viewProjMatrix);
	PerPassConstancts passCBData{};
	DirectX::XMStoreFloat4x4(&passCBData.viewMatrix, DirectX::XMMatrixTranspose(viewMatrix));
	DirectX::XMStoreFloat4x4(&passCBData.invViewMatrix, DirectX::XMMatrixTranspose(invViewMatrix));
	DirectX::XMStoreFloat4x4(&passCBData.projMatrix, DirectX::XMMatrixTranspose(projMatrix));
	DirectX::XMStoreFloat4x4(&passCBData.invProjMatrix, DirectX::XMMatrixTranspose(invProjMatrix));
	DirectX::XMStoreFloat4x4(&passCBData.viewProjMatrix, DirectX::XMMatrixTranspose(viewProjMatrix));
	DirectX::XMStoreFloat4x4(&passCBData.invViewProjMatrix, DirectX::XMMatrixTranspose(invViewProjMatrix));
	passCBData.eyeWorldPos = m_CameraPos;
	passCBData.renderTargetSize = DirectX::XMFLOAT2(static_cast<float>(GetWidth()), static_cast<float>(GetHeight()));
	passCBData.invRenderTargetSize = DirectX::XMFLOAT2(1.0f / GetWidth(), 1.0f / GetHeight());
	passCBData.nearZ = m_NearZ;
	passCBData.farZ = m_FarZ;
	passCBData.totalTime = totalTime;
	passCBData.deltaTime = deltaTime;
	passCBData.ambientLight = m_AmbientLight;
	passCBData.lights[0].strength = m_DirectLight;
	// 按模长为1计算，因此已经是单位向量
	float lightPosX = 1.f * sinf(m_LightTheta) * cosf(m_LightPhi);
	float lightPosY = 1.f * cosf(m_LightTheta);
	float lightPosZ = 1.f * sinf(m_LightTheta) * sinf(m_LightPhi);
	DirectX::XMVECTOR lightDir = DirectX::XMVectorSet(-lightPosX, -lightPosY, -lightPosZ, 1.0f);
	DirectX::XMStoreFloat3(&passCBData.lights[0].direction, lightDir);
	// 第二个光源
	passCBData.lights[1].strength = DirectX::XMFLOAT3(0.5f, 0.5f, 0.4f);
	passCBData.lights[1].direction = DirectX::XMFLOAT3(0.f, -0.866f, 0.5f);

	// 拷贝数据到渲染过程常量缓冲区
	m_CurrentFrameResource->pPassCBuffer->CopyData(0, passCBData);
}

void DXWavesWithBlend::UpdateCamera(float deltaTime, float totalTime)
{
	// 极坐标转化为笛卡尔坐标系
	m_CameraPos.x = m_CameraDistance * sinf(m_CameraTheta) * cosf(m_CameraPhi);
	m_CameraPos.y = m_CameraDistance * cosf(m_CameraTheta);
	m_CameraPos.z = m_CameraDistance * sinf(m_CameraTheta) * sinf(m_CameraPhi);
	// 计算相机观察空间变换矩阵
	DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(m_CameraPos.x, m_CameraPos.y, m_CameraPos.z, 1.0f);
	DirectX::XMVECTOR focusePos = DirectX::XMVectorZero();
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);
	DirectX::XMStoreFloat4x4(&m_ViewMatrix, DirectX::XMMatrixLookAtLH(eyePos, focusePos, up));
}

void DXWavesWithBlend::UpdateWave(float deltaTime, float totalTime)
{
	static float s_NextTriggerTime = 0.f;
	// 每0.25s产生一个水波涟漪
	if ((totalTime - s_NextTriggerTime) >= 0.25f)
	{
		s_NextTriggerTime += 0.25f;
		// 随机一个行和列，边界位置禁止随机，避免涟漪扩散越界
		int randomRow = MathUtil::Rand(4, m_Wave->GetRowNum() - 5);
		int randomCol = MathUtil::Rand(4, m_Wave->GetColNum() - 5);
		// 随机一个波峰高度
		float r = MathUtil::RandF(0.2f, 0.5f);
		m_Wave->Disturb(randomRow, randomCol, r);
	}
	m_Wave->UpdateWave(deltaTime);
	// 每帧更新动态顶点缓冲区数据
	for (int i = 0; i < m_Wave->GetVertexCount(); ++i)
	{
		Vertex vert{};
		vert.position = m_Wave->GetPosition(i);
		vert.normal = m_Wave->GetNormal(i);
		vert.texCoord = m_Wave->GetTexCoord(i);

		m_CurrentFrameResource->pDynamicVertexBuffer->CopyData(i, vert);
	}
	// 设置动态顶点缓冲区
	m_SceneObjects["Wave"]->m_VertexBufferGPU = m_CurrentFrameResource->pDynamicVertexBuffer->GetResource();
}

void DXWavesWithBlend::WaveFlow(float deltaTime, float totalTime)
{
	WavesWithBlendMaterial* pMaterial = m_AllMaterials["Water"].get();
	// 行矩阵，位置在矩阵第三行
	float& offsetU = pMaterial->matMatrix(3, 0);
	float& offsetV = pMaterial->matMatrix(3, 1);
	// UV移动动画
	offsetU += 0.07f * deltaTime;
	offsetV += 0.02f * deltaTime;
	offsetU = offsetU > 1.f ? offsetU - 1.f : offsetU;
	offsetV = offsetV > 1.f ? offsetV - 1.f : offsetV;
	// 重新标记材质数据更新
	pMaterial->dirty = kNumFrameResource;
}

void DXWavesWithBlend::PopulateCommandList()
{
	// 清理当前帧资源的指令分配器和指令列表
	ThrowIfFailed(m_CurrentFrameResource->pCmdAllocator->Reset());
	ThrowIfFailed(m_CommandList->Reset(m_CurrentFrameResource->pCmdAllocator.Get(), m_PSOs[static_cast<int>(EnumRenderLayer::LayerOpaque)].Get()));
	// 获取当前的渲染目标缓冲区
	ID3D12Resource* pCurrentBackBuffer = m_RenderTargets[m_CurrentBackBufferIndex].Get();
	// 渲染目标缓冲区资源转换为渲染目标状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pCurrentBackBuffer
		, D3D12_RESOURCE_STATE_PRESENT
		, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// TODO 渲染指令
	m_CommandList->RSSetViewports(1, &m_Viewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);
	// 清理目标后台缓冲区
	m_CommandList->ClearRenderTargetView(m_RenderTargetDesciptorHandles[m_CurrentBackBufferIndex], DirectX::Colors::Gray, 0, nullptr);
	// 清理深度模板缓冲区
	m_CommandList->ClearDepthStencilView(m_DepthStencilDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);
	// 绑定渲染目标后台缓冲区
	m_CommandList->OMSetRenderTargets(1U, &m_RenderTargetDesciptorHandles[m_CurrentBackBufferIndex], true, &m_DepthStencilDescriptorHandle);
	// 绑定SRV描述符堆
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_SrvDescriptorHeap.Get() };
	m_CommandList->SetDescriptorHeaps(1U, descriptorHeaps);
	// 绑定根签名
	m_CommandList->SetGraphicsRootSignature(m_RootSignture.Get());
	// 绑定渲染过程常量缓冲区
	m_CommandList->SetGraphicsRootConstantBufferView(3U, m_CurrentFrameResource->pPassCBuffer->GetResource()->GetGPUVirtualAddress());
	// 绘制不透明渲染项
	// m_CommandList->SetPipelineState(m_PSOs[static_cast<int>(EnumRenderLayer::LayerOpaque)].Get());
	DrawRenderItem(m_RenderItemLayers[static_cast<int>(EnumRenderLayer::LayerOpaque)]);
	// 切换到透明渲染管线状态对象
	m_CommandList->SetPipelineState(m_PSOs[static_cast<int>(EnumRenderLayer::LayerTransparent)].Get());
	// 设置自定义的混合因子，Blend状态设置为D3D12_BLEND_BLEND_FACTOR时可以使用
	m_CommandList->OMSetBlendFactor(m_BlendFactor);
	// 绘制透明渲染项
	DrawRenderItem(m_RenderItemLayers[static_cast<int>(EnumRenderLayer::LayerTransparent)]);
	// 绘制需要ALPHA_TEST的渲染项
	m_CommandList->SetPipelineState(m_PSOs[static_cast<int>(EnumRenderLayer::LayerAlphaTest)].Get());
	DrawRenderItem(m_RenderItemLayers[static_cast<int>(EnumRenderLayer::LayerAlphaTest)]);
	// 提交DearIMGui的渲染指令
	if (IMGuiWavesWithBlend::GetInstance() != nullptr)
	{
		IMGuiWavesWithBlend::GetInstance()->PopulateDearIMGuiCommand(m_CommandList.Get());
	}
	// 渲染目标缓冲区资源转换为呈现状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pCurrentBackBuffer
		, D3D12_RESOURCE_STATE_RENDER_TARGET
		, D3D12_RESOURCE_STATE_PRESENT));
	// 本帧的渲染指令记录完成，关闭指令列表，准备提交
	ThrowIfFailed(m_CommandList->Close());
}

void DXWavesWithBlend::DrawRenderItem(const std::vector<WavesWithBlendRenderItem*>& renderItems)
{
	for (size_t i = 0; i < renderItems.size(); ++i)
	{
		const WavesWithBlendRenderItem* pRenderItem = renderItems[i];
		// 绑定顶点缓冲区
		m_CommandList->IASetVertexBuffers(0, 1, &pRenderItem->pMeshData->GetVertexBufferView());
		// 绑定索引缓冲区
		m_CommandList->IASetIndexBuffer(&pRenderItem->pMeshData->GetIndexBufferView());
		// 图元拓扑类型
		m_CommandList->IASetPrimitiveTopology(pRenderItem->primitiveType);
		// 绑定物体缓冲区，起始位置偏移到当前渲染项索引的物体常量缓冲区
		D3D12_GPU_VIRTUAL_ADDRESS objCBLocation = m_CurrentFrameResource->pObjCBuffer->GetResource()->GetGPUVirtualAddress();
		objCBLocation += pRenderItem->objConstantBufferIndex * m_CurrentFrameResource->pObjCBuffer->GetElementSize();
		m_CommandList->SetGraphicsRootConstantBufferView(1, objCBLocation);
		// 绑定材质缓冲区
		D3D12_GPU_VIRTUAL_ADDRESS matCBLocaton = m_CurrentFrameResource->pMatCBuffer->GetResource()->GetGPUVirtualAddress();
		matCBLocaton += pRenderItem->pMaterial->matConstantBufferIndex * m_CurrentFrameResource->pMatCBuffer->GetElementSize();
		m_CommandList->SetGraphicsRootConstantBufferView(2, matCBLocaton);
		// 绑定着色器资源（纹理）描述符
		CD3DX12_GPU_DESCRIPTOR_HANDLE handle(m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		handle.Offset(pRenderItem->pMaterial->diffuseMapIndex, m_CBVUAVDescriptorSize);
		m_CommandList->SetGraphicsRootDescriptorTable(0, handle);
		// 绘制
		m_CommandList->DrawIndexedInstanced(pRenderItem->indexCount, 1, pRenderItem->startIndexLocation, pRenderItem->startVertexLocation, 0);
	}
}