/*
 * D3D12HelloShapes.cpp
 * Demo04 - DirectX12渲染多个几何图形
 */

#pragma once

#include "DirectXBaseWork/DirectXBaseWork.h"

// 前置声明 - 帧资源
struct FrameResource;

/// <summary>
/// 常量缓冲区结构体
/// </summary>
struct ObjectConstants
{

};

/// <summary>
/// DX示例 - 绘制多个几何体
/// </summary>
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

private:
	/// <summary>
	/// 构建帧资源环形队列
	/// </summary>
	void BuildFrameResource();

private:
	/// <summary>
	/// 帧资源的个数
	/// </summary>
	const int kNumFrameResource = 3;
	/// <summary>
	/// 帧资源环形数组，重复使用空闲的帧资源
	/// </summary>
	std::vector<std::unique_ptr<FrameResource>> m_FrameResources{};
	/// <summary>
	/// 当前的帧资源索引
	/// </summary>
	int m_CurrentFrameResourceIndex{ 0 };
	/// <summary>
	/// 当前的帧资源
	/// </summary>
	FrameResource* m_CurrentFrameResource{ nullptr };

};