/**
 * DXHelloStenciling.cpp
 * Demo09_DXHelloStenciling练习项目，模板知识练习项目实现文件
 **/

#include "stdafx.h"
#include "DXHelloStenciling.h"
#include "IMGuiHelloStenciling.h"
#include "DirectXBaseWork/GeometryGenerator.h"
#include "DirectXBaseWork/DDSTextureLoader.h"

/// <summary>
/// 帧资源个数
/// </summary>
const int kNumFrameResource = 3;

DXHelloStenciling::DXHelloStenciling(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
	m_AssetPath += L"\\DirectXWork\\Demo09_DXHelloStenciling\\Source\\DXWorkStenciling\\";
}

DXHelloStenciling::~DXHelloStenciling()
{
}

void DXHelloStenciling::OnMouseDown(UINT8 keyCode, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void DXHelloStenciling::OnMouseUp(UINT8 keyCode, int x, int y)
{
}

void DXHelloStenciling::OnMouseMove(UINT8 keyCode, int x, int y)
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
		m_CameraDistance = MathUtil::Clamp(m_CameraDistance, 30.f, 100.f);
	}
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void DXHelloStenciling::OnResize(UINT width, UINT height)
{
	DirectXBaseWork::OnResize(width, height);
	// 投影变换矩阵
	DirectX::XMStoreFloat4x4(&m_ProjMatrix, DirectX::XMMatrixPerspectiveFovLH(m_FOV, GetAspectRatio(), m_NearZ, m_FarZ));
}

bool DXHelloStenciling::OnInit()
{
	// 等级管线初始化指令执行完毕
	FlushCommandQueue();
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));
	// 初始化HelloStenciling
	BuildRoomGeoMesh();
	LoadTexture();
	BuildStaticSampler();
	BuildMaterial();
	BuildRenderItem();
	BuildFrameResource();
	BuildInputLayout();
	CompileShaderFiles();
	BuildRootSignature();
	BuildPSOs(IMGuiHelloStenciling::GetInstance()->GetEnableFog(), IMGuiHelloStenciling::GetInstance()->GetFillMode());
	// 执行HelloStenciling相关的初始化指令
	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(1, cmdList);
	FlushCommandQueue();

	IMGuiHelloStenciling::GetInstance()->SetHelloStencilingDemo(this);

	return true;
}

