/**
 * DXHelloTexture.h
 * 纹理贴图实例程序头文件
 **/

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"

/// <summary>
/// 纹理贴图示例程序
/// </summary>
class DXHelloTexture : public DirectXBaseWork
{
public:
	DXHelloTexture(std::wstring title, UINT width, UINT height);
	virtual ~DXHelloTexture();

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