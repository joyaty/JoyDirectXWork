/*
 * D3D12HelloShapes.cpp
 * Demo04 - DirectX12渲染多个几何图形
 */

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"

class D3D12HelloShapes : public DirectXBaseWork
{
public:
	D3D12HelloShapes(std::wstring title, UINT width = 1280U, UINT height = 720U);
	virtual ~D3D12HelloShapes();

protected:
	bool OnInit() override;
	void OnUpdate(float deltaTime, float totalTime) override;
	void OnRender() override;
	void OnDestroy() override;
};