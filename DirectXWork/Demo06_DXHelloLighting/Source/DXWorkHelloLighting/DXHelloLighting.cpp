/**
 * DXHelloLighting.cpp
 * 光照模型实例程序代码文件
 */

#include "stdafx.h"
#include "DXHelloLighting.h"

/// <summary>
/// 帧资源个数
/// </summary>
const int kFrameResourceCount = 3;

DXHelloLighting::DXHelloLighting(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
	m_AssetPath += L"\\DirectXWork\\Demo06_DXHelloLighting\\Source\\DXWorkHelloLighting";
}

DXHelloLighting::~DXHelloLighting()
{
}

bool DXHelloLighting::OnInit()
{
	return true;
}

void DXHelloLighting::OnUpdate(float delaTime, float totalTime)
{
}

void DXHelloLighting::OnRender()
{
}

void DXHelloLighting::OnDestroy()
{
}