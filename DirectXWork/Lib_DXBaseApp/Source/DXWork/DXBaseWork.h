/***
 * DX示例项目基类头文件
 */

#pragma once

using namespace Microsoft::WRL;

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

	virtual void OnKeyDown(UINT8 keyCode) {}
	virtual void OnKeyUp(UINT8 keyCode) {}
	virtual void OnMouseDown(UINT8 keyCode, int x, int y) {}
	virtual void OnMouseUp(UINT8 keyCode, int x, int y) {}
	virtual void OnMouseMove(UINT8 keyCode, int x, int y) {}

	UINT GetWidth() const { return m_Width; }
	UINT GetHeight() const { return m_Height; }
	const WCHAR* GetTitle() const { return m_Title.c_str(); }

	ID3D12Device* GetD3D12Device() { return m_Device.Get(); }
	ID3D12DescriptorHeap* GetD3D12DescriptorHeap() { return m_SrvHeap.Get(); }
	virtual UINT GetFrameCount() = 0;

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
	/// 设置窗口标题
	/// </summary>
	/// <param name="text"></param>
	void SetCustomWindowText(LPCWSTR text);

	/// <summary>
	/// 查找第一个支持DrectX12的显示适配器
	/// 如果没有找着可用的显示适配器，则ppAdapter返回nullptr.
	/// </summary>
	/// <param name="pFactory"></param>
	/// <param name="ppAdapter">显示适配器指针</param>
	/// <param name="requestHighPerforrmanceAdapter"></param>
	void GetHardwareAdapter(_In_ IDXGIFactory1* pFactory
		, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter
		, bool requestHighPerforrmanceAdapter = false);

protected:
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

protected:
	ComPtr<ID3D12Device> m_Device;
	ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
	ComPtr<ID3D12DescriptorHeap> m_SrvHeap;

};
