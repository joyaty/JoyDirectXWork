/*
 * D3D12HelloShapes.cpp
 * Demo04 - DirectX12渲染多个几何图形
 */

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"
#include "DirectXBaseWork/MathUtil.h"

// 前置声明 - 帧资源
struct HelloShapesFrameResource;
// 前置声明 - 绘制的网格数据
struct MeshGeometry;
// 前置声明 - 渲染项
struct HelloShapesRenderItem;

/// <summary>
/// 物体常量缓冲区结构体
/// 每个绘制物体自己独有一个常量缓冲区
/// </summary>
struct ObjectConstants
{
public:
	/// <summary>
	/// 本地到世界空间的变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 worldMatrix{};
};

/// <summary>
/// 渲染过程常量缓冲区
/// 一个渲染过程，各个物体通用的常量缓冲区数据
/// </summary>
struct PassConstants
{
public:
	/// <summary>
	/// 观察矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 viewMatrix{};
	/// <summary>
	/// 观察矩阵的逆矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 invViewMatrix{};
	/// <summary>
	/// 投影矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 projMatrix{};
	/// <summary>
	/// 投影矩阵的逆矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 invProjMatrix{};
	/// <summary>
	/// 观察投影矩阵 观察矩阵 x 投影矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 viewProjMatrix{};
	/// <summary>
	/// 观察投影矩阵的逆矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 invViewProjMatrix{};
	/// <summary>
	/// 观察者世界空间位置
	/// </summary>
	DirectX::XMFLOAT3 eyeWorldPos{};
	/// <summary>
	/// 渲染目标宽高
	/// </summary>
	DirectX::XMFLOAT2 renderTargetSize{};
	/// <summary>
	/// 渲染对象的宽高倒数
	/// </summary>
	DirectX::XMFLOAT2 invRenderTargetSize{};
	/// <summary>
	/// 近平面
	/// </summary>
	float nearZ;
	/// <summary>
	/// 远平面
	/// </summary>
	float farZ;
	/// <summary>
	/// 总时间
	/// </summary>
	float totalTime{};
	/// <summary>
	/// 帧间隔时间
	/// </summary>
	float deltaTime{};
};

/// <summary>
/// 本范例使用的顶点数据类型
/// </summary>
struct ShapeVertex
{
public:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
};

/// <summary>
/// DX示例 - 绘制多个几何体
/// </summary>
class D3D12HelloShapes : public DirectXBaseWork
{
public:
	D3D12HelloShapes(std::wstring title, UINT width = 1280U, UINT height = 720U);
	virtual ~D3D12HelloShapes();

	/// <summary>
	/// 更新PSO设置
	/// </summary>
	/// <param name="cullMode"></param>
	/// <param name="fillMode"></param>
	void UpdatePSO(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode);

public:
	void OnMouseDown(UINT8 keyCode, int x, int y) override;
	void OnMouseUp(UINT8 keyCode, int x, int y) override;
	void OnMouseMove(UINT8 keyCode, int x, int y) override;
	void OnResize(UINT width, UINT height) override;

protected:
	bool OnInit() override;
	void OnUpdate(float deltaTime, float totalTime) override;
	void OnRender() override;
	void OnDestroy() override;

private:
	/// <summary>
	/// 构建输入布局
	/// </summary>
	void BuildInputLayout();

	/// <summary>
	/// 构建根签名
	/// </summary>
	void BuildRootSignature();

	/// <summary>
	/// 构建渲染管线状态对象
	/// </summary>
	/// <param name="cullMode"></param>
	/// <param name="fillMode"></param>
	void BuildPSO(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode);

	/// <summary>
	/// 编译Shader
	/// </summary>
	void CompileShaderFile();

	/// <summary>
	/// 构建帧资源环形队列
	/// </summary>
	void BuildFrameResource();

	/// <summary>
	/// 构建几何体
	/// </summary>
	void BuildShapeGeometry();

	/// <summary>
	/// 构建渲染项
	/// </summary>
	void BuildRenderItem();

	/// <summary>
	/// 构建常量缓冲区描述符堆
	/// </summary>
	void BuildCBVDescriptorHeaps();

	/// <summary>
	/// 构建常量缓冲区视图
	/// </summary>
	void BuildConstantBufferView();

	/// <summary>
	/// 更新物体的常量缓冲区数据
	/// </summary>
	void UpdateObjectCBs(float deltaTime, float totalTime);

	/// <summary>
	/// 更新渲染过程的常量缓冲区数据
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdatePassCBs(float deltaTime, float totalTime);
	
	/// <summary>
	/// 更新相机(观察者)的变换矩阵
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdateCamera(float deltaTime, float totalTime);

	/// <summary>
	/// 记录渲染指令
	/// </summary>
	void PopulateCommandList();

	/// <summary>
	/// 绘制渲染项
	/// </summary>
	void DrawRenderItems();

private:
	/// <summary>
	/// 帧资源环形数组，重复使用空闲的帧资源
	/// </summary>
	std::vector<std::unique_ptr<HelloShapesFrameResource>> m_FrameResources{};

	/// <summary>
	/// 当前的帧资源索引
	/// </summary>
	int m_CurrentFrameResourceIndex{ 0 };
	/// <summary>
	/// 当前的帧资源
	/// </summary>
	HelloShapesFrameResource* m_CurrentFrameResource{ nullptr };

	/// <summary>
	/// 需要绘制的Mesh数据集合
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_GeoMeshes{};

	/// <summary>
	/// 所有的渲染项
	/// </summary>
	std::vector<std::unique_ptr<HelloShapesRenderItem>> m_AllRenderItems{};

	/// <summary>
	/// 常量缓冲区描述符堆
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CBVDescriptorHeap{ nullptr };

	/// <summary>
	/// 渲染管线状态对象
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PSO{ nullptr };

	/// <summary>
	/// 根签名
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature{ nullptr };

	/// <summary>
	/// 顶点着色器编译后字节码
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_VSByteCode{ nullptr };

	/// <summary>
	/// 像素着色器编译后字节码
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_PSByteCode{ nullptr };

	/// <summary>
	/// 输入布局描述
	/// </summary>
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputElementDescs{};

	/// <summary>
	/// 本地世界变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_WorldMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 观察变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_ViewMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 投影变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_ProjMatrix{ MathUtil::Identity4x4() };

	/// <summary>
	/// 球坐标系半径
	/// </summary>
	float m_Radius{ 10.f };
	/// <summary>
	/// 位矢与Y轴的夹角
	/// </summary>
	float m_Theta{ DirectX::XM_PIDIV4 };
	/// <summary>
	/// 位矢在XZ平面的投影和X轴的夹角
	/// </summary>
	float m_Phi{ 1.5f * DirectX::XM_PI };
	/// <summary>
	/// 观察者(相机)位置信息
	/// </summary>
	DirectX::XMFLOAT3 m_EyesPos{};

	/// <summary>
	/// 上次鼠标位置
	/// </summary>
	POINT m_LastMousePos{};
};