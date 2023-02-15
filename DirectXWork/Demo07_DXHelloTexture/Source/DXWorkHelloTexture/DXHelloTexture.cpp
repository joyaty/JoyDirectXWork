/**
 * DXHelloTexture.cpp
 * 纹理贴图实例程序代码文件
 **/

#include "stdafx.h"
#include "DXHelloTexture.h"
#include "IMGuiHelloTexture.h"
#include "DirectXBaseWork/GeometryGenerator.h"
#include "DirectXBaseWork/DDSTextureLoader.h"
#include <DirectXColors.h>

using namespace DirectX;

/// <summary>
/// 帧资源数定义
/// </summary>
const int kFrameResourceCount = 3;

DXHelloTexture::DXHelloTexture(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
	m_AssetPath += L"\\DirectXWork\\Demo07_DXHelloTexture\\Source\\DXWorkHelloTexture\\";
}

DXHelloTexture::~DXHelloTexture()
{
}

void DXHelloTexture::OnMouseDown(UINT8 keyCode, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void DXHelloTexture::OnMouseUp(UINT8 keyCode, int x, int y)
{
}

void DXHelloTexture::OnMouseMove(UINT8 keyCode, int x, int y)
{
	if ((keyCode & MK_LBUTTON) != 0)
	{
		// 左键点击，旋转
		float dx = DirectX::XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos.x));
		float dy = DirectX::XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos.y));
		m_Phi += dx;
		m_Theta += dy;
		m_Theta = MathUtil::Clamp(m_Theta, 0.1f, MathUtil::Pi - 0.1f);
	}
	else if ((keyCode & MK_RBUTTON) != 0)
	{
		// 右键点击，缩放
		float dx = 0.005f * static_cast<float>(x - m_LastMousePos.x);
		float dy = 0.005f * static_cast<float>(y - m_LastMousePos.y);
		m_Radius += dx - dy;
		m_Radius = MathUtil::Clamp(m_Radius, 3.f, 15.f);
	}
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void DXHelloTexture::OnResize(UINT width, UINT height)
{
	DirectXBaseWork::OnResize(width, height);
	// 根据FOV，视口纵横比，近远平面计算投影矩阵
	DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(0.25f * MathUtil::Pi, GetAspectRatio(), 1.f, 1000.f);
	DirectX::XMStoreFloat4x4(&m_ProjMatrix, projMatrix);
}

void DXHelloTexture::OnKeyboardInput(float deltaTime, float totalTime)
{
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		m_SunPhi -= 1.0f * deltaTime;

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		m_SunPhi += 1.0f * deltaTime;

	if (GetAsyncKeyState(VK_UP) & 0x8000)
		m_SunTheta -= 1.0f * deltaTime;

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		m_SunTheta += 1.0f * deltaTime;

	m_SunTheta = MathUtil::Clamp(m_SunTheta, 0.1f, DirectX::XM_PIDIV2);
}

bool DXHelloTexture::OnInit()
{
	// 等待基类中的初始化指令
	FlushCommandQueue();
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));
	// 实例程序初始化
	BuildGeometry();
	LoadTexture();
	BuildSampler();
	BuildStaticSamplers();
	BuildMaterial();
	BuildRenderItem();
	BuildFrameResource();
	BuildInputLayout();
	BuildRootSignature();
	CompileShaderFile();
	BuildPSO();
	// 执行实例初始化指令
	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(1, cmdList);
	FlushCommandQueue();
	// 示例程序实例保存到DearIMGui调试面板上，便于调试操作
	IMGuiHelloTexture::GetInstance()->SetHelloTextureDemo(this);

	return true;
}

