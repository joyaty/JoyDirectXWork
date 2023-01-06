/**
 * HelloLightingRenderItem.h
 * HelloLighting工程使用的渲染项数据结构头文件
 **/

#pragma once

#include "DirectXBaseWork/MathUtil.h"

// 前置声明 - 渲染物体的Mesh数据结构
struct MeshGeometry;
// 前置声明 - 渲染物体的材质数据结构
struct HelloLightingMaterial;

/// <summary>
/// 声明全局常量 - kFrameResourceCount - 帧资源个数
/// </summary>
extern const int kFrameResourceCount;

/// <summary>
/// HelloLighting工程使用的渲染项数据结构
/// </summary>
struct HelloLightingRenderItem
{
public:
	/// <summary>
	/// 世界空间变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 worldMatrix{ MathUtil::Identity4x4() };

	/// <summary>
	/// 常量缓冲区索引
	/// </summary>
	int objConstantBufferIndex{ 0 };

	/// <summary>
	/// 图元拓扑
	/// </summary>
	D3D12_PRIMITIVE_TOPOLOGY primitiveType{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	/// <summary>
	/// 渲染项关联的物体网格
	/// </summary>
	MeshGeometry* pMeshGeometry{ nullptr };
	/// <summary>
	/// 渲染项关联的材质
	/// </summary>
	HelloLightingMaterial* pMat{ nullptr };

	/// <summary>
	/// 索引个数
	/// </summary>
	UINT indexCount{ 0 };
	/// <summary>
	/// 起始索引位置，多个渲染项的索引组成成一个大索引缓冲区
	/// </summary>
	UINT startIndexLocation{ 0 };
	/// <summary>
	/// 起始顶点位置，多个渲染项的顶点组合成一个大顶点缓冲区，每个顶点索引需要叠加在大缓冲区的偏移
	/// </summary>
	UINT startVetexLocation{ 0 };
	
	/// <summary>
	/// 更新脏标记
	/// </summary>
	int numFrameDirty{ kFrameResourceCount };
};