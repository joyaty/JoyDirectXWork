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

	/// <summary>
	/// 获取矩阵M的逆转置矩阵
	/// </summary>
	/// <param name="M"></param>
	/// <returns></returns>
	static DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX M)
	{
		DirectX::XMMATRIX A = M;
		// 将平移项置零，逆转置矩阵用于法线变换，法线是一个向量，平移操作无意义，置零可以避免逆转置矩阵与其他矩阵链接时，发生位移渗入导致错误
		A.r[3] = DirectX::XMVectorSet(0.f, 0.f, 0.f, 1.f);
		// 计算矩阵A的行列式
		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
		// 先计算逆矩阵，再计算转置矩阵
		return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
	}

public:
	template<typename T> static T Clamp(const T& value, const T& min, const T& max)
	{
		return value < min ? min : (value > max ? max : value);
	}

	// Returns random float in [0, 1).
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	// Returns random float in [a, b).
	static float RandF(float a, float b)
	{
		return a + RandF() * (b - a);
	}

	static int Rand(int a, int b)
	{
		return a + rand() % ((b - a) + 1);
	}

	template<typename T> static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T> static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	template<typename T> static T Lerp(const T& a, const T& b, float t)
	{
		return a + (b - a) * t;
	}

public:
	static const float Infinity;
	static const float Pi;
};