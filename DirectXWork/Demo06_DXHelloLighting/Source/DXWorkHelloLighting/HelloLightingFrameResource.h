/**
 * HelloLightingFrameResource.h
 * 帧资源头文件
 **/

#pragma once

#include "DirectXBaseWork/UploadBuffer.h"
#include "DirectXBaseWork/MathUtil.h"
#include "DirectXBaseWork/RenderConstantsStruct.h"

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
	/// <param name="passCount">渲染过程数量</param>
	HelloLightingFrameResource(ID3D12Device* pDevice, UINT objCount, UINT passCount);
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
	/// 渲染过程常量上传堆缓冲区资源
	/// </summary>
	std::unique_ptr<UploadBuffer<PerPassConstancts>> pPassUploadBuffer{ nullptr };

	/// <summary>
	/// 当前帧资源标记的围栏值
	/// </summary>
	UINT64 fenceValue{ 0 };
};
