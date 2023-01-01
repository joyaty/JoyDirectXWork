/**
 * LandAndWavesWithLighting.h
 * 带有光照的陆地与波浪示例程序Direct3D代码头文件
 */

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"

class LandAndWavesWithLighting : public DirectXBaseWork
{
public:
	LandAndWavesWithLighting(std::wstring title, UINT width = 1280U, UINT height = 720U);
	virtual ~LandAndWavesWithLighting();

protected:
	bool OnInit() override;
	void OnUpdate(float deltaTime, float totalTime) override;
	void OnRender() override;
	void OnDestroy() override;
};