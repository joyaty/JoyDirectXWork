/**
 * HelloTextureRenderItem.h
 * HelloTexture示例项目使用的渲染项数据结构
 **/

#pragma once

#include "DirectXBaseWork/D3D12Util.h"
#include "DirectXBaseWork/MathUtil.h"

/// <summary>
/// 声明帧资源数
/// </summary>
extern const int kFrameResourceCount;

/// <summary>
/// 顶点数据结构定义
/// </summary>
struct HelloTextureVertex
{
	/// <summary>
	/// 顶点位置
	/// </summary>
	DirectX::XMFLOAT3 position{};
	/// <summary>
	/// 顶点法线
	/// </summary>
	DirectX::XMFLOAT3 normal{};
	/// <summary>
	/// 顶点纹理坐标0
	/// </summary>
	DirectX::XMFLOAT2 texCoord0{};
};

 /// <summary>
 /// 渲染材质数据结构
 /// </summary>
struct HelloTextureMaterial
{
	/// <summary>
	/// 材质名称
	/// </summary>
	std::string name;
	/// <summary>
	/// 漫反射反照率
	/// </summary>
	DirectX::XMFLOAT4 albedo{};
	/// <summary>
	/// 材质菲涅尔效果R0属性
	/// </summary>
	DirectX::XMFLOAT3 fresnelR0{};
	/// <summary>
	/// 粗糙度
	/// </summary>
	float roughness{};

	/// <summary>
	/// 关联的材质常量缓冲区索引
	/// </summary>
	int matCBIndex{};

	/// <summary>
	/// 脏标记，每个帧资源初始默认都为脏
	/// </summary>
	int numDirty{ kFrameResourceCount };
};

/// <summary>
/// HelloTexture示例项目使用的渲染项数据结构
/// </summary>
struct HelloTextureRenderItem
{
	/// <summary>
	/// 世界变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 worldMatrix{ MathUtil::Identity4x4() };

	/// <summary>
	/// 物体常量缓冲区索引
	/// </summary>
	int objectCBIndex;
	/// <summary>
	/// 图元拓扑
	/// </summary>
	D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	/// <summary>
	/// 关联物体网格数据
	/// </summary>
	MeshGeometry* pMeshGeo{ nullptr };
	/// <summary>
	/// 材质数据
	/// </summary>
	HelloTextureMaterial* pMaterial{ nullptr };

	/// <summary>
	/// 索引总数
	/// </summary>
	int indexCount{};
	/// <summary>
	/// 起始索引偏移
	/// </summary>
	int startIndexOffset{};
	/// <summary>
	/// 起始顶点偏移
	/// </summary>
	int startVertexOffset{};

	/// <summary>
	/// 脏标记，初始每个帧资源都标记为脏
	/// </summary>
	int numDirty{ kFrameResourceCount };
};