/***
 * DX示例项目基类头文件
 */

#pragma once

class DXBaseWork
{
public:
	DXBaseWork(UINT width, UINT height, std::wstring name);
	virtual ~DXBaseWork();

public:
	virtual void OnInit() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnDestroy() = 0;

	virtual void OnKeyDown(UINT8 key) {}
	virtual void OnKeyUp(UINT8 key) {}

	UINT GetWidth() const { return m_Width; }
	UINT GetHeight() const { return m_Height; }
	const WCHAR* GetTitle() const { return m_Title.c_str(); }

	/// <summary>
	/// 解析命令行参数
	/// </summary>
	/// <param name="argv">命令行参数集合</param>
	/// <param name="argc">命令行参数数量</param>
	void ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc);

protected:
	/// <summary>
	/// 获取资源全路径
	/// </summary>
	/// <param name="assetName">资源名</param>
	/// <returns></returns>
	std::wstring GetAssetFullPath(LPCWSTR assetName);

	/// <summary>
	/// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
	/// If no such adapter can be found, *ppAdapter will be set to nullptr.
	/// </summary>
	/// <param name="pFactory"></param>
	/// <param name="ppAdapter"></param>
	/// <param name="requestHighPerforrmanceAdapter"></param>
	void GetHardwareAdapter(_In_ IDXGIFactory1* pFactory
		, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter
		, bool requestHighPerforrmanceAdapter = false);

private:
	// 视口参数
	UINT m_Width;
	UINT m_Height;
	float m_AspectRatio;
	// 窗口标题
	std::wstring m_Title;
	// 资源路径
	std::wstring m_AssetsPath;
	// Adapter info.
	bool m_UseWarpDevice;
};
