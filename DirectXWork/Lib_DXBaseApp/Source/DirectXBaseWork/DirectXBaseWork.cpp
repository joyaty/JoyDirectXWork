/**
 * DirectXBaseWork.cpp
 * ��ʼ��DirectX
 */

#include "stdafx.h"
#include "DirectXBaseWork.h"
#include "DXWork/DXWorkHelper.h"

using namespace Microsoft::WRL;

DirectXBaseWork::DirectXBaseWork(std::wstring title, UINT width, UINT height)
	: m_Title(title)
	, m_Width(width)
	, m_Height(height)
	, m_UseWarpDevice(false)
{
}

DirectXBaseWork::~DirectXBaseWork()
{
}

// _Use_decl_annotations_
void DirectXBaseWork::GetHardwardAdapter(IDXGIFactory1* pDXGIFactory, IDXGIAdapter1** ppTargetDXGIAdapter, bool requestHightPerformanceAdapter/* = false*/)
{
	*ppTargetDXGIAdapter = nullptr;
	ComPtr<IDXGIFactory6> pDXGIFactory6{};
	ComPtr<IDXGIAdapter1> pCurrentAdapter{};
	// ֧��IDXGIFactory6��ʹ��ָ��GPUƫ�õ�ö�ٽӿ�
	if (SUCCEEDED(pDXGIFactory->QueryInterface(IID_PPV_ARGS(pDXGIFactory6.GetAddressOf()))))
	{
		for (UINT nAdapterIndex = 0;
			SUCCEEDED(pDXGIFactory6->EnumAdapterByGpuPreference(nAdapterIndex
				, requestHightPerformanceAdapter ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED // �Ƿ���Ҫָ��������GPU���
				, IID_PPV_ARGS(pCurrentAdapter.GetAddressOf())));
			++nAdapterIndex)
		{
			// ��ȡ��ǰ��ʾ����������
			DXGI_ADAPTER_DESC1 desc;
			pCurrentAdapter->GetDesc1(&desc);
			// ��ѡ�������ʾ��������Ҫʹ�ã���Ҫ��������ʹ��"/warp"
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}
			// ����(���Է����豸��ֵ)����D3D12�豸���ɹ�ֱ�ӷ���
			if (SUCCEEDED(D3D12CreateDevice(pCurrentAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}
	if (pCurrentAdapter.Get() == nullptr)
	{
		for (UINT nAdapterIndex = 0;
			SUCCEEDED(pDXGIFactory->EnumAdapters1(nAdapterIndex, pCurrentAdapter.GetAddressOf()));
			++nAdapterIndex)
		{
			// ��ȡ��ǰ��ʾ����������
			DXGI_ADAPTER_DESC1 desc;
			pCurrentAdapter->GetDesc1(&desc);
			// ��ѡ�������ʾ��������Ҫʹ�ã���Ҫ��������ʹ��"/warp"
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}
			// ����(���Է����豸��ֵ)����D3D12�豸���ɹ�ֱ�ӷ���
			if (SUCCEEDED(D3D12CreateDevice(pCurrentAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}
	// ��ǰѡȡ����ʾ��������ΪĿ����ʾ������
	if (pCurrentAdapter.Get() != nullptr)
	{
		*ppTargetDXGIAdapter = pCurrentAdapter.Detach();
	}
}

void DirectXBaseWork::Initialize(HWND hWnd)
{
	m_Hwnd = hWnd;
	CreateD3D12Device();
	CreateCommandObjects();
	CreateSwapChain();
}

void DirectXBaseWork::CreateD3D12Device()
{
	UINT createDXGIFactoryFlags = 0;
#if defined(_DEBUG) || defined(DEBUG)
	// ����D3D12���Բ�
	ComPtr<ID3D12Debug> pDebugController{};
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(pDebugController.GetAddressOf()))))
	{
		pDebugController->EnableDebugLayer();
		createDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif // defined(_DEBUG) || defined(DEBUG)

	// ����IDXGIFactory
	ThrowIfFailed(CreateDXGIFactory2(createDXGIFactoryFlags, IID_PPV_ARGS(m_DXGIFactory.GetAddressOf())));
	// ��ȡ��ʾ��������ʹ����ʾ����������D3D12�豸
	if (m_UseWarpDevice)
	{
		// ʹ��WARP�豸��WARP = Windows Advanced Rasterization Platform��Windows�߼���դ��ƽ̨�����ɽ��������ǲ��������κ�Ӳ��ͼ���������Ĵ������Ⱦ��
		// GPUӲ�����ܹ���ʱ������ʹ��WARP
		ComPtr<IDXGIAdapter> pDXGIAdapter{};
		ThrowIfFailed(m_DXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(pDXGIAdapter.GetAddressOf())));
		D3D12CreateDevice(pDXGIAdapter.Get()				// ָ��ʹ�õ���ʾ��������nullptr��ΪĬ����ʾ������
			, D3D_FEATURE_LEVEL_11_0						// ���ܼ���
			, IID_PPV_ARGS(m_Device.GetAddressOf()));		// COMID
	}
	else
	{
		// ʹ��Ӳ���豸
		ComPtr<IDXGIAdapter1> pDXGIAdapter{};
		GetHardwardAdapter(m_DXGIFactory.Get(), pDXGIAdapter.GetAddressOf());
		D3D12CreateDevice(pDXGIAdapter.Get()							// ָ��ʹ�õ���ʾ��������nullptr��ΪĬ����ʾ������
			, D3D_FEATURE_LEVEL_11_0						// ���ܼ���
			, IID_PPV_ARGS(m_Device.GetAddressOf()));		// COMID
	}

	if (m_Device.Get() != nullptr)
	{
		// ����Ƿ�֧��4XMSAA����ȡ4XMSAA�µ�����Ʒ��
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels{};
		msQualityLevels.SampleCount = 4;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		msQualityLevels.Format = m_BackbufferFormat; // ��Ҫ���̨��������ʽһ��
		ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)));
		m_4XMSAAQualityLevel = msQualityLevels.NumQualityLevels;

		// ��ȡRenderTargetView��������С
		m_RTVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		// ��ȡDepth/Stencil��������С
		m_DSVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		// ��ȡConstantBufferView/ShaderResourceView/UnorderAccessView��������С
		m_CBVUAVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
}

void DirectXBaseWork::CreateCommandObjects()
{
	// �����������
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;	// GPU��ֱ��ִ�е������б�����
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_CommandQueue.GetAddressOf())));
	// �������������
	ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAllocator.GetAddressOf())));
	// ���������б�
	ThrowIfFailed(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(m_CommandList.GetAddressOf())));
	// �����б�����Ĭ�ϴ��ڼ�¼״̬����Ҫ�ر�
	ThrowIfFailed(m_CommandList->Close());
}

