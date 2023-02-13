/**
 * WavesWithBlendFrameResource.cpp
 * 帧资源数据结构实现文件
 **/

#include "stdafx.h"
#include "WavesWithBlendFrameResource.h"

WavesWithBlendFrameResource::WavesWithBlendFrameResource(ID3D12Device* pDevice, int objectCount, int matCount, int passCount/* = 1*/)
{
	ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(pCmdAllocator.GetAddressOf())));
	pObjCBuffer = std::make_unique<UploadBuffer<PerObjectConstants>>(pDevice, objectCount, true);
	pMatCBuffer = std::make_unique<UploadBuffer<PerMaterialConstants>>(pDevice, matCount, true);
	pPassCBuffer = std::make_unique<UploadBuffer<PerPassConstancts>>(pDevice, passCount, true);
}

WavesWithBlendFrameResource::~WavesWithBlendFrameResource()
{
}