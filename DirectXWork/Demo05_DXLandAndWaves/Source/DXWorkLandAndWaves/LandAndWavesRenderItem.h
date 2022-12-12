/**
 * LandAndWavesRenderItem.h
 * 陆地与波浪渲染项头文件声明
 */

#pragma once

#include "DirectXBaseWork/MathUtil.h"

// 前置声明 - 几何体网格数据
struct MeshGeometry;

// 全局常量声明 - 帧资源个数
extern const int kNumFrameResource;

/// <summary>
/// 陆地与波浪渲染项
/// </summary>
struct LandAndWavesRenderItem
{
public:
	LandAndWavesRenderItem() = default;

	/// <summary>
	/// 世界变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 worldMatrix{ MathUtil::Identity4x4() };

	/// <summary>
	/// 渲染项物体常量缓冲区索引
	/// </summary>
	UINT objectCBIndex{ 0 };

	/// <summary>
	/// 图元拓扑
	/// </summary>
	D3D12_PRIMITIVE_TOPOLOGY primitiveType{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	/// <summary>
	/// 关联的几何体
	/// </summary>
	MeshGeometry* pGeometryMesh{ nullptr };

	/// <summary>
	/// 渲染项索引的总数
	/// </summary>
	UINT indexCount{ 0 };
	/// <summary>
	/// 渲染项索引起始位置
	/// </summary>
	UINT startIndexLocation{ 0 };
	/// <summary>
	/// 顶点的起始位置
	/// </summary>
	int startVertexLocation{ 0 };

	/// <summary>
	/// 脏标记，初始化为每个帧资源的数据均均标记需要更新
	/// </summary>
	int numFramesDirty{ kNumFrameResource };
};
