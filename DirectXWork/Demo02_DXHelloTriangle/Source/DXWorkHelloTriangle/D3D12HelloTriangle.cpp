/*
 * D3D12HelloTriangle.cpp
 * Demo02 - DirectX12渲染三角形
 */

#include "stdafx.h"
#include "DXWorkHelloTriangle/D3D12HelloTriangle.h"
#include <array>
#include <DirectXColors.h>
#include "DirectXBaseWork/DXWorkHelper.h"
#include "DirectXBaseWork/D3D12Util.h"
#include "DXWorkHelloTriangle/IMGuiHelloTriangle.h"

D3D12HelloTriangle::D3D12HelloTriangle(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
	// 更新资源目录到当前实例程序文件夹下
	m_AssetPath += L"\\DirectXWork\\Demo02_DXHelloTriangle\\Source\\DXWorkHelloTriangle\\";
}

D3D12HelloTriangle::~D3D12HelloTriangle()
{
}

bool D3D12HelloTriangle::OnInit()
{
	FlushCommandQueue();
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));

	CreateInputLayout();
	CreateTriangleMesh();
	CompileShaderFile();
	CreateRootSignature();
	CreatePipelineState();

	ThrowIfFailed(m_CommandList->Close());
	// 执行初始化指令
	ID3D12CommandList* cmdLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	// 等待初始化完成
	FlushCommandQueue();

	return true;
}

void D3D12HelloTriangle::OnUpdate()
{
}

void D3D12HelloTriangle::OnRender()
{
	// 记录所有的渲染指令
	PopulateCommandList();
	// 提交指令列表
	ID3D12CommandList* ppCommandList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
	// 交换缓冲区
	ThrowIfFailed(m_SwapChain->Present(1, 0));
	// 刷新命令队列
	FlushCommandQueue();
}

void D3D12HelloTriangle::OnDestroy()
{
}

void D3D12HelloTriangle::CreateInputLayout()
{
	m_InputElementDescs =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 3 * sizeof(float), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
}

void D3D12HelloTriangle::CreateTriangleMesh()
{
	// 在默认光栅器设置下(默认光栅器设置，开启背面剔除，顺时针顶点顺序为三角形正面)，要注意顶点顺序需要注意顺时针还是逆时针
	// 如果顶点顺序为逆时针，则看不到三角形了，因为判断为三角形背面，被剔除
	std::array<HelloTriangle::Vertex, 3> vertices = 
	{
		HelloTriangle::Vertex({DirectX::XMFLOAT3(0.f, 0.5f, 0.f), DirectX::XMFLOAT4(DirectX::Colors::Red)}),
		HelloTriangle::Vertex({DirectX::XMFLOAT3(0.5f, -0.5f, 0.f), DirectX::XMFLOAT4(DirectX::Colors::Green)}),
		HelloTriangle::Vertex({DirectX::XMFLOAT3(-0.5f, -0.5f, 0.f), DirectX::XMFLOAT4(DirectX::Colors::Blue)})
	};
	// 顶点数据的字节数
	const UINT vbByteSize = (UINT)vertices.size() * sizeof(HelloTriangle::Vertex);
	// 构建三角形Mesh数据，内部包含了创建顶点缓冲区
	m_TriangleMesh = std::make_unique<MeshGeometry>();
	m_TriangleMesh->m_Name = "Triangle";
	m_TriangleMesh->m_VertexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), vertices.data(), vbByteSize, m_TriangleMesh->m_VertexBufferUploader);
	m_TriangleMesh->m_VertexSize = vbByteSize;
	m_TriangleMesh->m_VertexStride = sizeof(HelloTriangle::Vertex);

	// 在上传堆创建顶点缓冲区资源
	m_Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD)
		, D3D12_HEAP_FLAG_NONE
		, &CD3DX12_RESOURCE_DESC::Buffer(vbByteSize)
		, D3D12_RESOURCE_STATE_GENERIC_READ
		, nullptr
		, IID_PPV_ARGS(m_VertexBuffer.GetAddressOf())
	);
	// Copy the triangle data to the vertex buffer.
	UINT8* pVertexDataBegin;
	m_VertexBuffer->Map(0, &CD3DX12_RANGE(), reinterpret_cast<void**>(&pVertexDataBegin));
	memcpy(pVertexDataBegin, vertices.data(), vbByteSize);
	m_VertexBuffer->Unmap(0, nullptr);

	// Initialize the vertex buffer view.
	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = sizeof(HelloTriangle::Vertex);
	m_VertexBufferView.SizeInBytes = vbByteSize;
}

