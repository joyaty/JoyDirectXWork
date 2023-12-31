
#include "stdafx.h"
#include "DXSampleForReview.h"
#include <fstream>

using namespace DirectX;

const uint32_t kNumPillar = 4u;

DXSampleForReview::DXSampleForReview(const std::wstring& strTitle, uint32_t nWidth/* = 1280U*/, uint32_t nHeight/* = 720U*/)
	: m_Title(strTitle)
	, m_Width(nWidth)
	, m_Height(nHeight)
	, m_IsUseWarpDevice(false)
{
	WCHAR assetPath[512];
	GetAssetsPath(assetPath, _countof(assetPath));
	std::wstring strAssetPath = assetPath;
	size_t rootFolderIndex = strAssetPath.find(L"\\Build", 0);
	m_RootAssetPath = strAssetPath.substr(0, rootFolderIndex);
	m_AssetPath = m_RootAssetPath + _T("\\DirectXWork\\Demo11_DXReview\\");
	m_AspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);
}

bool DXSampleForReview::Initialize(HWND hWndInstance)
{
	m_WindowInstance = hWndInstance;
	CreateRenderDevice();
	CreateCommandObjects();
	CreateSwapchain();
	CreateDescriptorHeaps();
	OnResize(m_Width, m_Height);

	FlushCommandQueue();
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));

	CreateSceneObjects();
	BuildMaterials();
	BuildRenderItems();
	CreateCBDescriptorHeaps();
	BuildConstantBufferView();
	CreateRootSignature();
	CompileShaderFile();
	BuildPSO();

	// 提交初始化相关的命令列表
	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(1, cmdList);
	// 等待初始化指令执行完成
	FlushCommandQueue();

	return true;
}

void DXSampleForReview::Terminate()
{
	// 等待GPU执行完所有渲染指令
	FlushCommandQueue();
	// 关闭围栏事件
	CloseHandle(m_FenceEvent);
}

void DXSampleForReview::Update(float fDeltaTime, float fTotalTime)
{
	UpdateCamera();
	UpdateObjCBs(fDeltaTime, fTotalTime);
	UpdateMatCBs(fDeltaTime, fTotalTime);
	UpdatePassCB(fDeltaTime, fTotalTime);
}

void DXSampleForReview::Render()
{
	// 记录当前的渲染指令
	PopulateCommandList();
	// 提交到渲染队列
	ID3D12CommandList* cmdList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(1, cmdList);
	// 交换缓冲区
	ThrowIfFailed(m_SwapChain->Present(1, 0));
	// 等待渲染指令执行完成
	FlushCommandQueue();
	m_CurrentFrameBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
}

void DXSampleForReview::OnResize(uint32_t nWidth, uint32_t nHeight)
{
	// 等待渲染指令执行完毕，清理当前的指令列表
	FlushCommandQueue();
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));
	// 清理渲染目标缓冲区资源和深度模版缓冲区资源
	for (int i = kFrameBufferCount - 1; i >= 0; --i)
	{
		if (m_RenderTargetBuffers[i] != nullptr)
		{
			m_RenderTargetBuffers[i].Reset();
		}
	}
	if (m_DepthStencilBuffer != nullptr)
	{
		m_DepthStencilBuffer.Reset();
	}
	// 更新窗口尺寸，使用新尺寸，重置交换链中的渲染目标缓冲区资源
	m_Width = nWidth;
	m_Height = nHeight;
	m_SwapChain->ResizeBuffers(kFrameBufferCount, m_Width, m_Height, m_BackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	// 重建渲染目标缓冲区描述符和深度模板缓冲区视图
	BuildRTV();
	BuildDSV();
	m_CurrentFrameBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
	// 更新视口
	m_Viewport.Width = static_cast<float>(m_Width);
	m_Viewport.Height = static_cast<float>(m_Height);
	m_Viewport.TopLeftX = 0;
	m_Viewport.TopLeftY = 0;
	m_Viewport.MaxDepth = 1.f;
	m_Viewport.MinDepth = 0.f;
	// 更新裁剪区域
	m_ScissorRect = {0, 0, static_cast<int>(m_Width), static_cast<int>(m_Height) };

	// 执行初始化指令
	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdList[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(1, cmdList);
	FlushCommandQueue();
	// 根据FOV，屏幕比，远近平面位置计算投影矩阵
	DirectX::XMMATRIX projMatrix = DirectX::XMMatrixPerspectiveFovLH(0.25f * MathUtil::Pi, m_AspectRatio, 1.f, 1000.f);
	DirectX::XMStoreFloat4x4(&m_ProjMatrix, projMatrix);
}

void DXSampleForReview::OnKeyDown(UINT8 keyCode)
{
	
}

void DXSampleForReview::OnKeyUp(UINT8 keyCode)
{
}

void DXSampleForReview::OnMouseDown(UINT8 keyCode, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void DXSampleForReview::OnMouseUp(UINT8 keyCode, int x, int y)
{
}

void DXSampleForReview::OnMouseMove(UINT8 keyCode, int x, int y)
{
	if ((keyCode & MK_LBUTTON) != 0)
	{
		// 鼠标左键点击，处理旋转逻辑
		float dx = DirectX::XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos.x));
		float dy = DirectX::XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos.y));
		m_Phi += dx;
		m_Theta += dy;
		m_Theta = MathUtil::Clamp(m_Theta, 0.1f, MathUtil::Pi - 0.1f);
	}
	else if ((keyCode & MK_RBUTTON) != 0)
	{
		// 鼠标右键点击，处理缩放逻辑
		float dx = 0.005f * static_cast<float>(x - m_LastMousePos.x);
		float dy = 0.005f * static_cast<float>(y - m_LastMousePos.y);
		m_Radius += dx - dy;
		m_Radius = MathUtil::Clamp(m_Radius, 3.f, 100.f);
	}
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

