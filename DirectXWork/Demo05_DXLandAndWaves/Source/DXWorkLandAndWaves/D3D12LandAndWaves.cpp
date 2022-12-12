/**
 * D3D12LandAndWaves.h
 * 陆地与波浪示例程序Direct3D代码
 */

#include "stdafx.h"
#include "D3D12LandAndWaves.h"
#include "IMGuiLandAndWaves.h"
#include "DirectXBaseWork/DXWorkHelper.h"
#include "DirectXBaseWork/GeometryGenerator.h"
#include "LandAndWavesFrameResource.h"
#include "LandAndWavesRenderItem.h"
#include <DirectXColors.h>

/// <summary>
/// 帧资源个数
/// </summary>
const int kNumFrameResource = 3;

D3D12LandAndWaves::D3D12LandAndWaves(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
	m_AssetPath += L"\\DirectXWork\\Demo05_DXLandAndWaves\\Source\\DXWorkLandAndWaves\\";
}

D3D12LandAndWaves::~D3D12LandAndWaves()
{
}

bool D3D12LandAndWaves::OnInit()
{
	// 之前基类初始化指令
	FlushCommandQueue();
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));

	BuildInputLayout();
	BuildRootSignature();
	CompileShaderFile();
	BuildGrid();
	BuildRenderItem();
	BuildFrameResource();
	BuildPipelineState();
	// 执行初始化指令
	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdList), cmdList);
	// 等待初始化指令完成
	FlushCommandQueue();
	// IMGui调试面板持有HelloShapesDemo
	IMGuiLandAndWaves::GetInstance()->SetHelloShapesDemo(this);
	return true;
}

void D3D12LandAndWaves::OnUpdate(float deltaTime, float totalTime)
{
	UpdateCamera(deltaTime, totalTime);
	// 获取当前使用的帧资源
	m_CurrentFrameResourceIndex = (m_CurrentFrameResourceIndex + 1) % kNumFrameResource;
	m_pCurrentFrameResource = m_AllFrameResources[m_CurrentFrameResourceIndex].get();
	if (m_pCurrentFrameResource->fenceValue != 0 && m_pCurrentFrameResource->fenceValue > m_Fence->GetCompletedValue())
	{
		HANDLE eventHanle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(m_Fence->SetEventOnCompletion(m_pCurrentFrameResource->fenceValue, eventHanle));
		WaitForSingleObject(eventHanle, INFINITE);
		CloseHandle(eventHanle);
	}
	// 更新常量缓冲区数据
	UpdateObjectCBs(deltaTime, totalTime);
	UpdatePassCBs(deltaTime, totalTime);
}

void D3D12LandAndWaves::OnRender()
{
	// 记录本次渲染的所有指令
	PopulateCommandList();
	// 提交指令列表
	ID3D12CommandList* ppCommandList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(1, ppCommandList);
	// 交换缓冲区
	ThrowIfFailed(m_SwapChain->Present(1, 0));
	// 标记当前帧资源的围栏值，通过这个围栏值与渲染指令执行到的围栏值对比，可以判断当前帧资源是否已使用结束
	m_pCurrentFrameResource->fenceValue = ++m_FenceValue;
	// 命令队列添加新的指令，设置一个围栏值
	m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue);
}

void D3D12LandAndWaves::OnDestroy()
{

}

void D3D12LandAndWaves::OnMouseDown(UINT8 keyCode, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void D3D12LandAndWaves::OnMouseUp(UINT8 keyCode, int x, int y)
{
}

void D3D12LandAndWaves::OnMouseMove(UINT8 keyCode, int x, int y)
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
		m_Radius = MathUtil::Clamp(m_Radius, 50.f, 200.f);
	}
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}
void D3D12LandAndWaves::OnResize(UINT width, UINT height)
{
	DirectXBaseWork::OnResize(width, height);
	// 根据FOV，视口纵横比，近远平面计算投影矩阵
	DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(0.25f * MathUtil::Pi, GetAspectRatio(), 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&m_ProjMatrix, projMatrix);
}

void D3D12LandAndWaves::UpdateObjectCBs(float deltaTime, float totalTime)
{
	// 获取当前使用的帧资源
	UploadBuffer<ObjectConstants>* pObjectCB = m_pCurrentFrameResource->pObjCB.get();
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

void D3D12LandAndWaves::UpdatePassCBs(float deltaTime, float totalTime)
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
	DirectX::XMStoreFloat4x4(&passObj.invViewProjMatrix, DirectX::XMMatrixTranspose(invViewProjMatrix));
	passObj.eyeWorldPos = m_EyesPos;
	passObj.renderTargetSize = DirectX::XMFLOAT2(static_cast<float>(GetWidth()), static_cast<float>(GetHeight()));
	passObj.invRenderTargetSize = DirectX::XMFLOAT2(1.f / GetWidth(), 1.f / GetHeight());
	passObj.nearZ = 1.0f;
	passObj.farZ = 1000.0f;
	passObj.deltaTime = deltaTime;
	passObj.totalTime = totalTime;
	// 拷贝数据Pass常量缓冲区上
	m_pCurrentFrameResource->pPassCB->CopyData(0, passObj);
}

