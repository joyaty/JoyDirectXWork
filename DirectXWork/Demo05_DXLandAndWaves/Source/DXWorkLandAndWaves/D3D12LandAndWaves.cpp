/**
 * D3D12LandAndWaves.h
 * 陆地与波浪示例程序Direct3D代码
 */

#include "stdafx.h"
#include "D3D12LandAndWaves.h"
#include "IMGuiLandAndWaves.h"

D3D12LandAndWaves::D3D12LandAndWaves(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
	m_AssetPath += L"\\DirectXWork\\Demo05_DXLandAndWaves\\Source\\DXWorkLandAndWaves\\";
}

D3D12LandAndWaves::~D3D12LandAndWaves()
{
}

bool D3D12LandAndWaves::OnInit()
{
	return true;
}

void D3D12LandAndWaves::OnUpdate(float deltaTime, float totalTime)
{

}

void D3D12LandAndWaves::OnRender()
{

}

void D3D12LandAndWaves::OnDestroy()
{

}

void D3D12LandAndWaves::OnMouseDown(UINT8 keyCode, int x, int y)
{

}

void D3D12LandAndWaves::OnMouseUp(UINT8 keyCode, int x, int y)
{

}

void D3D12LandAndWaves::OnMouseMove(UINT8 keyCode, int x, int y)
{

}
void D3D12LandAndWaves::OnResize(UINT width, UINT height)
{

}