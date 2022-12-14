/**
 * LandAndWavesFrameResource.cpp
 * 陆地和波浪帧资源实现文件
 */

#include "stdafx.h"
#include "LandAndWavesFrameResource.h"

LandAndWavesFrameResource::LandAndWavesFrameResource(ID3D12Device* pDevice, UINT objCBCount, UINT passCBCount, UINT vertexCount)
{
	ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAllocator.GetAddressOf())));

	pObjCB = std::make_unique<UploadBuffer<ObjectConstants>>(pDevice, objCBCount, true);
	pPassCB = std::make_unique<UploadBuffer<PassConstants>>(pDevice, passCBCount, true);
	pDynamicVertices = std::make_unique<UploadBuffer<Vertex>>(pDevice, vertexCount, false);
}

LandAndWavesFrameResource::~LandAndWavesFrameResource()
{
}