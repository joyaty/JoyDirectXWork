/*
 * D3D12HelloCube.h 
 * Demo02 - DirectX12渲染立方体
 */

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"

class D3D12HelloCube : public DirectXBaseWork
{
public:
	D3D12HelloCube(std::wstring title, UINT width = 1280U, UINT height = 720U);
	virtual ~D3D12HelloCube();

protected:
	bool OnInit() override;
	void OnUpdate() override;
	void OnRender() override;
	void OnDestroy() override;

private:
};
