
/**
 * DirectXBaseWork.h
 * 初始化Direct3D
 */

#pragma once

class DirectXBaseWork
{
public:
	DirectXBaseWork(std::wstring title, UINT width = 1280U, UINT height = 720U);
	virtual ~DirectXBaseWork();

public:
	/// <summary>
	/// 获取硬件显示适配器
	/// </summary>
	/// <param name="pDXGIFactory">DXGIFactory接口对象，需要使用该对象枚举显示适配器</param>
	/// <param name="ppTargetDXGIAdapter">返回指向显示适配器指针的指针</param>
	/// <param name="requestHightPerformanceAdapter">是否需要高性能显示适配器，默认false</param>
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
	/// 创建描述符堆
	/// </summary>
	virtual void CreateDescriptorHeaps();

private:
	/// <summary>
	/// 创建D3D12设备
	/// </summary>
	void CreateD3D12Device();

	/// <summary>
	/// 创建命令队列和命令列表
	/// </summary>
	void CreateCommandObjects();

	/// <summary>
	/// 创建交换链
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// 解析命令行参数
	/// </summary>
	/// <param name="argv">命令行指令</param>
	/// <param name="argc">命令行指令数量</param>
	void ParseCommandLineArgs(WCHAR* argv[], int argc);

protected:
	/// <summary>
	/// IDXGIFactory接口对象，需要使用该对象枚举显示适配器，创建交换链等
	/// DX12应该使用IDXGIFactory4以上
	/// </summary>
	Microsoft::WRL::ComPtr<IDXGIFactory4> m_DXGIFactory{};
	/// <summary>
	/// D3D12设备
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Device> m_Device{};

	/// <summary>
	/// D3D12命令队列，GPU读取命令队列中的渲染命令进行渲染
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue{};
	/// <summary>
	/// D3D12命令列表，CPU通过命令列表将渲染命令提交到命令队列
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList{};
	/// <summary>
	/// D3D12命令分配器，命令列表记录命令会实际记录在关联的命令分配器上，命令队列会引用命令分配器中的命令
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator{};

	/// <summary>
	/// 交换链对象
	/// </summary>
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_SwapChain{};

	/// <summary>
	/// RenderTargetView描述符堆对象
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVDescriptorHeap{};
	/// <summary>
	/// Depth/Stencil描述符堆对象
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVDescriptorHeap{};

	/// <summary>
	/// RenderTargetView描述符大小
	/// </summary>
	UINT m_RTVDescriptorSize = 0U;
	/// <summary>
	/// Depth/Stencil描述符大小
	/// </summary>
	UINT m_DSVDescriptorSize = 0U;
	/// <summary>
	/// ConstantBufferView/ShaderResourceView/UnorderedAccessView描述符大小
	/// </summary>
	UINT m_CBVUAVDescriptorSize = 0U;

	/// <summary>
	/// 图像质量等级，对于某种纹理格式和采样数量的组合来说，其质量级别的有效范围为[0, NumQualityLevels - 1]
	/// </summary>
	UINT m_4XMSAAQualityLevel = 0;

	/// <summary>
	/// 当前的后台缓冲区索引，作为渲染目标时需要知道数据写入到那个缓冲区上
	/// </summary>
	UINT m_CurrentBackBufferIndex;

private:
	/// <summary>
	/// 窗口句柄
	/// </summary>
	HWND m_Hwnd{};

	/// <summary>
	/// DirectX示例名称，显示在窗口标题上
	/// </summary>
	std::wstring m_Title;

	/// <summary>
	/// 视口宽度，默认1280
	/// </summary>
	UINT m_Width = 1280U;

	/// <summary>
	/// 视口高度，默认720
	/// </summary>
	UINT m_Height = 720U;

	/// <summary>
	/// 是否使用WARP软件适配器
	/// </summary>
	bool m_UseWarpDevice = false;

	/// <summary>
	/// 是否使用4XMSAA
	/// </summary>
	bool m_Use4XMSAA = false;

	/// <summary>
	/// 后台缓冲区格式
	/// </summary>
	DXGI_FORMAT m_BackbufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

private:
	/// <summary>
	/// 双缓冲，指定交换链缓冲区数量为2
	/// </summary>
	static const UINT kFrameBufferCount = 2;
};

