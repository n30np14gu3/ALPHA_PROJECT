#include "config.h"
#include "strings.h"

static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static D3DPRESENT_PARAMETERS    g_d3dpp;

HWND MAIN_WINDOW = nullptr;

bool dragging = false;
POINT mousePoint;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;


	RECT wndrect;
	POINT point;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_LBUTTONDOWN:
		GetCursorPos(&mousePoint);
		dragging = true;
		SetCapture(MAIN_WINDOW);
		break;

	case WM_LBUTTONUP:
		if (dragging)
		{
			dragging = false;
			ReleaseCapture();
		}
		break;

	case WM_MOUSEMOVE:
		if (dragging)
		{
			GetCursorPos(&point);
			GetWindowRect(MAIN_WINDOW, &wndrect);
			wndrect.left = wndrect.left + (point.x - mousePoint.x);
			wndrect.top = wndrect.top + (point.y - mousePoint.y);
			SetWindowPos(MAIN_WINDOW, NULL, wndrect.left, wndrect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			mousePoint = point;
		}
		break;

	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

INT WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstanve,
	LPSTR lpCmdLine,
	INT nCmdShow
)
{
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0L,
		0L,
		GetModuleHandle(nullptr),
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		WINDOW_CLASS_NAME,
		nullptr
	};

	RegisterClassEx(&wc);

	HDC hDCScreen = GetDC(nullptr);
	int Horres = GetDeviceCaps(hDCScreen, HORZRES);
	int Vertres = GetDeviceCaps(hDCScreen, VERTRES);
	ReleaseDC(nullptr, hDCScreen);

	MAIN_WINDOW = CreateWindow(WINDOW_CLASS_NAME, WINDOW_NAME, WS_POPUP, Horres / 2 - 200, Vertres / 2 - 150, 400, 300, NULL, NULL, wc.hInstance, NULL);

	LONG lStyle = GetWindowLong(MAIN_WINDOW, GWL_STYLE);
	lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
	SetWindowLong(MAIN_WINDOW, GWL_STYLE, lStyle);

	LPDIRECT3D9 pD3D;
	if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
	{
		UnregisterClass(WINDOW_CLASS_NAME, wc.hInstance);
		return 0;
	}
	
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	if (pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, MAIN_WINDOW, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
	{
		pD3D->Release();
		UnregisterClass(WINDOW_CLASS_NAME, wc.hInstance);
		return 0;
	}


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(MAIN_WINDOW);
	ImGui_ImplDX9_Init(g_pd3dDevice);
	DWORD dwFlag = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;


	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	ShowWindow(MAIN_WINDOW, SW_SHOWDEFAULT);
	UpdateWindow(MAIN_WINDOW);

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}


		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();


		bool window_draw = true;


		ImGui::SetNextWindowPos(ImVec2(0, 0));
	
		if (ImGui::Begin("alpha loader", &window_draw, ImVec2(400, 300), 1.0f, dwFlag))
		{
			ImGui::Text("loader");
			ImGui::SameLine(380);
			if (ImGui::Button("X"))
				TerminateProcess(GetCurrentProcess(), 0);
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::SetCursorPosX(70);
			char login[256] = "Login";
			ImGui::InputText("", login, 256);
		}
		ImGui::End();
		ImGui::EndFrame();

		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}


		if (g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr) == D3DERR_DEVICELOST &&
			g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			g_pd3dDevice->Reset(&g_d3dpp);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (g_pd3dDevice)
		g_pd3dDevice->Release();

	if (pD3D)
		pD3D->Release();

	DestroyWindow(MAIN_WINDOW);

	UnregisterClass(WINDOW_CLASS_NAME, wc.hInstance);

	return 0;
};