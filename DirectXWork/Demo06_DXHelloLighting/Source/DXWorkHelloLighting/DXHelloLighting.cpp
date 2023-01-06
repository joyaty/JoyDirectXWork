/**
 * DXHelloLighting.cpp
 * 光照模型实例程序代码文件
 */

#include "stdafx.h"
#include "DXHelloLighting.h"
#include "HelloLightingFrameResource.h"
#include "HelloLightingRenderItem.h"
#include "IMGuiHelloLighting.h"
#include <DirectXBaseWork/DXWorkHelper.h>
#include <DirectXBaseWork/GeometryGenerator.h>
#include <DirectXColors.h>

/// <summary>
/// 帧资源个数
/// </summary>
const int kFrameResourceCount = 3;

DXHelloLighting::DXHelloLighting(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
	m_AssetPath += L"\\DirectXWork\\Demo06_DXHelloLighting\\Source\\DXWorkHelloLighting\\";
}

DXHelloLighting::~DXHelloLighting()
{
}

void DXHelloLighting::UpdatePSO(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode)
{
	FlushCommandQueue();

	if (m_PSO != nullptr)
	{
		m_PSO.Reset();
	}
	BuildPSO(cullMode, fillMode);
}

bool DXHelloLighting::OnInit()
{
	// 执行基类中的初始化指令
	FlushCommandQueue();
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));

	// 实例程序初始化
	InitInputLayout();
	BuildRootSignature();
	CompileShaderFile();
	BuildMaterial();
	BuildRenderItem();
	BuildFrameResource();
	BuildPSO(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);

	// 执行初始化指令
	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdList), cmdList);
	// 等待初始化指令执行完成
	FlushCommandQueue();
	// IMGui调试面板持有HelloLightingDemo
	IMGuiHelloLighting::GetInstance()->SetWorkHelloLighting(this);

	return true;
}

void DXHelloLighting::OnUpdate(float deltaTime, float totalTime)
{
	// 循环利用帧资源
	m_CurrentFrameResourceIndex = (m_CurrentBackBufferIndex + 1) % kFrameResourceCount;
	m_CurrentFrameResource = m_FrameResources[m_CurrentFrameResourceIndex].get();
	if (m_CurrentFrameResource->fenceValue != 0 && m_CurrentFrameResource->fenceValue > m_Fence->GetCompletedValue())
	{
		// 当前帧资源还未处理，需要等待
		HANDLE eventHanle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrentFrameResource->fenceValue, eventHanle));
		WaitForSingleObject(eventHanle, INFINITE);
		CloseHandle(eventHanle);
	}
	UpdateCamera(deltaTime, totalTime);
	UpdateObjectCB(deltaTime, totalTime);
	UpdateMaterialCB(deltaTime, totalTime);
	UpdatePassCB(deltaTime, totalTime);
}

void DXHelloLighting::OnRender()
{
	// 记录所有的渲染指令
	PopulateCommandList();
	// 提交指令列表
	ID3D12CommandList * ppCommandList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
	// 交换缓冲区
	ThrowIfFailed(m_SwapChain->Present(1, 0));
	// 当前使用的帧资源增加围栏值
	m_CurrentFrameResource->fenceValue = ++m_FenceValue;
	// 命令队列添加新的指令，设置一个围栏值
	m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue);
}

void DXHelloLighting::OnDestroy()
{
}

void DXHelloLighting::UpdateCamera(float deltaTime, float totalTime)
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

void DXHelloLighting::UpdateObjectCB(float deltaTime, float totalTime)
{
	// 获取当前使用的帧资源的物体常量缓冲区
	UploadBuffer<PerObjectConstants>* pObjectCB = m_CurrentFrameResource->pPerObjUploadBuffer.get();
	for (auto& pRenderItem : m_AllRenderItems)
	{
		// RenderItem数据处于脏标记下，才需要更新常量缓冲区
		if (pRenderItem->numFrameDirty > 0)
		{
			DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&pRenderItem->worldMatrix);
			PerObjectConstants objCB{};
			DirectX::XMStoreFloat4x4(&objCB.worldMatrix, DirectX::XMMatrixTranspose(worldMatrix));
			// 拷贝到当前索引的常量缓冲区
			pObjectCB->CopyData(pRenderItem->objConstantBufferIndex, objCB);
			// 脏标记 - 1
			--pRenderItem->numFrameDirty;
		}
	}
}

