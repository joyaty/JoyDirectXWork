/**
 * RenderConstantsStruct.h
 * 常量数据结构定义
 **/

#pragma once

#include "MathUtil.h"

/// <summary>
/// 最大光源数
/// </summary>
constexpr int MaxLights = 16;

/// <summary>
/// 渲染层级枚举
/// </summary>
enum class EnumRenderLayer : unsigned char
{
	// 不透明层
	LayerOpaque = 0,
	// 阴影层
	LayerShadow,
	// 模板缓冲区标记
	LayerStencilMask,
	// 镜面反射层
	LayerReflection,
	// 透明层
	LayerTransparent,
	// 透明度测试，透明片段直接丢弃
	LayerAlphaTest,

	// RenderLayer数量
	Count
};

/// <summary>
/// 光源数据结构
/// 注意成员数据的定义顺序不是随意的，而是遵循HLSL结构体封装规则。HLSL以填充对齐的形式，将结构体的元素打包为4D向量。且单个元素不能分布到两个4D向量中
/// 这样定义成员数据顺序，恰好可以组成3个4D向量。
/// </summary>
struct Light
{
	/// <summary>
	/// 光源颜色
	/// </summary>
	DirectX::XMFLOAT3 strength{ 0.5f, 0.5f, 0.5f };
	/// <summary>
	/// 衰减开始位置，点光源和聚光灯光源使用
	/// </summary>
	float falloffStart{ 1.f };
	/// <summary>
	/// 光照方向，平行光源和聚光灯光源使用
	/// </summary>
	DirectX::XMFLOAT3 direction{ 0.0f, -1.0f, 0.0f };
	/// <summary>
	/// 衰减到0位置，点光源和聚光灯光源使用
	/// </summary>
	float falloffEnd{ 10.f };
	/// <summary>
	/// 光源位置，点光源和聚光灯光源使用
	/// </summary>
	DirectX::XMFLOAT3 position{ 0.f, 0.f, 0.f };
	/// <summary>
	/// 夹角衰减参数幂，聚光灯光源使用
	/// </summary>
	float spotPower{ 64.f };
};

/// <summary>
/// 纹理数据结构
/// </summary>
struct Texture
{
	/// <summary>
	/// 纹理标识名称
	/// </summary>
	std::string name;
	/// <summary>
	/// 纹理文件名
	/// </summary>
	std::wstring fileName;
	/// <summary>
	/// 默认堆上的纹理资源
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> m_TextureGPU{ nullptr };
	/// <summary>
	/// 上传堆上的纹理资源，作为CPU拷贝数据到默认堆上的中转
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> m_TextureUpload{ nullptr };
};

 /// <summary>
 /// 渲染物体常量数据
 /// </summary>
struct PerObjectConstants
{
	/// <summary>
	/// 本地到世界空间的变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 worldMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 纹理坐标变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 texMatrix{ MathUtil::Identity4x4() };
};

/// <summary>
/// 渲染材质常量数据
/// </summary>
struct PerMaterialConstants
{
	/// <summary>
	/// 漫反射反照率
	/// </summary>
	DirectX::XMFLOAT4 albedo{};
	/// <summary>
	/// fresnel效果R0
	/// </summary>
	DirectX::XMFLOAT3 fresnelR0{};
	/// <summary>
	/// 粗糙度
	/// </summary>
	float roughness{};
	/// <summary>
	/// UV变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 uvMatrix{ MathUtil::Identity4x4() };
};

/// <summary>
/// 渲染过程的常量数据结构
/// </summary>
struct PerPassConstancts
{
	/// <summary>
	/// 观察矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 viewMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 观察矩阵的逆矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 invViewMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 投影矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 projMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 投影矩阵的逆矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 invProjMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 观察投影矩阵 观察矩阵 x 投影矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 viewProjMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 观察投影矩阵的逆矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 invViewProjMatrix{ MathUtil::Identity4x4() };
	/// <summary>
	/// 观察者世界空间位置
	/// </summary>
	DirectX::XMFLOAT3 eyeWorldPos{ 0.f, 0.f, 0.f };
	/// <summary>
	/// 填充站位，与eyeWorldPos构成一个4D向量，避免memcpy拷贝数据到GPU错位
	/// </summary>
	float __padding0{};
	/// <summary>
	/// 渲染目标宽高
	/// </summary>
	DirectX::XMFLOAT2 renderTargetSize{ 0.f, 0.f };
	/// <summary>
	/// 渲染对象的宽高倒数
	/// </summary>
	DirectX::XMFLOAT2 invRenderTargetSize{ 0.f, 0.f };
	/// <summary>
	/// 近平面
	/// </summary>
	float nearZ{ 0.f };
	/// <summary>
	/// 远平面
	/// </summary>
	float farZ{ 0.f };
	/// <summary>
	/// 总时间
	/// </summary>
	float totalTime{ 0.f };
	/// <summary>
	/// 帧间隔时间
	/// </summary>
	float deltaTime{ 0.f };
	/// <summary>
	/// 雾的颜色
	/// </summary>
	DirectX::XMFLOAT4 fogColor{ DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f) };
	/// <summary>
	/// 雾开始影响的到摄像机的距离
	/// </summary>
	float fogStart{ 75.f };
	/// <summary>
	/// 雾的范围
	/// </summary>
	float fogRange{ 150.f };
	/// <summary>
	/// 填充占位，与fogStart,fogRange构成4D向量
	/// </summary>
	float __padding1[2];
	/// <summary>
	/// 环境光
	/// </summary>
	DirectX::XMFLOAT4 ambientLight{ 0.f, 0.f, 0.f, 0.f };
	/// <summary>
	/// 所有光源数据
	/// </summary>
	Light lights[MaxLights];
};
