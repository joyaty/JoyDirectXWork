/**
 * HelloTextureFrameResource.cpp
 * 帧资源定义代码文件
 **/

#include "stdafx.h"
#include "HelloTextureFrameResource.h"

HelloTextureFrameResource::HelloTextureFrameResource(ID3D12Device* pDevice, int objectCount, int materialCount, int passCount)
{
	ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(pCommandAllocator.GetAddressOf())));
	pPerObjCB = std::make_unique<UploadBuffer<PerObjectConstants>>(pDevice, objectCount, true);
	pPerMatCB = std::make_unique<UploadBuffer<PerMaterialConstants>>(pDevice, materialCount, true);
	pPerPassCB = std::make_unique<UploadBuffer<PerPassConstancts>>(pDevice, materialCount, true);
}

HelloTextureFrameResource::~HelloTextureFrameResource()
{
}