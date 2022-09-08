/**
 * DearIMGui�Խ���ͷ�ļ�
 */

#pragma once

class DearIMGuiBaseHelper
{
public:
	/// <summary>
	/// ��ʼ��DearIMGui
	/// </summary>
	/// <param name="hwnd">Windows����</param>
	/// <param name="pD3D12Device">DX��ʾ�����豸</param>
	/// <param name="nFrameCount">����������</param>
	/// <param name="rtvFormat">RenderTargetView��ʽ</param>
	/// <param name="pSrvDescHeap">��������</param>
	/// <returns></returns>
	bool InitDearIMGui(HWND hwnd, ID3D12Device* pD3D12Device, int nFrameCount, DXGI_FORMAT rtvFormat, ID3D12DescriptorHeap* pSrvDescHeap);
	
	/// <summary>
	/// ����DearIMGui
	/// </summary>
	void TerminateIMGui();

	/// <summary>
	/// ����DearIMGui����
	/// </summary>
	void DrawDearIMGuiWindow();

	/// <summary>
	/// �ύDearIMGui��Ⱦָ��
	/// </summary>
	/// <param name="pCommandList">DirectX��Ⱦ����</param>
	void PopulateDearIMGuiCommand(ID3D12GraphicsCommandList* pCommandList);

protected:
	/// <summary>
	/// ��������Զ��崰��
	/// </summary>
	virtual void OnDrawWindow() = 0;
};