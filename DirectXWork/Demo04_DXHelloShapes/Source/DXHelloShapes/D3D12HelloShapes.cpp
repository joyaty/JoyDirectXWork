/*
 * D3D12HelloShapes.cpp
 * Demo04 - DirectX12渲染多个几何图形
 */

#include "stdafx.h"
#include "D3D12HelloShapes.h"
#include <DirectXColors.h>
#include "DirectXBaseWork/GeometryGenerator.h"
#include "HelloShapesRenderItem.h"
#include "HelloShapesFrameResource.h"
#include "IMGuiHelloShapes.h"


 /// <summary>
 /// 帧资源的个数
 /// </summary>
const int kNumFrameResource = 3;

D3D12HelloShapes::D3D12HelloShapes(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
	m_AssetPath += L"\\DirectXWork\\Demo04_DXHelloShapes\\Source\\DXHelloShapes\\";
}

D3D12HelloShapes::~D3D12HelloShapes()
{
}

void D3D12HelloShapes::OnMouseDown(UINT8 keyCode, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void D3D12HelloShapes::OnMouseUp(UINT8 keyCode, int x, int y)
{
}

void D3D12HelloShapes::OnMouseMove(UINT8 keyCode, int x, int y)
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

void D3D12HelloShapes::OnResize(UINT width, UINT height)
{
	DirectXBaseWork::OnResize(width, height);
	// 根据FOV，视口纵横比，近远平面计算投影矩阵
	DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(0.25f * MathUtil::Pi, GetAspectRatio(), 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&m_ProjMatrix, projMatrix);
}

bool D3D12HelloShapes::OnInit()
{
	// 之前基类初始化指令
	FlushCommandQueue();
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));

	BuildInputLayout();
	BuildRootSignature();
	CompileShaderFile();
	BuildShapeGeometry();
	BuildRenderItem();
	BuildFrameResource();
	BuildCBVDescriptorHeaps();
	BuildConstantBufferView();
	BuildPSO(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_WIREFRAME);
	// 执行初始化指令
	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdList), cmdList);

	FlushCommandQueue();

	// IMGui调试面板持有HelloShapesDemo
	IMGuiHelloShapes::GetInstance()->SetHelloShapesDemo(this);

	return true;
}

void D3D12HelloShapes::OnUpdate(float deltaTime, float totalTime)
{
	UpdateCamera(deltaTime, totalTime);
	// 循环往复获取帧资源循环数组中的帧资源
	m_CurrentFrameResourceIndex = (m_CurrentFrameResourceIndex + 1) % kNumFrameResource;
	m_CurrentFrameResource = m_FrameResources[m_CurrentFrameResourceIndex].get();
	// 当前完成的围栏值小于帧资源标记的围栏值，说明需要CPU需要等待GPU完成当前帧资源的绘制
	if (m_CurrentFrameResource->fenceValue != 0 && m_CurrentFrameResource->fenceValue > m_Fence->GetCompletedValue())
	{
		HANDLE eventHanle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrentFrameResource->fenceValue, eventHanle));
		WaitForSingleObject(eventHanle, INFINITE);
		CloseHandle(eventHanle);
	}
	// 更新常量缓冲区数据
	UpdateObjectCBs(deltaTime, totalTime);
	UpdatePassCBs(deltaTime, totalTime);
}

void D3D12HelloShapes::OnRender()
{
	// 记录所有的渲染指令
	PopulateCommandList();
	// 提交指令列表
	ID3D12CommandList* ppCommandList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
	// 交换缓冲区
	ThrowIfFailed(m_SwapChain->Present(1, 0));

	// 增加围栏值
	m_CurrentFrameResource->fenceValue = ++m_FenceValue;
	// 命令队列添加新的指令，设置一个围栏值
	m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue);
}

void D3D12HelloShapes::OnDestroy()
{
}

