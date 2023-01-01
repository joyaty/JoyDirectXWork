/**
 * DXHelloLighting.h
 * 光照模型实例程序头文件
 */

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"

/// <summary>
/// Direct3D构建光照模型
/// </summary>
class DXHelloLighting : public DirectXBaseWork
{
public:
	DXHelloLighting(std::wstring title, UINT width = 1280U, UINT height = 720U);
	virtual ~DXHelloLighting();

protected:
	bool OnInit() override;
	void OnUpdate(float deltaTime, float totalTime) override;
	void OnRender() override;
	void OnDestroy() override;
};