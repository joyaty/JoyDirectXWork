/**
 * HelloLightingFrameResource.h
 * 帧资源头文件
 **/

#pragma once

#include "DirectXBaseWork/UploadBuffer.h"
#include "DirectXBaseWork/MathUtil.h"
#include "DirectXBaseWork/RenderConstantsStruct.h"

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

private:
	/// <summary>
	/// 帧资源需要有独立的命令分配器，在帧资源GPU处理完毕再释放
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator{ nullptr };

	/// <summary>
	/// 物体常量上传堆缓冲区资源
	/// </summary>
	std::unique_ptr<UploadBuffer<PerObjectConstants>> m_PerObjUploadBuffer{ nullptr };

	/// <summary>
	/// 渲染过程常量上传堆缓冲区资源
	/// </summary>
	std::unique_ptr<UploadBuffer<PerPassConstancts>> m_PerPassUploadBuffer{ nullptr };

	/// <summary>
	/// 当前帧资源标记的围栏值
	/// </summary>
	int fenceValue{ 0 };
};