/// <summary>
/// 查询显卡硬件，创建逻辑渲染设备
/// </summary>
void DXSampleForReview::CreateRenderDevice()
{
	UINT iCreateDXGIFactoryFlags = 0;
#if defined(_DEBUG) || defined(DEBUG)
	// 启用D3D12调试层
	Microsoft::WRL::ComPtr<ID3D12Debug> pDebugController{};
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(pDebugController.GetAddressOf()))))
	{
		pDebugController->EnableDebugLayer();
		iCreateDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	ThrowIfFailed(CreateDXGIFactory2(iCreateDXGIFactoryFlags, IID_PPV_ARGS(m_DXGIFactoryV6.GetAddressOf())));

	if (m_IsUseWarpDevice)
	{
		// 使用微软软件光栅器渲染
		Microsoft::WRL::ComPtr<IDXGIAdapter> pCurrentAdapater{ nullptr };
		ThrowIfFailed(m_DXGIFactoryV6->EnumWarpAdapter(IID_PPV_ARGS(pCurrentAdapater.GetAddressOf())));
		D3D12CreateDevice(pCurrentAdapater.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_RenderDevice.GetAddressOf()));
		pCurrentAdapater->GetDesc(&m_AdapterDesc);
	}
	else
	{
		// 使用物理显卡
		Microsoft::WRL::ComPtr<IDXGIAdapter1> pCurrentHardwareAdapter{ nullptr };
		// 优先使用高性能显卡
		for (UINT nAdapterIndex = 0
			; SUCCEEDED(m_DXGIFactoryV6->EnumAdapterByGpuPreference(nAdapterIndex
				, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE
				, IID_PPV_ARGS(pCurrentHardwareAdapter.GetAddressOf())))
			; ++nAdapterIndex)
		{
			DXGI_ADAPTER_DESC1 descV1{};
			pCurrentHardwareAdapter->GetDesc1(&descV1);
			if ((descV1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}
			// 获取第一个可成功创建逻辑渲染设备的硬件显卡
			if (SUCCEEDED(D3D12CreateDevice(pCurrentHardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}

		if (pCurrentHardwareAdapter.Get() == nullptr)
		{
			// 通过EnumAdapterByGpuPreference获取识别，使用普通接口EnumAdapters1来枚举所有可用的显示适配器
			for (UINT nAdapterIndex = 0
				; SUCCEEDED(m_DXGIFactoryV6->EnumAdapters1(nAdapterIndex, pCurrentHardwareAdapter.GetAddressOf()))
				; ++nAdapterIndex)
			{
				DXGI_ADAPTER_DESC1 descV1{};
				pCurrentHardwareAdapter->GetDesc1(&descV1);
				if ((descV1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					continue;
				}
				// 获取第一个可成功创建逻辑渲染设备的硬件显卡
				if (SUCCEEDED(D3D12CreateDevice(pCurrentHardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
		}

		if (pCurrentHardwareAdapter.Get() != nullptr)
		{
			pCurrentHardwareAdapter->GetDesc(&m_AdapterDesc);
			ThrowIfFailed(D3D12CreateDevice(pCurrentHardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_RenderDevice.GetAddressOf())));
		}
	}

	// 成功创建逻辑渲染设备，获取渲染设备的一些参数，作为渲染管线初始化的可选项
	if (m_RenderDevice.Get() != nullptr)
	{
		// 检查是否支持4XMSAA，获取4XMSAA下的质量品质
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels{};
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.SampleCount = 4;
		msQualityLevels.NumQualityLevels = 0;
		msQualityLevels.Format = m_BackBufferFormat;
		ThrowIfFailed(m_RenderDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)));
		m_4XMSAAQualityLevel = msQualityLevels.NumQualityLevels;
		// 获取各类描述符堆的大小
		m_RTVDescriptorSize = m_RenderDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_DSVDescriptorSize = m_RenderDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		m_CBVUAVDescriptorSize = m_RenderDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_SamplerDescriptorSize = m_RenderDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

		// 创建围栏，同步CPU和GPU工作
		ThrowIfFailed(m_RenderDevice->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.GetAddressOf())));
		m_FenceEvent = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		if (m_FenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
	}
}

/// <summary>
/// 创建命令队列和命令列表
/// </summary>
void DXSampleForReview::CreateCommandObjects()
{
	// 创建命令队列，命令分配器，命令列表
	D3D12_COMMAND_QUEUE_DESC desc{};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	ThrowIfFailed(m_RenderDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_CommandQueue.GetAddressOf())));
	ThrowIfFailed(m_RenderDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAllocator.GetAddressOf()))); 
	ThrowIfFailed(m_RenderDevice->CreateCommandList(0U, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(m_CommandList.GetAddressOf())));
	// 命令列表创建后默认是记录状态，需要关闭
	ThrowIfFailed(m_CommandList->Close());
}

/// <summary>
/// 构建交换链
/// </summary>
void DXSampleForReview::CreateSwapchain()
{
	DXGI_SWAP_CHAIN_DESC1 desc{};
	desc.BufferCount = kFrameBufferCount;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 作为渲染目标
	desc.Width = m_Width;
	desc.Height = m_Height;
	desc.Format = m_BackBufferFormat;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.SampleDesc.Count = m_IsEnable4XMSAA ? 4 : 1;
	desc.SampleDesc.Quality = m_IsEnable4XMSAA ? (m_4XMSAAQualityLevel - 1) : 0;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> pSwapchain{ };
	ThrowIfFailed(m_DXGIFactoryV6->CreateSwapChainForHwnd(
		m_CommandQueue.Get() // 交换链需要通过命令队列刷新，DX11以及之前，此参数为ID3D12Device指针，DX12变更为ID3D12CommandQueue指针，不可为空
		, m_WindowInstance				// 窗口句柄
		, &desc							// 交换链描述
		, nullptr						// 全屏交换链描述，可选
		, nullptr						// 指定输出的显示设备，可选
		, pSwapchain.GetAddressOf()		// 输出交换链指针
		));
	// 不需要全屏模式，防止ALT+ENTER组合键进入全屏
	m_DXGIFactoryV6->MakeWindowAssociation(m_WindowInstance, DXGI_MWA_NO_ALT_ENTER);
	// 转化为SwapChain3，以便获取当前的后台缓冲区索引
	ThrowIfFailed(pSwapchain.As(&m_SwapChain));
	m_CurrentFrameBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
}

/// <summary>
/// 创建各类资源描述符堆
/// </summary>
void DXSampleForReview::CreateDescriptorHeaps()
{
	// 渲染目标缓冲区描述符堆，双缓冲，需要两个渲染目标缓冲区描述符
	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc{};
	rtvDesc.NodeMask = 0U;
	rtvDesc.NumDescriptors = kFrameBufferCount;
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(m_RenderDevice->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(m_RTVHeap.GetAddressOf())));
	// 深度模板缓冲区描述符堆，只需要一个深度模板缓冲区
	D3D12_DESCRIPTOR_HEAP_DESC dsvDesc{};
	dsvDesc.NodeMask = 0U;
	dsvDesc.NumDescriptors = 1;
	dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(m_RenderDevice->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(m_DSVHeap.GetAddressOf())));
}

