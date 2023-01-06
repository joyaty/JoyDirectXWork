/**
 * DXHelloLighting.h
 * 光照模型实例程序头文件
 */

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"
#include <DirectXBaseWork/MathUtil.h>

// 前置声明 - 几何体Mesh数据结构
struct MeshGeometry;
// 前置声明 - 材质数据结构
struct HelloLightingMaterial;
// 前置声明 - 渲染项数据结构
struct HelloLightingRenderItem;
// 前置声明 - 帧资源数据结构
struct HelloLightingFrameResource;

/// <summary>
/// Direct3D构建光照模型
/// </summary>
class DXHelloLighting : public DirectXBaseWork
{
public:
	DXHelloLighting(std::wstring title, UINT width = 1280U, UINT height = 720U);
	virtual ~DXHelloLighting();

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
	/// 构建材质
	/// </summary>
	void BuildMaterial();

	/// <summary>
	/// 初始化渲染项
	/// </summary>
	void BuildRenderItem();

	/// <summary>
	/// 构建帧资源
	/// </summary>
	void BuildFrameResource();

	/// <summary>
	/// 初始化顶点输入布局
	/// </summary>
	void InitInputLayout();

	/// <summary>
	/// 编译Shader
	/// </summary>
	void CompileShaderFile();

	/// <summary>
	/// 构建根签名
	/// </summary>
	void BuildRootSignature();

	/// <summary>
	/// 构建渲染管线状态对象
	/// </summary>
	void BuildPSO(D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode);

	/// <summary>
	/// 更新观察矩阵
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdateCamera(float deltaTime, float totalTime);

	/// <summary>
	/// 更新物体常量缓冲区
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdateObjectCB(float deltaTime, float totalTime);

	/// <summary>
	/// 更新材质常量缓冲区
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdateMaterialCB(float deltaTime, float totalTime);

	/// <summary>
	/// 更新渲染过程常量缓冲区
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="totalTime"></param>
	void UpdatePassCB(float deltaTime, float totalTime);

	/// <summary>
	/// 记录渲染指令
	/// </summary>
	void PopulateCommandList();
	/// <summary>
	/// 绘制渲染项
	/// </summary>
	void DrawRenderItem();

private:
	/// <summary>
	/// 输入布局元素描述
	/// </summary>
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputElementDesc{};
	/// <summary>
	/// 编译后的顶点着色器字节码
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_VSByteCode{ nullptr };
	/// <summary>
	/// 编译后的像素着色器字节码
	/// </summary>
	Microsoft::WRL::ComPtr<ID3DBlob> m_PSByteCode{ nullptr };
	/// <summary>
	/// 根签名
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature{ nullptr };
	/// <summary>
	/// 渲染管线状态对象
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PSO{ nullptr };
	/// <summary>
	/// 绘制的几何物体集合
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_GeoMesh{};
	/// <summary>
	/// 绘制的材质集合
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<HelloLightingMaterial>> m_Materials{};
	/// <summary>
	/// 所有的渲染项
	/// </summary>
	std::vector<std::unique_ptr<HelloLightingRenderItem>> m_AllRenderItems{};
	/// <summary>
	/// 帧资源
	/// </summary>
	std::vector<std::unique_ptr<HelloLightingFrameResource>> m_FrameResources{};
	/// <summary>
	/// 当前使用的帧资源索引
	/// </summary>
	int m_CurrentFrameResourceIndex{ 0 };
	/// <summary>
	/// 当前使用的帧资源
	/// </summary>
	HelloLightingFrameResource* m_CurrentFrameResource{ nullptr };

private:
	/// <summary>
	/// 投影矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_ProjMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 观察矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 m_ViewMatrix{ MathUtil::Identity4x4() };

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