void D3D12LandAndWaves::UpdateCamera(float deltaTime, float totalTime)
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

void D3D12LandAndWaves::BuildInputLayout()
{
	m_InputLayouts = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 3 * sizeof(float), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
}

void D3D12LandAndWaves::CompileShaderFile()
{
	m_VSByteCode = CompileShader(m_AssetPath + L"LandAndWaves.hlsl", nullptr, "VSMain", "vs_5_0");
	m_PSByteCode = CompileShader(m_AssetPath + L"LandAndWaves.hlsl", nullptr, "PSMain", "ps_5_0");
}

void D3D12LandAndWaves::BuildGrid()
{
	// 生成地表网格
	GeometryGenerator::MeshData gridMeshData = GeometryGenerator::CreateGrid(160.f, 160.f, 50, 50);
	// 顶点总数
	size_t totalVertexCount = gridMeshData.Vertices.size();
	// 通过地表网格Y轴偏移生成高低起伏地表
	std::vector<Vertex> vertices(totalVertexCount);
	for (size_t i = 0; i < gridMeshData.Vertices.size(); ++i)
	{
		Vertex vert{};
		vert.position = gridMeshData.Vertices[i].Position;
		vert.position.y = GetHillsHeight(vert.position.x, vert.position.z);
		// 基于高度给地形上色
		if (vert.position.y < -10.f)
		{
			// 沙滩的颜色
			vert.color = DirectX::XMFLOAT4(1.f, 0.96f, 0.62f, 1.f);
		}
		else if (vert.position.y < 5.f)
		{
			// 浅黄绿色
			vert.color = DirectX::XMFLOAT4(0.48f, 0.77f, 0.46f, 1.f);
		}
		else if (vert.position.y < 12.f)
		{
			// 深黄绿色
			vert.color = DirectX::XMFLOAT4(0.1f, 0.48f, 0.19f, 1.f);
		}
		else if (vert.position.y < 20.f)
		{
			// 深棕色
			vert.color = DirectX::XMFLOAT4(0.45f, 0.39f, 0.34f, 1.f);
		}
		else
		{
			// 白雪皑皑
			vert.color = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
		}
		vertices[i] = vert;
	}
	// 构建索引数据
	std::vector<std::uint16_t> indices{};
	indices.insert(indices.end(), gridMeshData.GetIndices16().begin(), gridMeshData.GetIndices16().end());
	// 顶点缓冲区大小
	UINT vertexBufferSize = sizeof(Vertex) * (UINT)vertices.size();
	// 索引缓冲区大小
	UINT indexBufferSize = sizeof(std::uint16_t) * (UINT)indices.size();
	// 构建地表Mesh数据
	std::unique_ptr<MeshGeometry> pMeshGeo = std::make_unique<MeshGeometry>();
	pMeshGeo->m_Name = "Terrain";
	// 在GPU上构建顶点缓冲区资源，缓存单个顶点数据的步长和顶点缓冲区的字节大小
	pMeshGeo->m_VertexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), vertices.data(), vertexBufferSize, pMeshGeo->m_VertexBufferUploader);
	pMeshGeo->m_VertexSize = vertexBufferSize;
	pMeshGeo->m_VertexStride = sizeof(Vertex);
	// 在GPU上构建索引缓冲区资源，缓存索引的数据类型和索引缓冲区的字节大小
	pMeshGeo->m_IndexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), indices.data(), indexBufferSize, pMeshGeo->m_IndexBufferUploader);
	pMeshGeo->m_IndexSize = indexBufferSize;
	pMeshGeo->m_IndexFormat = DXGI_FORMAT_R16_UINT;
	// 子Mesh - 地表Mesh在大缓冲区的偏移
	SubMeshGeometry subMeshGeo{};
	subMeshGeo.m_IndexCount = (UINT)indices.size();
	subMeshGeo.m_StartIndexLocation = 0U;
	subMeshGeo.m_BaseVertexLocation = 0U;
	pMeshGeo->m_SubMeshGeometrys["Terrain"] = subMeshGeo;
	// 保存到物体
	m_Meshes[pMeshGeo->m_Name] = std::move(pMeshGeo);
}

float D3D12LandAndWaves::GetHillsHeight(float x, float z)
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

void D3D12LandAndWaves::BuildRenderItem()
{
	std::unique_ptr<LandAndWavesRenderItem> pRenderItem = std::make_unique<LandAndWavesRenderItem>();
	pRenderItem->objectCBIndex = 0;
	pRenderItem->pGeometryMesh = m_Meshes["Terrain"].get();
	pRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pRenderItem->worldMatrix = MathUtil::Identity4x4();
	pRenderItem->indexCount = pRenderItem->pGeometryMesh->m_SubMeshGeometrys["Terrain"].m_IndexCount;
	pRenderItem->startIndexLocation = pRenderItem->pGeometryMesh->m_SubMeshGeometrys["Terrain"].m_StartIndexLocation;
	pRenderItem->startVertexLocation = pRenderItem->pGeometryMesh->m_SubMeshGeometrys["Terrain"].m_BaseVertexLocation;
	m_AllRenderItems.push_back(std::move(pRenderItem));
}

