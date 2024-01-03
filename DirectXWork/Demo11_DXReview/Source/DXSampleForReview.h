
#pragma once

#include "UploadBuffer.h"
#include "DemoRenderItem.h"
#include "GeometryGenerator.h"
#include "Utils.h"

/// <summary>
/// DX12复习范例
/// </summary>
class DXSampleForReview
{
public:
	DXSampleForReview(const std::wstring& strTitle, uint32_t nWidth = 1280U, uint32_t nHeight = 720U);

public:
	bool Initialize(HWND hWndInstance);
	void Terminate();
	void Update(float fDeltaTime, float fTotalTime);
	void Render();

	/// <summary>
	/// 窗口宽高发生变化
	/// </summary>
	/// <param name="nWidth">新的窗口宽度</param>
	/// <param name="nHeight">新的窗口高度</param>
	void OnResize(uint32_t nWidth, uint32_t nHeight);

	/// <summary>
	/// 键盘按键按下
	/// </summary>
	/// <param name="keyCode">键盘Key</param>
	void OnKeyDown(UINT8 keyCode);

	/// <summary>
	/// 键盘按键抬起
	/// </summary>
	/// <param name="keyCode">键盘Key</param>
	void OnKeyUp(UINT8 keyCode);

	/// <summary>
	/// 鼠标按键按下
	/// </summary>
	/// <param name="keyCode">鼠标Key</param>
	/// <param name="x">鼠标位置X</param>
	/// <param name="y">鼠标位置Y</param>
	void OnMouseDown(UINT8 keyCode, int x, int y);

	/// <summary>
	/// 鼠标按键抬起
	/// </summary>
	/// <param name="keyCode">鼠标Key</param>
	/// <param name="x">鼠标位置X</param>
	/// <param name="y">鼠标位置Y</param>
	void OnMouseUp(UINT8 keyCode, int x, int y);

	/// <summary>
	/// 鼠标移动
	/// </summary>
	/// <param name="keyCode">鼠标Key</param>
	/// <param name="x">鼠标位置X</param>
	/// <param name="y">鼠标位置Y</param>
	void OnMouseMove(UINT8 keyCode, int x, int y);

public:
	const wchar_t* GetTitle() const { return m_Title.c_str(); }
	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

private:
	/// <summary>
	/// 创建逻辑渲染设备
	/// </summary>
	void CreateRenderDevice();

	/// <summary>
	/// 创建命令队列和命令列表
	/// </summary>
	void CreateCommandObjects();

	/// <summary>
	/// 创建描述符堆
	/// </summary>
	void CreateDescriptorHeaps();

	/// <summary>
	/// 创建常量缓冲区描述符堆
	/// </summary>
	void CreateCBDescriptorHeaps();

	/// <summary>
	/// 构建交换链
	/// </summary>
	void CreateSwapchain();

	/// <summary>
	/// 构建渲染目标缓冲区
	/// </summary>
	void BuildRTV();

	/// <summary>
	/// 构建深度模板缓冲区
	/// </summary>
	void BuildDSV();

	/// <summary>
	/// 创建场景几何体物件
	/// </summary>
	void CreateSceneObjects();

	/// <summary>
	/// 加载纹理，构建着色器资源
	/// </summary>
	void BuildShaderResource();

	/// <summary>
	/// 构建采样器
	/// </summary>
	void BuildSampler();

	/// <summary>
	/// 构建静态采样器，实际程序中使用的采样器种类不是很多，DX12提供了静态采样器，避免创建采样器堆、绑定采样器等一系列复杂的操作
	/// </summary>
	void BuildStaticSamplers();

	/// <summary>
	/// 创建场景渲染使用的材质
	/// </summary>
	void BuildMaterials();

	/// <summary>
	/// 根据场景几何体物件，构建渲染项
	/// </summary>
	void BuildRenderItems();

	/// <summary>
	/// 构建常量缓冲区描述符
	/// </summary>
	void BuildConstantBufferView();

	/// <summary>
	/// 编译Shader文件
	/// </summary>
	void CompileShaderFile();

	/// <summary>
	/// 创建渲染管线状态对象
	/// </summary>
	void BuildPSO();

	/// <summary>
	/// 创建根签名
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// 刷新命令队列，等待执行完毕
	/// </summary>
	void FlushCommandQueue();

	/// <summary>
	/// 记录渲染指令
	/// </summary>
	void PopulateCommandList();

	/// <summary>
	/// 每帧更新脏标记物体常量缓冲区的数据
	/// </summary>
	/// <param name="fDeltaTime"></param>
	/// <param name="fTotalTime"></param>
	void UpdateObjCBs(float fDeltaTime, float fTotalTime);

