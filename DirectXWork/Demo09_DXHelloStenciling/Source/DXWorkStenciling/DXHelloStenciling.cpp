/**
 * DXHelloStenciling.cpp
 * Demo09_DXHelloStenciling练习项目，模板知识练习项目实现文件
 **/

#include "stdafx.h"
#include "DXHelloStenciling.h"

DXHelloStenciling::DXHelloStenciling(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
	m_AssetPath += L"\\DirectXWork\\Demo09_DXHelloStenciling\\Source\\DXWorkStenciling\\";
}
DXHelloStenciling::~DXHelloStenciling()
{
}


void DXHelloStenciling::OnMouseDown(UINT8 keyCode, int x, int y)
{
}

void DXHelloStenciling::OnMouseUp(UINT8 keyCode, int x, int y)
{
}

void DXHelloStenciling::OnMouseMove(UINT8 keyCode, int x, int y)
{
}

void DXHelloStenciling::OnResize(UINT width, UINT height)
{
}

bool DXHelloStenciling::OnInit()
{
	return true;
}

void DXHelloStenciling::OnUpdate(float deltaTime, float totalTime)
{
}

void DXHelloStenciling::OnRender()
{
}

void DXHelloStenciling::OnDestroy()
{
}