void DXHelloStenciling::OnUpdate(float deltaTime, float totalTime)
{
	// 切换帧资源
	m_ActiveFrameResourceIndex = (m_ActiveFrameResourceIndex + 1) % kNumFrameResource;
	m_ActiveFrameResource = m_AllFrameResources[m_ActiveFrameResourceIndex].get();
	if (m_ActiveFrameResource->fenceValue != 0 && m_ActiveFrameResource->fenceValue > m_Fence->GetCompletedValue())
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(m_Fence->SetEventOnCompletion(m_ActiveFrameResource->fenceValue, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
	// 切换
	UpdateCamera(deltaTime, totalTime);
	UpdateObjectCB(deltaTime, totalTime);
	UpdateMaterialCB(deltaTime, totalTime);
	UpdatePassCB(deltaTime, totalTime);
}

void DXHelloStenciling::OnRender()
{
	// 记录渲染指令
	PopulateCommandList();
	// 提交渲染指令
	ID3D12CommandList* cmdList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(1, cmdList);
	// 交换后台缓冲区
	ThrowIfFailed(m_SwapChain->Present(1, 0));
	// 标记围栏
	m_ActiveFrameResource->fenceValue = ++m_FenceValue;
	m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue);
}

void DXHelloStenciling::OnDestroy()
{
}

void DXHelloStenciling::BuildRoomGeoMesh()
{
	// 地面网格数据
	GeometryGenerator::MeshData gridMesh = GeometryGenerator::CreateGrid(m_FloorWidh, m_FloorWidh, 2, 2);
	UINT gridVertexCount = static_cast<UINT>(gridMesh.Vertices.size());
	UINT gridIndexCount = static_cast<UINT>(gridMesh.GetIndices16().size());
	// 墙面网格数据
	GeometryGenerator::MeshData quadMesh = GeometryGenerator::CreateQuad(m_FloorWidh, m_FloorWidh * 0.5f, 1, 1);
	UINT quadVertexCount = static_cast<UINT>(quadMesh.Vertices.size());
	UINT quadIndexCount = static_cast<UINT>(quadMesh.GetIndices16().size());
	// 地面与墙面整合为一个大的顶点缓冲区
	std::vector<Vertex> vertices{};
	vertices.resize(gridVertexCount + quadVertexCount);
	for (UINT i = 0; i < gridVertexCount; ++i)
	{
		vertices[i].position = gridMesh.Vertices[i].Position;
		vertices[i].normal = gridMesh.Vertices[i].Normal;
		vertices[i].texCoord = gridMesh.Vertices[i].TexCoord;
	}
	for (UINT i = 0; i < quadVertexCount; ++i)
	{
		int index = gridVertexCount + i;
		vertices[index].position = quadMesh.Vertices[i].Position;
		vertices[index].normal = quadMesh.Vertices[i].Normal;
		vertices[index].texCoord = quadMesh.Vertices[i].TexCoord;
	}
	// 整合为一个大的索引缓冲区
	std::vector<std::uint16_t> indices{};
	indices.insert(indices.end(), gridMesh.GetIndices16().begin(), gridMesh.GetIndices16().end());
	indices.insert(indices.end(), quadMesh.GetIndices16().begin(), quadMesh.GetIndices16().end());
	// 创建房间几何体(地面 + 墙面)
	UINT vertexBufferSize = sizeof(Vertex) * (gridVertexCount + quadVertexCount);
	UINT indexBufferSize = sizeof(std::uint16_t) * (gridIndexCount + quadIndexCount);
	std::unique_ptr<MeshGeometry> pMeshGeo = std::make_unique<MeshGeometry>();
	pMeshGeo->m_Name = "Room";
	pMeshGeo->m_VertexBufferCPU = nullptr;
	pMeshGeo->m_VertexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), vertices.data(), vertexBufferSize, pMeshGeo->m_VertexBufferUploader);
	pMeshGeo->m_VertexSize = vertexBufferSize;
	pMeshGeo->m_VertexStride = sizeof(Vertex);
	pMeshGeo->m_IndexBufferCPU = nullptr;
	pMeshGeo->m_IndexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), indices.data(), indexBufferSize, pMeshGeo->m_IndexBufferUploader);
	pMeshGeo->m_IndexSize = indexBufferSize;
	pMeshGeo->m_IndexFormat = DXGI_FORMAT_R16_UINT;
	// Room由地面和墙面子Mesh组成
	SubMeshGeometry floor{};
	floor.m_IndexCount = gridIndexCount;
	floor.m_StartIndexLocation = 0;
	floor.m_BaseVertexLocation = 0;
	SubMeshGeometry wall{};
	wall.m_IndexCount = quadIndexCount;
	wall.m_StartIndexLocation = gridIndexCount;
	wall.m_BaseVertexLocation = gridVertexCount;
	pMeshGeo->m_SubMeshGeometrys["Floor"] = floor;
	pMeshGeo->m_SubMeshGeometrys["Wall"] = wall;
	// 加入场景物件集合中
	m_SceneObjects[pMeshGeo->m_Name] = std::move(pMeshGeo);
}

