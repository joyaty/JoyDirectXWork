/*
 * D3D12HelloShapes.cpp
 * Demo04 - DirectX12渲染多个几何图形
 */

#include "stdafx.h"
#include "D3D12HelloShapes.h"
#include "FrameResource.h"

D3D12HelloShapes::D3D12HelloShapes(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
	m_AssetPath += L"\\DirectXWork\\Demo04_DXHelloShapes\\Source\\DXHelloShapes\\";
}

D3D12HelloShapes::~D3D12HelloShapes()
{
}

bool D3D12HelloShapes::OnInit()
{
	BuildFrameResource();
	return true;
}

void D3D12HelloShapes::OnUpdate(float deltaTime, float totalTime)
{
	// 循环往复获取帧资源循环数组中的帧资源
	m_CurrentFrameResourceIndex = (m_CurrentFrameResourceIndex + 1) % kNumFrameResource;
	m_CurrentFrameResource = m_FrameResources[m_CurrentFrameResourceIndex].get();
	// 当前完成的围栏值小于帧资源标记的围栏值，说明需要CPU需要等待GPU完成当前帧资源的绘制
	if (m_CurrentFrameResource->GetFenceValue() != 0 && m_CurrentFrameResource->GetFenceValue() > m_Fence->GetCompletedValue())
	{
		// FlushCommandQueue();
	}
}

void D3D12HelloShapes::OnRender()
{
}

void D3D12HelloShapes::OnDestroy()
{
}

void D3D12HelloShapes::BuildFrameResource()
{
	for (int i = 0; i < kNumFrameResource; ++i)
	{
		m_FrameResources.push_back(std::make_unique<FrameResource>(m_Device.Get(), 1));
	}
}