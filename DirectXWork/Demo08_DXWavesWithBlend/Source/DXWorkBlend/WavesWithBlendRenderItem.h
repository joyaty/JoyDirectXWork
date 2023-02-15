/**
 * WavesWithBlendRenderItem.h
 * 渲染项数据结构定义
 **/

#pragma once

#include "DirectXBaseWork/D3D12Util.h"
#include "DirectXBaseWork/MathUtil.h"

extern const int kNumFrameResource;

/// <summary>
/// 顶点数据结构
/// </summary>
struct Vertex
{
	/// <summary>
	/// 顶点坐标 - 局部空间
	/// </summary>
	DirectX::XMFLOAT3 position;
	/// <summary>
	/// 顶点法线
	/// </summary>
	DirectX::XMFLOAT3 normal;
	/// <summary>
	/// 纹理坐标
	/// </summary>
	DirectX::XMFLOAT2 texCoord;
};

/// <summary>
/// 材质数据结构
/// </summary>
struct WavesWithBlendMaterial
{
	std::string name;
	/// <summary>
	/// 漫反射反照率
	/// </summary>
	DirectX::XMFLOAT4 diffuseAlbedo;
	/// <summary>
	/// frenselR0
	/// </summary>
	DirectX::XMFLOAT3 fresnelR0;
	/// <summary>
	/// 粗糙度
	/// </summary>
	float rougness;
	/// <summary>
	/// 材质关联的常量缓冲区索引
	/// </summary>
	int matConstantBufferIndex;
	/// <summary>
	/// 漫反射贴图索引
	/// </summary>
	int diffuseMapIndex;
	/// <summary>
	/// 数据脏标记
	/// </summary>
	int dirty{ kNumFrameResource };
};

/// <summary>
/// 渲染项数据结构
/// </summary>
struct WavesWithBlendRenderItem
{
	/// <summary>
	/// 渲染项关联的Mesh
	/// </summary>
	MeshGeometry* pMeshData{ nullptr };
	/// <summary>
	/// 渲染项关联的材质
	/// </summary>
	WavesWithBlendMaterial* pMaterial{ nullptr };
	/// <summary>
	/// 关联的物体常量缓冲区索引
	/// </summary>
	int objConstantBufferIndex;
	/// <summary>
	/// 世界空间变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 worldMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 图元拓扑类型
	/// </summary>
	D3D_PRIMITIVE_TOPOLOGY primitiveType{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	/// <summary>
	/// 索引总数
	/// </summary>
	int indexCount{ 0 };
	/// <summary>
	/// 索引起始位置偏移
	/// </summary>
	int startIndexLocation{ 0 };
	/// <summary>
	/// 顶点起始位置偏移
	/// </summary>
	int startVertexLocation{ 0 };

	/// <summary>
	/// 数据脏标记
	/// </summary>
	int dirty{ kNumFrameResource };
};