void D3D12HelloTriangle::CompileShaderFile()
{
	m_VertexShaderByteCode = CompileShader(m_AssetPath + L"HelloTriangle.hlsl", nullptr, "VSMain", "vs_5_0");
	m_PixelShaderByteCode = CompileShader(m_AssetPath + L"HelloTriangle.hlsl", nullptr, "PSMain", "ps_5_0");
}

void D3D12HelloTriangle::CreateRootSignature()
{
	// 不需要常量缓冲区的资源，这里创建一个空的根签名（0个根参数，0个静态采样器）
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc{ D3D12_DEFAULT };
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// Direct3D 12中规定需要将根签名描述进行序列化处理，转化为ID3DBlob接口表示的序列化数据后，才可将根签名描述数据传入CreatRootSignature中，正式创建根签名
	Microsoft::WRL::ComPtr<ID3DBlob> serializeRootSig{ nullptr };
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob{ nullptr };
	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializeRootSig.GetAddressOf(), errorBlob.GetAddressOf());
	if (errorBlob != nullptr)
	{
		// 序列化根签名失败，输出错误信息
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);
	// 序列化处理后，正式创建根签名
	ThrowIfFailed(m_Device->CreateRootSignature(0U, serializeRootSig->GetBufferPointer(), serializeRootSig->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf())));
}

void D3D12HelloTriangle::CreatePipelineState()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
	ZeroMemory(&pipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	pipelineStateDesc.NodeMask = 0U;
	pipelineStateDesc.pRootSignature = m_RootSignature.Get();
	pipelineStateDesc.InputLayout = { m_InputElementDescs.data(), (UINT)m_InputElementDescs.size() };
	pipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_VertexShaderByteCode->GetBufferPointer()), m_VertexShaderByteCode->GetBufferSize() };
	pipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_PixelShaderByteCode->GetBufferPointer()), m_PixelShaderByteCode->GetBufferSize() };
	pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineStateDesc.DepthStencilState.DepthEnable = false;// = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipelineStateDesc.DepthStencilState.StencilEnable = false;
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.NumRenderTargets = 1U;
	pipelineStateDesc.RTVFormats[0] = GetBackBufferFormat();
	pipelineStateDesc.SampleDesc.Count = Get4XMSAAEnable() ? 4 : 1;
	pipelineStateDesc.SampleDesc.Quality = Get4XMSAAEnable() ? m_4XMSAAQualityLevel - 1 : 0;
	pipelineStateDesc.DSVFormat = GetDepthStencilBufferFormat();

	ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(m_PSO.GetAddressOf())));
}

void D3D12HelloTriangle::PopulateCommandList()
{
	// Command list allocators can only be reset when the associated command lists have finished execution on th GPU.
	// apps should use fence to determine GPU excution progress.
	ThrowIfFailed(m_CommandAllocator->Reset());
	// However, when ExecuteCommandList() is called on a particular command list, that command list can the be reset at any time and must be before re-recording.
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), m_PSO.Get()));
	// Record commands.
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	m_CommandList->RSSetViewports(1, &m_Viewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);
	// 当前后台缓冲区切换到渲染目标状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_CurrentBackBufferIndex].Get()
		, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentBackBufferIndex, m_RTVDescriptorSize);
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr/*&m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart()*/);
	
	// 清除后台缓冲区和深度缓冲区
	m_CommandList->ClearRenderTargetView(rtvHandle, DirectX::Colors::LightBlue, 0, nullptr);
	// m_CommandList->ClearDepthStencilView(m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	// 指定将要渲染的目标缓冲区
	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	m_CommandList->DrawInstanced(3, 1, 0, 0);

	// 提交DearIMGui的渲染命令
	if (IMGuiHelloTriangle::GetInstance() != nullptr)
	{
		IMGuiHelloTriangle::GetInstance()->PopulateDearIMGuiCommand(m_CommandList.Get());
	}
	// 当前后台缓冲区切换到显示状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_CurrentBackBufferIndex].Get()
		, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_CommandList->Close());
}
