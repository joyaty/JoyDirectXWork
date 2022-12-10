/*
 * FrameResource.h
 * Demo04 - 渲染项
 */

#pragma once

#include "DirectXBaseWork/MathUtil.h"

struct MeshGeometry;

extern const int kNumFrameResource;

/// <summary>
/// 渲染项数据结构
/// </summary>
struct HelloShapesRenderItem
{
public:
	HelloShapesRenderItem() = default;

	/// <summary>
	/// 本地空间到世界空间的变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 worldMatrix = MathUtil::Identity4x4();

	/// <summary>
	/// 渲染项关联的物体缓冲区的索引
	/// </summary>
	UINT objectCBIndex{ 0 };

	/// <summary>
	/// 此渲染项参与绘制的几何体
	/// </summary>
	MeshGeometry* meshGeo{ nullptr };

	/// <summary>
	/// 图元拓扑
	/// </summary>
	D3D12_PRIMITIVE_TOPOLOGY primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstance的参数
	/// <summary>
	/// 索引总数
	/// </summary>
	UINT32 indexCount{ 0 };
	/// <summary>
	/// 起始的索引位置，多个子缓冲区整合为大缓冲区使用
	/// </summary>
	UINT32 startIndexLocation{ 0 };
	/// <summary>
	/// 起始的顶点位置，多个子缓冲区整合为大缓冲区使用
	/// </summary>
	int startVertexLocation{ 0 };

	/// <summary>
	/// 更新脏标记
	/// </summary>
	int numFramesDirty{ kNumFrameResource };
};