void DirectXBaseWork::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 desc{};
	desc.BufferCount = kFrameBufferCount; // ����������
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// ��Ϊ��ȾĿ�����
	desc.Width = m_Width;				// ���������������
	desc.Height = m_Height;				// �������������߶�
	desc.Format = m_BackbufferFormat;	// ��������������ʽ
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.SampleDesc.Count = m_Use4XMSAA ? 4 : 1;			// ���ز�������
	desc.SampleDesc.Quality = m_Use4XMSAA ? m_4XMSAAQualityLevel : 0;	// ���ز�����������
	// ʹ����������������
	ComPtr<IDXGISwapChain1> pSwapChain{};
	ThrowIfFailed(m_DXGIFactory->CreateSwapChainForHwnd(
		m_CommandQueue.Get(),	// ��������Ҫͨ���������ˢ�£�DX11�Լ�֮ǰ���˲���ΪID3D12Deviceָ�룬DX12���ΪID3D12CommandQueueָ�룬����Ϊ��
		m_Hwnd,					// ���ھ��������Ϊ��
		&desc,					// ����������������Ϊ��
		nullptr,				// ȫ����������������ѡ
		nullptr,				// ָ���������ʾ�豸����ѡ
		pSwapChain.GetAddressOf()
	));
	// ����Ҫȫ��ģʽ����ֹALT+ENTER��ϼ�����ȫ��
	ThrowIfFailed(m_DXGIFactory->MakeWindowAssociation(m_Hwnd, DXGI_MWA_NO_ALT_ENTER));
	// ���潻���������Ҽ�¼��ǰ�ĺ�̨����������
	ThrowIfFailed(pSwapChain.As(&m_SwapChain));
	m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
}

void DirectXBaseWork::CreateDescriptorHeaps()
{
	// ����RTV��������
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorDesc{};
	rtvDescriptorDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorDesc.NumDescriptors = kFrameBufferCount;		// ���������������ȾĿ����ͼ������
	rtvDescriptorDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDescriptorDesc.NodeMask = 0U;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&rtvDescriptorDesc, IID_PPV_ARGS(m_RTVDescriptorHeap.GetAddressOf())));
	// ����DSV��������
	D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorDesc{};
	dsvDescriptorDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescriptorDesc.NumDescriptors = 1;				// ֻ��Ҫһ�����/ģ�建����
	dsvDescriptorDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDescriptorDesc.NodeMask = 0U;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(&dsvDescriptorDesc, IID_PPV_ARGS(m_DSVDescriptorHeap.GetAddressOf())));
}

// _Use_decl_annotations_
void DirectXBaseWork::ParseCommandLineArgs(WCHAR* argv[], int argc)
{
	for (int i = 1; i < argc; ++i)
	{
		if (_wcsnicmp(argv[i], L"-warp", wcslen(argv[i])) == 0
			|| _wcsnicmp(argv[i], L"/warp", wcslen(argv[i])) == 0)
		{
			m_UseWarpDevice = true;
			m_Title += L" (WARP)";
		}
	}
}


