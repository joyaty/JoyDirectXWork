/***
 * DirectX绘制窗口颜色
 */

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"

using Microsoft::WRL::ComPtr;

class D3D12HelloWindow : public DirectXBaseWork
{
public:
	D3D12HelloWindow(std::wstring name, UINT width, UINT height);

	virtual bool OnInit() override;
	virtual void OnUpdate() override;
	virtual void OnRender() override;
	virtual void OnDestroy() override;

private:
	void PopulateCommandList();
};