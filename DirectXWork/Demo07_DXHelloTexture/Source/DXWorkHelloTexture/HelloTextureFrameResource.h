

#pragma once

#include "DirectXBaseWork/UploadBuffer.h"
#include "DirectXBaseWork/RenderConstantsStruct.h"

/// <summary>
/// 帧资源
/// </summary>
struct HelloTextureFrameResource
{
public:
	HelloTextureFrameResource(ID3D12Device pDevice, int objectCount, int materialCount, int passCount);
	HelloTextureFrameResource(const HelloTextureFrameResource& frameResource) = delete;
	HelloTextureFrameResource operator= (const HelloTextureFrameResource& frameResource) = delete;
	~HelloTextureFrameResource();

public:
	/// <summary>
	/// 帧资源专属命令分配器
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator{ nullptr };
	/// <summary>
	/// 帧资源专属的物体上传缓冲区
	/// </summary>
	std::unique_ptr<UploadBuffer<PerObjectConstants>> perObjCB{ nullptr };
	/// <summary>
	/// 帧资源专属的渲染过程常量上传缓冲区
	/// </summary>
	std::unique_ptr<UploadBuffer<PerPassConstancts>> perPassCB{ nullptr };
};