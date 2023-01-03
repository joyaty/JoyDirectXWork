/**
 * HelloLightingFrameResource.cpp
 * 帧资源代码文件
 **/

#include "stdafx.h"
#include "HelloLightingFrameResource.h"

HelloLightingFrameResource::HelloLightingFrameResource(ID3D12Device* pDevice, UINT objCount, UINT passCount)
{
	ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAllocator.GetAddressOf())));
	m_PerObjUploadBuffer = std::make_unique<UploadBuffer<PerObjectConstants>>(pDevice, objCount, true);
	m_PerPassUploadBuffer = std::make_unique<UploadBuffer<PerPassConstancts>>(pDevice, passCount, true);
}

HelloLightingFrameResource::~HelloLightingFrameResource()
{
}