void DXHelloTexture::OnUpdate(float deltaTime, float totalTime)
{
	// 循环利用帧资源
	m_CurrentFrameResourceIndex = (m_CurrentFrameResourceIndex + 1) % kFrameResourceCount;
	m_pCurrentFrameResource = m_FrameResourses[m_CurrentFrameResourceIndex].get();
	if (m_pCurrentFrameResource->fenceValue != 0 && m_pCurrentFrameResource->fenceValue > m_Fence->GetCompletedValue())
	{
		// 当前帧资源还未处理，需要等待
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(m_Fence->SetEventOnCompletion(m_pCurrentFrameResource->fenceValue, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
	OnKeyboardInput(deltaTime, totalTime);
	// 更新常量缓冲区数据
	UpdateCamera(deltaTime, totalTime);
	UpdateObjectCB(deltaTime, totalTime);
	UpdateMatCB(deltaTime, totalTime);
	UpdatePassCB(deltaTime, totalTime);
}

void DXHelloTexture::OnRender()
{
	// 记录当前帧渲染指令
	PopulateCommandList();
	// 提交指令列表
	ID3D12CommandList* cmdList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(1U, cmdList);
	// 交换后台缓冲区
	ThrowIfFailed(m_SwapChain->Present(1, 0));
	// 更新当前使用帧资源的围栏值，并在渲染队列上设置该围栏值
	m_pCurrentFrameResource->fenceValue = ++m_FenceValue;
	m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue);
}

void DXHelloTexture::OnDestroy()
{
}

void DXHelloTexture::BuildInputLayout()
{
	m_InputElements =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 3 * sizeof(float), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 3 * sizeof(float) * 2, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

void DXHelloTexture::CompileShaderFile()
{
	m_VSByteCode = CompileShader(m_AssetPath + L"HelloTexture.hlsl", nullptr, "VSMain", "vs_5_0");
	m_PSByteCode = CompileShader(m_AssetPath + L"HelloTexture.hlsl", nullptr, "PSMain", "ps_5_0");
}

void DXHelloTexture::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE range[2]{};
	range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	range[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

	// 构造根参数
	CD3DX12_ROOT_PARAMETER rootParameters[5]{};
	rootParameters[0].InitAsConstantBufferView(0);		// 物体常量缓冲区根描述符
	rootParameters[1].InitAsConstantBufferView(1);		// 材质常量缓冲区根描述符
	rootParameters[2].InitAsConstantBufferView(2);		// 渲染过程常量缓冲区根描述符
	rootParameters[3].InitAsDescriptorTable(1, &range[0], D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[4].InitAsDescriptorTable(1, &range[1], D3D12_SHADER_VISIBILITY_PIXEL);
	// 构造根签名描述
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(5, rootParameters, (UINT)(m_StaticSamplers.size()), m_StaticSamplers.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	// 序列化根签名
	Microsoft::WRL::ComPtr<ID3DBlob> pSerializeRootSignature{ nullptr };
	Microsoft::WRL::ComPtr<ID3DBlob> pErrorMsg{ nullptr };
	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, pSerializeRootSignature.GetAddressOf(), pErrorMsg.GetAddressOf());
	if (pErrorMsg != nullptr)
	{
		::OutputDebugStringA((char*)pErrorMsg->GetBufferPointer());
	}
	ThrowIfFailed(hr);
	// 创建根签名
	ThrowIfFailed(m_Device->CreateRootSignature(0U, pSerializeRootSignature->GetBufferPointer(), pSerializeRootSignature->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf())));
}

void DXHelloTexture::BuildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	pipelineDesc.NodeMask = 0U;
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineDesc.pRootSignature = m_RootSignature.Get();
	pipelineDesc.InputLayout = { m_InputElements.data(), (UINT)m_InputElements.size() };
	pipelineDesc.VS = { reinterpret_cast<BYTE*>(m_VSByteCode->GetBufferPointer()), m_VSByteCode->GetBufferSize() };
	pipelineDesc.PS = { reinterpret_cast<BYTE*>(m_PSByteCode->GetBufferPointer()), m_PSByteCode->GetBufferSize() };
	pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	// pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineDesc.NumRenderTargets = 1U;
	pipelineDesc.RTVFormats[0] = GetBackBufferFormat();
	pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipelineDesc.DSVFormat = GetDepthStencilBufferFormat();
	pipelineDesc.SampleMask = UINT_MAX;
	pipelineDesc.SampleDesc.Count = Get4XMSAAEnable() ? 4 : 1;
	pipelineDesc.SampleDesc.Quality = Get4XMSAAEnable() ? m_4XMSAAQualityLevel - 1 : 0;
	m_Device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(m_PSO.GetAddressOf()));
}

void DXHelloTexture::BuildGeometry()
{
	GeometryGenerator::MeshData cubeMesh = GeometryGenerator::CreateCube(4.0f, 4.0f, 4.0f, 0);
	// 获取顶点数据
	UINT verticeCount = (UINT)cubeMesh.Vertices.size();
	std::vector<HelloTextureVertex> vertices(verticeCount);
	for (size_t i = 0; i < cubeMesh.Vertices.size(); ++i)
	{
		vertices[i].position = cubeMesh.Vertices[i].Position;
		vertices[i].normal = cubeMesh.Vertices[i].Normal;
		vertices[i].texCoord0 = cubeMesh.Vertices[i].TexCoord;
	}
	// 索引数据
	UINT indiceCount = (UINT)cubeMesh.GetIndices16().size();
	std::vector<std::uint16_t> indices{};
	indices.insert(indices.end(), cubeMesh.GetIndices16().begin(), cubeMesh.GetIndices16().end());
	// 在默认堆创建顶点缓冲区资源和索引缓冲区资源，拷贝顶点数据到GPU上
	UINT vertexBufferSize = verticeCount * sizeof(HelloTextureVertex);
	UINT indexBufferSize = indiceCount * sizeof(std::uint16_t);
	std::unique_ptr<MeshGeometry> pCubeGeo = std::make_unique<MeshGeometry>();
	pCubeGeo->m_Name = "CubeGeo";
	pCubeGeo->m_VertexBufferCPU = nullptr;
	pCubeGeo->m_VertexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), vertices.data(), vertexBufferSize, pCubeGeo->m_VertexBufferUploader);
	pCubeGeo->m_VertexSize = vertexBufferSize;
	pCubeGeo->m_VertexStride = sizeof(HelloTextureVertex);
	pCubeGeo->m_IndexBufferCPU = nullptr;
	pCubeGeo->m_IndexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), indices.data(), indexBufferSize, pCubeGeo->m_IndexBufferUploader);
	pCubeGeo->m_IndexFormat = DXGI_FORMAT_R16_UINT;
	pCubeGeo->m_IndexSize = indexBufferSize;
	// 只有一个Cube的子网格，无需偏移
	SubMeshGeometry subCube{};
	subCube.m_IndexCount = indiceCount;
	subCube.m_StartIndexLocation = 0;
	subCube.m_BaseVertexLocation = 0;
	pCubeGeo->m_SubMeshGeometrys["Cube"] = subCube;
	m_Geometrys[pCubeGeo->m_Name] = std::move(pCubeGeo);
}

