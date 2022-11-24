/*
 * 数学工具类
 * MathUtil.h
 */

#pragma once

#include <DirectXMath.h>

/// <summary>
/// 数学工具类，提供一些数学计算的工具方法和单位矩阵的常用常量
/// </summary>
class MathUtil
{
public:
	/// <summary>
	/// 获取4X4的单位矩阵
	/// </summary>
	/// <returns></returns>
	static DirectX::XMFLOAT4X4 Identity4x4()
	{
		static DirectX::XMFLOAT4X4 Identity4X4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		return Identity4X4;
	}

public:
	template<typename T> static T Clamp(const T& value, const T& min, const T& max)
	{
		return value < min ? min : (value > max ? max : value);
	}

public:
	static const float Infinity;
	static const float Pi;
};