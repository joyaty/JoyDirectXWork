/*
 * FrameResource.cpp
 * Demo04 - 帧资源
 */

#include "stdafx.h"
#include "FrameResource.h"
#include "D3D12HelloShapes.h"

FrameResource::FrameResource(ID3D12Device* pDevice, UINT objectCount)
{
	// 创建帧资源专属的命令分配器
	ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAllocator.GetAddressOf())));
	// 创建常量缓冲区帧资源
	m_ObjectConstants = std::make_unique<UploadBuffer<ObjectConstants>>(pDevice, objectCount, true);
}

FrameResource::~FrameResource()
{
}