void D3D12LandAndWaves::BuildFrameResource()
{
	for (int i = 0; i < kNumFrameResource; ++i)
	{
		m_AllFrameResources.push_back(std::make_unique<LandAndWavesFrameResource>(m_Device.Get(), (UINT)m_AllRenderItems.size(), 1U));
	}
}

void D3D12LandAndWaves::BuildRootSignature()
{
	// 构造根参数
	CD3DX12_ROOT_PARAMETER rootParams[2]{};
	rootParams[0].InitAsConstantBufferView(0); // 物体常量缓冲区根描述符
	rootParams[1].InitAsConstantBufferView(1); // 渲染过程常量缓冲区根描述符
	// 通过根参数，构建根签名描述
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, rootParams, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	// 根参数序列化处理
	Microsoft::WRL::ComPtr<ID3DBlob> pSerializedRootSig{ nullptr };
	Microsoft::WRL::ComPtr<ID3DBlob> pErrorMsg{ nullptr };
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, pSerializedRootSig.GetAddressOf(), pErrorMsg.GetAddressOf());
	if (pErrorMsg != nullptr)
	{
		::OutputDebugStringA((char*)pErrorMsg->GetBufferPointer());
	}
	ThrowIfFailed(hr);
	// 创建根签名
	ThrowIfFailed(m_Device->CreateRootSignature(0U, pSerializedRootSig->GetBufferPointer(), pSerializedRootSig->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf())));
}

void D3D12LandAndWaves::BuildPipelineState()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
	ZeroMemory(&pipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	pipelineStateDesc.NodeMask = 0U;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.InputLayout = { m_InputLayouts.data(), (UINT)m_InputLayouts.size() };
	pipelineStateDesc.pRootSignature = m_RootSignature.Get();
	pipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_VSByteCode->GetBufferPointer()), m_VSByteCode->GetBufferSize() };
	pipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_PSByteCode->GetBufferPointer()), m_PSByteCode->GetBufferSize() };
	pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	pipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
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

void D3D12LandAndWaves::PopulateCommandList()
{
	// 当前帧资源可用，可重置的指令分配器
	ThrowIfFailed(m_pCurrentFrameResource->m_CommandAllocator->Reset());
	// 使用帧资源分配器和渲染管线状态对象重置命令列表
	ThrowIfFailed(m_CommandList->Reset(m_pCurrentFrameResource->m_CommandAllocator.Get(), m_PSO.Get()));

	// 当前的渲染缓冲区资源切换到渲染目标状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_RenderTargets[m_CurrentBackBufferIndex].Get()
		, D3D12_RESOURCE_STATE_PRESENT
		, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// 设置视口和裁剪矩形
	m_CommandList->RSSetViewports(1, &m_Viewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);
	// 获取当前渲染目标视图在描述符堆的Handle
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentBackBufferIndex, m_RTVDescriptorSize);
	m_CommandList->ClearRenderTargetView(rtvHandle, DirectX::Colors::Gray, 0, nullptr);
	m_CommandList->ClearDepthStencilView(m_DepthStencilDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);
	// 设置渲染目标
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, true, &m_DepthStencilDescriptorHandle);
	// 设置根签名
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	// 设置渲染过程根描述符视图
	m_CommandList->SetGraphicsRootConstantBufferView(1, m_pCurrentFrameResource->pPassCB->GetResource()->GetGPUVirtualAddress());
	DrawRenderItem();
	// 提交DearIMGui的渲染命令
	if (IMGuiLandAndWaves::GetInstance() != nullptr)
	{
		IMGuiLandAndWaves::GetInstance()->PopulateDearIMGuiCommand(m_CommandList.Get());
	}

	// 渲染缓冲区切换到呈现状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_RenderTargets[m_CurrentBackBufferIndex].Get()
		, D3D12_RESOURCE_STATE_RENDER_TARGET
		, D3D12_RESOURCE_STATE_PRESENT));
	// 指令记录完毕，关闭渲染列表
	ThrowIfFailed(m_CommandList->Close());
}

void D3D12LandAndWaves::DrawRenderItem()
{
	for (size_t i = 0; i < m_AllRenderItems.size(); ++i)
	{
		LandAndWavesRenderItem* renderItem{ m_AllRenderItems[i].get() };
		m_CommandList->IASetVertexBuffers(0, 1, &renderItem->pGeometryMesh->GetVertexBufferView());
		m_CommandList->IASetIndexBuffer(&renderItem->pGeometryMesh->GetIndexBufferView());
		m_CommandList->IASetPrimitiveTopology(renderItem->primitiveType);
		D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = m_pCurrentFrameResource->pObjCB->GetResource()->GetGPUVirtualAddress();
		gpuAddress += renderItem->objectCBIndex * m_pCurrentFrameResource->pObjCB->GetElementSize();
		m_CommandList->SetGraphicsRootConstantBufferView(0, gpuAddress);
		m_CommandList->DrawIndexedInstanced(renderItem->indexCount, 1, renderItem->startIndexLocation, renderItem->startVertexLocation, 0);
	}
}