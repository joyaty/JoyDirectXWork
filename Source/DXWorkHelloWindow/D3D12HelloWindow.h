/***
 * DirectX绘制窗口颜色
 */

#pragma once

#include "DXWork/DXBaseWork.h"

using Microsoft::WRL::ComPtr;

class D3D12HelloWindow : public DXBaseWork
{
public:
	D3D12HelloWindow(UINT width, UINT height, std::wstring name);

	virtual void OnInit() override;
	virtual void OnUpdate() override;
	virtual void OnRender() override;
	virtual void OnDestroy() override;

private:
	// 双缓冲，这里指定渲染目标缓冲区数量指定为2
	static const UINT kFrameCount = 2;

	// Pipeline Objects.
	ComPtr<IDXGISwapChain3> m_SwapChain;
	ComPtr<ID3D12Device> m_Device;
	ComPtr<ID3D12Resource> m_RenderTargets[kFrameCount];
	ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
	ComPtr<ID3D12CommandQueue> m_CommandQueue;
	ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
	ComPtr<ID3D12PipelineState> m_PipelineState;
	ComPtr<ID3D12GraphicsCommandList> m_CommandList;
	UINT m_RtvDescriptorSize;

	// Synchronization Objects.
	UINT m_FrameIndex;
	HANDLE m_FenceEvent;
	ComPtr<ID3D12Fence> m_Fence;
	UINT64 m_FenceValue;

	void LoadPipeline();
	void LoadAsset();
	void PopulateCommandList();
	void WaitForPreviousFrame();
};