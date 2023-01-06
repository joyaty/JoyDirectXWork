/**
 * HelloLightingFrameResource.h
 * 帧资源头文件
 **/

#pragma once

#include "DirectXBaseWork/UploadBuffer.h"
#include "DirectXBaseWork/MathUtil.h"
#include "DirectXBaseWork/RenderConstantsStruct.h"

extern const int kFrameResourceCount;

/// <summary>
/// HelloLighting示例项目的材质属性数据结构
/// </summary>
struct HelloLightingMaterial
{
public:
	/// <summary>
	/// 材质名称
	/// </summary>
	std::string name{};

	/// <summary>
	/// 漫反射照率
	/// </summary>
	DirectX::XMFLOAT4 diffuseAlbedo{};
	/// <summary>
	/// 材质属性菲涅尔效应R(0)
	/// </summary>
	DirectX::XMFLOAT3 fresnelR0{};
	/// <summary>
	/// 粗糙[0,1]，0表示光滑，1表示最粗糙
	/// </summary>
	float roughness{};

	/// <summary>
	/// 材质常量缓冲区的索引
	/// </summary>
	int materialCBIndex{ -1 };

	/// <summary>
	/// 脏标记
	/// </summary>
	int numFrameDirty{ kFrameResourceCount };
};

 /// <summary>
 /// HelloLighting使用的顶点数据结构
 /// </summary>
struct HelloLightingVertex
{
public:
	/// <summary>
	/// 物体空间顶点位置
	/// </summary>
	DirectX::XMFLOAT3 position{};
	/// <summary>
	/// 顶点颜色
	/// </summary>
	DirectX::XMFLOAT4 color{};
	/// <summary>
	/// 顶点法线
	/// </summary>
	DirectX::XMFLOAT3 normal{};
};

/// <summary>
/// 材质常量缓冲数据结构
/// </summary>
struct PerMatConstants
{
	/// <summary>
	/// 漫反射照率
	/// </summary>
	DirectX::XMFLOAT4 diffuseAlbedo{ 1.f, 1.f, 1.f, 1.f };
	/// <summary>
	/// fresnelR0属性
	/// </summary>
	DirectX::XMFLOAT3 fresnelR0{ 0.01f, 0.01f, 0.01f };
	/// <summary>
	/// 粗糙度
	/// </summary>
	float roughness{ 0.25f };

};

/// <summary>
/// HelloLighting程序使用的帧资源
/// </summary>
struct HelloLightingFrameResource
{
public:
	/// <summary>
	/// 帧资源构造函数
	/// </summary>
	/// <param name="pDevice">GPU逻辑设备对象指针</param>
	/// <param name="objCount">渲染的物体数量</param>
	/// <param name="matCount">材质数量</param>
	/// <param name="passCount">渲染过程数量</param>
	HelloLightingFrameResource(ID3D12Device* pDevice, UINT objCount, UINT matCount, UINT passCount);
	HelloLightingFrameResource(const HelloLightingFrameResource& frameResource) = delete;
	HelloLightingFrameResource operator= (const HelloLightingFrameResource& frameResource) = delete;
	~HelloLightingFrameResource();

public:
	/// <summary>
	/// 帧资源需要有独立的命令分配器，在帧资源GPU处理完毕再释放
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pCommandAllocator{ nullptr };

	/// <summary>
	/// 物体常量上传堆缓冲区资源
	/// </summary>
	std::unique_ptr<UploadBuffer<PerObjectConstants>> pPerObjUploadBuffer{ nullptr };

	/// <summary>
	/// 材质常量上传缓冲区资源
	/// </summary>
	std::unique_ptr<UploadBuffer<PerMatConstants>> pPerMatUploadBuffer{ nullptr };

	/// <summary>
	/// 渲染过程常量上传堆缓冲区资源
	/// </summary>
	std::unique_ptr<UploadBuffer<PerPassConstancts>> pPassUploadBuffer{ nullptr };

	/// <summary>
	/// 当前帧资源标记的围栏值
	/// </summary>
	UINT64 fenceValue{ 0 };
};