	/// <summary>
	/// 更新材质常量缓冲区的数据
	/// </summary>
	/// <param name="fDeltaTime"></param>
	/// <param name="fTotalTime"></param>
	void UpdateMatCBs(float fDeltaTime, float fTotalTime);

	/// <summary>
	/// 每帧更新渲染Pass常量缓冲区的数据
	/// </summary>
	/// <param name="fDeltaTime"></param>
	/// <param name="fTotalTime"></param>
	void UpdatePassCB(float fDeltaTime, float fTotalTime);

	/// <summary>
	/// 更新相机位置
	/// </summary>
	void UpdateCamera();

	/// <summary>
	/// 绘制渲染项
	/// </summary>
	/// <param name="vecCurrentRenderItems"></param>
	void DrawRenderItem(const std::vector<DemoRenderItem*>& vecCurrentRenderItems);

private:
	/// <summary>
	/// 窗口标题
	/// </summary>
	std::wstring m_Title;
	/// <summary>
	/// 窗口宽
	/// </summary>
	uint32_t m_Width;
	/// <summary>
	/// 窗口高
	/// </summary>
	uint32_t m_Height;
	/// <summary>
	/// 显示宽高比
	/// </summary>
	float m_AspectRatio;
	/// <summary>
	/// 资源路径
	/// </summary>
	std::wstring m_AssetPath;
	/// <summary>
	/// 项目根目录
	/// </summary>
	std::wstring m_RootAssetPath;

	/// <summary>
	/// 窗口句柄
	/// </summary>
	HWND m_WindowInstance;

	/// <summary>
	/// 是否使用软件光栅渲染器
	/// </summary>
	bool m_IsUseWarpDevice;

	/// <summary>
	/// RenderTargetView大小
	/// </summary>
	UINT m_RTVDescriptorSize;
	/// <summary>
	/// DepthStencilView大小
	/// </summary>
	UINT m_DSVDescriptorSize;
	/// <summary>
	/// ConstantBufferView/ShaderResourceView/UnorderAccessView大小
	/// </summary>
	UINT m_CBVUAVDescriptorSize;
	/// <summary>
	/// 采样器描述符大小
	/// </summary>
	UINT m_SamplerDescriptorSize;
	
	/// <summary>
	/// 是否启用4X多次采样
	/// </summary>
	bool m_IsEnable4XMSAA = false;
	/// <summary>
	/// 图像质量等级，对于某种纹理格式和采样数量的组合来说，其质量级别的有效范围为[0, NumQualityLevels - 1]
	/// </summary>
	UINT m_4XMSAAQualityLevel = 0U;

	/// <summary>
	/// 输出缓冲区格式
	/// </summary>
	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	/// <summary>
	/// 深度模板缓冲区格式
	/// </summary>
	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	/// <summary>
	/// 方向光位矢与Y轴的夹角
	/// </summary>
	float m_SunTheta{ DirectX::XM_PIDIV4 };
	/// <summary>
	/// 方向光位矢在XZ平面的投影和X轴的夹角
	/// </summary>
	float m_SunPhi{ 1.25f * DirectX::XM_PI };
	/// <summary>
	/// 环境光
	/// </summary>
	DirectX::XMFLOAT4 m_AmbientLight{ 0.25f, 0.25f, 0.35f, 1.0f };
	/// <summary>
	/// 直接光
	/// </summary>
	DirectX::XMFLOAT3 m_DirectLight{ 0.8f, 0.7f, 0.7f };

	/// <summary>
	/// 雾颜色
	/// </summary>
	DirectX::XMFLOAT4 m_FogColor{ 0.5f, 0.5f, 0.5f, 1.0f };
	/// <summary>
	/// 雾开始出现距离(参照相机位置)
	/// </summary>
	float m_FogStart{ 50.f };
	/// <summary>
	/// 雾不可见距离(参照相机位置)
	/// </summary>
	float m_FogEnd{ 150.f };

private:
	/// <summary>
	/// 双缓冲，指定两个输出帧缓冲区
	/// </summary>
	static const UINT kFrameBufferCount = 2;

	/// <summary>
	/// IDXGIFactory接口对象，需要使用该对象枚举显示适配器，创建交换链等
	/// DX12应该使用IDXGIFactory4以上
	/// </summary>
	Microsoft::WRL::ComPtr<IDXGIFactory6> m_DXGIFactoryV6{ nullptr };
	/// <summary>
	/// 显示适配器描述
	/// </summary>
	DXGI_ADAPTER_DESC m_AdapterDesc{};

	/// <summary>
	/// 逻辑显示设备
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Device> m_RenderDevice{ nullptr };