void DXHelloTexture::LoadTexture()
{
	// 创建纹理资源
	std::unique_ptr<Texture> pTexture = std::make_unique<Texture>();
	pTexture->name = "WoodCrate";
	pTexture->fileName = m_AssetRootPath + L"\\Assets\\Textures\\WoodCrate02.dds";
	ThrowIfFailed(CreateDDSTextureFromFile12(m_Device.Get(), m_CommandList.Get(), pTexture->fileName.c_str(), pTexture->m_TextureGPU, pTexture->m_TextureUpload));
	// 创建着色器资源描述符堆
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NumDescriptors = 1U;
	heapDesc.NodeMask = 0U;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_SRVDescriptorHeap.GetAddressOf())));
	// 创建着色器资源描述符
	D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = pTexture->m_TextureGPU->GetDesc().Format;		// 格式，使用资源的格式类型，若资源是无类型格式，则此处必须填写具体的格式类型
	shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;			// 资源的维数，常见类型: Texture1D/Texture2D/Texture3D/TextureCube
	shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;	// 纹理采样返回的纹理数据向量。此字段可以对返回纹理数据向量进行重新排序，不需要则指定D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING，返回默认的数据顺序
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0U;			// 最详尽的mipmap层级索引
	shaderResourceViewDesc.Texture2D.MipLevels = pTexture->m_TextureGPU->GetDesc().MipLevels;	// mipmap层级数量
	shaderResourceViewDesc.Texture2D.PlaneSlice = 0U;				// 平面切片的索引
	shaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.f;		// 可访问的最新mipmap层级
	// 创建着色器资源描述符
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_SRVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	m_Device->CreateShaderResourceView(pTexture->m_TextureGPU.Get(), &shaderResourceViewDesc, handle);
	m_Textures[pTexture->name] = std::move(pTexture);
}

