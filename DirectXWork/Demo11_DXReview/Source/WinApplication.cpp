
#include "stdafx.h"
#include "WinApplication.h"
#include "DXSampleForReview.h"
#include "IMGuiCtrlForReview.h"
#include "Timer.h"

WinApplication::WinApplication(HINSTANCE hAppInstance, DXSampleForReview* pSampleForReview, IMGuiCtrlForReview* pGuiForReview)
	: m_AppInstance(hAppInstance)
	, m_pSample(pSampleForReview)
	, m_pCtrlGUI(pGuiForReview)
	, m_Width(pSampleForReview->GetWidth())
	, m_Height(pSampleForReview->GetHeight())
{
}

bool WinApplication::Initialize()
{
	WNDCLASSEX wndclsex{};
	ZeroMemory(&wndclsex, sizeof(wndclsex));
	wndclsex.cbSize = sizeof(wndclsex);
	wndclsex.style = CS_HREDRAW | CS_VREDRAW; //指定窗口类的样式，CS_HREDRAW | CS_VREDRAW，这两组标记表示单工作区宽高发生变化时，重绘窗口
	wndclsex.hInstance = m_AppInstance;
	wndclsex.lpfnWndProc = WindowProc; // 窗口过程函数指针，接受Windows消息并处理
	wndclsex.cbClsExtra = 0; // 通过这两个字段指定应用分配额外的内存空间，目前不需要，指定为0
	wndclsex.cbWndExtra = 0;
	wndclsex.hIcon = LoadIcon(0, IDI_APPLICATION);  // 设置窗口图标样式
	wndclsex.hCursor = LoadCursor(NULL, IDC_ARROW); // 设置鼠标样式
	wndclsex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH); // 设置笔刷，指定窗口背景色为灰色
	wndclsex.lpszMenuName = nullptr; // 设置窗口菜单，不需要
	wndclsex.lpszClassName = _T("JoyDX12Sample"); // 指定所创建窗口类结构体名称，可以随意填写，后续可以在需要此窗口类结构体时方便的引用
	// 为WNDCLASSEX注册实例，接下来就可根据这个实例创建窗口，构造WNDCLASS则使用RegisterClass(&wndCls)注册实例
	RegisterClassEx(&wndclsex);
	// 设置窗口尺寸
	RECT wndRect = { 0, 0, static_cast<LONG>(m_Width), static_cast<LONG>(m_Height) };
	AdjustWindowRect(&wndRect, WS_OVERLAPPEDWINDOW, FALSE);
	// 创建窗口
	m_WndInstance = CreateWindow(wndclsex.lpszClassName // 使用前面注册的实例创建窗口
		, m_pSample->GetTitle() // 窗口标题，显示在标题栏中
		, WS_OVERLAPPEDWINDOW // 窗口样式
		, CW_USEDEFAULT // 左上角位置X
		, CW_USEDEFAULT // 左上角位置Y
		, wndRect.right - wndRect.left // 窗口宽度
		, wndRect.bottom - wndRect.top // 窗口高度
		, nullptr // 无父窗口
		, nullptr // 不使用菜单
		, wndclsex.hInstance // 此窗口关联的应用句柄
		, this // 指向用户定义数据的指针，可用作WM_CREATE消息的lpParam参数
	);
	// 初始化DX
	m_pSample->Initialize(m_WndInstance);
	// 初始化IMGui调试面板
	m_pCtrlGUI->InitializeIMGui(m_WndInstance);

	ShowWindow(m_WndInstance, SW_SHOW);
	UpdateWindow(m_WndInstance);

	return true;
}

