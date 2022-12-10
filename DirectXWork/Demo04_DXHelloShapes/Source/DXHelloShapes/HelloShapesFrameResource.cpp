/*
 * FrameResource.cpp
 * Demo04 - 帧资源
 */

#include "stdafx.h"
#include "HelloShapesFrameResource.h"
#include "D3D12HelloShapes.h"

HelloShapesFrameResource::HelloShapesFrameResource(ID3D12Device* pDevice, UINT passCount, UINT objectCount)
{
	// 创建帧资源专属的命令分配器
	ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(pCommandAllocator.GetAddressOf())));
	// 创建常量缓冲区帧资源
	pObjectConstants = std::make_unique<UploadBuffer<ObjectConstants>>(pDevice, objectCount, true);
	pPassConstants = std::make_unique<UploadBuffer<PassConstants>>(pDevice, passCount, true);
}

HelloShapesFrameResource::~HelloShapesFrameResource()
{
}