void DXHelloTexture::BuildSampler()
{
	// 创建采样器描述符堆
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.NumDescriptors = 1U;
	heapDesc.NodeMask = 0U;
	m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_SamplerDescriptorHeap.GetAddressOf()));
	// 创建采样器描述符
	D3D12_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // 纹理图双线性过滤，mipmap层级间线性过滤，可称为三线性过滤
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 纹理水平u轴采用重复寻址模式
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 纹理水平v轴采用重复寻址模式
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 纹理水平w轴采用重复寻址模式
	samplerDesc.MinLOD = 0.f; // 可供选择的最小mipmap层级
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX; // 可供选择的最大mipmap层级
	samplerDesc.MipLODBias = 0.f; // mipmap偏移值，例如值为2，mipmap为3，则按层级3 + 2，即mipmap层级=5采样
	samplerDesc.MaxAnisotropy = 1; // 最大各向异性值，对于Filter = D3D12_FILTER_ANISOTROPIC或D3D12_FILTER_COMPARISON_ANISOTROPIC生效，值域[1,16]，值越大，消耗越大，效果越好
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS; // 用于实现阴影贴图（shadow mapping）这类效果的高级选项，目前不使用，设置为D3D12_COMPARISON_FUNC_ALWAYS
	m_Device->CreateSampler(&samplerDesc, m_SamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void DXHelloTexture::BuildStaticSamplers()
{
	// 纹理和mipmap点过滤 + wrap寻址模式
	m_StaticSamplers[0].Init(1,
		D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP);
	// 纹理和mipmap点过滤 + clamp寻址模式
	m_StaticSamplers[1].Init(2,
		D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
	// 纹理和mipmap线性过滤 + wrap寻址模式
	m_StaticSamplers[2].Init(3,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP);
	// 纹理和mipmap线性过滤 + clamp寻址模式
	m_StaticSamplers[3].Init(4,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
	// 纹理和mipmap各向异性过滤 + wrap寻址模式
	m_StaticSamplers[4].Init(5,
		D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		0.f,
		8U);
	// 纹理和mipmap各向异性过滤 + clamp寻址模式
	m_StaticSamplers[5].Init(6,
		D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		0.f,
		8U);
}

void DXHelloTexture::BuildMaterial()
{
	std::unique_ptr<HelloTextureMaterial> pMaterial = std::make_unique<HelloTextureMaterial>();
	pMaterial->name = "DefaultMat";
	pMaterial->albedo = DirectX::XMFLOAT4(1.f, 1.0f, 1.0f, 1.0f);
	pMaterial->fresnelR0 = DirectX::XMFLOAT3(0.95f, 0.93f, 0.88f);
	pMaterial->roughness = 0.25f;
	pMaterial->matCBIndex = 0;
	pMaterial->diffuseMapIndex = 0;
	m_Materials[pMaterial->name] = std::move(pMaterial);
}

void DXHelloTexture::BuildRenderItem()
{
	// 构造RenderItem
	std::unique_ptr<HelloTextureRenderItem> pRenderItem = std::make_unique<HelloTextureRenderItem>();
	DirectX::XMStoreFloat4x4(&pRenderItem->worldMatrix, DirectX::XMMatrixTranslation(0.f, 0.f, 0.f));
	pRenderItem->objectCBIndex = 0;
	pRenderItem->pMeshGeo = m_Geometrys["CubeGeo"].get();
	pRenderItem->pMaterial = m_Materials["DefaultMat"].get();
	pRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pRenderItem->indexCount = pRenderItem->pMeshGeo->m_SubMeshGeometrys["Cube"].m_IndexCount;
	pRenderItem->startIndexOffset = pRenderItem->pMeshGeo->m_SubMeshGeometrys["Cube"].m_StartIndexLocation;
	pRenderItem->startVertexOffset = pRenderItem->pMeshGeo->m_SubMeshGeometrys["Cube"].m_BaseVertexLocation;
	m_AllRenderItems.push_back(std::move(pRenderItem));
}

void DXHelloTexture::BuildFrameResource()
{
	for (int i = 0; i < kFrameResourceCount; ++i)
	{
		m_FrameResourses.push_back(std::make_unique<HelloTextureFrameResource>(m_Device.Get(), (int)m_Geometrys.size(), (int)m_Materials.size(), 1));
	}
	m_CurrentBackBufferIndex = 0;
	m_pCurrentFrameResource = m_FrameResourses[m_CurrentFrameResourceIndex].get();
}

void DXHelloTexture::UpdateCamera(float deltaTime, float totalTime)
{
	// 极坐标计算出相机为笛卡尔坐标位置
	m_EyesPos.x = m_Radius * sinf(m_Theta) * cosf(m_Phi);
	m_EyesPos.y = m_Radius * cosf(m_Theta);
	m_EyesPos.z = m_Radius * sinf(m_Theta) * sinf(m_Phi);
	// 根据相机位置，视点位置，向上的方向向量构建观察变换矩阵
	DirectX::XMVECTOR lookPos = DirectX::XMLoadFloat3(&m_EyesPos);
	DirectX::XMVECTOR foucsPos = DirectX::XMVectorZero();
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(lookPos, foucsPos, up);
	DirectX::XMStoreFloat4x4(&m_ViewMatrix, viewMatrix);
}

void DXHelloTexture::UpdateObjectCB(float deltaTime, float totalTime)
{
	for (size_t i = 0; i < m_AllRenderItems.size(); ++i)
	{
		HelloTextureRenderItem* pRenderItem = m_AllRenderItems[i].get();
		if (pRenderItem->numDirty > 0)
		{
			DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&pRenderItem->worldMatrix);
			PerObjectConstants objContants{};
			DirectX::XMStoreFloat4x4(&objContants.worldMatrix, DirectX::XMMatrixTranspose(worldMatrix));
			// 拷贝常量数据到物体常量缓冲区
			m_pCurrentFrameResource->pPerObjCB->CopyData(m_AllRenderItems[i]->objectCBIndex, objContants);
			--pRenderItem->numDirty;
		}
	}
}

void DXHelloTexture::UpdateMatCB(float deltaTime, float totalTime)
{
	for (const auto& iter : m_Materials)
	{
		HelloTextureMaterial* pMaterial = iter.second.get();
		if (pMaterial->numDirty > 0)
		{
			PerMaterialConstants matConstant{ pMaterial->albedo, pMaterial->fresnelR0, pMaterial->roughness };
			m_pCurrentFrameResource->pPerMatCB->CopyData(pMaterial->matCBIndex, matConstant);
			--pMaterial->numDirty;
		}
	}
}

void DXHelloTexture::UpdatePassCB(float deltaTime, float totalTime)
{
	// 计算观察投影矩阵以及相关的逆矩阵
	DirectX::XMMATRIX viewMatrix = DirectX::XMLoadFloat4x4(&m_ViewMatrix);
	DirectX::XMMATRIX projMatrix = DirectX::XMLoadFloat4x4(&m_ProjMatrix);
	DirectX::XMMATRIX viewProjMatrix = DirectX::XMMatrixMultiply(viewMatrix, projMatrix);
	DirectX::XMMATRIX invViewMatrix = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(viewMatrix), viewMatrix);
	DirectX::XMMATRIX invProjMatrix = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(projMatrix), projMatrix);
	DirectX::XMMATRIX invViewProjMatrix = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(viewProjMatrix), viewProjMatrix);
	// 填充常量缓冲区数据
	PerPassConstancts passObj{};
	DirectX::XMStoreFloat4x4(&passObj.viewMatrix, DirectX::XMMatrixTranspose(viewMatrix));
	DirectX::XMStoreFloat4x4(&passObj.invViewMatrix, DirectX::XMMatrixTranspose(invViewMatrix));
	DirectX::XMStoreFloat4x4(&passObj.projMatrix, DirectX::XMMatrixTranspose(projMatrix));
	DirectX::XMStoreFloat4x4(&passObj.invProjMatrix, DirectX::XMMatrixTranspose(invProjMatrix));
	DirectX::XMStoreFloat4x4(&passObj.viewProjMatrix, DirectX::XMMatrixTranspose(viewProjMatrix));
	DirectX::XMStoreFloat4x4(&passObj.invViewProjMatrix, DirectX::XMMatrixTranspose(invViewProjMatrix));
	passObj.eyeWorldPos = m_EyesPos;
	passObj.renderTargetSize = DirectX::XMFLOAT2(static_cast<float>(GetWidth()), static_cast<float>(GetHeight()));
	passObj.invRenderTargetSize = DirectX::XMFLOAT2(1.f / GetWidth(), 1.f / GetHeight());
	passObj.nearZ = 1.0f;
	passObj.farZ = 1000.0f;
	passObj.deltaTime = deltaTime;
	passObj.totalTime = totalTime;
	// 环境光源数据
	passObj.ambientLight = m_AmbientLight;
	// 光源数据
	DirectX::XMFLOAT3 sunPos{};
	sunPos.x = 1.f * sinf(m_SunTheta) * cosf(m_SunPhi);
	sunPos.y = 1.f * cosf(m_SunTheta);
	sunPos.z = 1.f * sinf(m_SunTheta) * sinf(m_SunPhi);
	DirectX::XMVECTOR lightDir = -DirectX::XMVectorSet(sunPos.x, sunPos.y, sunPos.z, 1.f);
	DirectX::XMStoreFloat3(&passObj.lights[0].direction, lightDir);
	passObj.lights[0].strength = m_DirectLight;

	// 拷贝数据Pass常量缓冲区上
	m_pCurrentFrameResource->pPerPassCB->CopyData(0, passObj);
}

