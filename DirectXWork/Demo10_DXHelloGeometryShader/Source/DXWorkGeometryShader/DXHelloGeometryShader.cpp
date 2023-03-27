/**
 * DXHelloGeometryShader.cpp
 * Demo10_DXHelloGeometryShader练习项目，几何着色器知识练习项目实现文件
 **/

#include "stdafx.h"
#include "DXHelloGeometryShader.h"

DXHelloGeometryShader::DXHelloGeometryShader(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
	m_AssetPath += L"\\DirectXWork\\Demo10_DXHelloGeometryShader\\Source\\DXWorkGeometryShader\\";
}

DXHelloGeometryShader::~DXHelloGeometryShader()
{
}

void DXHelloGeometryShader::OnMouseDown(UINT8 keyCode, int x, int y)
{
}

void DXHelloGeometryShader::OnMouseUp(UINT8 keyCode, int x, int y)
{
}

void DXHelloGeometryShader::OnMouseMove(UINT8 keyCode, int x, int y)
{
}

void DXHelloGeometryShader::OnResize(UINT width, UINT height)
{
}

bool DXHelloGeometryShader::OnInit()
{
	return true;
}

void DXHelloGeometryShader::OnUpdate(float deltaTime, float totalTime)
{
}

void DXHelloGeometryShader::OnRender()
{

}

void DXHelloGeometryShader::OnDestroy()
{
}