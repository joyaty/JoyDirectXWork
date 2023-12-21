#pragma once

#include <stdexcept>

inline std::string HrToString(HRESULT hr)
{
	char strs[64] = {};
	sprintf_s(strs, "HRESULT of 0x%08X", static_cast<UINT>(hr));
	return std::string(strs);
}
/// <summary>
/// HRESULT结果异常
/// </summary>
class HrException : public std::runtime_error
{
public:
	HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {}
	HRESULT Error() const { return m_hr; }
private:
	const HRESULT m_hr;
};

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw HrException(hr);
	}
}

#define SAFE_RELEASE(p) if(p) p->Release()

// ==================================================================

/// <summary>
/// 数学工具类
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

// =======================================================================
/// <summary>
/// DX相关的辅助函数
/// </summary>
class D3DHelper
{
public:
	/// <summary>
	/// 在GPU默认堆内存上创建缓冲区，同时返回关联的上传堆中的缓冲区，完成上传堆数据同步到默认堆上，上传堆的内存块可以释放
	/// </summary>
	/// <param name="pDevice">逻辑设备</param>
	/// <param name="pCommandList">指令列表</param>
	/// <param name="pData">写入的数据</param>
	/// <param name="dataSize">数据大小</param>
	/// <param name="pUploaderBuffer">中介的上传堆内存块</param>
	/// <returns></returns>
	static Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferInDefaultHeap(ID3D12Device* pDevice
		, ID3D12GraphicsCommandList* pCommandList
		, const void* pData, uint32_t dataSize
		, Microsoft::WRL::ComPtr<ID3D12Resource>& pUploaderBuffer);

	/// <summary>
	/// 将byteSize大小对齐到256的最小整数倍
	/// 常量缓冲区硬件要求其大小必须为256的整数倍
	/// </summary>
	/// <param name="byteSize"></param>
	/// <returns></returns>
	static uint32_t ConstantBufferByteSizeAligment(uint32_t byteSize)
	{
		return (byteSize + 255) & ~255;
	}
};

// ========================================================================
// Shader编译函数
#ifdef D3D_COMPILE_STANDARD_FILE_INCLUDE
inline Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
	const std::wstring& filename,
	const D3D_SHADER_MACRO* defines,
	const std::string& entrypoint,
	const std::string& target)
{
	UINT compileFlags = 0;
#if defined(_DEBUG) || defined(DBG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr;

	Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errors;
	hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
	{
		OutputDebugStringA((char*)errors->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	return byteCode;
}
#endif

// ========================================================================
// 文件路径工具函数
inline void GetAssetsPath(_Out_writes_(pathSize) WCHAR* path, UINT pathSize)
{
	if (path == nullptr)
	{
		throw std::exception();
	}

	DWORD size = GetModuleFileName(nullptr, path, pathSize);
	if (size == 0 || size == pathSize)
	{
		// Method failed or path was truncated.
		throw std::exception();
	}

	WCHAR* lastSlash = wcsrchr(path, L'\\');
	if (lastSlash)
	{
		*(lastSlash + 1) = L'\0';
	}
}