void DXHelloTexture::PopulateCommandList()
{
	// 重置当前帧资源的命令分配器和命令列表
	ThrowIfFailed(m_pCurrentFrameResource->pCommandAllocator->Reset());
	ThrowIfFailed(m_CommandList->Reset(m_pCurrentFrameResource->pCommandAllocator.Get(), m_PSO.Get()));
	// 切换后台缓冲区为渲染目标状态
	ID3D12Resource* pCurrentBackBuffer = m_RenderTargets[m_CurrentBackBufferIndex].Get();
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		pCurrentBackBuffer
		, D3D12_RESOURCE_STATE_PRESENT
		, D3D12_RESOURCE_STATE_RENDER_TARGET));
	// 设置视口和裁剪区域
	m_CommandList->RSSetViewports(1, &m_Viewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);
	// 清理渲染目标缓冲区和深度模板缓冲区
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RenderTargetDesciptorHandles[m_CurrentBackBufferIndex];
	m_CommandList->ClearRenderTargetView(rtvHandle, DirectX::Colors::Gray, 0, nullptr);
	m_CommandList->ClearDepthStencilView(m_DepthStencilDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);
	// 绑定渲染目标和深度模板缓冲区
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, true, &m_DepthStencilDescriptorHandle);
	// 设置使用的描述符堆
	ID3D12DescriptorHeap* samplerHeaps[] = { m_SamplerDescriptorHeap.Get(), m_SRVDescriptorHeap.Get() };
	m_CommandList->SetDescriptorHeaps(2, samplerHeaps);
	// 绑定根签名
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	// 绑定采样器
	m_CommandList->SetGraphicsRootDescriptorTable(4, m_SamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	// 绑定渲染过程常量缓冲区描述符
	m_CommandList->SetGraphicsRootConstantBufferView(2, m_pCurrentFrameResource->pPerPassCB->GetResource()->GetGPUVirtualAddress());
	// 绘制渲染项
	DrawRenderItem();
	// 提交DearIMGui的渲染命令
	if (IMGuiHelloTexture::GetInstance() != nullptr)
	{
		IMGuiHelloTexture::GetInstance()->PopulateDearIMGuiCommand(m_CommandList.Get());
	}
	// 切换后台缓冲区为呈现状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		pCurrentBackBuffer
		, D3D12_RESOURCE_STATE_RENDER_TARGET
		, D3D12_RESOURCE_STATE_PRESENT));

	// 记录指令完毕，关闭指令列表
	ThrowIfFailed(m_CommandList->Close());
}