void DXHelloLighting::UpdateMaterialCB(float deltaTime, float totalTime)
{
	UploadBuffer<PerMatConstants>* pMaterialCB = m_CurrentFrameResource->pPerMatUploadBuffer.get();
	for (auto& iter : m_Materials)
	{
		HelloLightingMaterial* pMaterial = iter.second.get();
		// 材质数据为脏，才更新材质常量缓冲区数据
		if (pMaterial->numFrameDirty > 0)
		{
			// 构建材质常量数据
			PerMatConstants matConstant{};
			matConstant.diffuseAlbedo = pMaterial->diffuseAlbedo;
			matConstant.fresnelR0 = pMaterial->fresnelR0;
			matConstant.roughness = pMaterial->roughness;
			// 拷贝数据到材质常量缓冲区
			pMaterialCB->CopyData(pMaterial->materialCBIndex, matConstant);
			// 脏标记-1
			--pMaterial->numFrameDirty;
		}
	}
}

void DXHelloLighting::UpdatePassCB(float deltaTime, float totalTime)
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
	// 拷贝数据Pass常量缓冲区上
	m_CurrentFrameResource->pPassUploadBuffer->CopyData(0, passObj);
}

void DXHelloLighting::OnMouseDown(UINT8 keyCode, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void DXHelloLighting::OnMouseUp(UINT8 keyCode, int x, int y)
{
}

void DXHelloLighting::OnMouseMove(UINT8 keyCode, int x, int y)
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

void DXHelloLighting::OnResize(UINT width, UINT height)
{
	DirectXBaseWork::OnResize(width, height);
	// 根据FOV，视口纵横比，近远平面计算投影矩阵
	DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(0.25f * MathUtil::Pi, GetAspectRatio(), 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&m_ProjMatrix, projMatrix);
}

void DXHelloLighting::BuildMaterial()
{
	// 创建测试材质
	std::unique_ptr<HelloLightingMaterial> pMaterial = std::make_unique<HelloLightingMaterial>();
	pMaterial->name = "HelloLightingTest";
	pMaterial->diffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pMaterial->fresnelR0 = DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f);
	pMaterial->roughness = 0.25f;
	pMaterial->materialCBIndex = 0;
	// 保存到材质集合中
	m_Materials[pMaterial->name] = std::move(pMaterial);
}