void DXHelloStenciling::LoadTexture()
{
	// 加载地面纹理
	std::unique_ptr<Texture> pFloorTexture = std::make_unique<Texture>();
	pFloorTexture->name = "Floor";
	pFloorTexture->fileName = m_AssetRootPath + L"\\Assets\\Textures\\checkboard.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(m_Device.Get(), m_CommandList.Get(), pFloorTexture->fileName.c_str(), pFloorTexture->m_TextureGPU, pFloorTexture->m_TextureUpload));
	// 加载墙面纹理
	std::unique_ptr<Texture> pWallTexture = std::make_unique<Texture>();
	pWallTexture->name = "Wall";
	pWallTexture->fileName = m_AssetRootPath + L"\\Assets\\Textures\\bricks3.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(m_Device.Get(), m_CommandList.Get(), pWallTexture->fileName.c_str(), pWallTexture->m_TextureGPU, pWallTexture->m_TextureUpload));
	
	// 创建着色器资源描述符堆
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.NodeMask = 0U;
	heapDesc.NumDescriptors = 2;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_SRVDescriptorHeap.GetAddressOf())));
	// 创建纹理资源的着色器资源描述符
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_SRVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	D3D12_SHADER_RESOURCE_VIEW_DESC floorSrvDesc{};
	floorSrvDesc.Format = pFloorTexture->m_TextureGPU->GetDesc().Format;
	floorSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	floorSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	floorSrvDesc.Texture2D.MipLevels = pFloorTexture->m_TextureGPU->GetDesc().MipLevels;
	floorSrvDesc.Texture2D.MostDetailedMip = 0U;
	floorSrvDesc.Texture2D.PlaneSlice = 0U;
	floorSrvDesc.Texture2D.ResourceMinLODClamp = 0.f;
	m_Device->CreateShaderResourceView(pFloorTexture->m_TextureGPU.Get(), &floorSrvDesc, handle);
	// 偏移到下一个描述符，绑定墙面纹理
	handle.Offset(m_CBVUAVDescriptorSize);
	D3D12_SHADER_RESOURCE_VIEW_DESC wallSrvDesc{};
	wallSrvDesc.Format = pWallTexture->m_TextureGPU->GetDesc().Format;
	wallSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	wallSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	wallSrvDesc.Texture2D.MipLevels = pWallTexture->m_TextureGPU->GetDesc().MipLevels;
	wallSrvDesc.Texture2D.MostDetailedMip = 0U;
	wallSrvDesc.Texture2D.PlaneSlice = 0U;
	wallSrvDesc.Texture2D.ResourceMinLODClamp = 0.f;
	m_Device->CreateShaderResourceView(pWallTexture->m_TextureGPU.Get(), &wallSrvDesc, handle);

	// 纹理保存到纹理集合中
	m_AllTextures[pFloorTexture->name] = std::move(pFloorTexture);
	m_AllTextures[pWallTexture->name] = std::move(pWallTexture);
}

void DXHelloStenciling::BuildStaticSampler()
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

void DXHelloStenciling::BuildMaterial()
{
	std::unique_ptr<HelloStencilingMaterial> pFloorMaterial = std::make_unique<HelloStencilingMaterial>();
	pFloorMaterial->name = "FloorMat";
	pFloorMaterial->diffuseAlbedo = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	pFloorMaterial->fresnelR0 = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
	pFloorMaterial->roughness = 0.25f;
	pFloorMaterial->matCBufferIndex = 0;
	pFloorMaterial->diffuseMapIndex = 0;

	std::unique_ptr<HelloStencilingMaterial> pWallMaterial = std::make_unique<HelloStencilingMaterial>();
	pWallMaterial->name = "WallMat";
	pWallMaterial->diffuseAlbedo = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	pWallMaterial->fresnelR0 = DirectX::XMFLOAT3(0.07f, 0.07f, 0.07f);
	pWallMaterial->roughness = 0.3f;
	pWallMaterial->matCBufferIndex = 1;
	pWallMaterial->diffuseMapIndex = 1;

	m_AllMaterials[pFloorMaterial->name] = std::move(pFloorMaterial);
	m_AllMaterials[pWallMaterial->name] = std::move(pWallMaterial);
}

