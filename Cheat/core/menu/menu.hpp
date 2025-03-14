#pragma once
#include "../../dependencies/common_includes.hpp"
#include "../../SDK/static/fonts/DINPro.h"
#include "../../SDK/static/fonts/BeauSans.h"

extern IDirect3DStateBlock9 *state_block;

enum esp_info_position {
	RIGHT,
	CENTER_DOWN,
	CENTER_UP
};

struct esp_info_s {
	esp_info_s(const std::string name, const color colors, const int position = RIGHT) {
		f_name = name; f_color = colors; f_position = position;
	}

	std::string f_name;
	int f_position;
	color f_color;
};

class c_menu {
public:
	void run_popup();
	void run();
	void run_visuals_preview();

	void __stdcall create_objects(IDirect3DDevice9* device) {

		if (hooks::window)
			ImGui_ImplDX9_CreateDeviceObjects();
	}

	void __stdcall invalidate_objects() {
		ImGui_ImplDX9_InvalidateDeviceObjects();
	}

	void __stdcall setup_resent(IDirect3DDevice9* device) {
		if(!ImGui_ImplWin32_Init(hooks::window))
			return;

		if(!ImGui_ImplDX9_Init(device))
			return;

		ImGuiStyle& style = ImGui::GetStyle();
		style.Alpha = 1.0f;
		style.WindowPadding = ImVec2(12, 12);
		style.WindowMinSize = ImVec2(32, 32);
		style.WindowRounding = 0.0f;
		style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
		style.FramePadding = ImVec2(4, 4);
		style.FrameRounding = 0.0f;
		style.ItemSpacing = ImVec2(8, 8);
		style.ItemInnerSpacing = ImVec2(8, 8);
		style.TouchExtraPadding = ImVec2(0, 0);
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 0.0f;
		style.ScrollbarSize = 6.0f;
		style.ScrollbarRounding = 0.0f;
		style.GrabMinSize = 5.0f;
		style.GrabRounding = 0.0f;
		style.ButtonTextAlign = ImVec2(0.0f, 0.5f);
		style.DisplayWindowPadding = ImVec2(22, 22);
		style.DisplaySafeAreaPadding = ImVec2(4, 4);
		style.AntiAliasedLines = true;
		style.TabRounding = 0;
		
		style.CurveTessellationTol = 1.f;

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);
		colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.08f, 0.09f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.14f, 0.18f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.13f, 0.17f, 0.22f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.17f, 0.22f, 0.29f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.08f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.75f, 0.09f, 0.38f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.75f, 0.09f, 0.38f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.75f, 0.09f, 0.38f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.84f, 0.09f, 0.42f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.10f, 0.22f, 0.31f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.10f, 0.25f, 0.35f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.11f, 0.30f, 0.41f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.10f, 0.22f, 0.31f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.10f, 0.25f, 0.35f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.11f, 0.30f, 0.41f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.75f, 0.09f, 0.38f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.84f, 0.09f, 0.42f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.10f, 0.22f, 0.31f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.10f, 0.25f, 0.35f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.11f, 0.30f, 0.41f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		create_objects(device);
	}

	void apply_fonts() {
		ImGui::CreateContext();

		static const ImWchar ranges[] =
		{
			0x0020, 0x00FF, // Basic Latin + Latin Supplement
			0x0400, 0x044F, // Cyrillic
			0,
		};

		ImFontConfig font_config;
		font_config.OversampleH = 1; //or 2 is the same
		font_config.OversampleV = 1;
		font_config.PixelSnapH = 1;

		font_main = ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)BeauSans, sizeof(BeauSans), 12, &font_config, ranges);
		font_menu = ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)DINPro, sizeof(DINPro), 18, &font_config, ranges);
	}

	void __stdcall end_present(IDirect3DDevice9* device) {
		if (opened) {
			if (ImGui::GetStyle().Alpha > 1.f)
				ImGui::GetStyle().Alpha = 1.f;
			else if (ImGui::GetStyle().Alpha != 1.f)
				ImGui::GetStyle().Alpha += 0.05f;
		}

		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		state_block->Apply();
		state_block->Release();
	}

	void __stdcall pre_render(IDirect3DDevice9* device) {
		D3DVIEWPORT9 d3d_viewport;
		device->GetViewport(&d3d_viewport);

		device->CreateStateBlock(D3DSBT_ALL, &state_block);
		state_block->Capture();

		device->SetVertexShader(nullptr);
		device->SetPixelShader(nullptr);
		device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

		device->SetRenderState(D3DRS_LIGHTING, FALSE);
		device->SetRenderState(D3DRS_FOGENABLE, FALSE);
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
		device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		device->SetRenderState(D3DRS_STENCILENABLE, FALSE);

		device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
		device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

		device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
		device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
	}

	void __stdcall post_render() {
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	ImFont* font_main;
	ImFont* font_menu;

	ImFont* font_main_caps;
	bool opened = false;
private:
	ImDrawData _drawData;
	DWORD dwOld_D3DRS_COLORWRITEENABLE;
};

extern c_menu menu;