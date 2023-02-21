/**
 * HelloStencilingRenderItem.h
 * 渲染项相关数据结构定义
 **/

#pragma once

#include "DirectXBaseWork/D3D12Util.h"
#include "DirectXBaseWork/MathUtil.h"

/// <summary>
/// 外部声明
/// </summary>
extern const int kNumFrameResource;

/// <summary>
/// 顶点数据结构
/// </summary>
struct Vertex
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
	/// 顶点纹理坐标
	/// </summary>
	DirectX::XMFLOAT2 texCoord{};
};

/// <summary>
/// 材质数据结构
/// </summary>
struct HelloStencilingMaterial
{
	/// <summary>
	/// 材质名称标识
	/// </summary>
	std::string name;

	/// <summary>
	/// UV变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 matMatrix{ MathUtil::Identity4x4() };

	/// <summary>
	/// 漫反射反照率
	/// </summary>
	DirectX::XMFLOAT4 diffuseAlbedo{};
	/// <summary>
	/// fresnel效应R0属性
	/// </summary>
	DirectX::XMFLOAT3 fresnelR0{};
	/// <summary>
	/// 粗糙度
	/// </summary>
	float roughness{};

	/// <summary>
	/// 漫反射反照率贴图索引
	/// </summary>
	int diffuseMapIndex{};

	/// <summary>
	/// 材质常量缓冲区索引
	/// </summary>
	int matCBufferIndex{};

	/// <summary>
	/// 数据脏标记
	/// </summary>
	int dirty{ kNumFrameResource };
};

/// <summary>
/// 渲染项数据结构
/// </summary>
struct HelloStencilingRenderItem
{
	/// <summary>
	/// 渲染项关联的几何体
	/// </summary>
	MeshGeometry* pGeometryMesh{ nullptr };
	/// <summary>
	/// 渲染项关联的材质
	/// </summary>
	HelloStencilingMaterial* pMaterial{ nullptr };
	/// <summary>
	/// 图元拓扑
	/// </summary>
	D3D_PRIMITIVE_TOPOLOGY primitiveType{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	/// <summary>
	/// 世界空间变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 worldMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 纹理变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 texMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 渲染项关联的物体常量缓冲区索引
	/// </summary>
	int objectCBufferIndex{};

	/// <summary>
	/// 索引总数
	/// </summary>
	int indexCount{ 0 };
	/// <summary>
	/// 索引起始偏移
	/// </summary>
	int startIndexLoaction{ 0 };
	/// <summary>
	/// 顶点起始偏移
	/// </summary>
	int startVertexLocation{ 0 };

	/// <summary>
	/// 数据脏标记
	/// </summary>
	int dirty{ kNumFrameResource };
};