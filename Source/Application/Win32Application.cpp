/**
 * Win32���ڴ����װԴ�ļ�
 */

#include "stdafx.h"
#include "Win32Application.h"
#include <tchar.h>

HWND Win32Application::m_HWND = nullptr;

int Win32Application::Run(HINSTANCE hInstance, int nCmdShow)
{
	// ����һ��WNDCLASSEX�ṹ�壬�������ڵ�һЩ���ԣ��������������Ĵ������Դ�������
	WNDCLASSEX wndClsEx = { 0 };
	wndClsEx.cbSize = sizeof(WNDCLASSEX);
	wndClsEx.style = CS_HREDRAW | CS_VREDRAW;		// ָ�����������ʽ��CS_HREDRAW | CS_VREDRAW���������Ǳ�ʾ����������߷����仯ʱ���ػ洰��
	wndClsEx.lpfnWndProc = WindowProc;				// ���ڹ��̺���ָ�룬����Windows��Ϣ������
	wndClsEx.cbClsExtra = 0;						// ͨ���������ֶ�ָ��Ӧ�÷��������ڴ�ռ䣬Ŀǰ����Ҫ��ָ��Ϊ0
	wndClsEx.cbWndExtra = 0;						// ͨ���������ֶ�ָ��Ӧ�÷��������ڴ�ռ䣬Ŀǰ����Ҫ��ָ��Ϊ0
	wndClsEx.hInstance = hInstance;					// ��ǰӦ��ʵ�������ͨ��WinMain������ڴ���
	wndClsEx.hIcon = LoadIcon(0, IDI_APPLICATION);	// ָ���������ڵ�ͼ����
	wndClsEx.hCursor = LoadCursor(NULL, IDC_ARROW);	// ָ�������ʽ
	wndClsEx.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH); // ָ����ˢ������Դ�ָ�����ڹ�����������ɫ
	wndClsEx.lpszMenuName = nullptr;				// ָ�����ڲ˵�������Ҫ�����ָ��Ϊnull
	wndClsEx.lpszClassName = _T("JoyDirectX12Work");	// ָ��������������ṹ�����ƣ�����������д��������������Ҫ�˴�����ṹ��ʱ���������
	// ΪWNDCLASSEXע��ʵ�����������Ϳɸ������ʵ����������
	RegisterClassEx(&wndClsEx);

	RECT wndRect = { 0, 0, 1280, 720 };
	AdjustWindowRect(&wndRect, WS_OVERLAPPEDWINDOW, FALSE);

	// �������ڲ��ұ��洰��Handle
	m_HWND = CreateWindow(
		wndClsEx.lpszClassName,			// ʹ��ǰ��ע���ʵ����������
		_T("Joy DirectX Work"),			// ���ڱ��⣬��ʾ�ڱ�������
		WS_OVERLAPPEDWINDOW,			// ������ʽ��
		CW_USEDEFAULT,					// ���Ͻ�λ��X
		CW_USEDEFAULT,					// ���Ͻ�λ��Y
		wndRect.right - wndRect.left,	// ���ڿ��
		wndRect.bottom - wndRect.top,	// ���ڸ߶�
		nullptr,						// �޸�����
		nullptr,						// ��ʹ�ò˵�
		wndClsEx.hInstance,				// �˴��ڹ�����Ӧ�þ��
		nullptr);						// ָ���û��������ݵ�ָ�룬������WM_CREATE��Ϣ��lpParam����	

	// ��ʾ����
	ShowWindow(m_HWND, nCmdShow);
	UpdateWindow(m_HWND);
	// ������ѭ��
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		// ������Ϣ����
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// TODO ִ�г����߼�
	}
	// �����˳���Ϣ
	return static_cast<char>(msg.wParam);
}

LRESULT CALLBACK Win32Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		// TODO something
		return 0;
	case WM_SIZE:
		// TODO something
		return 0;
	case WM_KEYDOWN:
		// ����Esc��ť���رճ��򴰿�
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hWnd);
		}
		return 0;
	case WM_KEYUP:
		// TODO something
		return 0;
	case WM_PAINT:
		// TODO something
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	// ���������¼�����ϵͳĬ�ϴ��ڹ��̴���
	return DefWindowProc(hWnd, message, wParam, lParam);
}