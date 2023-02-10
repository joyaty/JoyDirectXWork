/**
 * DXWavesWithBlend.h
 * Demo08_DXWavesWithBlend示例项目主实现头文件
 **/

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"

/// <summary>
/// Demo08_DXWavesWithBlend示例项目主实现
/// </summary>
class DXWavesWithBlend : public DirectXBaseWork
{
public:
	DXWavesWithBlend(std::wstring title, UINT width = 1280U, UINT height = 720U);
	virtual ~DXWavesWithBlend();

public:
	void OnResize(UINT width, UINT height) override;
	void OnMouseDown(UINT8 keyCode, int x, int y) override;
	void OnMouseUp(UINT8 keyCode, int x, int y) override;
	void OnMouseMove(UINT8 keyCode, int x, int y) override;

protected:
	bool OnInit() override;
	void OnUpdate(float deltaTime, float totalTime) override;
	void OnRender() override;
	void OnDestroy() override;
};