void DXHelloLighting::BuildRenderItem()
{
	GeometryGenerator::MeshData sphereMesh = GeometryGenerator::CreateGeoSphere(2.f, 3);
	size_t totalVertexCount = sphereMesh.Vertices.size();
	// 构建顶点数据
	std::vector<HelloLightingVertex> vertices(totalVertexCount);
	for (size_t i = 0; i < totalVertexCount; ++i)
	{
		vertices[i].position = sphereMesh.Vertices[i].Position;
		vertices[i].color = DirectX::XMFLOAT4(DirectX::Colors::White);
		vertices[i].normal = sphereMesh.Vertices[i].Normal;
	}
	// 构建索引数据
	std::vector<std::uint16_t> indices{};
	indices.insert(indices.end(), sphereMesh.GetIndices16().begin(), sphereMesh.GetIndices16().end());
	// 顶点缓冲区和索引缓冲区的大小
	UINT vertexBufferSize = sizeof(HelloLightingVertex) * (UINT)totalVertexCount;
	UINT indexBufferSize = sizeof(std::uint16_t) * (UINT)indices.size();
	std::unique_ptr<MeshGeometry> sphereGeometry = std::make_unique<MeshGeometry>();
	sphereGeometry->m_Name = "SphereGeo";
	sphereGeometry->m_VertexBufferCPU = nullptr; // 暂时用不到CPU端的顶点数据，直接置为空指针
	sphereGeometry->m_VertexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), vertices.data(), vertexBufferSize, sphereGeometry->m_VertexBufferUploader);
	sphereGeometry->m_VertexSize = vertexBufferSize;
	sphereGeometry->m_VertexStride = sizeof(HelloLightingVertex);
	sphereGeometry->m_IndexBufferCPU = nullptr; // 暂时用不到CPU端的索引数据，直接置为空指针
	sphereGeometry->m_IndexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), indices.data(), indexBufferSize, sphereGeometry->m_IndexBufferUploader);
	sphereGeometry->m_IndexFormat = DXGI_FORMAT_R16_UINT;
	sphereGeometry->m_IndexSize = indexBufferSize;
	// 子mesh，本例只有一个球体，故无索引和顶点偏移，直接置零，索引数量等于全部的索引数
	SubMeshGeometry subMeshSphere{};
	subMeshSphere.m_IndexCount = (UINT)indices.size();
	subMeshSphere.m_StartIndexLocation = 0U;
	subMeshSphere.m_BaseVertexLocation = 0U;
	sphereGeometry->m_SubMeshGeometrys["Sphere"] = subMeshSphere;
	// 存入场景的几何体集合
	m_GeoMesh[sphereGeometry->m_Name] = std::move(sphereGeometry);

	// 遍历场景中的物体，构建渲染项
	std::unique_ptr<HelloLightingRenderItem> sphereRenderItem = std::make_unique<HelloLightingRenderItem>();
	// 世界位置放置在(0,0,0)点，构建世界变换矩阵
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(0.f, 0.f, 0.f);
	DirectX::XMStoreFloat4x4(&sphereRenderItem->worldMatrix, worldMatrix);
	sphereRenderItem->pMeshGeometry = m_GeoMesh["SphereGeo"].get();		// 指定渲染项关联的网格
	sphereRenderItem->pMat = m_Materials["HelloLightingTest"].get();	// 指定渲染项关联的材质
	sphereRenderItem->objConstantBufferIndex = 0;
	sphereRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	sphereRenderItem->indexCount = sphereRenderItem->pMeshGeometry->m_SubMeshGeometrys["Sphere"].m_IndexCount;
	sphereRenderItem->startIndexLocation = sphereRenderItem->pMeshGeometry->m_SubMeshGeometrys["Sphere"].m_StartIndexLocation;
	sphereRenderItem->startVetexLocation = sphereRenderItem->pMeshGeometry->m_SubMeshGeometrys["Sphere"].m_BaseVertexLocation;
	m_AllRenderItems.push_back(std::move(sphereRenderItem));
}

void DXHelloLighting::BuildFrameResource()
{
	for (int i = 0; i < kFrameResourceCount; ++i)
	{
		m_FrameResources.push_back(std::make_unique<HelloLightingFrameResource>(m_Device.Get(), (UINT)m_AllRenderItems.size(), (UINT)m_Materials.size(), 1));
	}
	m_CurrentFrameResourceIndex = 0;
	m_CurrentFrameResource = m_FrameResources[m_CurrentBackBufferIndex].get();
}

void DXHelloLighting::InitInputLayout()
{
	m_InputElementDesc =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 3 * sizeof(float), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 3 * sizeof(float) + 4 * sizeof(UINT), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

void DXHelloLighting::CompileShaderFile()
{
	m_VSByteCode = CompileShader(m_AssetPath + L"HelloLighting.hlsl", nullptr, "VSMain", "vs_5_0");
	m_PSByteCode = CompileShader(m_AssetPath + L"HelloLighting.hlsl", nullptr, "PSMain", "ps_5_0");
}

void DXHelloLighting::BuildRootSignature()
{
	// 构建根参数
	CD3DX12_ROOT_PARAMETER rootParameters[3]{};
	rootParameters[0].InitAsConstantBufferView(0); // 物体常量缓冲区根描述符
	rootParameters[1].InitAsConstantBufferView(1); // 材质常量缓冲区根描述符
	rootParameters[2].InitAsConstantBufferView(2); // 渲染过程常量缓冲区根描述符
	// 构建根签名描述
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(3, rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	// 根参数序列化处理
	Microsoft::WRL::ComPtr<ID3DBlob> pSerializeRootSig{ nullptr };
	Microsoft::WRL::ComPtr<ID3DBlob> pErrorMsg{ nullptr };
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, pSerializeRootSig.GetAddressOf(), pErrorMsg.GetAddressOf());
	if (pErrorMsg != nullptr)
	{
		::OutputDebugStringA((char*)pErrorMsg->GetBufferPointer());
	}
	ThrowIfFailed(hr);
	// 创建根签名
	ThrowIfFailed(m_Device->CreateRootSignature(0U, pSerializeRootSig->GetBufferPointer(), pSerializeRootSig->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf())));
}

