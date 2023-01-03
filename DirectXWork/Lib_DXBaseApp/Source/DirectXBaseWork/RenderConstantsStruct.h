/**
 * RenderConstantsStruct.h
 * 常量数据结构定义
 **/

#pragma once

#include "MathUtil.h"

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
};
