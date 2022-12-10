/**
 * D3D12LandAndWaves.h
 * 陆地与波浪示例程序Direct3D代码头文件声明
 */

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"

/// <summary>
/// 陆地与波浪示例程序Direct3D代码
/// </summary>
class D3D12LandAndWaves : public DirectXBaseWork
{
public:
	D3D12LandAndWaves(std::wstring title, UINT width = 1280U, UINT height = 720U);
	virtual ~D3D12LandAndWaves();

public:
	void OnMouseDown(UINT8 keyCode, int x, int y) override;
	void OnMouseUp(UINT8 keyCode, int x, int y) override;
	void OnMouseMove(UINT8 keyCode, int x, int y) override;
	void OnResize(UINT width, UINT height) override;

protected:
	bool OnInit() override;
	void OnUpdate(float deltaTime, float totalTime) override;
	void OnRender() override;
	void OnDestroy() override;
};