	/// <summary>
	/// 围栏
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
	/// <summary>
	/// 围栏激发事件
	/// </summary>
	HANDLE m_FenceEvent;
	/// <summary>
	/// 围栏值，每次递增，用于同步CPU和GPU的工作
	/// </summary>
	UINT m_FenceValue;

	/// <summary>
	/// 命令队列，环形队列，CPU不断提交指令到队列上，GPU不断处理队列上的指令
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue{ nullptr };
	/// <summary>
	/// 图形命令列表，收集一帧的渲染指令，提交到命令队列上
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList{ nullptr };
	/// <summary>
	/// 命令分配器，指令的存储地址
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator{ nullptr };

	/// <summary>
	/// 交换链
	/// </summary>
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_SwapChain{ nullptr };
	/// <summary>
	/// 当前使用的输出缓冲区索引
	/// </summary>
	UINT m_CurrentFrameBufferIndex;

	/// <summary>
	/// 渲染目标描述符堆
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVHeap{ nullptr };
	/// <summary>
	/// 深度模板缓冲区描述符堆
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVHeap{ nullptr };
	/// <summary>
	/// 常量缓冲区描述符堆
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CBVHeap{ nullptr };
	/// <summary>
	/// 着色器资源描述符堆
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SRVHeap{ nullptr };
	/// <summary>
	/// 采样器资源描述符堆
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SamplerDescriptorHeap{ nullptr };

	/// <summary>
	/// 渲染目标资源
	/// </summary>
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kFrameBufferCount> m_RenderTargetBuffers{};
	/// <summary>
	/// 渲染目标关联的描述符
	/// </summary>
	std::array<CD3DX12_CPU_DESCRIPTOR_HANDLE, kFrameBufferCount> m_RtvHandles{};

	/// <summary>
	/// 深度模板缓冲区资源
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer{};
	/// <summary>
	/// 深度模板缓冲区关联的描述符
	/// </summary>
	CD3DX12_CPU_DESCRIPTOR_HANDLE m_DsvHandle;

	/// <summary>
	/// 根签名
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature{ nullptr };

	/// <summary>
	/// 编译后的顶点着色器
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_VSByteCode{ nullptr };
	/// <summary>
	/// 编译后的像素着色器
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_PSByteCode{ nullptr };

	/// <summary>
	/// 渲染管线状态对象
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PSOs[static_cast<int>(EnumRenderLayer::kLayerCount)]{};

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayoutDescs{};

	/// <summary>
	/// 视口大小
	/// </summary>
	D3D12_VIEWPORT m_Viewport;

	/// <summary>
	/// 裁剪区域
	/// </summary>
	D3D12_RECT m_ScissorRect;

	/// <summary>
	/// 场景中所有的几何体物件
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_SceneObjectes{};
	/// <summary>
	/// 场景中所有的材质
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<Material>> m_AllMaterials{};
	/// <summary>
	/// 场景中所有的纹理
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<Texture>> m_AllTextures{};
	/// <summary>
	/// 渲染项集合
	/// </summary>
	std::vector<std::unique_ptr<DemoRenderItem>> m_AllRenderItems{};
	/// <summary>
	/// 按渲染层级归类渲染项
	/// </summary>
	std::vector<DemoRenderItem*> m_RenderItemsByRenderLayer[static_cast<int>(EnumRenderLayer::kLayerCount)];

	/// <summary>
	/// 用于单个物体的常量缓冲区
	/// </summary>
	std::unique_ptr<UploadBuffer<PerObjectConstants>> m_PerOjectConstantBuffer{ nullptr };
	/// <summary>
	/// 用于单个材质的常量缓冲区
	/// </summary>
	std::unique_ptr<UploadBuffer<PerMaterialConstants>> m_PerMaterialConstantBuffer{ nullptr };
	/// <summary>
	/// 用于一个渲染Pass的常量缓冲区
	/// </summary>
	std::unique_ptr<UploadBuffer<PerPassConstants>> m_PerPassConstantBuffer{ nullptr };
	/// <summary>
	/// 静态采样器描述
	/// </summary>
	std::array<CD3DX12_STATIC_SAMPLER_DESC, 6> m_StaticSamplers{};

	/// <summary>
	/// 观察变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_ViewMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 投影变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_ProjMatrix{ MathUtil::Identity4x4() };

	// 观察者球坐标的一些参数，用于计算观察者位置
	float m_Radius{ 30.f };
	float m_Theta{ DirectX::XM_PIDIV4 };
	float m_Phi{ 1.5f * DirectX::XM_PI };
	/// <summary>
	/// 观察者位置
	/// </summary>
	DirectX::XMFLOAT3 m_EyePos{};

	/// <summary>
	/// 上次鼠标所在的位置
	/// </summary>
	POINT m_LastMousePos{};
};