/// <summary>
/// 创建常量缓冲区描述符堆
/// </summary>
void DXSampleForReview::CreateCBDescriptorHeaps()
{
	uint32_t objCount = static_cast<uint32_t>(m_AllRenderItems.size());
	uint32_t matCount = static_cast<uint32_t>(m_AllMaterials.size());
	// 创建常量缓冲区描述符
	D3D12_DESCRIPTOR_HEAP_DESC cbvDesc{};
	cbvDesc.NodeMask = 0U;
	cbvDesc.NumDescriptors = objCount + matCount + 1; // 渲染网格数量 + 材质数量 + 渲染过程缓冲区数量
	cbvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(m_RenderDevice->CreateDescriptorHeap(&cbvDesc, IID_PPV_ARGS(m_CBVHeap.GetAddressOf())));
}

/// <summary>
/// 构建渲染目标缓冲区视图，再构建交换链之后调后
/// </summary>
void DXSampleForReview::BuildRTV()
{
	// 获取RTV描述符堆的第一个描述符
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ m_RTVHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT i = 0; i < kFrameBufferCount; ++i)
	{
		// 从前面创建的交换链中获取渲染目标缓冲区资源
		ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(m_RenderTargetBuffers[i].GetAddressOf())));
		// 绑定渲染目标缓冲区资源和描述符
		m_RenderDevice->CreateRenderTargetView(m_RenderTargetBuffers[i].Get(), nullptr, rtvHandle);
		// 相同索引位置记录渲染目标缓冲区所关联的描述符
		m_RtvHandles[i] = rtvHandle;
		// 偏移到描述符堆的下一个描述符位置
		rtvHandle.Offset(1, m_RTVDescriptorSize);
	}
}

/// <summary>
/// 构建深度模板缓冲区视图，需要在构建渲染指令队列和指令列表之后调用，因为需要使用指令列表切换资源的状态
/// </summary>
void DXSampleForReview::BuildDSV()
{
	D3D12_RESOURCE_DESC dsvDesc{};
	dsvDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 深度模板缓冲区本质是一张2D纹理
	dsvDesc.Alignment = 0;
	dsvDesc.Width = m_Width;
	dsvDesc.Height = m_Height;
	dsvDesc.DepthOrArraySize = 1; // 纹素单位表示深度还是数组大小
	dsvDesc.MipLevels = 1; // 深度模板缓冲区不需要mipmaps
	dsvDesc.Format = m_DepthStencilFormat;
	dsvDesc.SampleDesc.Count = m_IsEnable4XMSAA ? 4 : 1;
	dsvDesc.SampleDesc.Quality = m_IsEnable4XMSAA ? (m_4XMSAAQualityLevel - 1) : 0;
	dsvDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	// 深度模板缓冲区为GPU可读，所在的堆类别为默认堆，这里指定堆属性为默认堆，进而在对应的堆上创建深度模板缓冲区资源
	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_CLEAR_VALUE clearValue{ m_DepthStencilFormat, 1.f, 0 };
	ThrowIfFailed(m_RenderDevice->CreateCommittedResource(&heapProperties
		, D3D12_HEAP_FLAG_NONE
		, &dsvDesc
		, D3D12_RESOURCE_STATE_COMMON
		, &clearValue
		, IID_PPV_ARGS(m_DepthStencilBuffer.GetAddressOf())));
	// 获取描述符，绑定到相应的深度模板缓冲区资源
	m_DsvHandle = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
	m_RenderDevice->CreateDepthStencilView(m_DepthStencilBuffer.Get()
		, nullptr // 深度模板缓冲区描述符格式，由于深度模板缓冲区资源创建时指定了相应的格式，故这里可以为空
		, m_DsvHandle);
	// 将深度模板缓冲区资源状态转换为深度写入状态
	m_CommandList->ResourceBarrier(1U, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_DepthStencilBuffer.Get()
		, D3D12_RESOURCE_STATE_COMMON
		, D3D12_RESOURCE_STATE_DEPTH_WRITE
	));
}

