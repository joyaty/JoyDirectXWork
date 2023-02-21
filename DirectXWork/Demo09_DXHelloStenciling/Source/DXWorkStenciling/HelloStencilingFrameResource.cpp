/**
 * HelloStencilingFrameResource.cpp
 * 帧资源数据结构实现文件
 **/

#include "stdafx.h"
#include "HelloStencilingFrameResource.h"

HelloStencilingFrameResource::HelloStencilingFrameResource(ID3D12Device* pDevice, int objectCount, int matCount, int passCount/* = 1*/)
{
	ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(pCommandAllocator.GetAddressOf()));
	pObjectCBuffer = std::make_unique<UploadBuffer<PerObjectConstants>>(pDevice, objectCount, true);
	pMatCBuffer = std::make_unique<UploadBuffer<PerMaterialConstants>>(pDevice, matCount, true);
	pPassCBuffer = std::make_unique<UploadBuffer<PerPassConstancts>>(pDevice, passCount, true);
}

HelloStencilingFrameResource::~HelloStencilingFrameResource()
{
}