void D3D12HelloShapes::UpdateObjectCBs(float deltaTime, float totalTime)
{
	// 获取当前使用的帧资源
	UploadBuffer<ObjectConstants>* pObjectCB = m_CurrentFrameResource->pObjectConstants.get();
	for (auto& pRenderItem : m_AllRenderItems)
	{
		// RenderItem数据处于脏标记下，才需要更新常量缓冲区
		if (pRenderItem->numFramesDirty > 0)
		{
			DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&pRenderItem->worldMatrix);
			ObjectConstants objCB{};
			DirectX::XMStoreFloat4x4(&objCB.worldMatrix, DirectX::XMMatrixTranspose(worldMatrix));
			// 拷贝到当前索引的常量缓冲区
			pObjectCB->CopyData(pRenderItem->objectCBIndex, objCB);
			// 脏标记 - 1
			--pRenderItem->numFramesDirty;
		}
	}
}

void D3D12HelloShapes::UpdatePassCBs(float deltaTime, float totalTime)
{
	// 计算观察投影矩阵以及相关的逆矩阵
	DirectX::XMMATRIX viewMatrix = DirectX::XMLoadFloat4x4(&m_ViewMatrix);
	DirectX::XMMATRIX projMatrix = DirectX::XMLoadFloat4x4(&m_ProjMatrix);
	DirectX::XMMATRIX viewProjMatrix = DirectX::XMMatrixMultiply(viewMatrix, projMatrix);
	DirectX::XMMATRIX invViewMatrix = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(viewMatrix), viewMatrix);
	DirectX::XMMATRIX invProjMatrix = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(projMatrix), projMatrix);
	DirectX::XMMATRIX invViewProjMatrix = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(viewProjMatrix), viewProjMatrix);
	// 填充常量缓冲区数据
	PassConstants passObj{};
	DirectX::XMStoreFloat4x4(&passObj.viewMatrix, DirectX::XMMatrixTranspose(viewMatrix));
	DirectX::XMStoreFloat4x4(&passObj.invViewMatrix, DirectX::XMMatrixTranspose(invViewMatrix));
	DirectX::XMStoreFloat4x4(&passObj.projMatrix, DirectX::XMMatrixTranspose(projMatrix));
	DirectX::XMStoreFloat4x4(&passObj.invProjMatrix, DirectX::XMMatrixTranspose(invProjMatrix));
	DirectX::XMStoreFloat4x4(&passObj.viewProjMatrix, DirectX::XMMatrixTranspose(viewProjMatrix));
	DirectX::XMStoreFloat4x4(&passObj.invProjMatrix, DirectX::XMMatrixTranspose(invProjMatrix));
	passObj.eyeWorldPos = m_EyesPos;
	passObj.renderTargetSize = DirectX::XMFLOAT2(static_cast<float>(GetWidth()), static_cast<float>(GetHeight()));
	passObj.invRenderTargetSize = DirectX::XMFLOAT2(1.f / GetWidth(), 1.f / GetHeight());
	passObj.nearZ = 1.0f;
	passObj.farZ = 1000.0f;
	passObj.deltaTime = deltaTime;
	passObj.totalTime = totalTime;
	// 拷贝数据Pass常量缓冲区上
	m_CurrentFrameResource->pPassConstants->CopyData(0, passObj);
}

void D3D12HelloShapes::UpdateCamera(float deltaTime, float totalTime)
{
	// 观察者位置
	m_EyesPos.x = m_Radius * sinf(m_Theta) * cosf(m_Phi);
	m_EyesPos.y = m_Radius * cosf(m_Theta);
	m_EyesPos.z = m_Radius * sinf(m_Theta) * sinf(m_Phi);
	// 根据相机位置，视点位置，向上的方向向量构建观察变换矩阵
	DirectX::XMVECTOR cameraPos = DirectX::XMVectorSet(m_EyesPos.x, m_EyesPos.y, m_EyesPos.z, 1.0f);
	DirectX::XMVECTOR target = DirectX::XMVectorZero();
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(cameraPos, target, up);
	DirectX::XMStoreFloat4x4(&m_ViewMatrix, viewMatrix);
}

void D3D12HelloShapes::BuildInputLayout()
{
	m_InputElementDescs =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 3 * sizeof(float), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
}