/// <summary>
/// 创建场景几何体物件
/// </summary>
void DXSampleForReview::CreateSceneObjects()
{
	// 创建地平面
	Mesh planeMesh{};
	GeometryGenerator::CreateGrid(20, 20, planeMesh, 20, 20);
	uint32_t planeVertexBufferSize = sizeof(Vertex) * static_cast<uint32_t>(planeMesh.Vertices.size());
	uint32_t planeIndexBufferSize = sizeof(std::uint16_t) * static_cast<uint32_t>(planeMesh.GetIndices16().size());
	std::unique_ptr<MeshGeometry> pPlaneGeometry = std::make_unique<MeshGeometry>();
	pPlaneGeometry->m_Name = "Plane";
	pPlaneGeometry->m_VertexBufferGPU = D3DHelper::CreateBufferInDefaultHeap(m_RenderDevice.Get(), m_CommandList.Get(), planeMesh.Vertices.data(), planeVertexBufferSize, pPlaneGeometry->m_VertexBufferUploader);
	pPlaneGeometry->m_VertexSize = planeVertexBufferSize;
	pPlaneGeometry->m_VertexStride = sizeof(Vertex);
	pPlaneGeometry->m_IndexBufferGPU = D3DHelper::CreateBufferInDefaultHeap(m_RenderDevice.Get(), m_CommandList.Get(), planeMesh.GetIndices16().data(), planeIndexBufferSize, pPlaneGeometry->m_IndexBufferUploader);
	pPlaneGeometry->m_IndexSize = planeIndexBufferSize;
	pPlaneGeometry->m_IndexFormat = DXGI_FORMAT_R16_UINT;
	MeshGeometry::SubMeshGeometry subGridMesh{};
	subGridMesh.m_IndexCount = static_cast<uint32_t>(planeMesh.GetIndices16().size());
	subGridMesh.m_StartIndexLocation = 0;
	subGridMesh.m_BaseVertexLocation = 0;
	pPlaneGeometry->m_SubMeshGeometrys["Grid"] = subGridMesh;
	m_SceneObjectes[pPlaneGeometry->m_Name] = std::move(pPlaneGeometry);
	// 4个柱子
	for (int i = 0; i < kNumPillar; ++i)
	{
		std::unique_ptr<MeshGeometry> pPillarGeometry = std::make_unique<MeshGeometry>();
		pPillarGeometry->m_Name = "Pillar" + std::to_string(i);
		// 每个柱子包含一个圆柱几何体和一个球体，两个网格体的顶点数据汇聚到一个缓冲区，作为SubMesh存在
		std::vector<Vertex> pillarsVertices{};
		std::vector<uint16_t> pillarsIndices{};
		Mesh cylinderMesh{};
		Mesh sphereMesh{};
		GeometryGenerator::CreateCylinder(1.f, 1.f, 5, 16, 1, cylinderMesh);
		GeometryGenerator::CreateSphere(1.5f, 2, sphereMesh);
		pillarsVertices.insert(pillarsVertices.end(), cylinderMesh.Vertices.begin(), cylinderMesh.Vertices.end());
		pillarsVertices.insert(pillarsVertices.end(), sphereMesh.Vertices.begin(), sphereMesh.Vertices.end());
		pillarsIndices.insert(pillarsIndices.end(), cylinderMesh.GetIndices16().begin(), cylinderMesh.GetIndices16().end());
		pillarsIndices.insert(pillarsIndices.end(), sphereMesh.GetIndices16().begin(), sphereMesh.GetIndices16().end());

		uint32_t pillarVertexBufferSize = sizeof(Vertex) * static_cast<uint32_t>(pillarsVertices.size());
		uint32_t pillarIndiceBufferSize = sizeof(uint16_t) * static_cast<uint32_t>(pillarsIndices.size());
		pPillarGeometry->m_VertexBufferGPU = D3DHelper::CreateBufferInDefaultHeap(m_RenderDevice.Get(), m_CommandList.Get(), pillarsVertices.data(), pillarVertexBufferSize, pPillarGeometry->m_VertexBufferUploader);
		pPillarGeometry->m_VertexSize = pillarVertexBufferSize;
		pPillarGeometry->m_VertexStride = sizeof(Vertex);
		pPillarGeometry->m_IndexBufferGPU = D3DHelper::CreateBufferInDefaultHeap(m_RenderDevice.Get(), m_CommandList.Get(), pillarsIndices.data(), pillarIndiceBufferSize, pPillarGeometry->m_IndexBufferUploader);
		pPillarGeometry->m_IndexSize = pillarIndiceBufferSize;
		pPillarGeometry->m_IndexFormat = DXGI_FORMAT_R16_UINT;
		MeshGeometry::SubMeshGeometry subCylinderMesh{};
		subCylinderMesh.m_IndexCount = static_cast<uint32_t>(cylinderMesh.GetIndices16().size());
		subCylinderMesh.m_StartIndexLocation = 0;
		subCylinderMesh.m_BaseVertexLocation = 0;
		pPillarGeometry->m_SubMeshGeometrys["Cylinder"] = subCylinderMesh;
		MeshGeometry::SubMeshGeometry subSphereMesh{};
		subSphereMesh.m_IndexCount = static_cast<uint32_t>(sphereMesh.GetIndices16().size());
		subSphereMesh.m_StartIndexLocation = subCylinderMesh.m_IndexCount;
		subSphereMesh.m_BaseVertexLocation = static_cast<uint32_t>(cylinderMesh.Vertices.size());
		pPillarGeometry->m_SubMeshGeometrys["Sphere"] = subSphereMesh;

		m_SceneObjectes[pPillarGeometry->m_Name] = std::move(pPillarGeometry);
	}
	// 创建一个立方体
	Mesh cubeMesh{};
	// GeometryGenerator::CreateCylinder(1.f, 3.f, 5.f, 32, 5, cubeMesh);
	GeometryGenerator::CreateCube(8.f, 2.f, 8.f, 0, cubeMesh);
	// GeometryGenerator::CreateQuad(10, 10, cubeMesh, 5, 5);
	// GeometryGenerator::CreateGrid(10, 10, cubeMesh, 10, 10);
	// GeometryGenerator::CreateSphere(1, 3, cubeMesh);
	// 顶点缓冲区和索引缓冲区的大小
	uint32_t vertexBufferSize = sizeof(Vertex) * static_cast<uint32_t>(cubeMesh.Vertices.size());
	uint32_t indexBufferSize = sizeof(std::uint16_t) * static_cast<uint32_t>(cubeMesh.GetIndices16().size());
	std::unique_ptr<MeshGeometry> pMeshGeometry = std::make_unique<MeshGeometry>();
	pMeshGeometry->m_Name = "Platform";
	pMeshGeometry->m_VertexBufferGPU = D3DHelper::CreateBufferInDefaultHeap(m_RenderDevice.Get(), m_CommandList.Get(), cubeMesh.Vertices.data(), vertexBufferSize, pMeshGeometry->m_VertexBufferUploader);
	pMeshGeometry->m_VertexStride = sizeof(Vertex);
	pMeshGeometry->m_VertexSize = static_cast<uint32_t>(pMeshGeometry->m_VertexStride * cubeMesh.Vertices.size());
	pMeshGeometry->m_IndexBufferGPU = D3DHelper::CreateBufferInDefaultHeap(m_RenderDevice.Get(), m_CommandList.Get(), cubeMesh.GetIndices16().data(), indexBufferSize, pMeshGeometry->m_IndexBufferUploader);
	pMeshGeometry->m_IndexFormat = DXGI_FORMAT_R16_UINT;
	pMeshGeometry->m_IndexSize = sizeof(std::uint16_t) * static_cast<uint32_t>(cubeMesh.GetIndices16().size());
	MeshGeometry::SubMeshGeometry subCubeMesh{};
	subCubeMesh.m_IndexCount = static_cast<uint32_t>(cubeMesh.GetIndices16().size());
	subCubeMesh.m_StartIndexLocation = 0;
	subCubeMesh.m_BaseVertexLocation = 0;
	pMeshGeometry->m_SubMeshGeometrys["Cube"] = subCubeMesh;
	m_SceneObjectes[pMeshGeometry->m_Name] = std::move(pMeshGeometry);
	// 骷髅模型，从文件读取
	Mesh skullMesh{};
	std::ifstream fin(m_RootAssetPath + _T("\\Assets\\Models\\skull.txt"));
	uint32_t skullVertexCount{};
	uint32_t skullTriangleCount{};
	std::string strIgnore{};
	fin >> strIgnore >> skullVertexCount;
	fin >> strIgnore >> skullTriangleCount;
	fin >> strIgnore >> strIgnore >> strIgnore;
	fin >> strIgnore;
	std::vector<Vertex> skullVertices{};
	skullVertices.resize(skullVertexCount);
	for (uint32_t i = 0; i < skullVertexCount; ++i)
	{
		fin >> skullVertices[i].Position.x >> skullVertices[i].Position.y >> skullVertices[i].Position.z
			>> skullVertices[i].Normal.x >> skullVertices[i].Normal.y >> skullVertices[i].Normal.z;
		// 没有纹理，纹理坐标直接设置为0
		skullVertices[i].TexCoord = DirectX::XMFLOAT2(0.f, 0.f);
		skullVertices[i].TangentU = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
		skullVertices[i].Color = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	}
	fin >> strIgnore >> strIgnore >> strIgnore;
	std::vector<uint16_t> skullIndices{};
	skullIndices.resize(skullTriangleCount * 3);
	for (uint32_t i = 0; i < skullTriangleCount; ++i)
	{
		fin >> skullIndices[i * 3] >> skullIndices[i * 3 + 1] >> skullIndices[i * 3 + 2];
	}
	fin.close();

	uint32_t skullVertexBufferSize = sizeof(Vertex) * skullVertexCount;
	uint32_t skullIndexBufferSize = sizeof(std::uint16_t) * skullTriangleCount * 3;
	std::unique_ptr<MeshGeometry> pSkullGeometry = std::make_unique<MeshGeometry>();
	pSkullGeometry->m_Name = "Skull";
	pSkullGeometry->m_VertexBufferGPU = D3DHelper::CreateBufferInDefaultHeap(m_RenderDevice.Get(), m_CommandList.Get(), skullVertices.data(), skullVertexBufferSize, pSkullGeometry->m_VertexBufferUploader);
	pSkullGeometry->m_VertexSize = skullVertexBufferSize;
	pSkullGeometry->m_VertexStride = sizeof(Vertex);
	pSkullGeometry->m_IndexBufferGPU = D3DHelper::CreateBufferInDefaultHeap(m_RenderDevice.Get(), m_CommandList.Get(), skullIndices.data(), skullIndexBufferSize, pSkullGeometry->m_IndexBufferUploader);
	pSkullGeometry->m_IndexSize = skullIndexBufferSize;
	pSkullGeometry->m_IndexFormat = DXGI_FORMAT_R16_UINT;
	MeshGeometry::SubMeshGeometry skullSubMesh{};
	skullSubMesh.m_IndexCount = skullTriangleCount * 3;
	skullSubMesh.m_StartIndexLocation = 0;
	skullSubMesh.m_BaseVertexLocation = 0;
	pSkullGeometry->m_SubMeshGeometrys["Skull"] = skullSubMesh;
	m_SceneObjectes[pSkullGeometry->m_Name] = std::move(pSkullGeometry);
}