void DXHelloStenciling::BuildRenderItem()
{
	// 创建地面渲染项
	std::unique_ptr<HelloStencilingRenderItem> pFloorRenderItem = std::make_unique<HelloStencilingRenderItem>();
	pFloorRenderItem->pGeometryMesh = m_SceneObjects["Room"].get();
	pFloorRenderItem->pMaterial = m_AllMaterials["FloorMat"].get();
	DirectX::XMFLOAT3 worldPos = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	DirectX::XMStoreFloat4x4(&pFloorRenderItem->worldMatrix, DirectX::XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z));
	DirectX::XMStoreFloat4x4(&pFloorRenderItem->texMatrix, DirectX::XMMatrixScaling(5.f, 5.f, 1.f));
	pFloorRenderItem->objectCBufferIndex = 0;
	pFloorRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pFloorRenderItem->indexCount = pFloorRenderItem->pGeometryMesh->m_SubMeshGeometrys["Floor"].m_IndexCount;
	pFloorRenderItem->startIndexLoaction = pFloorRenderItem->pGeometryMesh->m_SubMeshGeometrys["Floor"].m_StartIndexLocation;
	pFloorRenderItem->startVertexLocation = pFloorRenderItem->pGeometryMesh->m_SubMeshGeometrys["Floor"].m_BaseVertexLocation;
	m_AllRenderItem.push_back(std::move(pFloorRenderItem));
	m_RenderItemLayouts[static_cast<int>(EnumRenderLayer::LayerOpaque)].push_back(m_AllRenderItem.back().get());
	// 创建墙面渲染项
	std::unique_ptr<HelloStencilingRenderItem> pWallRenderItem = std::make_unique<HelloStencilingRenderItem>();
	pWallRenderItem->pGeometryMesh = m_SceneObjects["Room"].get();
	pWallRenderItem->pMaterial = m_AllMaterials["WallMat"].get();
	worldPos = DirectX::XMFLOAT3(0.f, m_FloorWidh * 0.5f * 0.5f, m_FloorWidh * 0.5f);
	DirectX::XMStoreFloat4x4(&pWallRenderItem->worldMatrix, DirectX::XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z));
	DirectX::XMStoreFloat4x4(&pWallRenderItem->texMatrix, DirectX::XMMatrixScaling(5.f, 5.f, 1.f));
	pWallRenderItem->objectCBufferIndex = 1;
	pWallRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pWallRenderItem->indexCount = pWallRenderItem->pGeometryMesh->m_SubMeshGeometrys["Wall"].m_IndexCount;
	pWallRenderItem->startIndexLoaction = pWallRenderItem->pGeometryMesh->m_SubMeshGeometrys["Wall"].m_StartIndexLocation;
	pWallRenderItem->startVertexLocation = pWallRenderItem->pGeometryMesh->m_SubMeshGeometrys["Wall"].m_BaseVertexLocation;
	m_AllRenderItem.push_back(std::move(pWallRenderItem));
	m_RenderItemLayouts[static_cast<int>(EnumRenderLayer::LayerOpaque)].push_back(m_AllRenderItem.back().get());
}

void DXHelloStenciling::BuildFrameResource()
{
	for (int i = 0; i < kNumFrameResource; ++i)
	{
		m_AllFrameResources.push_back(std::make_unique<HelloStencilingFrameResource>(m_Device.Get(), static_cast<int>(m_AllRenderItem.size()), static_cast<int>(m_AllMaterials.size()), 1));
	}
	m_ActiveFrameResourceIndex = 0;
	m_ActiveFrameResource = m_AllFrameResources[m_ActiveFrameResourceIndex].get();
}

void DXHelloStenciling::BuildRootSignature()
{
	// 描述符表
	CD3DX12_DESCRIPTOR_RANGE ranges[1]{};
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1U, 0U);
	// 初始化根参数
	CD3DX12_ROOT_PARAMETER parameters[4]{};
	parameters[0].InitAsDescriptorTable(1U, ranges, D3D12_SHADER_VISIBILITY_PIXEL);
	parameters[1].InitAsConstantBufferView(0U); // 物体常量缓冲区
	parameters[2].InitAsConstantBufferView(1U); // 材质常量缓冲区
	parameters[3].InitAsConstantBufferView(2U); // 渲染过程常量缓冲区
	// 创建根参数描述
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Init(4U, parameters, static_cast<UINT>(m_StaticSamplerDescs.size()), m_StaticSamplerDescs.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	// 序列化根签名
	Microsoft::WRL::ComPtr<ID3DBlob> pSerializeRootSignature{ nullptr };
	Microsoft::WRL::ComPtr<ID3DBlob> pErrorMsg{ nullptr };
	HRESULT result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, pSerializeRootSignature.GetAddressOf(), pErrorMsg.GetAddressOf());
	if (result != S_OK)
	{
		::OutputDebugStringA((static_cast<char*>(pErrorMsg->GetBufferPointer())));
	}
	ThrowIfFailed(result);
	// 创建根签名
	ThrowIfFailed(m_Device->CreateRootSignature(0U, pSerializeRootSignature->GetBufferPointer(), pSerializeRootSignature->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf())));
}