void D3D12HelloShapes::BuildRootSignature()
{
	// 本例有物体常量缓冲区和Pass常量缓冲区
	CD3DX12_ROOT_PARAMETER slotRootParameter[2]{};
	// 物体的常量缓冲区描述符表
	CD3DX12_DESCRIPTOR_RANGE cbvTable0{};
	cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	// 渲染过程常量缓冲区描述符表
	CD3DX12_DESCRIPTOR_RANGE cbvTable1{};
	cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
	// 填充到根参数中
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable0);
	slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable1);
	// 根签名描述符由一组根参数构成
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	// 需要先将根参数序列化处理
	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig{ nullptr };
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob{ nullptr };
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());
	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);
	// 创建根签名
	ThrowIfFailed(m_Device->CreateRootSignature(0U, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf())));
}

void D3D12HelloShapes::UpdatePSO(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode)
{
	FlushCommandQueue();

	if (m_PSO != nullptr)
	{
		m_PSO.Reset();
	}
	BuildPSO(cullMode, fillMode);
}

void D3D12HelloShapes::BuildPSO(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
	ZeroMemory(&pipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	pipelineStateDesc.NodeMask = 0U;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.InputLayout = { m_InputElementDescs.data(), (UINT)m_InputElementDescs.size() };
	pipelineStateDesc.pRootSignature = m_RootSignature.Get();
	pipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_VSByteCode->GetBufferPointer()), m_VSByteCode->GetBufferSize() };
	pipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_PSByteCode->GetBufferPointer()), m_PSByteCode->GetBufferSize() };
	pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipelineStateDesc.RasterizerState.FillMode = fillMode;
	pipelineStateDesc.RasterizerState.CullMode = cullMode;
	pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipelineStateDesc.NumRenderTargets = 1U;
	pipelineStateDesc.RTVFormats[0] = GetBackBufferFormat();
	pipelineStateDesc.DSVFormat = GetDepthStencilBufferFormat();
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.SampleDesc.Count = Get4XMSAAEnable() ? 4 : 1;
	pipelineStateDesc.SampleDesc.Quality = Get4XMSAAEnable() ? m_4XMSAAQualityLevel - 1 : 0;

	ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(m_PSO.GetAddressOf())));
}

void D3D12HelloShapes::CompileShaderFile()
{
	m_VSByteCode = CompileShader(m_AssetPath + L"HelloShapeShader.hlsl", nullptr, "VSMain", "vs_5_0");
	m_PSByteCode = CompileShader(m_AssetPath + L"HelloShapeShader.hlsl", nullptr, "PSMain", "ps_5_0");
}

