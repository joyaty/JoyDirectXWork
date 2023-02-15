/**
 * DXWavesWithBlend.cpp
 * Demo08_DXWavesWithBlend示例项目主实现文件
 **/

#include "stdafx.h"
#include "DXWavesWithBlend.h"
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
}

void DXWavesWithBlend::OnMouseDown(UINT8 keyCode, int x, int y)
{
}

void DXWavesWithBlend::OnMouseUp(UINT8 keyCode, int x, int y)
{
}

void DXWavesWithBlend::OnMouseMove(UINT8 keyCode, int x, int y)
{
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
	BuildSrvHeap();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResource();
	// 执行实例初始化指令
	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(1, cmdLists);
	FlushCommandQueue();
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
	UpdateObjectConstant(deltaTime, totalTime);
	UpdateMaterialConstant(deltaTime, totalTime);
	UpdatePassConstant(deltaTime, totalTime);
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
}

void DXWavesWithBlend::BuildFenceCubeMesh()
{
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
	pWaterMaterial->diffuseAlbedo = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
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
	pTerrainRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pTerrainRenderItem->indexCount = pTerrainRenderItem->pMeshData->m_SubMeshGeometrys["Terrain"].m_IndexCount;
	pTerrainRenderItem->startIndexLocation = pTerrainRenderItem->pMeshData->m_SubMeshGeometrys["Terrain"].m_StartIndexLocation;
	pTerrainRenderItem->startVertexLocation = pTerrainRenderItem->pMeshData->m_SubMeshGeometrys["Terrain"].m_BaseVertexLocation;
	m_AllRenderItems.push_back(std::move(pTerrainRenderItem));
	m_OpaqueRenderItems.push_back(m_AllRenderItems.back().get());
	// TODO 构建水渲染项

	// TODO 构建围栏渲染项
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
}

void DXWavesWithBlend::BuildRootSignature()
{
	// 描述符表
	CD3DX12_DESCRIPTOR_RANGE ranges[1];
	ranges->Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	// 构造根参数
	CD3DX12_ROOT_PARAMETER parameters[4];
	parameters->InitAsDescriptorTable(1, ranges, D3D12_SHADER_VISIBILITY_PIXEL); // 着色器资源描述符表
	parameters->InitAsConstantBufferView(0); // 物体常量缓冲区
	parameters->InitAsConstantBufferView(1); // 材质常量缓冲区
	parameters->InitAsConstantBufferView(2); // 渲染过程常量缓冲区
	// 根签名描述(根参数+静态采样器描述)
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Init(4, parameters, (UINT)m_StaticSamplerDescs.size(), m_StaticSamplerDescs.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
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

void DXWavesWithBlend::BuildPSOs()
{
}

void DXWavesWithBlend::BuildFrameResource()
{
	for (int i = 0; i < kNumFrameResource; ++i)
	{
		std::unique_ptr<WavesWithBlendFrameResource> pFrameResource = std::make_unique<WavesWithBlendFrameResource>(m_Device.Get(), (int)m_SceneObjects.size(), (int)m_AllMaterials.size(), 1);
		m_FrameResources.push_back(std::move(pFrameResource));
	}
	m_CurrentFrameResourceIndex = 0;
	m_CurrentFrameResource = m_FrameResources[m_CurrentFrameResourceIndex].get();
}

void DXWavesWithBlend::UpdateObjectConstant(float deltaTime, float totalTime)
{
}

void DXWavesWithBlend::UpdateMaterialConstant(float deltaTime, float totalTime)
{
}

void DXWavesWithBlend::UpdatePassConstant(float deltaTime, float totalTime)
{
}

void DXWavesWithBlend::UpdateCamera(float deltaTime, float totalTime)
{
}

void DXWavesWithBlend::PopulateCommandList()
{
	// 清理当前帧资源的指令分配器和指令列表
	ThrowIfFailed(m_CurrentFrameResource->pCmdAllocator->Reset());
	ThrowIfFailed(m_CommandList->Reset(m_CurrentFrameResource->pCmdAllocator.Get(), nullptr));
	// 获取当前的渲染目标缓冲区
	ID3D12Resource* pCurrentBackBuffer = m_RenderTargets[m_CurrentBackBufferIndex].Get();
	// 渲染目标缓冲区资源转换为渲染目标状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pCurrentBackBuffer
		, D3D12_RESOURCE_STATE_PRESENT
		, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// TODO 渲染指令


	// 渲染目标缓冲区资源转换为呈现状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pCurrentBackBuffer
		, D3D12_RESOURCE_STATE_RENDER_TARGET
		, D3D12_RESOURCE_STATE_PRESENT));
	// 本帧的渲染指令记录完成，关闭指令列表，准备提交
	ThrowIfFailed(m_CommandList->Close());
}