int WinApplication::Run()
{
	// 重置计时器周期
	m_Timer.Reset();
	// 开启程序主循环
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		// 有系统消息，处理程序消息
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// 无消息，处理程序逻辑
			m_Timer.Tick();
			if (!m_IsAppPause)
			{
				m_pCtrlGUI->DrawIMGUI();
				m_pSample->Update(m_Timer.DeltaTime(), m_Timer.TotalTime());
				m_pSample->Render();
			}
			else
			{
				Sleep(100);
			}
		}
	}
	// 执行退出清理
	m_pCtrlGUI->TerminateIMGui();
	m_pSample->Terminate();
	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WinApplication::WndProc(HWND hWindowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_IsAppPause = true;
			m_Timer.Stop();
		}
		else
		{
			m_IsAppPause = false;
			m_Timer.Start();
		}
		return 0;
	}
	case WM_SIZE:			// Size变化
	{
		m_Width = LOWORD(lParam);
		m_Height = HIWORD(lParam);
		if (wParam == SIZE_MINIMIZED)
		{
			m_IsAppPause = true;
			m_IsAppMinimized = true;
			m_IsAppMaximized = false;
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			m_IsAppPause = false;
			m_IsAppMinimized = false;
			m_IsAppMaximized = true;
			OnResize();
		}
		else if (wParam == SIZE_RESTORED)
		{
			if (m_IsAppMinimized || m_IsAppMaximized)
			{
				m_IsAppPause = false;
				m_IsAppMaximized = false;
				m_IsAppMinimized = false;
				OnResize();
			}
			else if (!m_IsResizing)
			{
				OnResize();
			}
		}
		return 0;
	}
	case WM_ENTERSIZEMOVE:		// 进入Size调整
	{
		m_IsAppPause = true;
		m_IsResizing = true;
		m_Timer.Stop();
		return 0;
	}
	case WM_EXITSIZEMOVE:		// 退出Size调整
	{
		m_IsAppPause = false;
		m_IsResizing = false;
		m_Timer.Start();
		OnResize();
		return 0;
	}
	case WM_GETMINMAXINFO:	
	{
		// 处理此消息，避免当前窗口尺寸过小
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	}
	case WM_KEYDOWN:			// 键盘按键按下消息
	{
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(m_WndInstance);
		}
		else
		{
			OnKeyDown(static_cast<UINT8>(wParam));
		}
		return 0;
	}
	case WM_KEYUP:				// 键盘按键抬起消息
	{
		OnKeyUp(static_cast<UINT8>(wParam));
		return 0;
	}

	case WM_LBUTTONDOWN:		// 鼠标左中右键按下消息
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	{
		OnMouseDown(static_cast<UINT8>(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	case WM_LBUTTONUP:			// 鼠标左中右键抬起消息
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	{
		OnMouseUp(static_cast<UINT8>(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	case WM_MOUSEMOVE:			// 鼠标移动消息
	{
		OnMouseMove(static_cast<UINT8>(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	case WM_DESTROY:			// 退出消息
	{
		PostQuitMessage(0);
		return 0;
	}
	}
	// 其他类型事件交由系统默认窗口过程处理
	return DefWindowProc(hWindowHandle, message, wParam, lParam);
}

void WinApplication::OnResize()
{
	if (m_pSample != nullptr)
	{
		m_pSample->OnResize(m_Width, m_Height);
	}
}

void WinApplication::OnKeyDown(UINT8 keyCode)
{
	if (m_pSample != nullptr)
	{
		m_pSample->OnKeyDown(keyCode);
	}
}

void WinApplication::OnKeyUp(UINT8 keyCode)
{
	if (m_pSample != nullptr)
	{
		m_pSample->OnKeyUp(keyCode);
	}
}

void WinApplication::OnMouseDown(UINT8 keyCode, int x, int y)
{
	if (m_pSample != nullptr)
	{
		m_pSample->OnMouseDown(keyCode, x, y);
	}
	SetCapture(m_WndInstance);
}

void WinApplication::OnMouseUp(UINT8 keyCode, int x, int y)
{
	if (m_pSample != nullptr)
	{
		m_pSample->OnMouseUp(keyCode, x, y);
	}
	ReleaseCapture();
}

void WinApplication::OnMouseMove(UINT8 keyCode, int x, int y)
{
	if (m_pSample != nullptr)
	{
		m_pSample->OnMouseMove(keyCode, x, y);
	}
}

/// <summary>
/// 窗口过程消息处理
/// </summary>
/// <param name="hWnd"></param>
/// <param name="message"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
/// <returns></returns>
LRESULT CALLBACK WinApplication::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		// WM_CREATE的lParam为用户自定义数据，本例为Win32Application实例，设置到GWLP_USERDATA中，备后续使用
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
		return 0;
	}
	}
	// 获取缓存在GWLP_USERDATA中的WinApplication示例，调用实例方法处理窗口消息
	WinApplication* pWinApp = reinterpret_cast<WinApplication*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (pWinApp != nullptr)
	{
		return pWinApp->WndProc(hWnd, message, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