void DXHelloStenciling::BuildInputLayout()
{
	m_InputElementDesces = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 3 * sizeof(float), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 3 * sizeof(float) * 2, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

void DXHelloStenciling::CompileShaderFiles()
{
	m_StandardVSByteCode = CompileShader(m_AssetPath + L"HelloStenciling.hlsl", nullptr, "VSMain", "vs_5_0");
	m_StandardPSByteCode = CompileShader(m_AssetPath + L"HelloStenciling.hlsl", nullptr, "PSMain", "ps_5_0");
	// 启用雾效
	D3D_SHADER_MACRO enableFogMacro[] =
	{
		{ "FOG_ENABLE", "1" },
		{ NULL, NULL }
	};
	m_EnableFogPSByteCode = CompileShader(m_AssetPath + L"HelloStenciling.hlsl", enableFogMacro, "PSMain", "ps_5_0");
}

void DXHelloStenciling::BuildPSOs(bool enableFog, D3D12_FILL_MODE fillMode)
{
	m_EnableFog = enableFog;
	m_FillMode = fillMode;
	// 不透明物体PSO
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueDesc{};
	opaqueDesc.NodeMask = 0U;
	opaqueDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	opaqueDesc.pRootSignature = m_RootSignature.Get();
	opaqueDesc.NumRenderTargets = 1U;
	opaqueDesc.RTVFormats[0] = GetBackBufferFormat();
	opaqueDesc.DSVFormat = GetDepthStencilBufferFormat();
	opaqueDesc.SampleDesc.Count = Get4XMSAAEnable() ? 4U : 1U;
	opaqueDesc.SampleDesc.Quality = Get4XMSAAEnable() ? m_4XMSAAQualityLevel - 1 : 0;
	opaqueDesc.SampleMask = UINT_MAX;
	opaqueDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaqueDesc.InputLayout = { m_InputElementDesces.data(), static_cast<UINT>(m_InputElementDesces.size()) };
	opaqueDesc.VS = { reinterpret_cast<BYTE*>(m_StandardVSByteCode->GetBufferPointer()), m_StandardVSByteCode->GetBufferSize() };
	opaqueDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaqueDesc.RasterizerState.FillMode = m_FillMode;
	if (m_EnableFog)
	{
		opaqueDesc.PS = { reinterpret_cast<BYTE*>(m_EnableFogPSByteCode->GetBufferPointer()), m_EnableFogPSByteCode->GetBufferSize() };
	}
	else
	{
		opaqueDesc.PS = { reinterpret_cast<BYTE*>(m_StandardPSByteCode->GetBufferPointer()), m_StandardPSByteCode->GetBufferSize() };
	}
	opaqueDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaqueDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&opaqueDesc, IID_PPV_ARGS(m_PSOs[static_cast<int>(EnumRenderLayer::LayerOpaque)].GetAddressOf())));
}

void DXHelloStenciling::UpdateCamera(float deltaTime, float totalTime)
{
	// 相机位置由球坐标系转为笛卡尔坐标系
	m_CameraPos.x = m_CameraDistance * sinf(m_CameraTheta) * sinf(m_CameraPhi);
	m_CameraPos.y = m_CameraDistance * cosf(m_CameraTheta);
	m_CameraPos.z = m_CameraDistance * sinf(m_CameraTheta) * cosf(m_CameraPhi);
	// 通过相机位置，焦点位置计算观察变换矩阵
	DirectX::XMVECTOR eyePos = DirectX::XMLoadFloat3(&m_CameraPos);
	DirectX::XMVECTOR focusPos = DirectX::XMVectorZero();
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);
	DirectX::XMStoreFloat4x4(&m_ViewMatrix, DirectX::XMMatrixLookAtLH(eyePos, focusPos, up));
}

