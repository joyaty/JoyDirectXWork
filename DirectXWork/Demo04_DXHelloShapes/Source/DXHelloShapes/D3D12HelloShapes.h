/*
 * D3D12HelloShapes.cpp
 * Demo04 - DirectX12渲染多个几何图形
 */

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"

// 前置声明 - 帧资源
struct FrameResource;
// 前置声明 - 绘制的网格数据
struct MeshGeometry;
// 前置声明 - 渲染项
struct RenderItem;

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
	/// 渲染对象宽高
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

protected:
	bool OnInit() override;
	void OnUpdate(float deltaTime, float totalTime) override;
	void OnRender() override;
	void OnDestroy() override;

private:
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

private:
	/// <summary>
	/// 帧资源环形数组，重复使用空闲的帧资源
	/// </summary>
	std::vector<std::unique_ptr<FrameResource>> m_FrameResources{};
	/// <summary>
	/// 当前的帧资源索引
	/// </summary>
	int m_CurrentFrameResourceIndex{ 0 };
	/// <summary>
	/// 当前的帧资源
	/// </summary>
	FrameResource* m_CurrentFrameResource{ nullptr };

	/// <summary>
	/// 需要绘制的Mesh数据集合
	/// </summary>
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_GeoMeshes{};
	/// <summary>
	/// 所有的渲染项
	/// </summary>
	std::vector<std::unique_ptr<RenderItem>> m_AllRenderItems{};


	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CBVDescriptorHeap{ nullptr };
};