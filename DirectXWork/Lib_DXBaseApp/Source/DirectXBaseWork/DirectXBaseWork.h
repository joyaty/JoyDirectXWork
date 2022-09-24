
/**
 * DirectXBaseWork.h
 * ��ʼ��Direct3D
 */

#pragma once

class DirectXBaseWork
{
public:
	DirectXBaseWork(std::wstring title, UINT width = 1280U, UINT height = 720U);
	virtual ~DirectXBaseWork();

public:
	/// <summary>
	/// ��ȡӲ����ʾ������
	/// </summary>
	/// <param name="pDXGIFactory">DXGIFactory�ӿڶ�����Ҫʹ�øö���ö����ʾ������</param>
	/// <param name="ppTargetDXGIAdapter">����ָ����ʾ������ָ���ָ��</param>
	/// <param name="requestHightPerformanceAdapter">�Ƿ���Ҫ��������ʾ��������Ĭ��false</param>
	static void GetHardwardAdapter(IDXGIFactory1* pDXGIFactory, IDXGIAdapter1** ppTargetDXGIAdapter, bool requestHightPerformanceAdapter = false);

public:
	const wchar_t* GetTitle() const { return m_Title.c_str(); }
	UINT GetWidth() const { return m_Width; }
	UINT GetHeight() const { return m_Height; }

public:
	void Initialize(HWND hWnd);

protected:
	virtual void OnInit() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnDestroy() = 0;

protected:
	/// <summary>
	/// ������������
	/// </summary>
	virtual void CreateDescriptorHeaps();

private:
	/// <summary>
	/// ����D3D12�豸
	/// </summary>
	void CreateD3D12Device();

	/// <summary>
	/// ����������к������б�
	/// </summary>
	void CreateCommandObjects();

	/// <summary>
	/// ����������
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// ���������в���
	/// </summary>
	/// <param name="argv">������ָ��</param>
	/// <param name="argc">������ָ������</param>
	void ParseCommandLineArgs(WCHAR* argv[], int argc);

protected:
	/// <summary>
	/// IDXGIFactory�ӿڶ�����Ҫʹ�øö���ö����ʾ��������������������
	/// DX12Ӧ��ʹ��IDXGIFactory4����
	/// </summary>
	Microsoft::WRL::ComPtr<IDXGIFactory4> m_DXGIFactory{};
	/// <summary>
	/// D3D12�豸
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Device> m_Device{};

	/// <summary>
	/// D3D12������У�GPU��ȡ��������е���Ⱦ���������Ⱦ
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue{};
	/// <summary>
	/// D3D12�����б�CPUͨ�������б���Ⱦ�����ύ���������
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList{};
	/// <summary>
	/// D3D12����������������б��¼�����ʵ�ʼ�¼�ڹ���������������ϣ�������л���������������е�����
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator{};

	/// <summary>
	/// ����������
	/// </summary>
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_SwapChain{};

	/// <summary>
	/// RenderTargetView�������Ѷ���
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVDescriptorHeap{};
	/// <summary>
	/// Depth/Stencil�������Ѷ���
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVDescriptorHeap{};

	/// <summary>
	/// RenderTargetView��������С
	/// </summary>
	UINT m_RTVDescriptorSize = 0U;
	/// <summary>
	/// Depth/Stencil��������С
	/// </summary>
	UINT m_DSVDescriptorSize = 0U;
	/// <summary>
	/// ConstantBufferView/ShaderResourceView/UnorderedAccessView��������С
	/// </summary>
	UINT m_CBVUAVDescriptorSize = 0U;

	/// <summary>
	/// ͼ�������ȼ�������ĳ�������ʽ�Ͳ��������������˵���������������Ч��ΧΪ[0, NumQualityLevels - 1]
	/// </summary>
	UINT m_4XMSAAQualityLevel = 0;

	/// <summary>
	/// ��ǰ�ĺ�̨��������������Ϊ��ȾĿ��ʱ��Ҫ֪������д�뵽�Ǹ���������
	/// </summary>
	UINT m_CurrentBackBufferIndex;

private:
	/// <summary>
	/// ���ھ��
	/// </summary>
	HWND m_Hwnd{};

	/// <summary>
	/// DirectXʾ�����ƣ���ʾ�ڴ��ڱ�����
	/// </summary>
	std::wstring m_Title;

	/// <summary>
	/// �ӿڿ�ȣ�Ĭ��1280
	/// </summary>
	UINT m_Width = 1280U;

	/// <summary>
	/// �ӿڸ߶ȣ�Ĭ��720
	/// </summary>
	UINT m_Height = 720U;

	/// <summary>
	/// �Ƿ�ʹ��WARP���������
	/// </summary>
	bool m_UseWarpDevice = false;

	/// <summary>
	/// �Ƿ�ʹ��4XMSAA
	/// </summary>
	bool m_Use4XMSAA = false;

	/// <summary>
	/// ��̨��������ʽ
	/// </summary>
	DXGI_FORMAT m_BackbufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

private:
	/// <summary>
	/// ˫���壬ָ������������������Ϊ2
	/// </summary>
	static const UINT kFrameBufferCount = 2;
};

