/**
 * LandAndWavesWithLighting.cpp
 * 带有光照的陆地与波浪示例程序Direct3D代码文件
 */

#include "stdafx.h"
#include "LandAndWavesWithLighting.h"

LandAndWavesWithLighting::LandAndWavesWithLighting(std::wstring title, UINT width/* = 1280U*/, UINT height/* = 720U*/)
	: DirectXBaseWork(title, width, height)
{
	m_AssetPath += L"\\DirectXWork\\Demo07_DXLandAndWavesWithLighting\\Source\\DXWorkLandAndWavesWithLighting";
}

LandAndWavesWithLighting::~LandAndWavesWithLighting()
{
}

bool LandAndWavesWithLighting::OnInit()
{
	return true;
}

void LandAndWavesWithLighting::OnUpdate(float deltaTime, float totalTime)
{
}

void LandAndWavesWithLighting::OnRender()
{
}

void LandAndWavesWithLighting::OnDestroy()
{
}