void DXSampleForReview::BuildMaterials()
{
	// TODO 构建渲染场景使用的材质
	std::unique_ptr<Material> pTestMat = std::make_unique<Material>();
	pTestMat->m_Name = "Test";
	pTestMat->m_DiffuseAlbedo = DirectX::XMFLOAT4(1.f, 1.0f, 1.0f, 1.0f);
	pTestMat->m_FresnelR0 = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
	pTestMat->m_Roughness = 0.25f;
	pTestMat->m_CbvIndex = 0;
	// 保存到材质集合中
	m_AllMaterials[pTestMat->m_Name] = std::move(pTestMat);
}

void DXSampleForReview::BuildRenderItems()
{
	// 地表
	std::unique_ptr<DemoRenderItem> planeRenderItem = std::make_unique<DemoRenderItem>();
	planeRenderItem->m_MeshGeo = m_SceneObjectes["Plane"].get();
	DirectX::XMStoreFloat4x4(&planeRenderItem->m_LoclToWorldMatrix, DirectX::XMMatrixTranslation(0.f, 0.f, 0.f));
	planeRenderItem->m_ObjectCBIndex = 0;
	planeRenderItem->m_Material = m_AllMaterials["Test"].get();
	planeRenderItem->m_NumFrameDirty = 1;
	planeRenderItem->m_IndexCount = planeRenderItem->m_MeshGeo->m_SubMeshGeometrys["Grid"].m_IndexCount;
	planeRenderItem->m_StartIndexLocation = planeRenderItem->m_MeshGeo->m_SubMeshGeometrys["Grid"].m_StartIndexLocation;
	planeRenderItem->m_StartVertexLocation = planeRenderItem->m_MeshGeo->m_SubMeshGeometrys["Grid"].m_BaseVertexLocation;
	m_AllRenderItems.emplace_back(std::move(planeRenderItem));
	// 4个柱子
	int weights[kNumPillar * 2] = { -1, -1, -1, 1, 1, 1, 1, -1 };
	float offsetBaseValue = 7.f;
	for (int i = 0; i < kNumPillar; ++i)
	{
		std::string geometryName = "Pillar" + std::to_string(i);
		std::unique_ptr<DemoRenderItem> cylinderRenderItem = std::make_unique<DemoRenderItem>();
		cylinderRenderItem->m_MeshGeo = m_SceneObjectes[geometryName].get();
		DirectX::XMStoreFloat4x4(&cylinderRenderItem->m_LoclToWorldMatrix, DirectX::XMMatrixTranslation(offsetBaseValue * weights[2 * i], 5.f * 0.5f, offsetBaseValue * weights[2 * i + 1]));
		cylinderRenderItem->m_ObjectCBIndex = 2 * i + 1;
		cylinderRenderItem->m_Material = m_AllMaterials["Test"].get();
		cylinderRenderItem->m_NumFrameDirty = 1;
		cylinderRenderItem->m_IndexCount = cylinderRenderItem->m_MeshGeo->m_SubMeshGeometrys["Cylinder"].m_IndexCount;
		cylinderRenderItem->m_StartIndexLocation = cylinderRenderItem->m_MeshGeo->m_SubMeshGeometrys["Cylinder"].m_StartIndexLocation;
		cylinderRenderItem->m_StartVertexLocation = cylinderRenderItem->m_MeshGeo->m_SubMeshGeometrys["Cylinder"].m_BaseVertexLocation;
		m_AllRenderItems.emplace_back(std::move(cylinderRenderItem));

		std::unique_ptr<DemoRenderItem> sphereRenderItem = std::make_unique<DemoRenderItem>();
		sphereRenderItem->m_MeshGeo = m_SceneObjectes[geometryName].get();
		DirectX::XMStoreFloat4x4(&sphereRenderItem->m_LoclToWorldMatrix, DirectX::XMMatrixTranslation(offsetBaseValue * weights[2 * i], 5.f + 1.5f, offsetBaseValue * weights[2 * i + 1]));
		sphereRenderItem->m_ObjectCBIndex = 2 * i + 2;
		sphereRenderItem->m_Material = m_AllMaterials["Test"].get();
		sphereRenderItem->m_NumFrameDirty = 1;
		sphereRenderItem->m_IndexCount = sphereRenderItem->m_MeshGeo->m_SubMeshGeometrys["Sphere"].m_IndexCount;
		sphereRenderItem->m_StartIndexLocation = sphereRenderItem->m_MeshGeo->m_SubMeshGeometrys["Sphere"].m_StartIndexLocation;
		sphereRenderItem->m_StartVertexLocation = sphereRenderItem->m_MeshGeo->m_SubMeshGeometrys["Sphere"].m_BaseVertexLocation;
		m_AllRenderItems.emplace_back(std::move(sphereRenderItem));
	}
	// 平台
	std::unique_ptr<DemoRenderItem> platformRenderItem = std::make_unique<DemoRenderItem>();
	platformRenderItem->m_MeshGeo = m_SceneObjectes["Platform"].get();
	DirectX::XMStoreFloat4x4(&platformRenderItem->m_LoclToWorldMatrix, DirectX::XMMatrixTranslation(0.f, 1.f, 0.f));
	platformRenderItem->m_ObjectCBIndex = 2 * kNumPillar + 1;
	platformRenderItem->m_Material = m_AllMaterials["Test"].get();
	platformRenderItem->m_NumFrameDirty = 1;
	platformRenderItem->m_IndexCount = platformRenderItem->m_MeshGeo->m_SubMeshGeometrys["Cube"].m_IndexCount;
	platformRenderItem->m_StartIndexLocation = platformRenderItem->m_MeshGeo->m_SubMeshGeometrys["Cube"].m_StartIndexLocation;
	platformRenderItem->m_StartVertexLocation = platformRenderItem->m_MeshGeo->m_SubMeshGeometrys["Cube"].m_BaseVertexLocation;
	m_AllRenderItems.emplace_back(std::move(platformRenderItem));
	// 骷髅模型
	std::unique_ptr<DemoRenderItem> skullRenderItem = std::make_unique<DemoRenderItem>();
	skullRenderItem->m_MeshGeo = m_SceneObjectes["Skull"].get();
	DirectX::XMMATRIX transMatrix = DirectX::XMMatrixTranslation(0.f, 2.2f, 0.f);
	DirectX::XMMATRIX rotaMatrix = DirectX::XMMatrixRotationY(DirectX::XM_PIDIV4);
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(0.7f, 0.7f, 0.7f);
	DirectX::XMStoreFloat4x4(&skullRenderItem->m_LoclToWorldMatrix, scaleMatrix * rotaMatrix * transMatrix);
	skullRenderItem->m_ObjectCBIndex = 2 * kNumPillar + 2;
	skullRenderItem->m_Material = m_AllMaterials["Test"].get();
	skullRenderItem->m_NumFrameDirty = 1;
	skullRenderItem->m_IndexCount = skullRenderItem->m_MeshGeo->m_SubMeshGeometrys["Skull"].m_IndexCount;
	skullRenderItem->m_StartIndexLocation = skullRenderItem->m_MeshGeo->m_SubMeshGeometrys["Skull"].m_StartIndexLocation;
	skullRenderItem->m_StartVertexLocation = skullRenderItem->m_MeshGeo->m_SubMeshGeometrys["Skull"].m_BaseVertexLocation;
	m_AllRenderItems.emplace_back(std::move(skullRenderItem));
}