void DXHelloLighting::BuildPSO(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
	pipelineStateDesc.NodeMask = 0U;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.InputLayout = { m_InputElementDesc.data(), (UINT)m_InputElementDesc.size() };
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
	// 构建渲染管线状态对象
	ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(m_PSO.GetAddressOf())));
}

void DXHelloLighting::PopulateCommandList()
{
	// 帧资源空闲，重置当前帧资源的命令分配器
	ThrowIfFailed(m_CurrentFrameResource->pCommandAllocator->Reset());
	// 重置命令列表
	ThrowIfFailed(m_CommandList->Reset(m_CurrentFrameResource->pCommandAllocator.Get(), m_PSO.Get()));
	// 当前渲染目标缓冲区资源转化为渲染目标状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_RenderTargets[m_CurrentBackBufferIndex].Get()
		, D3D12_RESOURCE_STATE_PRESENT
		, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// 设置视口和裁剪矩形
	m_CommandList->RSSetViewports(1, &m_Viewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);
	// 获取当前的渲染目标资源Handle
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentBackBufferIndex, m_RTVDescriptorSize);
	m_CommandList->ClearRenderTargetView(rtvHandle, DirectX::Colors::Gray, 0, nullptr);
	m_CommandList->ClearDepthStencilView(m_DepthStencilDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	// 指定渲染目标和深度模板缓冲区
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, true, &m_DepthStencilDescriptorHandle);
	// 设置根签名
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	// 设置渲染过程常量缓冲区根描述符
	m_CommandList->SetGraphicsRootConstantBufferView(2, m_CurrentFrameResource->pPassUploadBuffer->GetResource()->GetGPUVirtualAddress());
	// 绘制渲染项
	DrawRenderItem();
	// 提交DearIMGui的渲染命令
	if (IMGuiHelloLighting::GetInstance() != nullptr)
	{
		IMGuiHelloLighting::GetInstance()->PopulateDearIMGuiCommand(m_CommandList.Get());
	}
	// 渲染缓冲区切换到呈现状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_RenderTargets[m_CurrentBackBufferIndex].Get()
		, D3D12_RESOURCE_STATE_RENDER_TARGET
		, D3D12_RESOURCE_STATE_PRESENT));
	// 指令记录完毕，关闭渲染列表，准备提交
	ThrowIfFailed(m_CommandList->Close());
}

void DXHelloLighting::DrawRenderItem()
{
	for (size_t i = 0; i < m_AllRenderItems.size(); ++i)
	{
		HelloLightingRenderItem* pRenderItem{ m_AllRenderItems[i].get() };
		// 设置顶点缓冲区和索引缓冲区
		m_CommandList->IASetVertexBuffers(0, 1, &pRenderItem->pMeshGeometry->GetVertexBufferView());
		m_CommandList->IASetIndexBuffer(&pRenderItem->pMeshGeometry->GetIndexBufferView());
		m_CommandList->IASetPrimitiveTopology(pRenderItem->primitiveType);
		// 设置物体常量缓冲区根描述符
		D3D12_GPU_VIRTUAL_ADDRESS objCBGPUAddress{ m_CurrentFrameResource->pPerObjUploadBuffer->GetResource()->GetGPUVirtualAddress() };
		// 偏移到指定索引的物体常量缓冲区
		objCBGPUAddress += pRenderItem->objConstantBufferIndex * m_CurrentFrameResource->pPerObjUploadBuffer->GetElementSize();
		m_CommandList->SetGraphicsRootConstantBufferView(0, objCBGPUAddress);
		// 设置材质常量缓冲区根描述符
		D3D12_GPU_VIRTUAL_ADDRESS matCBGPUAddress{ m_CurrentFrameResource->pPerMatUploadBuffer->GetResource()->GetGPUVirtualAddress() };
		// 偏移到指定索引的材质常量缓冲区
		matCBGPUAddress += pRenderItem->objConstantBufferIndex * m_CurrentFrameResource->pPerMatUploadBuffer->GetElementSize();
		m_CommandList->SetGraphicsRootConstantBufferView(1, matCBGPUAddress);
		// 绘制
		m_CommandList->DrawIndexedInstanced(pRenderItem->indexCount, 1, pRenderItem->startIndexLocation, pRenderItem->startVetexLocation, 0);
	}
}