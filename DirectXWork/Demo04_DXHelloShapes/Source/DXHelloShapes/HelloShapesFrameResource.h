/*
 * FrameResource.h
 * Demo04 - 帧资源
 */

#pragma once

#include "DirectXBaseWork/UploadBuffer.h"

struct ObjectConstants;
struct PassConstants;

struct HelloShapesFrameResource
{
public:
	HelloShapesFrameResource(ID3D12Device* pDevice, UINT passCount, UINT objectCount);
	HelloShapesFrameResource(const HelloShapesFrameResource& rhs) = delete;
	HelloShapesFrameResource& operator=(const HelloShapesFrameResource& rhs) = delete;
	~HelloShapesFrameResource();

public:
	/// <summary>
	/// 帧资源专属的命令分配器，在GPU处理完此命令分配器相关的命令之前，不能重置
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pCommandAllocator{ nullptr };

	/// <summary>
	/// 渲染物体常量缓冲区资源，在GPU处理完此命令分配器相关的命令之前，不能更新
	/// </summary>
	std::unique_ptr<UploadBuffer<ObjectConstants>> pObjectConstants{ nullptr };

	/// <summary>
	/// 渲染过程常量缓冲区资源，在GPU处理完此命令分配器相关的命令之前，不能更新
	/// </summary>
	std::unique_ptr<UploadBuffer<PassConstants>> pPassConstants{ nullptr };

	/// <summary>
	/// 标记的围栏值，可以通过此值判断资源是否GPU还需要使用
	/// </summary>
	UINT64 fenceValue = 0;
};