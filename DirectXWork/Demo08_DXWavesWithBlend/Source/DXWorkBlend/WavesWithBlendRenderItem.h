/**
 * WavesWithBlendRenderItem.h
 * 渲染项数据结构定义
 **/

#pragma once 

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

struct Material
{
	std::string name;


};

struct WavesWithBlendRenderItem
{

};