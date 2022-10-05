/*
 * D3D12HelloCube.cpp
 * Demo02 - DirectX12渲染立方体
 */

#include "stdafx.h"
#include "D3D12HelloCube.h"


D3D12HelloCube::D3D12HelloCube(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
}

D3D12HelloCube::~D3D12HelloCube()
{
}

bool D3D12HelloCube::OnInit()
{
	return true;
}

void D3D12HelloCube::OnUpdate()
{
}

void D3D12HelloCube::OnRender()
{
}

void D3D12HelloCube::OnDestroy()
{

}