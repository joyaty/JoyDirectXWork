/**
 * LandAndWavesFrameResource.h
 * 陆地和波浪帧资源头文件
 */

#pragma once

#include "DirectXBaseWork/UploadBuffer.h"
#include "DirectXBaseWork/MathUtil.h"

/// <summary>
/// 物体常量缓冲区
/// </summary>
struct ObjectConstants
{
public:
	/// <summary>
	/// 世界变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 worldMatrix{ MathUtil::Identity4x4() };
};

/// <summary>
/// 渲染过程常量缓冲区
/// </summary>
struct PassConstants
{
public:
	/// <summary>
	/// 观察矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 viewMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 观察矩阵的逆矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 invViewMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 投影矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 projMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 投影矩阵的逆矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 invProjMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 观察投影矩阵 观察矩阵 x 投影矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 viewProjMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 观察投影矩阵的逆矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 invViewProjMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 观察者世界空间位置
	/// </summary>
	DirectX::XMFLOAT3 eyeWorldPos{ 0.f, 0.f, 0.f };
	/// <summary>
	/// 渲染目标宽高
	/// </summary>
	DirectX::XMFLOAT2 renderTargetSize{ 0.f, 0.f };
	/// <summary>
	/// 渲染对象的宽高倒数
	/// </summary>
	DirectX::XMFLOAT2 invRenderTargetSize{ 0.f, 0.f };
	/// <summary>
	/// 近平面
	/// </summary>
	float nearZ{ 0.f };
	/// <summary>
	/// 远平面
	/// </summary>
	float farZ{ 0.f };
	/// <summary>
	/// 总时间
	/// </summary>
	float totalTime{ 0.f };
	/// <summary>
	/// 帧间隔时间
	/// </summary>
	float deltaTime{ 0.f };
};

/// <summary>
/// 顶点属性
/// </summary>
struct Vertex
{
public:
	/// <summary>
	/// 顶点位置
	/// </summary>
	DirectX::XMFLOAT3 position;
	/// <summary>
	/// 顶点颜色
	/// </summary>
	DirectX::XMFLOAT4 color;
};

/// <summary>
/// 陆地和波浪帧资源
/// </summary>
struct LandAndWavesFrameResource
{
public:
	LandAndWavesFrameResource(ID3D12Device* pDevice, UINT objCBCount, UINT passCBCount);
	LandAndWavesFrameResource(const LandAndWavesFrameResource& rhs) = delete;
	LandAndWavesFrameResource operator=(const LandAndWavesFrameResource& rhs) = delete;
	~LandAndWavesFrameResource();

public:
	/// <summary>
	/// 帧资源使用的命令分配器
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator{ nullptr };
	/// <summary>
	/// 物体常量缓冲区上传堆资源
	/// </summary>
	std::unique_ptr<UploadBuffer<ObjectConstants>> pObjCB{ nullptr };
	/// <summary>
	/// 过程常量缓冲区上传堆资源
	/// </summary>
	std::unique_ptr<UploadBuffer<PassConstants>> pPassCB{ nullptr };
	/// <summary>
	/// 标记的围栏值，标识帧资源是否被使用
	/// </summary>
	UINT64 fenceValue{};
};