void DXHelloStenciling::UpdateObjectCB(float deltaTime, float totalTime)
{
	for (size_t i = 0; i < m_AllRenderItem.size(); ++i)
	{
		HelloStencilingRenderItem* pRenderItem = m_AllRenderItem[i].get();
		if (pRenderItem->dirty > 0)
		{
			PerObjectConstants objCBData{};
			DirectX::XMMATRIX texMatrix = DirectX::XMLoadFloat4x4(&pRenderItem->texMatrix);
			DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&pRenderItem->worldMatrix);
			DirectX::XMStoreFloat4x4(&objCBData.worldMatrix, DirectX::XMMatrixTranspose(worldMatrix));
			DirectX::XMStoreFloat4x4(&objCBData.texMatrix, DirectX::XMMatrixTranspose(texMatrix));
			m_ActiveFrameResource->pObjectCBuffer->CopyData(pRenderItem->objectCBufferIndex, objCBData);
			--pRenderItem->dirty;
		}
	}
}

void DXHelloStenciling::UpdateMaterialCB(float delteTime, float totalTime)
{
	for (const auto& iter : m_AllMaterials)
	{
		HelloStencilingMaterial* pMaterial = iter.second.get();
		if (pMaterial->dirty > 0)
		{
			PerMaterialConstants matCBData{};
			matCBData.albedo = pMaterial->diffuseAlbedo;
			matCBData.fresnelR0 = pMaterial->fresnelR0;
			matCBData.roughness = pMaterial->roughness;
			DirectX::XMMATRIX matMatrix = DirectX::XMLoadFloat4x4(&pMaterial->matMatrix);
			DirectX::XMStoreFloat4x4(&matCBData.uvMatrix, DirectX::XMMatrixTranspose(matMatrix));
			m_ActiveFrameResource->pMatCBuffer->CopyData(pMaterial->matCBufferIndex, matCBData);
			--pMaterial->dirty;
		}
	}
}

void DXHelloStenciling::UpdatePassCB(float deltaTime, float totalTime)
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
	passCBData.fogStart = 50.f;
	passCBData.fogRange = 80.f;
	passCBData.ambientLight = m_AmbientLight;
	passCBData.lights[0].strength = m_DirectLight;
	// 按模长为1计算，因此已经是单位向量
	float lightPosX = 1.f * sinf(m_LightTheta) * sinf(m_LightPhi);
	float lightPosY = 1.f * cosf(m_LightTheta);
	float lightPosZ = 1.f * sinf(m_LightTheta) * cosf(m_LightPhi);
	DirectX::XMVECTOR lightDir = DirectX::XMVectorSet(-lightPosX, -lightPosY, -lightPosZ, 1.0f);
	DirectX::XMStoreFloat3(&passCBData.lights[0].direction, lightDir);
	// 第二个光源
	passCBData.lights[1].strength = DirectX::XMFLOAT3(0.5f, 0.5f, 0.4f);
	passCBData.lights[1].direction = DirectX::XMFLOAT3(0.f, -0.866f, 0.5f);

	// 拷贝数据到渲染过程常量缓冲区
	m_ActiveFrameResource->pPassCBuffer->CopyData(0, passCBData);
}

