/*
 * FrameResource.h
 * Demo04 - 帧资源
 */

#pragma once

#include "DirectXBaseWork/UploadBuffer.h"

struct ObjectConstants;

struct FrameResource
{
public:
	FrameResource(ID3D12Device* pDevice, UINT objectCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource();

public:
	/// <summary>
	/// 获取当前帧资源标记的围栏值
	/// </summary>
	UINT64 GetFenceValue() const { return m_FenceValue; }

private:
	/// <summary>
	/// 帧资源专属的命令分配器，在GPU处理完此命令分配器相关的命令之前，不能重置
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator{ nullptr };

	/// <summary>
	/// 常量缓冲区资源，在GPU处理完此命令分配器相关的命令之前，不能更新
	/// </summary>
	std::unique_ptr<UploadBuffer<ObjectConstants>> m_ObjectConstants{ nullptr };

	/// <summary>
	/// 标记的围栏值，可以通过此值判断资源是否GPU还需要使用
	/// </summary>
	UINT64 m_FenceValue = 0;
};