void DXHelloTexture::DrawRenderItem()
{
	for (auto& pRenderItem : m_AllRenderItems)
	{
		// 绑定顶点缓冲区和索引缓冲区描述符
		m_CommandList->IASetVertexBuffers(0, 1, &pRenderItem->pMeshGeo->GetVertexBufferView());
		m_CommandList->IASetIndexBuffer(&pRenderItem->pMeshGeo->GetIndexBufferView());
		// 绑定图元拓扑
		m_CommandList->IASetPrimitiveTopology(pRenderItem->primitiveType);
		// 绑定漫反射反照率贴图
		CD3DX12_GPU_DESCRIPTOR_HANDLE handle(m_SRVDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		handle.Offset(pRenderItem->pMaterial->diffuseMapIndex * m_CBVUAVDescriptorSize);
		m_CommandList->SetGraphicsRootDescriptorTable(3, handle);
		// 绑定物体常量缓冲区描述符
		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = m_pCurrentFrameResource->pPerObjCB->GetResource()->GetGPUVirtualAddress();
		objCBAddress += pRenderItem->objectCBIndex * m_pCurrentFrameResource->pPerObjCB->GetElementSize();
		m_CommandList->SetGraphicsRootConstantBufferView(0, objCBAddress);
		// 绑定材质常量缓冲区描述符
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = m_pCurrentFrameResource->pPerMatCB->GetResource()->GetGPUVirtualAddress();
		matCBAddress += pRenderItem->pMaterial->matCBIndex * m_pCurrentFrameResource->pPerMatCB->GetElementSize();
		m_CommandList->SetGraphicsRootConstantBufferView(1, matCBAddress);
		// 绘制
		m_CommandList->DrawIndexedInstanced(pRenderItem->indexCount, 1, pRenderItem->startIndexOffset, pRenderItem->startVertexOffset, 0);
	}
}