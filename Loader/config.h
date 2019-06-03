#pragma once
#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "d3d9.lib")


#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_dx9.h"
#include "ImGUI/imgui_impl_win32.h"
#include <dinput.h>
#include <d3d9.h>

extern LPDIRECT3DDEVICE9		g_pd3dDevice;
extern D3DPRESENT_PARAMETERS    g_d3dpp;

extern HWND  MAIN_WINDOW;