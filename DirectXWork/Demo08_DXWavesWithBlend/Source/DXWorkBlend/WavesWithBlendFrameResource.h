/**
 * WavesWithBlendFrameResource.h
 * 帧资源数据结构定义
 **/

#pragma once

#include "DirectXBaseWork/UploadBuffer.h"
#include "DirectXBaseWork/RenderConstantsStruct.h"
#include "WavesWithBlendRenderItem.h"

struct WavesWithBlendFrameResource
{
public:
	WavesWithBlendFrameResource(ID3D12Device* pDevice, int objectCount, int matCount, int waveVertexCount, int passCount = 1);
	WavesWithBlendFrameResource(const WavesWithBlendFrameResource& frameResource) = delete;
	WavesWithBlendFrameResource operator=(const WavesWithBlendFrameResource& frameResource) = delete;
	~WavesWithBlendFrameResource();

public:
	/// <summary>
	/// 帧资源需要有独立的命令分配器，避免不同帧相互影响
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pCmdAllocator{ nullptr };
	/// <summary>
	/// 独立的物体常量缓冲区
	/// </summary>
	std::unique_ptr<UploadBuffer<PerObjectConstants>> pObjCBuffer{ nullptr };
	/// <summary>
	/// 独立的材质常量缓冲区
	/// </summary>
	std::unique_ptr<UploadBuffer<PerMaterialConstants>> pMatCBuffer{ nullptr };
	/// <summary>
	/// 独立的渲染Pass常量缓冲区
	/// </summary>
	std::unique_ptr<UploadBuffer<PerPassConstancts>> pPassCBuffer{ nullptr };
	/// <summary>
	/// 在上传堆的动态顶点缓冲区
	/// </summary>
	std::unique_ptr<UploadBuffer<Vertex>> pDynamicVertexBuffer{ nullptr };

	/// <summary>
	/// 围栏值，与当前管线执行到的围栏值对比，确认当前帧资源是否已使用
	/// </summary>
	UINT64 fenceValue{ 0 };
};