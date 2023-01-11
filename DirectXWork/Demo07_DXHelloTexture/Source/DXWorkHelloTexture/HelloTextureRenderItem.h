/**
 * HelloTextureRenderItem.h
 * HelloTexture示例项目使用的渲染项数据结构
 **/

#pragma once

#include "DirectXBaseWork/D3D12Util.h"

/// <summary>
/// HelloTexture示例项目使用的渲染项数据结构
/// </summary>
struct HelloTextureRenderItem
{
	/// <summary>
	/// 物体常量缓冲区索引
	/// </summary>
	int objectCBIndex;
	/// <summary>
	/// 材质常量缓冲区索引
	/// </summary>
	int matCBIndex;
	/// <summary>
	/// 图元拓扑
	/// </summary>
	D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	/// <summary>
	/// 关联物体网格数据
	/// </summary>
	MeshGeometry* pMeshGeo{ nullptr };

	
};