/// <summary>
/// 构建常量缓冲区描述符
/// </summary>
void DXSampleForReview::BuildConstantBufferView()
{
	uint32_t objectCount = static_cast<uint32_t>(m_AllRenderItems.size());
	// 创建物体层级常量缓冲区
	m_PerOjectConstantBuffer = std::make_unique<UploadBuffer<PerObjectConstants>>(m_RenderDevice.Get(), objectCount, true);
	// 获取物体常量缓冲区资源的起始地址
	for (uint32_t i = 0; i < objectCount; ++i)
	{
		// 大的缓冲区资源有多个物体级的常量缓冲区组成，计算每个物体常量缓冲区的偏移，从而确定准确地址
		D3D12_GPU_VIRTUAL_ADDRESS objCbAddr = m_PerOjectConstantBuffer->GetResource()->GetGPUVirtualAddress();
		objCbAddr += i * m_PerOjectConstantBuffer->GetElementSize();
		// 获取CBV描述符的handle，
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_CBVHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(i, m_CBVUAVDescriptorSize);
		// 绑定常量缓冲区资源和描述符堆，创建常量缓冲区描述符
		D3D12_CONSTANT_BUFFER_VIEW_DESC desc{};
		desc.BufferLocation = objCbAddr;
		desc.SizeInBytes = m_PerOjectConstantBuffer->GetElementSize();
		m_RenderDevice->CreateConstantBufferView(&desc, handle);
	}

	uint32_t materialCount = static_cast<uint32_t>(m_AllMaterials.size());
	m_PerMaterialConstantBuffer = std::make_unique<UploadBuffer<PerMaterialConstants>>(m_RenderDevice.Get(), materialCount, true);
	// 为每个材质常量缓冲区绑定描述符
	for (uint32_t i = 0; i < materialCount; ++i)
	{
		// 获取各个常量缓冲区资源的GPU地址
		D3D12_GPU_VIRTUAL_ADDRESS matCbAddr = m_PerMaterialConstantBuffer->GetResource()->GetGPUVirtualAddress();
		matCbAddr += i * m_PerMaterialConstantBuffer->GetElementSize();
		// 获取描述符的handle
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_CBVHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(i + objectCount, m_CBVUAVDescriptorSize);
		D3D12_CONSTANT_BUFFER_VIEW_DESC desc{};
		desc.BufferLocation = matCbAddr;
		desc.SizeInBytes = m_PerMaterialConstantBuffer->GetElementSize();
		m_RenderDevice->CreateConstantBufferView(&desc, handle);
	}

	// 创建渲染Pass层级常量缓冲区资源
	m_PerPassConstantBuffer = std::make_unique<UploadBuffer<PerPassConstants>>(m_RenderDevice.Get(), 1u, true);
	D3D12_GPU_VIRTUAL_ADDRESS passCBAddr = m_PerPassConstantBuffer->GetResource()->GetGPUVirtualAddress();
	// 获取空闲的handle，注意偏移，前objectCount个已经被ObjectConstantBuffer占用
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_CBVHeap->GetCPUDescriptorHandleForHeapStart());
	handle.Offset(objectCount + materialCount, m_CBVUAVDescriptorSize);
	// 绑定常量缓冲区资源和描述符堆，创建常量缓冲区描述符
	D3D12_CONSTANT_BUFFER_VIEW_DESC desc{};
	desc.BufferLocation = passCBAddr;
	desc.SizeInBytes = m_PerPassConstantBuffer->GetElementSize();
	m_RenderDevice->CreateConstantBufferView(&desc, handle);
}

