/*
 * D3D12HelloCube.cpp
 * Demo02 - DirectX12渲染立方体
 */

#include "stdafx.h"
#include "D3D12HelloCube.h"
#include <array>
#include <DirectXColors.h>
#include "DirectXBaseWork/D3D12Util.h"
#include "DirectXBaseWork/DXWorkHelper.h"
#include "IMGuiHelloCube.h"

using namespace DirectX;

D3D12HelloCube::D3D12HelloCube(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
	// 更新资源目录到当前实例程序文件夹下
	m_AssetPath += L"\\DirectXWork\\Demo02_DXHelloCube\\Source\\DXWorkHelloCube\\";
}

D3D12HelloCube::~D3D12HelloCube()
{
}

bool D3D12HelloCube::OnInit()
{
	FlushCommandQueue();
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));

	CreateInputLayout();
	CreateConstantBuffer();
	CreateRootSignature();
	CreateCubeGeometry();
	CompileShaderFile();
	BuildGraphicsPiplineState();
	ThrowIfFailed(m_CommandList->Close());
	// 执行初始化指令
	ID3D12CommandList* cmdLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	// 等待初始化完成
	FlushCommandQueue();

	return true;
}

void D3D12HelloCube::OnUpdate()
{
	ObjectConstants objConstancts;
	m_pConstantBuffer->CopyData(0, objConstancts);
}

void D3D12HelloCube::OnRender()
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

void D3D12HelloCube::OnDestroy()
{

}

void D3D12HelloCube::CreateInputLayout()
{
	m_InputElementDescs = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 3 * sizeof(float), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};
}

void D3D12HelloCube::CreateConstantBuffer()
{
	// 创建常量缓冲区的描述符堆
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc{};
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NumDescriptors = 1; // 目前只需要创建一个常量缓冲区
	cbvHeapDesc.NodeMask = 0U;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(m_CBVHeap.GetAddressOf())));
	// 上传堆中创建常量缓冲区
	m_pConstantBuffer = std::make_unique<UploadBuffer<ObjectConstants>>(m_Device.Get(), 1, true);
	// 常量缓冲区上可能有多个物体的常量数据，需要偏移到当前物体的常量数据内存位置，本例只有一个物体，故直接为0
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_pConstantBuffer->GetResource()->GetGPUVirtualAddress();
	int cubeCBufferIndex = 0;
	cbAddress += cubeCBufferIndex * m_pConstantBuffer->GetElementSize();
	// 创建常量缓冲区视图
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = m_pConstantBuffer->GetElementSize();
	m_Device->CreateConstantBufferView(&cbvDesc, m_CBVHeap->GetCPUDescriptorHandleForHeapStart());
}

void D3D12HelloCube::CreateRootSignature()
{
	// 根参数，可以是根常量/根描述符/描述符表
	CD3DX12_ROOT_PARAMETER slotRootParameter[1] = {};
	// 本例是一个常量缓冲区描述符表
	CD3DX12_DESCRIPTOR_RANGE cbvTable{};
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV // 描述符表类型为常量缓冲区
		, 1		// 描述符个数为1个
		, 0);	// 描述符绑定到基准着色器寄存器(base shader register)
	// 用描述符表初始化根参数
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);
	// 根签名描述又一组根参数构成
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	// Direct3D 12中规定需要将根签名描述进行序列化处理，转化为ID3DBlob接口表示的序列化数据后，才可将根签名描述数据传入CreatRootSignature中，正式创建根签名
	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig{ nullptr };
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob{ nullptr };
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());
	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);
	// 序列化处理后，正式创建根签名
	ThrowIfFailed(m_Device->CreateRootSignature(0U, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf())));
}

void D3D12HelloCube::CompileShaderFile()
{
	m_VertexShaderByteCode = CompileShader(m_AssetPath + L"HelloCubeShader.hlsl", nullptr, "VSMain", "vs_5_0");
	m_PixelShaderByteCode = CompileShader(m_AssetPath + L"HelloCubeShader.hlsl", nullptr, "PSMain", "ps_5_0");
}

void D3D12HelloCube::CreateCubeGeometry()
{
	// 立方体顶点属性
	std::array<Vertex, 8> cubeVertices =
	{
		Vertex({XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White)}),
		Vertex({XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Black)}),
		Vertex({XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(Colors::Red)}),
		Vertex({XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(Colors::Green)}),
		Vertex({XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(Colors::Blue)}),
		Vertex({XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(Colors::Yellow)}),
		Vertex({XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(Colors::Cyan)}),
		Vertex({XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(Colors::Magenta)})
	};
	// 索引数据
	std::array<UINT16, 36> cubeIndices =
	{
		// 立方体下表面
		0, 1, 2,
		2, 3, 0,
		// 立方体上表面
		4, 5, 6,
		6, 7, 4,
		// 立方体前表面
		0, 1, 5,
		5, 4, 0,
		// 立方体后表面
		3, 2, 6,
		6, 7, 3,
		// 立方体左表面
		0, 3, 7,
		7, 4, 0,
		// 立方体右表面
		1, 2, 6,
		6, 5, 1
	};
	// 计算顶点数据和索引数据的字节大小
	const UINT vbByteSize = (UINT)cubeVertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)cubeIndices.size() * sizeof(UINT16);
	// 构建立方体数据
	m_pMeshGeometry = std::make_unique<MeshGeometry>();
	m_pMeshGeometry->m_Name = "Cube";
	// 拷贝顶点数据和索引数据到CPU内存作为副本数据
	ThrowIfFailed(D3DCreateBlob(vbByteSize, m_pMeshGeometry->m_VertexBufferCPU.GetAddressOf()));
	CopyMemory(m_pMeshGeometry->m_VertexBufferCPU->GetBufferPointer(), cubeVertices.data(), vbByteSize);
	ThrowIfFailed(D3DCreateBlob(vbByteSize, m_pMeshGeometry->m_IndexBufferCPU.GetAddressOf()));
	CopyMemory(m_pMeshGeometry->m_IndexBufferCPU->GetBufferPointer(), cubeIndices.data(), ibByteSize);
	// 创建顶点缓冲区和索引缓冲区资源
	m_pMeshGeometry->m_VertexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), cubeVertices.data(), vbByteSize, m_pMeshGeometry->m_VertexBufferUploader);
	m_pMeshGeometry->m_IndexBufferGPU = D3D12Util::CreateBufferInDefaultHeap(m_Device.Get(), m_CommandList.Get(), cubeIndices.data(), ibByteSize, m_pMeshGeometry->m_IndexBufferUploader);
	m_pMeshGeometry->m_VertexStride = sizeof(Vertex);
	m_pMeshGeometry->m_VertexSize = vbByteSize;
	m_pMeshGeometry->m_IndexFormat = DXGI_FORMAT_R16_UINT;
	m_pMeshGeometry->m_IndexSize = ibByteSize;
	// 只有一个CubeMesh构成全部的顶点和索引数据
	m_pMeshGeometry->m_SubMeshGeometrys["CubeMesh"] = { (UINT)cubeIndices.size(), 0U, 0U };
}

