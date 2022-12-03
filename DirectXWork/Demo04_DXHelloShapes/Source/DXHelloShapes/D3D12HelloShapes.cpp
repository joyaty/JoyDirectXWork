/*
 * D3D12HelloShapes.cpp
 * Demo04 - DirectX12渲染多个几何图形
 */

#include "stdafx.h"
#include "D3D12HelloShapes.h"

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
	return true;
}

void D3D12HelloShapes::OnUpdate(float deltaTime, float totalTime)
{
}

void D3D12HelloShapes::OnRender()
{
}

void D3D12HelloShapes::OnDestroy()
{
}