void D3D12HelloShapes::BuildShapeGeometry()
{
	// 构建Cube几何体
	GeometryGenerator::MeshData cube = GeometryGenerator::CreateCube(2.f, 2.f, 2.f, 0);
	GeometryGenerator::MeshData cylinder = GeometryGenerator::CreateCylinder(1.f, 1.f, 2.f, 16, 1);
	// 顶点总数
	size_t totalVertexCount = cube.Vertices.size() + cylinder.Vertices.size();
	// 构建所有Shape的顶点数据
	std::vector<ShapeVertex> vertices(totalVertexCount);
	for (size_t i = 0; i < cube.Vertices.size(); ++i)
	{
		ShapeVertex vert{};
		vert.position = cube.Vertices[i].Position;
		vert.color = DirectX::XMFLOAT4(DirectX::Colors::Red);
		vertices[i] = vert;
	}
	for (size_t i = 0; i < cylinder.Vertices.size(); ++i)
	{
		ShapeVertex vert{};
		vert.position = cylinder.Vertices[i].Position;
		vert.color = DirectX::XMFLOAT4(DirectX::Colors::Blue);
		vertices[cube.Vertices.size() + i] = vert;
	}
	// 构建所有Shape的索引数据
	std::vector<std::uint16_t> indices{};
	indices.insert(indices.end(), cube.GetIndices16().begin(), cube.GetIndices16().end());
	indices.insert(indices.end(), cylinder.GetIndices16().begin(), cylinder.GetIndices16().end());
	// 顶点缓冲区的大小
	UINT vertexBufferSize = sizeof(ShapeVertex) * (UINT)vertices.size();
	UINT indexBufferSize = sizeof(std::uint16_t) * (UINT)indices.size();
	// 构建需要绘制的整体网格数据
	std::unique_ptr<MeshGeometry> meshGeo = std::make_unique<MeshGeometry>();
	meshGeo->m_Name = "ShapeGeo";
	// 在GPU上构建顶点缓冲区资源，缓存单个顶点数据的步长和顶点缓冲区的字节大小
	meshGeo->m_VertexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), vertices.data(), vertexBufferSize, meshGeo->m_VertexBufferUploader);
	meshGeo->m_VertexStride = sizeof(ShapeVertex);
	meshGeo->m_VertexSize = vertexBufferSize;
	// 在GPU上构建索引缓冲区资源，缓存索引的数据类型和索引缓冲区的字节大小
	meshGeo->m_IndexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), indices.data(), indexBufferSize, meshGeo->m_IndexBufferUploader);
	meshGeo->m_IndexFormat = DXGI_FORMAT_R16_UINT;
	meshGeo->m_IndexSize = indexBufferSize;
	// 整体网格数据可用多个子Mesh组成，各个子Mesh的顶点和索引数据，组合成一个大的顶点和索引数据，子Mesh中记录自己Mesh的顶点和索引数据在整体上的偏移
	// 子Mesh - Cube
	SubMeshGeometry subMeshCube{};
	subMeshCube.m_IndexCount = (UINT)cube.GetIndices16().size();
	subMeshCube.m_StartIndexLocation = 0U;
	subMeshCube.m_BaseVertexLocation = 0U;
	meshGeo->m_SubMeshGeometrys["Cube"] = subMeshCube;
	// 子Mesh - Cylinder
	SubMeshGeometry subMeshCylinder{};
	subMeshCylinder.m_IndexCount = (UINT)cylinder.GetIndices16().size();
	subMeshCylinder.m_StartIndexLocation = subMeshCube.m_IndexCount;
	subMeshCylinder.m_BaseVertexLocation = (UINT)cube.Vertices.size();
	meshGeo->m_SubMeshGeometrys["Cylinder"] = subMeshCylinder;
	// 缓存需要绘制的网格数据
	m_GeoMeshes[meshGeo->m_Name] = std::move(meshGeo);
}

void D3D12HelloShapes::BuildRenderItem()
{
	std::unique_ptr<HelloShapesRenderItem> cubeRenderItem = std::make_unique<HelloShapesRenderItem>();
	cubeRenderItem->meshGeo = m_GeoMeshes["ShapeGeo"].get();
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(-1.5f, 0.0f, 0.0f);
	DirectX::XMStoreFloat4x4(&cubeRenderItem->worldMatrix, worldMatrix);
	cubeRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	cubeRenderItem->objectCBIndex = 0;
	cubeRenderItem->indexCount = cubeRenderItem->meshGeo->m_SubMeshGeometrys["Cube"].m_IndexCount;
	cubeRenderItem->startIndexLocation = cubeRenderItem->meshGeo->m_SubMeshGeometrys["Cube"].m_StartIndexLocation;
	cubeRenderItem->startVertexLocation = cubeRenderItem->meshGeo->m_SubMeshGeometrys["Cube"].m_BaseVertexLocation;
	// 保存到所有的渲染项向量中
	m_AllRenderItems.push_back(std::move(cubeRenderItem));

	std::unique_ptr<HelloShapesRenderItem> cylinderRenderItem = std::make_unique<HelloShapesRenderItem>();
	cylinderRenderItem->meshGeo = m_GeoMeshes["ShapeGeo"].get();
	worldMatrix = DirectX::XMMatrixTranslation(1.5f, 0.0f, 0.0f);
	DirectX::XMStoreFloat4x4(&cylinderRenderItem->worldMatrix, worldMatrix);
	cylinderRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	cylinderRenderItem->objectCBIndex = 1;
	cylinderRenderItem->indexCount = cylinderRenderItem->meshGeo->m_SubMeshGeometrys["Cylinder"].m_IndexCount;
	cylinderRenderItem->startIndexLocation = cylinderRenderItem->meshGeo->m_SubMeshGeometrys["Cylinder"].m_StartIndexLocation;
	cylinderRenderItem->startVertexLocation = cylinderRenderItem->meshGeo->m_SubMeshGeometrys["Cylinder"].m_BaseVertexLocation;
	m_AllRenderItems.push_back(std::move(cylinderRenderItem));
}