void DXHelloStenciling::PopulateCommandList()
{
	// 清理当前帧资源的指令分配器和指令列表
	ThrowIfFailed(m_ActiveFrameResource->pCommandAllocator->Reset());
	ThrowIfFailed(m_CommandList->Reset(m_ActiveFrameResource->pCommandAllocator.Get(), m_PSOs[static_cast<int>(EnumRenderLayer::LayerOpaque)].Get()));
	// 获取当前的渲染目标缓冲区
	ID3D12Resource* pBackbuffer = m_RenderTargets[m_CurrentBackBufferIndex].Get();
	// 后台缓冲区资源切换到渲染目标状态
	m_CommandList->ResourceBarrier(1U, &CD3DX12_RESOURCE_BARRIER::Transition(pBackbuffer
		, D3D12_RESOURCE_STATE_PRESENT
		, D3D12_RESOURCE_STATE_RENDER_TARGET));

	m_CommandList->RSSetViewports(1U, &m_Viewport);
	m_CommandList->RSSetScissorRects(1U, &m_ScissorRect);
	// 清理后台缓冲区和深度模板缓冲区
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RenderTargetDesciptorHandles[m_CurrentBackBufferIndex];
	m_CommandList->ClearRenderTargetView(rtvHandle, DirectX::Colors::Gray, 0, nullptr);
	m_CommandList->ClearDepthStencilView(m_DepthStencilDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0U, 0U, nullptr);
	// 绑定渲染目标
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, true, &m_DepthStencilDescriptorHandle);
	// 设置根签名
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	// 绑定着色器资源描述符堆
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_SRVDescriptorHeap.Get() };
	m_CommandList->SetDescriptorHeaps(1U, descriptorHeaps);
	// 设置渲染过程根描述符
	m_CommandList->SetGraphicsRootConstantBufferView(3U, m_ActiveFrameResource->pPassCBuffer->GetResource()->GetGPUVirtualAddress());
	// 绘制不透明渲染项
	DrawRenderItem(m_RenderItemLayouts[static_cast<int>(EnumRenderLayer::LayerOpaque)]);

	// 提交IMGUI渲染指令
	IMGuiHelloStenciling::GetInstance()->PopulateDearIMGuiCommand(m_CommandList.Get());

	// 后台缓冲区资源切换到可呈现状态
	m_CommandList->ResourceBarrier(1U, &CD3DX12_RESOURCE_BARRIER::Transition(pBackbuffer
		, D3D12_RESOURCE_STATE_RENDER_TARGET
		, D3D12_RESOURCE_STATE_PRESENT));
	// 指令记录完毕，关闭指令列表，准备提交
	ThrowIfFailed(m_CommandList->Close());
}

void DXHelloStenciling::DrawRenderItem(const std::vector<HelloStencilingRenderItem*>& renderItems)
{
	for (size_t i = 0; i < renderItems.size(); ++i)
	{
		const HelloStencilingRenderItem* pRenderItem = renderItems[i];
		// 绑定图元拓扑，顶点缓冲区，索引缓冲区
		m_CommandList->IASetPrimitiveTopology(pRenderItem->primitiveType);
		m_CommandList->IASetVertexBuffers(0, 1, &pRenderItem->pGeometryMesh->GetVertexBufferView());
		m_CommandList->IASetIndexBuffer(&pRenderItem->pGeometryMesh->GetIndexBufferView());
		// 设置物体常量缓冲区根描述符
		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = m_ActiveFrameResource->pObjectCBuffer->GetResource()->GetGPUVirtualAddress();
		objCBAddress += pRenderItem->objectCBufferIndex * m_ActiveFrameResource->pObjectCBuffer->GetElementSize();
		m_CommandList->SetGraphicsRootConstantBufferView(1U, objCBAddress);
		// 设置材质常量缓冲区根描述符
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = m_ActiveFrameResource->pMatCBuffer->GetResource()->GetGPUVirtualAddress();
		matCBAddress += pRenderItem->pMaterial->matCBufferIndex * m_ActiveFrameResource->pMatCBuffer->GetElementSize();
		m_CommandList->SetGraphicsRootConstantBufferView(2U, matCBAddress);
		// 设置着色器资源描述符表
		CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(m_SRVDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		srvHandle.Offset(pRenderItem->pMaterial->diffuseMapIndex, m_CBVUAVDescriptorSize);
		m_CommandList->SetGraphicsRootDescriptorTable(0U, srvHandle);
		// 绘制
		m_CommandList->DrawIndexedInstanced(pRenderItem->indexCount, 1U, pRenderItem->startIndexLoaction, pRenderItem->startVertexLocation, 0U);
	}
}