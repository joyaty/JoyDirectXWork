/***
 * DXʾ����Ŀ����ͷ�ļ�
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
	/// ���������в���
	/// </summary>
	/// <param name="argv">�����в�������</param>
	/// <param name="argc">�����в�������</param>
	void ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc);

protected:
	/// <summary>
	/// ��ȡ��Դȫ·��
	/// </summary>
	/// <param name="assetName">��Դ��</param>
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
	// �ӿڲ���
	UINT m_Width;
	UINT m_Height;
	float m_AspectRatio;
	// ���ڱ���
	std::wstring m_Title;
	// ��Դ·��
	std::wstring m_AssetsPath;
	// Adapter info.
	bool m_UseWarpDevice;
};
