/**
 * DXHelloStenciling.h
 * Demo09_DXHelloStenciling练习项目，模板知识练习项目头文件
 **/

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"

/// <summary>
/// 模板练习项目
/// </summary>
class DXHelloStenciling : public DirectXBaseWork
{
public:
	DXHelloStenciling(std::wstring title, UINT width = 1280U, UINT height = 720U);
	virtual ~DXHelloStenciling();

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