/**
 * DXWavesWithBlend.cpp
 * Demo08_DXWavesWithBlend示例项目主实现文件
 **/

#include "DXWavesWithBlend.h"

DXWavesWithBlend::DXWavesWithBlend(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
	m_AssetPath += L"\\DirectXWork\\Demo08_DXWavesWithBlend\\Source\\DXWorkBlend\\";
}

DXWavesWithBlend::~DXWavesWithBlend()
{
}

void DXWavesWithBlend::OnResize(UINT width, UINT height)
{
	DirectXBaseWork::OnResize(width, height);
}

void DXWavesWithBlend::OnMouseDown(UINT8 keyCode, int x, int y)
{
}

void DXWavesWithBlend::OnMouseUp(UINT8 keyCode, int x, int y)
{
}

void DXWavesWithBlend::OnMouseMove(UINT8 keyCode, int x, int y)
{
}

bool DXWavesWithBlend::OnInit()
{
	return true;
}

void DXWavesWithBlend::OnUpdate(float deltaTime, float totalTime)
{
}

void DXWavesWithBlend::OnRender()
{
}

void DXWavesWithBlend::OnDestroy()
{
}

