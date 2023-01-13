/**
 * HelloTextureFrameResource.h
 * 帧资源定义头文件
 **/

#pragma once

#include "DirectXBaseWork/UploadBuffer.h"
#include "DirectXBaseWork/RenderConstantsStruct.h"

/// <summary>
/// 帧资源数据结构
/// </summary>
struct HelloTextureFrameResource
{
public:
	HelloTextureFrameResource(ID3D12Device* pDevice, int objectCount, int materialCount, int passCount);
	HelloTextureFrameResource(const HelloTextureFrameResource& frameResource) = delete;
	HelloTextureFrameResource operator= (const HelloTextureFrameResource& frameResource) = delete;
	~HelloTextureFrameResource();

public:
	/// <summary>
	/// 帧资源专属命令分配器
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pCommandAllocator{ nullptr };
	/// <summary>
	/// 帧资源专属的物体上传缓冲区
	/// </summary>
	std::unique_ptr<UploadBuffer<PerObjectConstants>> pPerObjCB{ nullptr };
	/// <summary>
	/// 帧资源专属的材质上传缓冲区
	/// </summary>
	std::unique_ptr<UploadBuffer<PerMaterialConstants>> pPerMatCB{ nullptr };
	/// <summary>
	/// 帧资源专属的渲染过程上传缓冲区
	/// </summary>
	std::unique_ptr<UploadBuffer<PerPassConstancts>> pPerPassCB{ nullptr };

	/// <summary>
	/// 帧资源围栏值，标识帧资源是否被使用中
	/// </summary>
	UINT64 fenceValue{ 0 };
};