/// <summary>
/// 编译Shader文件
/// </summary>
void DXSampleForReview::CompileShaderFile()
{
	m_VSByteCode = CompileShader(m_AssetPath + _T("Source\\DXReviewEntry.hlsl"), nullptr, "VSMain", "vs_5_0");
	m_PSByteCode = CompileShader(m_AssetPath + _T("Source\\DXReviewEntry.hlsl"), nullptr, "PSMain", "ps_5_0");
}

/// <summary>
/// 创建根签名
/// </summary>
void DXSampleForReview::CreateRootSignature()
{
	// 使用了2个常量缓冲区，
	CD3DX12_ROOT_PARAMETER slotRootParameter[3]{};
	// 通过描述符表的方式场景根参数
	CD3DX12_DESCRIPTOR_RANGE objCbvTable{};				// 作为物体常量缓冲区
	objCbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	CD3DX12_DESCRIPTOR_RANGE matCbvTable{};				// 作为材质常量缓冲区
	matCbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
	CD3DX12_DESCRIPTOR_RANGE passCbvTable{};			// 作为渲染过程常量缓冲区
	passCbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);
	slotRootParameter[0].InitAsDescriptorTable(1, &objCbvTable);
	slotRootParameter[1].InitAsDescriptorTable(1, &matCbvTable);
	slotRootParameter[2].InitAsDescriptorTable(1, &passCbvTable);
	// 根参数序列化
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(3, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	Microsoft::WRL::ComPtr<ID3DBlob> pSerializedRootSig{ nullptr };
	Microsoft::WRL::ComPtr<ID3DBlob> pErrorBlob{ nullptr };
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, pSerializedRootSig.GetAddressOf(), pErrorBlob.GetAddressOf());
	if (pErrorBlob != nullptr)
	{
		::OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);
	// 创建根签名
	ThrowIfFailed(m_RenderDevice->CreateRootSignature(0U, pSerializedRootSig->GetBufferPointer(), pSerializedRootSig->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf())));
}

/// <summary>
/// 创建渲染管线状态对象
/// </summary>
void DXSampleForReview::BuildPSO()
{
	m_InputLayoutDescs =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 3 * sizeof(float), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, (3 + 4) * sizeof(float), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, (3 + 4 + 3) * sizeof(float), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	// int value =  sizeof(m_InputLayoutDescs);
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
	ZeroMemory(&pipelineStateDesc, sizeof(pipelineStateDesc));
	pipelineStateDesc.NodeMask = 0U;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.InputLayout = { m_InputLayoutDescs.data(), (UINT)m_InputLayoutDescs.size() };
	pipelineStateDesc.pRootSignature = m_RootSignature.Get();
	pipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_VSByteCode->GetBufferPointer()), m_VSByteCode->GetBufferSize() };
	pipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_PSByteCode->GetBufferPointer()), m_PSByteCode->GetBufferSize() };
	pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipelineStateDesc.NumRenderTargets = 1u;
	pipelineStateDesc.RTVFormats[0] = m_BackBufferFormat;
	pipelineStateDesc.DSVFormat = m_DepthStencilFormat;
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.SampleDesc.Count = m_IsEnable4XMSAA ? 4 : 1;
	pipelineStateDesc.SampleDesc.Quality = m_IsEnable4XMSAA ? m_4XMSAAQualityLevel - 1 : 0;
	ThrowIfFailed(m_RenderDevice->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(m_PSO.GetAddressOf())));
}

void DXSampleForReview::UpdateObjCBs(float fDeltaTime, float fTotalTime)
{
	for (auto& pRenderItem : m_AllRenderItems)
	{
		if (pRenderItem->m_NumFrameDirty > 0)
		{
			// 获取最新的世界变换矩阵
			DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&pRenderItem->m_LoclToWorldMatrix);
			// 变换矩阵写入到常量缓冲区内存中。注意这里进行了一次矩阵转置
			PerObjectConstants objCB{};
			DirectX::XMStoreFloat4x4(&objCB.m_LocalToWorldMatrix, DirectX::XMMatrixTranspose(worldMatrix));
			m_PerOjectConstantBuffer->CopyData(pRenderItem->m_ObjectCBIndex, objCB);
			// 数据更新成功，脏标记-1
			--pRenderItem->m_NumFrameDirty;
		}
	}
}

void DXSampleForReview::UpdateMatCBs(float fDeltaTime, float fTotalTime)
{
	for (auto& pMaterial : m_AllMaterials)
	{
		if (pMaterial.second->m_NumFrameDirty > 0)
		{
			PerMaterialConstants matCB{};
			matCB.m_DiffuseAlbedo = pMaterial.second->m_DiffuseAlbedo;
			matCB.m_FresnelR0 = pMaterial.second->m_FresnelR0;
			matCB.m_Roughness = pMaterial.second->m_Roughness;
			m_PerMaterialConstantBuffer->CopyData(pMaterial.second->m_CbvIndex, matCB);
			--pMaterial.second->m_NumFrameDirty;
		}
	}
}

void DXSampleForReview::UpdatePassCB(float fDeltaTime, float fTotalTime)
{
	DirectX::XMMATRIX viewMatrix = DirectX::XMLoadFloat4x4(&m_ViewMatrix);
	DirectX::XMMATRIX projMatrix = DirectX::XMLoadFloat4x4(&m_ProjMatrix);
	DirectX::XMMATRIX viewProjMatrix = DirectX::XMMatrixMultiply(viewMatrix, projMatrix);
	DirectX::XMMATRIX invViewMatrix = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(viewMatrix), viewMatrix);
	DirectX::XMMATRIX invProjMatrix = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(projMatrix), projMatrix);
	DirectX::XMMATRIX invViewProjMatrix = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(viewProjMatrix), viewProjMatrix);
	PerPassConstants passCBData{};
	DirectX::XMStoreFloat4x4(&passCBData.m_ViewMatrix, DirectX::XMMatrixTranspose(viewMatrix));
	DirectX::XMStoreFloat4x4(&passCBData.m_InvViewMatrix, DirectX::XMMatrixTranspose(invViewMatrix));
	DirectX::XMStoreFloat4x4(&passCBData.m_ProjMatrix, DirectX::XMMatrixTranspose(projMatrix));
	DirectX::XMStoreFloat4x4(&passCBData.m_InvProjMatrix, DirectX::XMMatrixTranspose(invProjMatrix));
	DirectX::XMStoreFloat4x4(&passCBData.m_ViewProjMatrix, DirectX::XMMatrixTranspose(viewProjMatrix));
	DirectX::XMStoreFloat4x4(&passCBData.m_InvViewProjMatrix, DirectX::XMMatrixTranspose(invViewProjMatrix));
	passCBData.m_EyePos = m_EyePos;
	passCBData.m_NearPlane = 1.f;
	passCBData.m_FarPlane = 1000.f;
	passCBData.m_RenderTargetSize = DirectX::XMFLOAT2(static_cast<float>(m_Width), static_cast<float>(m_Height));
	passCBData.m_InvRenderTargetSize = DirectX::XMFLOAT2(1.f / static_cast<float>(m_Width), 1.f / static_cast<float>(m_Height));
	passCBData.m_DeltaTime = fDeltaTime;
	passCBData.m_TotalTime = fTotalTime;

	passCBData.m_AmbientLight = m_AmbientLight;
	passCBData.m_AllLights[0].m_Strength = m_DirectLight;
	DirectX::XMFLOAT3 lightPos{};
	lightPos.x = 1.f * sinf(m_SunTheta) * cos(m_SunPhi);
	lightPos.y = 1.f * cosf(m_SunTheta);
	lightPos.z = 1.f * sinf(m_SunTheta) * sinf(m_SunPhi);
	DirectX::XMVECTOR lightDir = -DirectX::XMVectorSet(lightPos.x, lightPos.y, lightPos.z, 1.f);
	DirectX::XMStoreFloat3(&passCBData.m_AllLights[0].m_Direction, lightDir);

	// 拷贝到渲染Pass常量缓冲区上
	m_PerPassConstantBuffer->CopyData(0, passCBData);
}

