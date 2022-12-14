/**
 * RenderConstantsStruct.h
 * 常量数据结构定义
 **/

#pragma once

#include "MathUtil.h"

/// <summary>
/// 最大光源数
/// </summary>
#ifndef MaxLights
#define MaxLights 16
#endif // !MaxLights

/// <summary>
/// 光源数据结构
/// 注意成员数据的定义顺序不是随意的，而是遵循HLSL结构体封装规则。HLSL以填充对齐的形式，将结构体的元素打包为4D向量。且单个元素不能分布到两个4D向量中
/// 这样定义成员数据顺序，恰好可以组成3个4D向量。
/// </summary>
struct Light
{
public:
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
 /// 渲染物体常量数据
 /// </summary>
struct PerObjectConstants
{
public:
	/// <summary>
	/// 本地到世界空间的变换矩阵
	/// </summary>
	DirectX::XMFLOAT4X4 worldMatrix{ MathUtil::Identity4x4() };
};

/// <summary>
/// 渲染材质常量数据
/// </summary>
struct PerMaterialConstants
{
	/// <summary>
	/// 材质名称
	/// </summary>
	std::string name{};
	/// <summary>
	/// 
	/// </summary>
	DirectX::XMFLOAT4 albedo{};

	DirectX::XMFLOAT3 fresnelR0{};
};

/// <summary>
/// 渲染过程的常量数据结构
/// </summary>
struct PerPassConstancts
{
public:
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
	/// 环境光
	/// </summary>
	DirectX::XMFLOAT4 ambientLight{ 0.f, 0.f, 0.f, 0.f };
	/// <summary>
	/// 所有光源数据
	/// </summary>
	Light lights[MaxLights];
};
