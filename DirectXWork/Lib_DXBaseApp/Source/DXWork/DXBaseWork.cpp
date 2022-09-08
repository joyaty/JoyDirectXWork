﻿/***
 * DX示例项目基类源文件
 */

#include "stdafx.h"
#include "DXBaseWork.h"
#include "DXWorkHelper.h"
#include "Application/Win32Application.h"

using namespace Microsoft::WRL;

DXBaseWork::DXBaseWork(UINT width, UINT height, std::wstring name)
	: m_Width(width)
	, m_Height(height)
	, m_Title(name)
	, m_UseWarpDevice(false)
{
	m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

DXBaseWork::~DXBaseWork()
{
}

std::wstring DXBaseWork::GetAssetFullPath(LPCWSTR assetName)
{
	return m_AssetsPath + assetName;
}


_Use_decl_annotations_
void DXBaseWork::GetHardwareAdapter(IDXGIFactory1* pFactory
	, IDXGIAdapter1** ppAdapter
	, bool requestHighPerformanceAdapter)
{
	*ppAdapter = nullptr;
	ComPtr<IDXGIAdapter1> adapter;
	ComPtr<IDXGIFactory6> factory6;

	if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(factory6.GetAddressOf()))))
	{
		for (UINT adapterIndex = 0;
			SUCCEEDED(factory6->EnumAdapterByGpuPreference(
				adapterIndex,
				requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
				IID_PPV_ARGS(adapter.GetAddressOf())));
			++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	if (adapter.Get() == nullptr)
	{
		for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	*ppAdapter = adapter.Detach();
}

void DXBaseWork::SetCustomWindowText(LPCWSTR text)
{
	std::wstring windowText = m_Title + L": " + text;
	SetWindowText(Win32Application::GetHwnd(), windowText.c_str());
}

_Use_decl_annotations_
void DXBaseWork::ParseCommandLineArgs(WCHAR* argv[], int argc)
{
	for (int i = 1; i < argc; ++i)
	{
		if (_wcsnicmp(argv[i], L"-warp", wcslen(argv[i])) == 0
			|| _wcsnicmp(argv[i], L"/warp", wcslen(argv[i])) == 0)
		{
			m_UseWarpDevice = true;
			m_Title += L" (WARP)";
		}
	}
}