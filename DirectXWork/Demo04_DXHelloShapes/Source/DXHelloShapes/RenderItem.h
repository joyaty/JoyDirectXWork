/*
 * FrameResource.h
 * Demo04 - 渲染项
 */

#pragma once

#include "DirectXBaseWork/MathUtil.h"

class MeshGeometry;

/// <summary>
/// 渲染项数据结构
/// </summary>
struct RenderItem
{
public:
	RenderItem() = default;

	/// <summary>
	/// 本地空间到世界空间的变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 worldMatrix = MathUtil::Identity4x4();

	/// <summary>
	/// 此渲染项参与绘制的几何体
	/// </summary>
	MeshGeometry* geo{ nullptr };

	/// <summary>
	/// 图元拓扑
	/// </summary>
	D3D12_PRIMITIVE_TOPOLOGY primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};