void DXSampleForReview::UpdateCamera()
{
	// 观察者位置球坐标转换为笛卡尔坐标
	m_EyePos.x = m_Radius * sinf(m_Theta) * cosf(m_Phi);
	m_EyePos.y = m_Radius * cosf(m_Theta);
	m_EyePos.z = m_Radius * sinf(m_Theta) * sinf(m_Phi);
	// 根据观察者位置，视点位置，向上方向计算观察矩阵
	DirectX::XMVECTOR cameraPos = DirectX::XMVectorSet(m_EyePos.x, m_EyePos.y, m_EyePos.z, 1.f);
	DirectX::XMVECTOR lookPos = DirectX::XMVectorZero();
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(cameraPos, lookPos, up);
	DirectX::XMStoreFloat4x4(&m_ViewMatrix, viewMatrix);
}

/// <summary>
/// 刷新命令队列，等待执行完毕
/// </summary>
void DXSampleForReview::FlushCommandQueue()
{
	// 递增围栏值，通知GPU设置围栏值
	++m_FenceValue;
	// 给命令队列尾部添加一条指令，设置围栏值
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), m_FenceValue));
	if (m_Fence->GetCompletedValue() < m_FenceValue)
	{
		// 设置GPU抵达当前目标围栏值，激发时间，通知CPU结束等待，继续执行
		ThrowIfFailed(m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent));
		// CPU等待GPU完成围栏标记，
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
}

/// <summary>
/// 记录渲染指令
/// </summary>
void DXSampleForReview::PopulateCommandList()
{
	ThrowIfFailed(m_CommandAllocator->Reset());
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), m_PSO.Get()));
	// 当前的交换链缓冲区切换为渲染目标
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_RenderTargetBuffers[m_CurrentFrameBufferIndex].Get()
		, D3D12_RESOURCE_STATE_PRESENT
		, D3D12_RESOURCE_STATE_RENDER_TARGET
	));
	// 获取当前使用的交换链缓冲区
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentFrameBufferIndex, m_RTVDescriptorSize);
	// 设置视口和裁剪
	m_CommandList->RSSetViewports(1, &m_Viewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);
	// 清理渲染模板缓冲区和深度模板缓冲区
	m_CommandList->ClearRenderTargetView(rtvHandle, DirectX::Colors::Gray, 0, nullptr);
	m_CommandList->ClearDepthStencilView(m_DsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);
	// 设置当前的渲染目标
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, true, &m_DsvHandle);
	ID3D12DescriptorHeap* descriptorHeaps = { m_CBVHeap.Get() };
	m_CommandList->SetDescriptorHeaps(1, &descriptorHeaps);
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	uint32_t passCBVIndex = static_cast<uint32_t>(m_AllRenderItems.size()) + static_cast<uint32_t>(m_AllMaterials.size());
	CD3DX12_GPU_DESCRIPTOR_HANDLE passCBVHandle(m_CBVHeap->GetGPUDescriptorHandleForHeapStart());
	passCBVHandle.Offset(passCBVIndex, m_CBVUAVDescriptorSize);
	m_CommandList->SetGraphicsRootDescriptorTable(2, passCBVHandle);

	DrawRenderItem();

	// 当前的交换链缓冲区切换为显示状态
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		m_RenderTargetBuffers[m_CurrentFrameBufferIndex].Get()
		, D3D12_RESOURCE_STATE_RENDER_TARGET
		, D3D12_RESOURCE_STATE_PRESENT
	));
	ThrowIfFailed(m_CommandList->Close());
}

/// <summary>
/// 绘制渲染项
/// </summary>
void DXSampleForReview::DrawRenderItem()
{
	int objCount = static_cast<int>(m_AllRenderItems.size());
	for (size_t i = 0; i < m_AllRenderItems.size(); ++i)
	{
		DemoRenderItem* pRenderItem = m_AllRenderItems[i].get();
		m_CommandList->IASetVertexBuffers(0, 1, &pRenderItem->m_MeshGeo->GetVertexBufferView());
		m_CommandList->IASetIndexBuffer(&pRenderItem->m_MeshGeo->GetIndexBufferView());
		m_CommandList->IASetPrimitiveTopology(pRenderItem->m_PrimitiveType);
		// 绑定渲染项关联的常量缓冲区
		CD3DX12_GPU_DESCRIPTOR_HANDLE objCBHandle(m_CBVHeap->GetGPUDescriptorHandleForHeapStart());
		objCBHandle.Offset(pRenderItem->m_ObjectCBIndex * m_CBVUAVDescriptorSize);
		m_CommandList->SetGraphicsRootDescriptorTable(0, objCBHandle);
		// 绑定关联材质的常量缓冲区
		CD3DX12_GPU_DESCRIPTOR_HANDLE matCBHandle(m_CBVHeap->GetGPUDescriptorHandleForHeapStart());
		matCBHandle.Offset(objCount + pRenderItem->m_Material->m_CbvIndex, m_CBVUAVDescriptorSize);
		m_CommandList->SetGraphicsRootDescriptorTable(1, matCBHandle);
		m_CommandList->DrawIndexedInstanced(pRenderItem->m_IndexCount, 1, pRenderItem->m_StartIndexLocation, pRenderItem->m_StartVertexLocation, 0);
	}
}



