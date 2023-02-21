/**
 * HelloStencilingFrameResource.h
 * 帧资源数据结构定义头文件
 **/

#pragma once

#include "DirectXBaseWork/UploadBuffer.h"
#include "DirectXBaseWork/RenderConstantsStruct.h"

/// <summary>
/// 帧资源数据结构
/// </summary>
struct HelloStencilingFrameResource
{
public:
	HelloStencilingFrameResource(ID3D12Device* pDevice, int objectCount, int matCount, int passCount = 1);
	HelloStencilingFrameResource(const HelloStencilingFrameResource& frameResource) = delete;
	HelloStencilingFrameResource operator= (const HelloStencilingFrameResource& frameResource) = delete;
	~HelloStencilingFrameResource();

public:
	/// <summary>
	/// 指令分配器
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pCommandAllocator{ nullptr };
	/// <summary>
	/// 物体常量缓冲区
	/// </summary>
	std::unique_ptr<UploadBuffer<PerObjectConstants>> pObjectCBuffer{ nullptr };
	/// <summary>
	/// 材质常量缓冲区
	/// </summary>
	std::unique_ptr<UploadBuffer<PerMaterialConstants>> pMatCBuffer{ nullptr };
	/// <summary>
	/// 渲染过程常量缓冲区
	/// </summary>
	std::unique_ptr<UploadBuffer<PerPassConstancts>> pPassCBuffer{ nullptr };

	/// <summary>
	/// 围栏值，标识帧资源是否被处理
	/// </summary>
	UINT64 fenceValue{ 0U };
};