void D3D12HelloShapes::BuildFrameResource()
{
	for (int i = 0; i < kNumFrameResource; ++i)
	{
		m_FrameResources.push_back(std::make_unique<HelloShapesFrameResource>(m_Device.Get(), 1U, (UINT)m_AllRenderItems.size()));
	}
}

void D3D12HelloShapes::BuildCBVDescriptorHeaps()
{
	// 每个帧资源有自己独立的渲染过程常量缓冲区和物体常量缓冲区(由渲染项物体自己的常量缓冲区组合而成)
	// 故总共需要的常量缓冲区描述符个数为(1 + RenderItemCount) * FrameResourceCount
	UINT numDescriptor = (1U + (UINT)m_AllRenderItems.size()) * kNumFrameResource;
	// 构建描述符堆描述
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc{};
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.NodeMask = 0U;
	cbvHeapDesc.NumDescriptors = numDescriptor;
	// 构建常量缓冲区描述符堆
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(m_CBVDescriptorHeap.GetAddressOf())));
}

void D3D12HelloShapes::BuildConstantBufferView()
{
	// 渲染项的个数
	UINT objectCount = (UINT)m_AllRenderItems.size();
	// 遍历所有的帧资源，为每个帧资源和渲染先创建独立的物体常量缓冲视图
	for (size_t i = 0; i < m_FrameResources.size(); ++i)
	{
		// 获取物体常量缓冲区资源
		ID3D12Resource* pObjCBResource = m_FrameResources[i]->pObjectConstants->GetResource();
		// 获取常量缓冲区的大小(单个元素)
		UINT cbByteSize = m_FrameResources[i]->pObjectConstants->GetElementSize();
		// 遍历所有的渲染项，创建渲染项关联物体的常量缓冲区视图
		for (size_t j = 0; j < objectCount; ++j)
		{
			// 常量缓冲区资源的GPU内存地址
			D3D12_GPU_VIRTUAL_ADDRESS cbAddress = pObjCBResource->GetGPUVirtualAddress();
			// 根据单元素常量缓冲区字节大小，计算当前渲染项关联物体常量缓冲区的地址
			cbAddress += j * cbByteSize;
			// 创建常量缓冲区视图描述
			D3D12_CONSTANT_BUFFER_VIEW_DESC desc{};
			desc.BufferLocation = cbAddress;
			desc.SizeInBytes = cbByteSize;
			// 描述符堆有多个描述符，根据帧资源和渲染项的索引按需分配handle
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_CBVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			handle.Offset((UINT)(i * objectCount + j), m_CBVUAVDescriptorSize);
			// 创建常量缓冲区视图
			m_Device->CreateConstantBufferView(&desc, handle);
		}
	}
	// 创建渲染过程常量缓冲区视图
	for (size_t i = 0; i < m_FrameResources.size(); ++i)
	{
		// 获取渲染过程常量缓冲区资源
		ID3D12Resource* pPassCBResource = m_FrameResources[i]->pPassConstants->GetResource();
		// 获取渲染过程常量缓冲区的字节大小(单个元素)
		UINT cbByteSize = m_FrameResources[i]->pPassConstants->GetElementSize();
		// 获取渲染过程常量缓冲区资源的GPU内存地址，每个帧资源只需要一个渲染过程常量缓冲区，不需要偏移
		D3D12_GPU_VIRTUAL_ADDRESS address = pPassCBResource->GetGPUVirtualAddress();
		// 创建常量缓冲区视图描述
		D3D12_CONSTANT_BUFFER_VIEW_DESC desc{};
		desc.BufferLocation = address;
		desc.SizeInBytes = cbByteSize;
		// 从描述符堆中分配Handle
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_CBVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset((UINT)(kNumFrameResource * objectCount + i), m_CBVUAVDescriptorSize);
		// 创建常量缓冲视图
		m_Device->CreateConstantBufferView(&desc, handle);
	}
}

