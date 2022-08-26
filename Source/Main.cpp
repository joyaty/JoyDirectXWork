
#include<Windows.h>
#include<tchar.h>


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int main()
{
	WNDCLASSEX wndClsEx = { 0 };
	wndClsEx.cbSize = sizeof(WNDCLASSEX);
	wndClsEx.style = CS_HREDRAW | CS_VREDRAW;
	wndClsEx.lpfnWndProc = WindowProc;
	wndClsEx.hInstance = GetModuleHandle(NULL);
	wndClsEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClsEx.lpszClassName = _T("My DirectX12 Work");
	RegisterClassEx(&wndClsEx);

	RECT wndRect = { 0, 0, 800, 600 };
	AdjustWindowRect(&wndRect, WS_OVERLAPPEDWINDOW, false);

	HWND hwnd = CreateWindow(
		wndClsEx.lpszClassName,
		wndClsEx.lpszClassName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wndRect.right - wndRect.left,
		wndRect.bottom - wndRect.top,
		nullptr,
		nullptr,
		wndClsEx.hInstance,
		nullptr);

	ShowWindow(hwnd, SW_SHOWDEFAULT);

	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return static_cast<char>(msg.wParam);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		// TODO something
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

	return DefWindowProc(hWnd, message, wParam, lParam);
}