void D3D12HelloCube::BuildGraphicsPiplineState()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
	ZeroMemory(&pipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	pipelineStateDesc.NodeMask = 0U;
	pipelineStateDesc.pRootSignature = m_RootSignature.Get();
	pipelineStateDesc.InputLayout = { m_InputElementDescs.data(), (UINT)m_InputElementDescs.size() };
	pipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_VertexShaderByteCode->GetBufferPointer()), m_VertexShaderByteCode->GetBufferSize() };
	pipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_PixelShaderByteCode->GetBufferPointer()), m_PixelShaderByteCode->GetBufferSize() };
	pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.NumRenderTargets = 1U;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.RTVFormats[0] = GetBackBufferFormat();
	pipelineStateDesc.SampleDesc.Count = Get4XMSAAEnable() ? 4 : 1;
	pipelineStateDesc.SampleDesc.Quality = Get4XMSAAEnable() ? m_4XMSAAQualityLevel - 1 : 0;
	pipelineStateDesc.DSVFormat = GetDepthStencilBufferFormat();

	ThrowIfFailed(m_Device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(m_PSO.GetAddressOf())));
}

void D3D12HelloCube::PopulateCommandList()
{
	// Command list allocators can only be reset when the associated command lists have finished execution on th GPU.
	// apps should use fence to determine GPU excution progress.
	ThrowIfFailed(m_CommandAllocator->Reset());
	// However, when ExecuteCommandList() is called on a particular command list, that command list can the be reset at any time and must be before re-recording.
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), m_PSO.Get()));
	// 当前后台缓冲区切换到渲染目标状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_CurrentBackBufferIndex].Get()
		, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentBackBufferIndex, m_RTVDescriptorSize);
	
	// Record commands.
	m_CommandList->RSSetViewports(1, &m_Viewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);
	// 清除后台缓冲区和深度缓冲区
	m_CommandList->ClearRenderTargetView(rtvHandle, DirectX::Colors::Blue, 0, nullptr);
	m_CommandList->ClearDepthStencilView(m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	// 指定将要渲染的目标缓冲区
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, true, &m_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	ID3D12DescriptorHeap* descriptorHeaps = { m_CBVHeap.Get() };
	m_CommandList->SetDescriptorHeaps(1, &descriptorHeaps);
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	m_CommandList->IASetVertexBuffers(0, 1, &m_pMeshGeometry->GetVertexBufferView());
	m_CommandList->IASetIndexBuffer(&m_pMeshGeometry->GetIndexBufferView());
	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->SetGraphicsRootDescriptorTable(0, m_CBVHeap->GetGPUDescriptorHandleForHeapStart());
	m_CommandList->DrawIndexedInstanced(m_pMeshGeometry->m_SubMeshGeometrys["CubeMesh"].m_IndexCount, 1, 0, 0, 0);

	// 提交DearIMGui的渲染命令
	if (IMGuiHelloCube::GetInstance() != nullptr)
	{
		IMGuiHelloCube::GetInstance()->PopulateDearIMGuiCommand(m_CommandList.Get());
	}
	// 当前后台缓冲区切换到显示状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_CurrentBackBufferIndex].Get()
		, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_CommandList->Close());
}

void D3D12HelloCube::OnMouseDown(UINT8 keyCode, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	std::string str{};
	str.append("x=").append(std::to_string(x)).append(", y=").append(std::to_string(y)).append("\n");
	::OutputDebugStringA(str.c_str());
}

void D3D12HelloCube::OnMouseUp(UINT8 keyCode, int x, int y)
{
}

void D3D12HelloCube::OnMouseMove(UINT8 keyCode, int x, int y)
{
	if ((keyCode & MK_LBUTTON) != 0)
	{
		// 左键点击
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos.y));
		m_Theta += dx;
		m_Phi += dy;
		m_Phi = MathUtil::Clamp(m_Phi, 0.1f, MathUtil::Pi - 0.1f);
	}
	else if ((keyCode & MK_RBUTTON) != 0)
	{
		// 右键点击
		float dx = 0.005f * static_cast<float>(x - m_LastMousePos.x);
		float dy = 0.005f * static_cast<float>(y - m_LastMousePos.y);
		m_Radius += dx - dy;
		m_Radius = MathUtil::Clamp(m_Radius, 3.f, 15.f);
	}
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}