void D3D12HelloShapes::PopulateCommandList()
{
	// 重置当前帧资源的命令分配器
	ThrowIfFailed(m_CurrentFrameResource->pCommandAllocator->Reset());
	// 重置命令列表
	ThrowIfFailed(m_CommandList->Reset(m_CurrentFrameResource->pCommandAllocator.Get(), m_PSO.Get()));

	// 当前的渲染缓冲区资源切换到渲染目标状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_RenderTargets[m_CurrentBackBufferIndex].Get()
		, D3D12_RESOURCE_STATE_PRESENT
		, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// 设置视口和裁剪矩形
	m_CommandList->RSSetViewports(1, &m_Viewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);
	// 清理渲染目标缓冲区和深度模板缓冲区
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHanle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentBackBufferIndex, m_RTVDescriptorSize);
	m_CommandList->ClearRenderTargetView(rtvHanle, DirectX::Colors::Gray, 0, nullptr);
	m_CommandList->ClearDepthStencilView(m_DepthStencilDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	// 指定渲染的目标
	m_CommandList->OMSetRenderTargets(1, &rtvHanle, true, &m_DepthStencilDescriptorHandle);
	// 设置描述符堆
	ID3D12DescriptorHeap* descriptorHeap = { m_CBVDescriptorHeap.Get() };
	m_CommandList->SetDescriptorHeaps(1, &descriptorHeap);
	// 设置根签名
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	// 设置Pass常量缓冲区根描述符表
	int passCBVIndex = kNumFrameResource * (int)m_AllRenderItems.size() + m_CurrentFrameResourceIndex;
	CD3DX12_GPU_DESCRIPTOR_HANDLE passCBVHanle(m_CBVDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	passCBVHanle.Offset(passCBVIndex, m_CBVUAVDescriptorSize);
	m_CommandList->SetGraphicsRootDescriptorTable(1, passCBVHanle);
	// 绘制渲染项
	DrawRenderItems();
	// 提交DearIMGui的渲染命令
	if (IMGuiHelloShapes::GetInstance() != nullptr)
	{
		IMGuiHelloShapes::GetInstance()->PopulateDearIMGuiCommand(m_CommandList.Get());
	}
	
	// 渲染缓冲区切换到呈现状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_RenderTargets[m_CurrentBackBufferIndex].Get()
		, D3D12_RESOURCE_STATE_RENDER_TARGET
		, D3D12_RESOURCE_STATE_PRESENT));
	// 指令记录完毕，关闭渲染列表
	ThrowIfFailed(m_CommandList->Close());
}

void D3D12HelloShapes::DrawRenderItems()
{
	for (size_t i = 0; i < m_AllRenderItems.size(); ++i)
	{
		HelloShapesRenderItem* pRenderItem = m_AllRenderItems[i].get();
		// 设置当前渲染项的顶点缓冲区和索引缓冲区
		m_CommandList->IASetVertexBuffers(0, 1, &pRenderItem->meshGeo->GetVertexBufferView());
		m_CommandList->IASetIndexBuffer(&pRenderItem->meshGeo->GetIndexBufferView());
		// 设置图元
		m_CommandList->IASetPrimitiveTopology(pRenderItem->primitiveType);
		// 设置物体常量缓冲区根描述符
		CD3DX12_GPU_DESCRIPTOR_HANDLE objCBHanle(m_CBVDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		objCBHanle.Offset(m_CurrentFrameResourceIndex * (int)m_AllRenderItems.size() + pRenderItem->objectCBIndex, m_CBVUAVDescriptorSize);
		m_CommandList->SetGraphicsRootDescriptorTable(0, objCBHanle);
		// 绘制
		m_CommandList->DrawIndexedInstanced(pRenderItem->indexCount, 1, pRenderItem->startIndexLocation, pRenderItem->startVertexLocation, 0);
	}
}