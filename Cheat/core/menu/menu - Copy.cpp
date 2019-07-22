#define IMGUI_DEFINE_MATH_OPERATORS
#include "menu.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "../features/misc/logs.hpp"
#include "../features/skinchanger/parser.hpp"
#include "../../SDK/crypto/XorStr.h"

#include "../../SDK/static/modules_ids.h"
#include "../../SDK/globals/globals.h"
#include "../../SDK/license/license_manager.h"

c_menu menu;


std::string utf8_encode(const std::wstring &wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

#define UNLEN 256
IDirect3DStateBlock9 *state_block;
bool reverse = false;
int offset = 0;
bool show_popup = false;
bool save_config = false;
bool load_config = false;
namespace ImGui {
	const char* const KeyNames[] = {
	"Unknown",
	"VK_LBUTTON",
	"VK_RBUTTON",
	"VK_CANCEL",
	"VK_MBUTTON",
	"VK_XBUTTON1",
	"VK_XBUTTON2",
	"Unknown",
	"VK_BACK",
	"VK_TAB",
	"Unknown",
	"Unknown",
	"VK_CLEAR",
	"VK_RETURN",
	"Unknown",
	"Unknown",
	"VK_SHIFT",
	"VK_CONTROL",
	"VK_MENU",
	"VK_PAUSE",
	"VK_CAPITAL",
	"VK_KANA",
	"Unknown",
	"VK_JUNJA",
	"VK_FINAL",
	"VK_KANJI",
	"Unknown",
	"VK_ESCAPE",
	"VK_CONVERT",
	"VK_NONCONVERT",
	"VK_ACCEPT",
	"VK_MODECHANGE",
	"VK_SPACE",
	"VK_PRIOR",
	"VK_NEXT",
	"VK_END",
	"VK_HOME",
	"VK_LEFT",
	"VK_UP",
	"VK_RIGHT",
	"VK_DOWN",
	"VK_SELECT",
	"VK_PRINT",
	"VK_EXECUTE",
	"VK_SNAPSHOT",
	"VK_INSERT",
	"VK_DELETE",
	"VK_HELP",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"VK_LWIN",
	"VK_RWIN",
	"VK_APPS",
	"Unknown",
	"VK_SLEEP",
	"VK_NUMPAD0",
	"VK_NUMPAD1",
	"VK_NUMPAD2",
	"VK_NUMPAD3",
	"VK_NUMPAD4",
	"VK_NUMPAD5",
	"VK_NUMPAD6",
	"VK_NUMPAD7",
	"VK_NUMPAD8",
	"VK_NUMPAD9",
	"VK_MULTIPLY",
	"VK_ADD",
	"VK_SEPARATOR",
	"VK_SUBTRACT",
	"VK_DECIMAL",
	"VK_DIVIDE",
	"VK_F1",
	"VK_F2",
	"VK_F3",
	"VK_F4",
	"VK_F5",
	"VK_F6",
	"VK_F7",
	"VK_F8",
	"VK_F9",
	"VK_F10",
	"VK_F11",
	"VK_F12",
	"VK_F13",
	"VK_F14",
	"VK_F15",
	"VK_F16",
	"VK_F17",
	"VK_F18",
	"VK_F19",
	"VK_F20",
	"VK_F21",
	"VK_F22",
	"VK_F23",
	"VK_F24",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_NUMLOCK",
	"VK_SCROLL",
	"VK_OEM_NEC_EQUAL",
	"VK_OEM_FJ_MASSHOU",
	"VK_OEM_FJ_TOUROKU",
	"VK_OEM_FJ_LOYA",
	"VK_OEM_FJ_ROYA",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_LSHIFT",
	"VK_RSHIFT",
	"VK_LCONTROL",
	"VK_RCONTROL",
	"VK_LMENU",
	"VK_RMENU"
	};

#define VK_LBUTTON        0x01
#define VK_RBUTTON        0x02
#define VK_MBUTTON        0x04    /* NOT contiguous with L & RBUTTON */
#define VK_XBUTTON1       0x05    /* NOT contiguous with L & RBUTTON */
#define VK_XBUTTON2       0x06    /* NOT contiguous with L & RBUTTON */
#define VK_BACK           0x08
#define VK_RMENU          0xA5


	bool Hotkey(const char* label, int* k, const ImVec2& size_arg)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		ImGuiIO& io = g.IO;
		const ImGuiStyle& style = g.Style;


		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		ImVec2 size = CalcItemSize(size_arg, CalcItemWidth(), label_size.y + style.FramePadding.y * 2.0f);
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + size);
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? (style.ItemInnerSpacing.x + label_size.x) : 0.0f, 0.0f));

		ItemSize(total_bb, style.FramePadding.y);
		if (!ItemAdd(total_bb, id))
			return false;

		const bool focus_requested = FocusableItemRegister(window, id);

		const bool hovered = IsItemHovered();

		if (hovered) {
			SetHoveredID(id);
			g.MouseCursor = ImGuiMouseCursor_TextInput;
		}

		const bool user_clicked = hovered && io.MouseClicked[0];

		if (focus_requested || user_clicked) {
			if (g.ActiveId != id) {
				// Start edition
				memset(io.MouseDown, 0, sizeof(io.MouseDown));
				memset(io.KeysDown, 0, sizeof(io.KeysDown));
				*k = 0;
			}
			SetActiveID(id, window);
			FocusWindow(window);
		}
		else if (io.MouseClicked[0]) {
			// Release focus when we click outside
			if (g.ActiveId == id)
				ClearActiveID();
		}

		bool value_changed = false;
		int key = *k;

		if (g.ActiveId == id) {
			for (auto i = 0; i < 5; i++) {
				if (io.MouseDown[i]) {
					switch (i) {
					case 0:
						key = VK_LBUTTON;
						break;
					case 1:
						key = VK_RBUTTON;
						break;
					case 2:
						key = VK_MBUTTON;
						break;
					case 3:
						key = VK_XBUTTON1;
						break;
					case 4:
						key = VK_XBUTTON2;
						break;
					}
					value_changed = true;
					ClearActiveID();
				}
			}
			if (!value_changed) {
				for (auto i = VK_BACK; i <= VK_RMENU; i++) {
					if (io.KeysDown[i]) {
						key = i;
						value_changed = true;
						ClearActiveID();
					}
				}
			}

			if (IsKeyPressedMap(ImGuiKey_Escape)) {
				*k = 0;
				ClearActiveID();
			}
			else {
				*k = key;
			}
		}

		// Render
		// Select which buffer we are going to display. When ImGuiInputTextFlags_NoLiveEdit is set 'buf' might still be the old value. We set buf to NULL to prevent accidental usage from now on.

		char buf_display[64] = "None";

		RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

		if (*k != 0 && g.ActiveId != id) {
			strcpy(buf_display, KeyNames[*k]);
		}
		else if (g.ActiveId == id) {
			strcpy(buf_display, "<Press a key>");
		}

		const ImRect clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + size.x, frame_bb.Min.y + size.y); // Not using frame_bb.Max because we have adjusted size
		ImVec2 render_pos = frame_bb.Min + style.FramePadding;
		RenderTextClipped(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding, buf_display, NULL, NULL);
		if (label_size.x > 0)
			RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

		return value_changed;
	}

	long get_mils() {
		auto duration = std::chrono::system_clock::now().time_since_epoch();
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	}

	void begin_popup(const char* text, int on_screen_mils, bool* done) {
		if (!done)
			show_popup = true;

		ImGuiIO &io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		int width = io.DisplaySize.x;
		static long old_time = -1;
		ImGui::SetNextWindowPos(ImVec2(width - offset, 100));
		style.WindowMinSize = ImVec2(100.f, 20.f);
		ImGui::Begin(XorStr("##Toast"), &show_popup, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImVec2 p = ImGui::GetCursorScreenPos();

		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x, p.y + 32), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + +30), ImColor(25, 25, 80, 255), ImColor(25, 25, 80, 255), ImColor(25, 25, 80, 255), ImColor(26, 35, 46, 255));

		long current_time_ms = get_mils();

		ImVec2 text_size = ImGui::CalcTextSize(text);
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2 - text_size.y / 2);
		ImGui::Text(text);

		if (!reverse) {
			if (offset < ImGui::GetWindowWidth())
				offset += (ImGui::GetWindowWidth() + 5) * ((1000.0f / ImGui::GetIO().Framerate) / 100);

			if (offset >= ImGui::GetWindowWidth() && old_time == -1) {
				old_time = current_time_ms;
			}
		}

		if (current_time_ms - old_time >= on_screen_mils && old_time != -1)
			reverse = true;

		if (reverse) {
			if (offset > 0)
				offset -= (ImGui::GetWindowWidth() + 5) * ((1000.0f / ImGui::GetIO().Framerate) / 100);
			if (offset <= 0) {
				offset = 0;
				reverse = false;
				*done = true;
				old_time = -1;
				show_popup = false;
			}
		}

		ImGui::End();
	}


}

void c_menu::run() 
{
	static bool aim_enabled = checkModuleActive(globals::user_modules, MODULE_AIM_BOT);
	static bool trigger_enabled = checkModuleActive(globals::user_modules, MODULE_TRIGGER_BOT) & false;
	static bool bhop_enabled = checkModuleActive(globals::user_modules, MODULE_BUNNY_HOP) & false;
	static bool wallhack_enabled = checkModuleActive(globals::user_modules, MODULE_WALLHACK) & false;
	static bool changer_enabled = checkModuleActive(globals::user_modules, MODULE_SKIN_CHANGER) & false;

	if(opened)
	{
		ImGui::SetNextWindowSize(ImVec2(900, 700), ImGuiSetCond_FirstUseEver);
		ImGui::PushFont(font_menu);
		ImGui::Begin(XorStr("ALPHA CHEAT [beta] SP1"), &opened, ImGuiWindowFlags_NoCollapse);
		{
			if(ImGui::BeginTabBar(XorStr("##main_tabs"), ImGuiTabBarFlags_None))
			{
				if(aim_enabled || trigger_enabled)
				{
					if (ImGui::BeginTabItem(XorStr("AIM BOT")))
					{
						ImGui::BeginChild(XorStr("##aimbot_window"));
						{
							if (ImGui::BeginTabBar(XorStr("##aim_bot_tabs"), ImGuiTabBarFlags_None))
							{
								if(aim_enabled)
								{
									if (ImGui::BeginTabItem(XorStr("LEGIT BOT")))
									{
										if (ImGui::BeginTabBar(XorStr("##legit_bot_tabs"), ImGuiTabBarFlags_None))
										{
											if (ImGui::BeginTabItem(XorStr("GLOBAL")))
											{
												ImGui::PushFont(font_main);
												ImGui::Checkbox(XorStr("dynamic fov"), &config_system.get_config().legit.distance_based_fov);
												ImGui::Checkbox(XorStr("aim in scope only"), &config_system.get_config().legit.scope_aim);
												ImGui::Checkbox(XorStr("smoke check"), &config_system.get_config().legit.smoke_check);
												ImGui::Checkbox(XorStr("friendly fire"), &config_system.get_config().legit.team_check);
												ImGui::Checkbox(XorStr("auto pistol"), &config_system.get_config().legit.auto_pistol);
												ImGui::SliderInt(XorStr("kill delay"), &config_system.get_config().legit.kill_delay, 0, 350);
												ImGui::PopFont();
												ImGui::EndTabItem();
											}

											if (ImGui::BeginTabItem(XorStr("LOCAL")))
											{
												ImGui::PushFont(font_main);
												ImGui::Text(XorStr("settings for->%s"), config_system.current_weapon_name.c_str());
												ImGui::Checkbox(XorStr("legit enable"), &config_system.current_weapon->legit_settings.enable);
												if (config_system.current_weapon->legit_settings.enable)
												{
													ImGui::Checkbox(XorStr("silent"), &config_system.current_weapon->legit_settings.silent);
													ImGui::Checkbox(XorStr("nearest"), &config_system.current_weapon->legit_settings.nearest);

													if (!config_system.current_weapon->legit_settings.nearest)
													{
														ImGui::Combo(XorStr("hitbox"), &config_system.current_weapon->legit_settings.hitbox, XorStr("head\0neck\0chest\0stomach\0pelvis\0"));
													}
													ImGui::SliderFloat(XorStr("fov"), &config_system.current_weapon->legit_settings.fov, 0.0f, 10.0f, XorStr("%.3f"));
													ImGui::SliderFloat(XorStr("smooth"), &config_system.current_weapon->legit_settings.smooth, 1.f, 10.f, XorStr("%.2f"));
													ImGui::SliderFloat(XorStr("rcs x"), &config_system.current_weapon->legit_settings.rcs_x, 0.0f, 1.0f, XorStr("%.2f"));
													ImGui::SliderFloat(XorStr("rcs y"), &config_system.current_weapon->legit_settings.rcs_y, 0.0f, 1.0f, XorStr("%.2f"));
												}
												ImGui::PopFont();
												ImGui::EndTabItem();
											}

											ImGui::EndTabBar();
										}
										ImGui::EndTabItem();
									}

									if (ImGui::BeginTabItem(XorStr("BACKTRACK")))
									{
										ImGui::BeginChild(XorStr("##backtrack_settings"));
										{
											ImGui::PushFont(font_main);
											ImGui::Checkbox(XorStr("backtrack enable"), &config_system.get_config().legit.backtrack.enable);
											if (config_system.get_config().legit.backtrack.enable)
											{
												ImGui::SliderFloat(XorStr("records (ms)"), &config_system.get_config().legit.backtrack.backtrack_ms, 1.0f, 200.0f, "%.2f");
											}
											ImGui::PopFont();
											ImGui::EndChild();
										}
										ImGui::EndTabItem();
									}

									if (ImGui::BeginTabItem(XorStr("RCS STANDALONE")))
									{
										ImGui::Checkbox(XorStr("rcs enable"), &config_system.get_config().legit.rcs_standalone.enabled);
										if (config_system.get_config().legit.rcs_standalone.enabled)
										{
											ImGui::SliderFloat(XorStr("rcs amount x"), &config_system.get_config().legit.rcs_standalone.rcs_x, 0.0f, 1.0f, "%.2f");
											ImGui::SliderFloat(XorStr("rcs amount y"), &config_system.get_config().legit.rcs_standalone.rcs_y, 0.0f, 1.0f, "%.2f");
										}
										ImGui::EndTabItem();
									}

								}

								if(trigger_enabled)
								{
									if (ImGui::BeginTabItem(XorStr("TRIGGER BOT")))
									{
										ImGui::PushFont(font_main);
										ImGui::Text("settings for->%s", config_system.current_weapon_name.c_str());
										ImGui::Checkbox(XorStr("trigger enable"), &config_system.current_weapon->trigger_settings.enable);
										if (config_system.current_weapon->trigger_settings.enable)
										{
											ImGui::Checkbox("on key", &config_system.current_weapon->trigger_settings.on_key);
											if (config_system.current_weapon->trigger_settings.on_key)
											{
												ImGui::Hotkey(XorStr("##triger key"), &config_system.current_weapon->trigger_settings.key_id, ImVec2(150, 20));
											}
											if (ImGui::BeginCombo(XorStr("hitboxes"), "...")) {
												ImGui::Selectable(XorStr("head"), &config_system.current_weapon->trigger_settings.hitbox_head, ImGuiSelectableFlags_DontClosePopups);
												ImGui::Selectable(XorStr("body"), &config_system.current_weapon->trigger_settings.hitbox_body, ImGuiSelectableFlags_DontClosePopups);
												ImGui::Selectable(XorStr("arms"), &config_system.current_weapon->trigger_settings.hitbox_arms, ImGuiSelectableFlags_DontClosePopups);
												ImGui::Selectable(XorStr("legs"), &config_system.current_weapon->trigger_settings.hitbox_legs, ImGuiSelectableFlags_DontClosePopups);
												ImGui::EndCombo();
											}
											ImGui::SliderInt(XorStr("trigger delay"), &config_system.current_weapon->trigger_settings.delay, 1, 50);
											ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
											ImGui::Checkbox(XorStr("trigger recoil"), &config_system.current_weapon->trigger_settings.rcs);
											ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
										}
										ImGui::PopFont();
										ImGui::EndTabItem();
									}
								}

								ImGui::EndTabBar();
							}

							ImGui::EndChild();
						}
						ImGui::EndTabItem();
					}
				}

				if(wallhack_enabled || bhop_enabled)
				{
					if(wallhack_enabled)
					{
						if (ImGui::BeginTabItem(XorStr("VISUALS")))
						{
							ImGui::BeginChild(XorStr("##visuals_window"));
							{
								if (ImGui::BeginTabBar(XorStr("##visuals_tabs"), ImGuiTabBarFlags_None))
								{
									if (ImGui::BeginTabItem(XorStr("ENTITY ESP")))
									{
										ImGui::PushFont(font_main);
										ImGui::Checkbox(XorStr("visuals global active"), &config_system.get_config().visuals.global_enabled);
										if (config_system.get_config().visuals.global_enabled) 
										{
											ImGui::Checkbox(XorStr("teammate"), &config_system.get_config().visuals.show_team);
											ImGui::Checkbox(XorStr("visible only"), &config_system.get_config().visuals.visible_only);
										}

										ImGui::Checkbox(XorStr("draw name"), &config_system.get_config().visuals.player.name);
										if (config_system.get_config().visuals.player.name)
										{
											ImGui::ColorEdit4(XorStr("name color"), reinterpret_cast<float*>(&config_system.get_config().colors.esp_name), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
											ImGui::Spacing();
										}

										ImGui::Checkbox(XorStr("draw box"), &config_system.get_config().visuals.player.box);
										if(config_system.get_config().visuals.player.box)
										{
											ImGui::ColorEdit4(XorStr("box color"), reinterpret_cast<float*>(&config_system.get_config().colors.esp_box), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
											ImGui::Spacing();
										}

										ImGui::Checkbox(XorStr("draw health"), &config_system.get_config().visuals.player.health);

										ImGui::Checkbox(XorStr("draw weapon name"), &config_system.get_config().visuals.player.weapon_name);
										if(config_system.get_config().visuals.player.weapon_name)
										{
											ImGui::ColorEdit4(XorStr("weapon color"), reinterpret_cast<float*>(&config_system.get_config().colors.weapons), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
											ImGui::Spacing();
										}
										
										ImGui::Checkbox(XorStr("draw weapon icon"), &config_system.get_config().visuals.player.weapon_icon);
										if(config_system.get_config().visuals.player.weapon_icon)
										{
											ImGui::ColorEdit4(XorStr("weapon icon color"), reinterpret_cast<float*>(&config_system.get_config().colors.esp_weapon_icon), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
											ImGui::Spacing();
										}

										if (ImGui::BeginCombo(XorStr("player flags"), "..."))
										{
											ImGui::Selectable(XorStr("bombcarrier"), &config_system.get_config().visuals.player.flags.bombcarrier, ImGuiSelectableFlags_DontClosePopups);
											ImGui::Selectable(XorStr("is flashed"), &config_system.get_config().visuals.player.flags.is_flashed, ImGuiSelectableFlags_DontClosePopups);
											ImGui::Selectable(XorStr("is scoped"), &config_system.get_config().visuals.player.flags.is_scoped, ImGuiSelectableFlags_DontClosePopups);
											ImGui::Selectable(XorStr("is defusing"), &config_system.get_config().visuals.player.flags.is_defusing, ImGuiSelectableFlags_DontClosePopups);
											ImGui::Selectable(XorStr("position callout"), &config_system.get_config().visuals.player.flags.position_callout, ImGuiSelectableFlags_DontClosePopups);
											ImGui::Selectable(XorStr("defusekit"), &config_system.get_config().visuals.player.flags.defuse_kit, ImGuiSelectableFlags_DontClosePopups);
											ImGui::Selectable(XorStr("armor"), &config_system.get_config().visuals.player.flags.armor, ImGuiSelectableFlags_DontClosePopups);
											ImGui::Selectable(XorStr("money $$"), &config_system.get_config().visuals.player.flags.money, ImGuiSelectableFlags_DontClosePopups);

											ImGui::EndCombo();
										}

										ImGui::Checkbox(XorStr("draw footstep"), &config_system.get_config().visuals.player.footsteps);
										if(config_system.get_config().visuals.player.footsteps)
										{
											ImGui::ColorEdit4(XorStr("footstep color"), reinterpret_cast<float*>(&config_system.get_config().colors.esp_footsteps), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
											ImGui::Spacing();
										}

										ImGui::Checkbox(XorStr("draw skeleton"), &config_system.get_config().visuals.player.skeleton);
										ImGui::PopFont();
										ImGui::EndTabItem();
									}

									if(ImGui::BeginTabItem(XorStr("LCAL PLAYER EFFECTS")))
									{
										ImGui::PushFont(font_main);
										ImGui::SliderInt(XorStr("field of view"), &config_system.get_config().visuals.player.fov, 0, 60);
										ImGui::Checkbox(XorStr("force crosshair"), &config_system.get_config().visuals.player.force_crosshair);
										ImGui::PopFont();
										ImGui::EndTabItem();
									}

									if (ImGui::BeginTabItem(XorStr("WORLD")))
									{
										ImGui::PushFont(font_main);
										ImGui::Checkbox(XorStr("ambient light"), &config_system.get_config().visuals.ambient_light);
										if(config_system.get_config().visuals.ambient_light)
										{
											ImGui::ColorEdit4(XorStr("ambient light color"), reinterpret_cast<float*>(&config_system.get_config().colors.ambient), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
											ImGui::Spacing();
										}
										
										ImGui::Checkbox(XorStr("night mode"), &config_system.get_config().visuals.night_mode);
										if (config_system.get_config().visuals.night_mode) {
											ImGui::SliderInt(XorStr("brightness"), &config_system.get_config().visuals.brightness, 0, 100);
											ImGui::Spacing();
										}

										ImGui::ColorEdit4(XorStr("sky color"), reinterpret_cast<float*>(&config_system.get_config().colors.sky), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

										if (ImGui::BeginCombo(XorStr("world & player removals"), "..."))
										{
											ImGui::Selectable(XorStr("fog"), &config_system.get_config().visuals.removals.fog, ImGuiSelectableFlags_DontClosePopups);
											ImGui::Selectable(XorStr("smoke"), &config_system.get_config().visuals.removals.smoke, ImGuiSelectableFlags_DontClosePopups);
											ImGui::Selectable(XorStr("flash"), &config_system.get_config().visuals.removals.flash, ImGuiSelectableFlags_DontClosePopups);
											ImGui::Selectable(XorStr("sleeves"), &config_system.get_config().visuals.removals.sleeves, ImGuiSelectableFlags_DontClosePopups);
											ImGui::Selectable(XorStr("hands"), &config_system.get_config().visuals.removals.hands, ImGuiSelectableFlags_DontClosePopups);
											ImGui::Selectable(XorStr("scope overlay"), &config_system.get_config().visuals.removals.skope_overlay, ImGuiSelectableFlags_DontClosePopups);
											ImGui::EndCombo();
										}

										if (ImGui::BeginCombo(XorStr("add to world"), "..."))
										{
											ImGui::Selectable(XorStr("planted bomb"), &config_system.get_config().visuals.world.planted_bomb, ImGuiSelectableFlags_DontClosePopups);
											ImGui::Selectable(XorStr("dropped weapons name"), &config_system.get_config().visuals.world.dropped_weapons_name, ImGuiSelectableFlags_DontClosePopups);
											ImGui::Selectable(XorStr("dropped weapons icon"), &config_system.get_config().visuals.world.dropped_weapons_icon, ImGuiSelectableFlags_DontClosePopups);
											ImGui::Selectable(XorStr("projectiles"), &config_system.get_config().visuals.world.projectiles, ImGuiSelectableFlags_DontClosePopups);
											ImGui::Selectable(XorStr("misc entities"), &config_system.get_config().visuals.world.misc_entities, ImGuiSelectableFlags_DontClosePopups);
											ImGui::Selectable(XorStr("danger zone"), &config_system.get_config().visuals.world.danger_zone, ImGuiSelectableFlags_DontClosePopups);
											ImGui::EndCombo();
										}
										ImGui::PopFont();
										ImGui::EndTabItem();
									}

									if (ImGui::BeginTabItem(XorStr("GLOW")))
									{
										ImGui::PushFont(font_main);
										ImGui::Checkbox(XorStr("active"), &config_system.get_config().visuals.glow.enabled);
										if(config_system.get_config().visuals.glow.enabled)
										{
											ImGui::Checkbox(XorStr("enemy"), &config_system.get_config().visuals.glow.enemy);
											if(config_system.get_config().visuals.glow.enemy)
											{
												ImGui::ColorEdit4(XorStr("glow emeny color"), reinterpret_cast<float*>(&config_system.get_config().colors.glow_enemy), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
												ImGui::Spacing();
											}

											ImGui::Checkbox(XorStr("teammate"), &config_system.get_config().visuals.glow.temmmate);
											if(config_system.get_config().visuals.glow.temmmate)
											{
												ImGui::ColorEdit4(XorStr("glow color team"), reinterpret_cast<float*>(&config_system.get_config().colors.glow_team), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
												ImGui::Spacing();
											}

											ImGui::Checkbox(XorStr("planted bomb"), &config_system.get_config().visuals.glow.planted_bomb);
											if(config_system.get_config().visuals.glow.planted_bomb)
											{
												ImGui::ColorEdit4(XorStr("glow color planted"), reinterpret_cast<float*>(&config_system.get_config().colors.nades), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
												ImGui::Spacing();
											}
											
											ImGui::Checkbox(XorStr("dropped weapons"), &config_system.get_config().visuals.glow.dropped_weapons);
											ImGui::Checkbox(XorStr("dropped nades"), &config_system.get_config().visuals.glow.nades);
										}
										ImGui::PopFont();
										ImGui::EndTabItem();
									}

									if(ImGui::BeginTabItem(XorStr("HELPERS")))
									{
										ImGui::PushFont(font_main);
										ImGui::Checkbox(XorStr("grenade predition"), &config_system.get_config().visuals.helpers.grenade_prediction);
										ImGui::Checkbox(XorStr("damage indicator"), &config_system.get_config().visuals.helpers.damage_indicator);
										if(config_system.get_config().visuals.helpers.damage_indicator)
										{
											ImGui::ColorEdit4(XorStr("damage indicator color"), reinterpret_cast<float*>(&config_system.get_config().colors.damage_indicator), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
										}
										ImGui::PopFont();
										ImGui::EndTabItem();
									}

									if (ImGui::BeginTabItem(XorStr("CHAMS")))
									{
										ImGui::PushFont(font_main);
										ImGui::Combo(XorStr("material"), &config_system.get_config().visuals.chams.material, XorStr("textured\0flat\0metallic\0pulsating\0"));
										ImGui::Spacing();
										ImGui::Checkbox(XorStr("smoke check"), &config_system.get_config().visuals.chams.smoke_check);
										ImGui::Spacing();
										ImGui::Checkbox(XorStr("enemy"), &config_system.get_config().visuals.chams.enemy);
										if(config_system.get_config().visuals.chams.enemy)
										{
											ImGui::ColorEdit4(XorStr("enemy color"), reinterpret_cast<float*>(&config_system.get_config().colors.chams_enemy_visible), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
											ImGui::Checkbox(XorStr("enemy (behind wall)"), &config_system.get_config().visuals.chams.enemy_xoz);
											if(config_system.get_config().visuals.chams.enemy_xoz)
											{
												ImGui::ColorEdit4(XorStr("enemy (behind wall) color"), reinterpret_cast<float*>(&config_system.get_config().colors.chams_enemy_invisible), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
											}
											ImGui::Spacing();
										}
										ImGui::Checkbox(XorStr("teammate"), &config_system.get_config().visuals.chams.temmmate);
										if(config_system.get_config().visuals.chams.temmmate)
										{
											ImGui::ColorEdit4(XorStr("teammate color"), reinterpret_cast<float*>(&config_system.get_config().colors.chams_team_visible), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
											ImGui::Checkbox(XorStr("teammate (behind wall)"), &config_system.get_config().visuals.chams.temmate_xoz);
											if(config_system.get_config().visuals.chams.temmate_xoz)
											{
												ImGui::ColorEdit4(XorStr("teammate (behind wall) color"), reinterpret_cast<float*>(&config_system.get_config().colors.chams_enemy_visible), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
											}
											ImGui::Spacing();
										}

										ImGui::Checkbox(XorStr("weapon chams"), &config_system.get_config().visuals.chams.weapon);
										if (config_system.get_config().visuals.chams.weapon)
										{
											ImGui::ColorEdit4(XorStr("weapon chams color"), reinterpret_cast<float*>(&config_system.get_config().colors.chams_hand_weapon), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
											ImGui::Checkbox(XorStr("weapons wireframe"), &config_system.get_config().visuals.chams.weapon_wireframe);
											ImGui::Spacing();
										}

										ImGui::Checkbox(XorStr("hand chams"), &config_system.get_config().visuals.chams.hands);
										if (config_system.get_config().visuals.chams.hands)
										{
											ImGui::ColorEdit4(XorStr("hand chams color"), reinterpret_cast<float*>(&config_system.get_config().colors.chams_hands), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
											ImGui::Checkbox(XorStr("hands wireframe"), &config_system.get_config().visuals.chams.hands_wireframe);
											ImGui::Spacing();
										}

										ImGui::Checkbox(XorStr("sleeve chams"), &config_system.get_config().visuals.chams.sleeve);
										if (config_system.get_config().visuals.chams.sleeve)
										{
											ImGui::ColorEdit4(XorStr("sleeve chams color"), reinterpret_cast<float*>(&config_system.get_config().colors.chams_sleeve), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
											ImGui::Checkbox(XorStr("sleeve wireframe"), &config_system.get_config().visuals.chams.sleeve_wireframe);
											ImGui::Spacing();
										}

										ImGui::Spacing();
										ImGui::Checkbox(XorStr("backtrack player"), &config_system.get_config().visuals.chams.backtrack);
										ImGui::PopFont();
										ImGui::EndTabItem();
									}
									ImGui::EndTabBar();
								}
								ImGui::EndChild();
							}
							ImGui::EndTabItem();
						}

					}

					if (ImGui::BeginTabItem(XorStr("MISC")))
					{
						ImGui::BeginChild(XorStr("##misc_window"));
						{
							if (ImGui::BeginTabBar(XorStr("##misc_tabs"), ImGuiTabBarFlags_None))
							{
								if(bhop_enabled)
								{
									if (ImGui::BeginTabItem(XorStr("BUNNY HOP")))
									{
										ImGui::PushFont(font_main);
										ImGui::Checkbox(XorStr("bunny hop enable"), &config_system.get_config().bhop.enabled);
										if (config_system.get_config().bhop.enabled) 
										{
											ImGui::Checkbox(XorStr("auto strafe"), &config_system.get_config().bhop.auto_strafe);
											ImGui::Checkbox(XorStr("edge jump"), &config_system.get_config().bhop.edge_jump.enabled);
											if (config_system.get_config().bhop.edge_jump.enabled) 
											{
												ImGui::Hotkey(XorStr("edge jump key"), &config_system.get_config().bhop.edge_jump.jump_key, ImVec2(150, 20));
												ImGui::Checkbox(XorStr("duck in air"), &config_system.get_config().bhop.edge_jump.duck_in_air);
											}
										}
										ImGui::PopFont();
										ImGui::EndTabItem();
									}
								}

								if(wallhack_enabled)
								{
									if (ImGui::BeginTabItem(XorStr("OTHER MISC")))
									{
										ImGui::PushFont(font_main);
										ImGui::Checkbox(XorStr("global active"), &config_system.get_config().misc.global_active);
										ImGui::Checkbox(XorStr("clantag"), &config_system.get_config().misc.clantag);
										ImGui::Checkbox(XorStr("in game radar"), &config_system.get_config().misc.radar);

										if (ImGui::BeginCombo(XorStr("logs"), "..."))
										{
											ImGui::Selectable(XorStr("player hurt"), &config_system.get_config().misc.logs.hurt, ImGuiSelectableFlags_DontClosePopups);
											ImGui::Selectable(XorStr("player bought"), &config_system.get_config().misc.logs.bouth, ImGuiSelectableFlags_DontClosePopups);
											ImGui::EndCombo();
										}

										ImGui::Checkbox(XorStr("hitmarker"), &config_system.get_config().misc.hitmarker.enabled);
										if (config_system.get_config().misc.hitmarker.enabled) 
										{
											ImGui::Combo(XorStr("hitmarker sound"), &config_system.get_config().misc.hitmarker.sound, XorStr("none\0one\0two\0three\0"));
										}
										ImGui::Checkbox(XorStr("anti screenshot"), &config_system.get_config().misc.anti_screenshoot);
										ImGui::Checkbox(XorStr("spectators"), &config_system.get_config().misc.spectators);
										ImGui::Checkbox(XorStr("disable post processing"), &config_system.get_config().misc.disable_post_processing);
										ImGui::Checkbox(XorStr("recoil crosshair"), &config_system.get_config().misc.recoil_crosshair);
										ImGui::Checkbox(XorStr("rank reveal"), &config_system.get_config().misc.radar);
										if (ImGui::Button(XorStr("hide name")))
										{
											utilities::change_name(XorStr("\n\xAD\xAD\xAD"));
										}
										ImGui::PopFont();
										ImGui::EndTabItem();
									}
								}
								ImGui::EndTabBar();
							}
							ImGui::EndChild();
						}
						ImGui::EndTabItem();
					}

				}

				if(changer_enabled)
				{
					if (ImGui::BeginTabItem(XorStr("CHANGER")))
					{
						ImGui::BeginChild(XorStr("##changer_window"));
						{
							if (ImGui::BeginTabBar(XorStr("##changer_tabs"), ImGuiTabBarFlags_None))
							{
								if (ImGui::BeginTabItem(XorStr("SKINS")))
								{
									ImGui::PushFont(font_main);
									ImGui::Checkbox("global enable", &config_system.get_config().skin_changer.enable_skinchanger);
									ImGui::Spacing();
									ImGui::Spacing();
									ImGui::BeginChild(XorStr("KNFIE"));
									{
										ImGui::Combo(XorStr("model"), &config_system.get_config().skin_changer.knife_model, XorStr("default\0bayonet\0m9\0karambit\0bowie\0butterfly\0falchion\0flip\0gut\0huntsman\0shaddow daggers\0navaja\0stiletto\0talon\0ursus\0"));
										ImGui::Combo(XorStr("skin"), &config_system.get_config().skin_changer.knife_skin_vector, [](void* data, int idx, const char** out_text) {
											*out_text = parser_skins[idx].name.c_str();
											return true;
											}, nullptr, parser_skins.size(), 10);
										config_system.get_config().skin_changer.knife_skin = parser_skins[config_system.get_config().skin_changer.knife_skin_vector].id;
										ImGui::Spacing();
										ImGui::Combo(XorStr("condition"), &config_system.get_config().skin_changer.knife_wear, XorStr("factory new\0minimal wear\0field-tested\0well-worn\0battle-scarred\0"));
										ImGui::Spacing();
										if (ImGui::Button(XorStr("full apply"))) {
											utilities::force_update();
										}
										ImGui::EndChild();
									}
									ImGui::Spacing();

									ImGui::BeginChild(XorStr("WEAPON"));
									{
										ImGui::Text(XorStr("skin for->%s"), config_system.current_weapon_name.c_str());
										ImGui::Combo(XorStr("skin to apply"), &config_system.current_weapon->skin_info.paint_kit_vector, [](void* data, int idx, const char** out_text) {
											*out_text = parser_skins[idx].name.c_str();
											return true;
											}, nullptr, parser_skins.size(), 10);

										if (ImGui::Button(XorStr("apply skin")))
										{
											config_system.current_weapon->skin_info.paint_kit = parser_skins[config_system.current_weapon->skin_info.paint_kit_vector].id;
											config_system.current_weapon->skin_info.setted = true;
											utilities::force_update();
										}
										ImGui::EndChild();
									}
									ImGui::PopFont();
									ImGui::EndTabItem();
								}

								if (ImGui::BeginTabItem(XorStr("GLOVES")))
								{
									ImGui::PushFont(font_main);
									ImGui::Checkbox(XorStr("glove changer enable"), &config_system.get_config().skin_changer.enable_glovechanger);
									ImGui::Spacing();
									ImGui::Spacing();
									ImGui::Combo(XorStr("collection"), &config_system.get_config().skin_changer.glove_model, XorStr("default\0blood\0sport\0slick\0leather\0moto\0specin\0hydra\0"));
									ImGui::Combo(XorStr("glove skin"), &config_system.get_config().skin_changer.glove_skin_vector, [](void* data, int idx, const char** out_text) {
										*out_text = parser_gloves[idx].name.c_str();
										return true;
										}, nullptr, parser_gloves.size(), 10);
									config_system.get_config().skin_changer.glove_skin = parser_gloves[config_system.get_config().skin_changer.glove_skin_vector].id;
									ImGui::Spacing();
									ImGui::Combo(XorStr("glove condition"), &config_system.get_config().skin_changer.glove_wear, XorStr("factory new\0minimal wear\0field-tested\0well-worn\0battle-scarred\0"));
									ImGui::Spacing();
									if (ImGui::Button(XorStr("force update gloves"))) {
										utilities::force_update();
									}
									ImGui::PopFont();
									ImGui::EndTabItem();
								}

								ImGui::EndTabBar();
							}
							ImGui::EndChild();
						}
						ImGui::EndTabItem();
					}
				}

				if (ImGui::BeginTabItem(XorStr("CONFIG")))
				{
					ImGui::BeginChild(XorStr("##config_window"));
					{
						if (ImGui::BeginTabBar(XorStr("##config_tabs"), ImGuiTabBarFlags_None))
						{
							if (ImGui::BeginTabItem(XorStr("CONFIG SYSTEM")))
							{
								ImGui::BeginChild("##config_tab");
								{
									ImGui::Spacing();
									ImGui::Spacing();
									constexpr auto& config_items = config_system.get_configs();
									static int current_config = -1;

									if (static_cast<size_t>(current_config) >= config_items.size())
										current_config = -1;

									static char buffer[16];

									if (ImGui::ListBox("", &current_config, [](void* data, int idx, const char** out_text) {
										auto& vector = *static_cast<std::vector<std::string>*>(data);
										*out_text = vector[idx].c_str();
										return true;
										}, &config_items, config_items.size(), 5) && current_config != -1)
										strcpy(buffer, config_items[current_config].c_str());

										ImGui::PushItemWidth(178);
										if (ImGui::InputText("", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
											if (current_config != -1)
												config_system.rename(current_config, buffer);
										}
										ImGui::NextColumn();

										if (ImGui::Button((XorStr("create")))) {
											config_system.add(buffer);
										}

										ImGui::SameLine();

										if (ImGui::Button((XorStr("reset")))) {
											config_system.reset();
										}

										if (current_config != -1) {
											if (ImGui::Button((XorStr("load")))) {
												config_system.load(current_config);
												load_config = true;
											}

											ImGui::SameLine();

											if (ImGui::Button(XorStr("save"))) {
												config_system.save(current_config);
												save_config = true;

											}


											if (ImGui::Button((XorStr("remove")))) {
												config_system.remove(current_config);
											}
										}
										ImGui::EndChild();
								}
								ImGui::EndTabItem();
							}

							if (ImGui::BeginTabItem(XorStr("USER INFO")))
							{
								ImGui::EndTabItem();
							}

							ImGui::EndTabBar();
						}
						
					}
					ImGui::EndChild();
				}

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::PopFont();
		ImGui::End();
		ImGui::EndFrame();
	}
}
	//if (opened) 
	//{
	//	static bool expiredSkin = false;
	//	static bool expiredEsp = false;
	//	ImGui::SetNextWindowSize(ImVec2(700, 730), ImGuiSetCond_FirstUseEver);
	//	ImGui::Begin(XorStr("alpha project"), &opened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar); {
	//		ImVec2 p = ImGui::GetCursorScreenPos();

	//		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x, p.y + 30), ImVec2(p.x + ImGui::GetWindowWidth(), p.y - 3), ImColor(26, 35, 46));
	//		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x, p.y + 32), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + +30), ImColor(25, 25, 80, 255), ImColor(25, 25, 80, 255), ImColor(25, 25, 80, 255), ImColor(26, 35, 46, 255));
	//		ImGui::PushFont(font_menu);
	//		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7); 
	//		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 7);
	//		std::string windowName = XorStr("ALPHA | CHEAT [beta]");
	//		ImGui::Text(windowName.c_str());
	//		ImGui::SameLine();

	//		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5);
	//		if (ImGui::ButtonT(XorStr("legit bot"), ImVec2(60, 30), page, 0, false, false)) page = 0; ImGui::SameLine(0, 0);
	//		if (ImGui::ButtonT(XorStr("visuals"), ImVec2(50, 30), page, 1, false, false)) page = 1; ImGui::SameLine(0, 0);
	//		if (ImGui::ButtonT(XorStr("misc"), ImVec2(40, 30), page, 2, false, false)) page = 2; ImGui::SameLine(0, 0);
	//		if (ImGui::ButtonT(XorStr("skin changer"), ImVec2(100, 30), page, 3, false, false)) page = 3; ImGui::SameLine(0, 0);
	//		if (ImGui::ButtonT(XorStr("config & info"), ImVec2(90, 30), page, 4, false, false)) page = 4; ImGui::SameLine(0, 0);
	//		ImGui::PopFont();

	//		ImGui::PushFont(font_menu);
	//		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 45);
	//		//ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 222);

	//		//if (config_system.get_config().visuals_preview) {
	//		//	run_visuals_preview();
	//		//}

	//		static int test = 0;
	//		switch (page) 
	//		{
	//		case 0:
	//			ImGui::Columns(2, NULL, false);
	//			ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
	//			ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
	//			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
	//			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
	//			ImGui::BeginChild(XorStr("aimbot [globals]"), ImVec2(339, 300), true); {
	//				if (license_manager::checkModuleActive(globals::user_modules, MODULE_AIM_BOT))
	//				{
	//					ImGui::Spacing();
	//					ImGui::Spacing();
	//					ImGui::Checkbox("dynamic fov", &config_system.get_config().legit.distance_based_fov);
	//					ImGui::Checkbox("aim in scope only", &config_system.get_config().legit.scope_aim);
	//					ImGui::Checkbox("smoke check", &config_system.get_config().legit.smoke_check);
	//					ImGui::Checkbox("friendly fire", &config_system.get_config().legit.team_check);
	//					ImGui::Checkbox("auto pistol", &config_system.get_config().legit.auto_pistol);
	//					ImGui::SliderInt("kill delay", &config_system.get_config().legit.kill_delay, 0, 350);
	//				}
	//				else
	//				{
	//					ImGui::Text("AIM BOT EXPIRED!");
	//				}
	//			} ImGui::EndChild(true);

	//			ImGui::PopStyleVar();
	//			ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
	//			ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
	//			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(18, 18));

	//			ImGui::BeginChild("extras", ImVec2(339, 360), true); 
	//			{
	//				if (license_manager::checkModuleActive(globals::user_modules, MODULE_AIM_BOT))
	//				{
	//					ImGui::Spacing();
	//					ImGui::Checkbox("backtrack", &config_system.get_config().legit.backtrack.enable);
	//					ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_config().legit.backtrack.enable ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
	//					if (config_system.get_config().legit.backtrack.enable)
	//					{
	//						ImGui::SliderFloat("records (ms)", &config_system.get_config().legit.backtrack.backtrack_ms, 1.0f, 200.0f, "%.2f");
	//					}
	//					ImGui::PopStyleColor();
	//					ImGui::Checkbox("standalone rcs", &config_system.get_config().legit.rcs_standalone.enabled);
	//					if (config_system.get_config().legit.rcs_standalone.enabled) {
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	//						ImGui::SliderFloat("rcs amount x", &config_system.get_config().legit.rcs_standalone.rcs_x, 0.0f, 1.0f, "%.2f");
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	//						ImGui::SliderFloat("rcs amount y", &config_system.get_config().legit.rcs_standalone.rcs_y, 0.0f, 1.0f, "%.2f");
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	//					}
	//				}
	//			}
	//			ImGui::EndChild(true);

	//			ImGui::NextColumn();

	//			ImGui::BeginChild(("settings [" + std::string(config_system.current_weapon_name) + "]").c_str(), ImVec2(339, 670), true); {

	//				if (license_manager::checkModuleActive(globals::user_modules, MODULE_AIM_BOT))
	//				{
	//					ImGui::PushFont(font_menu);

	//					ImGui::Spacing();
	//					ImGui::Spacing();
	//					ImGui::Checkbox("aimbot enable", &config_system.current_weapon.legit_settings.enable);
	//					if(config_system.current_weapon.legit_settings.enable)
	//					{
	//						ImGui::PushStyleColor(ImGuiCol_Text, config_system.current_weapon.legit_settings.enable ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));

	//						ImGui::Checkbox("silent", &config_system.current_weapon.legit_settings.silent);
	//						ImGui::Checkbox("nearest", &config_system.current_weapon.legit_settings.nearest);


	//						if (!config_system.current_weapon.legit_settings.nearest)
	//						{
	//							ImGui::Combo(XorStr("hitbox"), &config_system.current_weapon.legit_settings.hitbox, XorStr("head\0neck\0chest\0stomach\0pelvis\0"));
	//						}
	//						ImGui::SliderFloat(XorStr("fov"), &config_system.current_weapon.legit_settings.fov, 0.0f, 10.0f, XorStr("%.3f"));
	//						ImGui::SliderFloat(XorStr("smooth"), &config_system.current_weapon.legit_settings.smooth, 1.f, 10.f, XorStr("%.2f"));
	//						ImGui::SliderFloat(XorStr("rcs x"), &config_system.current_weapon.legit_settings.rcs_x, 0.0f, 1.0f, XorStr("%.2f"));
	//						ImGui::SliderFloat(XorStr("rcs y"), &config_system.current_weapon.legit_settings.rcs_y, 0.0f, 1.0f, XorStr("%.2f"));

	//						ImGui::PopStyleColor();
	//					}
	//					ImGui::Spacing();
	//					ImGui::Spacing();


	//					if (license_manager::checkModuleActive(globals::user_modules, MODULE_TRIGGER_BOT))
	//					{
	//						ImGui::Checkbox("triggerbot enable", &config_system.current_weapon.trigger_settings.enable);
	//						if(config_system.current_weapon.trigger_settings.enable)
	//						{
	//							ImGui::Checkbox("on key", &config_system.current_weapon.trigger_settings.on_key);
	//							if(config_system.current_weapon.trigger_settings.on_key)
	//							{
	//								ImGui::Hotkey("##triger key", &config_system.current_weapon.trigger_settings.key_id, ImVec2(130, 30));
	//							}
	//							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	//							if (ImGui::BeginCombo("hitboxes", "...", ImVec2(0, 105))) {
	//								ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
	//								ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//								ImGui::Selectable(("head"), &config_system.current_weapon.trigger_settings.hitbox_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//								ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//								ImGui::Selectable(("body"), &config_system.current_weapon.trigger_settings.hitbox_body, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//								ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//								ImGui::Selectable(("arms"), &config_system.current_weapon.trigger_settings.hitbox_arms, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//								ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//								ImGui::Selectable(("legs"), &config_system.current_weapon.trigger_settings.hitbox_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	//								ImGui::EndCombo();
	//								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	//							}
	//							ImGui::SliderInt("trigger delay", &config_system.current_weapon.trigger_settings.delay, 1, 50);
	//							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	//							ImGui::Checkbox("trigger recoil", &config_system.current_weapon.trigger_settings.rcs);
	//							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	//						}

	//					}
	//					else
	//					{
	//						ImGui::Text("TRIGGER BOT EXPIRED!");
	//					}
	//					ImGui::PopFont();
	//				}
	//				
	//			} ImGui::EndChild(true);
	//			ImGui::PopStyleColor();
	//			ImGui::PopStyleVar();
	//			break;
	//		case 1:
	//			ImGui::Columns(2, NULL, false);
	//			ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
	//			ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
	//			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
	//			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
	//			ImGui::BeginChild("player", ImVec2(339, 340), true); 
	//			{
	//				ImGui::Spacing();
	//				ImGui::Spacing();
	//				if (license_manager::checkModuleActive(globals::user_modules, MODULE_WALLHACK))
	//				{
	//					ImGui::Checkbox("global active", &config_system.get_config().visuals.global_enabled);
	//					if (config_system.get_config().visuals.global_enabled) {
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 7);
	//						ImGui::Checkbox("teammate", &config_system.get_config().visuals.show_team);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 7);
	//						ImGui::Checkbox("visible only", &config_system.get_config().visuals.visible_only);
	//					}
	//					ImGui::Checkbox("name", &config_system.get_config().visuals.player.name);
	//					ImGui::ColorEdit4("name color", reinterpret_cast<float*>(&config_system.get_config().colors.esp_name), ImGuiColorEditFlags_NoInputs);
	//					ImGui::Checkbox("box", &config_system.get_config().visuals.player.box);
	//					ImGui::ColorEdit4("box color", reinterpret_cast<float*>(&config_system.get_config().colors.esp_box), ImGuiColorEditFlags_NoInputs);
	//					ImGui::Checkbox("health", &config_system.get_config().visuals.player.health);
	//					ImGui::Checkbox("weapon name", &config_system.get_config().visuals.player.weapon_name);
	//					ImGui::ColorEdit4("weapon color", reinterpret_cast<float*>(&config_system.get_config().colors.weapons), ImGuiColorEditFlags_NoInputs);
	//					ImGui::Checkbox("weapon icon", &config_system.get_config().visuals.player.weapon_icon);
	//					ImGui::ColorEdit4("weapon icon color", reinterpret_cast<float*>(&config_system.get_config().colors.esp_weapon_icon), ImGuiColorEditFlags_NoInputs);

	//					ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_config().visuals.global_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
	//					if (ImGui::BeginCombo("flags", "...", ImVec2(0, 105))) {
	//						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("bombcarrier"), &config_system.get_config().visuals.player.flags.bombcarrier, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("is flashed"), &config_system.get_config().visuals.player.flags.is_flashed, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("is scoped"), &config_system.get_config().visuals.player.flags.is_scoped, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("is defusing"), &config_system.get_config().visuals.player.flags.is_defusing, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("position callout"), &config_system.get_config().visuals.player.flags.position_callout, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("defusekit"), &config_system.get_config().visuals.player.flags.defuse_kit, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("armor"), &config_system.get_config().visuals.player.flags.armor, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("money $$"), &config_system.get_config().visuals.player.flags.money, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);

	//						ImGui::EndCombo();
	//					}
	//					ImGui::PopStyleColor();
	//					ImGui::Checkbox("footstep", &config_system.get_config().visuals.player.footsteps);
	//					ImGui::ColorEdit4("footstep color", reinterpret_cast<float*>(&config_system.get_config().colors.esp_footsteps), ImGuiColorEditFlags_NoInputs);
	//					ImGui::Checkbox("skeleton", &config_system.get_config().visuals.player.skeleton);
	//					//ImGui::Checkbox("backtrack skeleton", &config_system.get_config().backtrack_skeleton);
	//				}
	//				else
	//				{
	//					expiredEsp = true;
	//					ImGui::Text("ESP EXPIRED!");
	//				}
	//			}
	//			ImGui::EndChild(true);

	//			if(!expiredEsp)
	//			{
	//				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
	//				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();

	//				ImGui::BeginChild("effects", ImVec2(339, 330), true); {
	//					ImGui::Spacing();
	//					ImGui::Spacing();
	//					ImGui::Checkbox("force crosshair", &config_system.get_config().visuals.player.force_crosshair);
	//					ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_config().visuals.global_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
	//					//ImGui::SliderInt("viewmodel field of view", &config_system.get_config().viewmodel_fov, 0, 135);
	//					ImGui::SliderInt("field of view", &config_system.get_config().visuals.player.fov, 0, 60);
	//					ImGui::Checkbox("ambient light", &config_system.get_config().visuals.ambient_light);
	//					ImGui::ColorEdit4("ambient light color", reinterpret_cast<float*>(&config_system.get_config().colors.ambient), ImGuiColorEditFlags_NoInputs);
	//					ImGui::Checkbox("night mode", &config_system.get_config().visuals.night_mode);
	//					ImGui::ColorEdit4("sky color", reinterpret_cast<float*>(&config_system.get_config().colors.sky), ImGuiColorEditFlags_NoInputs);
	//					if (config_system.get_config().visuals.night_mode) {
	//						ImGui::SliderInt("brightness", &config_system.get_config().visuals.brightness, 0, 100);
	//					}

	//					if (ImGui::BeginCombo("removals", "...", ImVec2(0, 105))) {
	//						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("fog"), &config_system.get_config().visuals.removals.fog, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("smoke"), &config_system.get_config().visuals.removals.smoke, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("flash"), &config_system.get_config().visuals.removals.flash, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("sleeves"), &config_system.get_config().visuals.removals.sleeves, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("hands"), &config_system.get_config().visuals.removals.hands, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("scope overlay"), &config_system.get_config().visuals.removals.skope_overlay, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::EndCombo();
	//					}

	//					if (ImGui::BeginCombo("world", "...", ImVec2(0, 105))) {
	//						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("planted bomb"), &config_system.get_config().visuals.world.planted_bomb, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("dropped weapons name"), &config_system.get_config().visuals.world.dropped_weapons_name, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("dropped weapons icon"), &config_system.get_config().visuals.world.dropped_weapons_icon, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("projectiles"), &config_system.get_config().visuals.world.projectiles, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("misc entities"), &config_system.get_config().visuals.world.misc_entities, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("danger zone"), &config_system.get_config().visuals.world.danger_zone, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::EndCombo();
	//					}
	//					ImGui::PopStyleColor();
	//				}
	//				ImGui::EndChild(true);

	//				ImGui::NextColumn();

	//				ImGui::BeginChild("helpers", ImVec2(339, 110), true); {
	//					ImGui::Spacing();
	//					ImGui::Spacing();
	//					ImGui::Checkbox("grenade predition", &config_system.get_config().visuals.helpers.grenade_prediction);

	//					ImGui::Checkbox("damage indicator", &config_system.get_config().visuals.helpers.damage_indicator);
	//					ImGui::ColorEdit4("damage indicator color ", reinterpret_cast<float*>(&config_system.get_config().colors.damage_indicator), ImGuiColorEditFlags_NoInputs);
	//				}
	//				ImGui::EndChild(true);

	//				ImGui::BeginChild("glow", ImVec2(339, 200), true); {
	//					ImGui::Spacing();
	//					ImGui::Spacing();

	//					ImGui::Checkbox("active", &config_system.get_config().visuals.glow.enabled);
	//					ImGui::Checkbox("enemy", &config_system.get_config().visuals.glow.enemy);
	//					ImGui::ColorEdit4("glow color", reinterpret_cast<float*>(&config_system.get_config().colors.glow_enemy), ImGuiColorEditFlags_NoInputs);
	//					ImGui::Checkbox("teammate", &config_system.get_config().visuals.glow.temmmate);
	//					ImGui::ColorEdit4("glow color team", reinterpret_cast<float*>(&config_system.get_config().colors.glow_team), ImGuiColorEditFlags_NoInputs);
	//					ImGui::Checkbox("planted bomb", &config_system.get_config().visuals.glow.planted_bomb);
	//					ImGui::ColorEdit4("glow color planted", reinterpret_cast<float*>(&config_system.get_config().colors.nades), ImGuiColorEditFlags_NoInputs);
	//					ImGui::Checkbox("dropped weapons", &config_system.get_config().visuals.glow.dropped_weapons);
	//					//ImGui::ColorEdit4("glow color weapons", reinterpret_cast<float*>(&config_system.get_config().colors.nades), ImGuiColorEditFlags_NoInputs);
	//					ImGui::Checkbox("dropped nades", &config_system.get_config().visuals.glow.nades);
	//					//ImGui::ColorEdit4("glow color nades", reinterpret_cast<float*>(&config_system.get_config().colors.nades), ImGuiColorEditFlags_NoInputs);

	//				}
	//				ImGui::EndChild(true);

	//				ImGui::Spacing();

	//				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);

	//				ImGui::BeginChild("chams", ImVec2(339, 350), true); {
	//					ImGui::Spacing();
	//					ImGui::Spacing();

	//					ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_config().visuals.global_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
	//					ImGui::Combo("material", &config_system.get_config().visuals.chams.material, "textured\0flat\0metallic\0pulsating\0");
	//					ImGui::PopStyleColor();
	//					ImGui::Spacing();
	//					ImGui::Checkbox("smoke check", &config_system.get_config().visuals.chams.smoke_check);
	//					ImGui::Spacing();
	//					ImGui::Checkbox("enemy", &config_system.get_config().visuals.chams.enemy);
	//					ImGui::ColorEdit4("enemy color", reinterpret_cast<float*>(&config_system.get_config().colors.chams_enemy_visible), ImGuiColorEditFlags_NoInputs);
	//					ImGui::Checkbox("enemy (behind wall)", &config_system.get_config().visuals.chams.enemy_xoz);
	//					ImGui::ColorEdit4("enemy (behind wall) color", reinterpret_cast<float*>(&config_system.get_config().colors.chams_enemy_invisible), ImGuiColorEditFlags_NoInputs);

	//					ImGui::Checkbox("teammate", &config_system.get_config().visuals.chams.temmmate);
	//					ImGui::ColorEdit4("teammate color", reinterpret_cast<float*>(&config_system.get_config().colors.chams_team_visible), ImGuiColorEditFlags_NoInputs);
	//					ImGui::Checkbox("teammate (behind wall)", &config_system.get_config().visuals.chams.temmate_xoz);
	//					ImGui::ColorEdit4("teammate (behind wall) color", reinterpret_cast<float*>(&config_system.get_config().colors.chams_enemy_visible), ImGuiColorEditFlags_NoInputs);

	//					ImGui::Checkbox("weapon chams", &config_system.get_config().visuals.chams.weapon);
	//					ImGui::ColorEdit4("weapon chams color", reinterpret_cast<float*>(&config_system.get_config().colors.chams_hand_weapon), ImGuiColorEditFlags_NoInputs);
	//					if (config_system.get_config().visuals.chams.weapon)
	//						ImGui::Checkbox("weapons wireframe", &config_system.get_config().visuals.chams.weapon_wireframe);

	//					ImGui::Checkbox("hand chams", &config_system.get_config().visuals.chams.hands);
	//					ImGui::ColorEdit4("hand chams color", reinterpret_cast<float*>(&config_system.get_config().colors.chams_hands), ImGuiColorEditFlags_NoInputs);
	//					if (config_system.get_config().visuals.chams.hands)
	//						ImGui::Checkbox("hands wireframe", &config_system.get_config().visuals.chams.hands_wireframe);
	//					
	//					ImGui::Checkbox("sleeve chams", &config_system.get_config().visuals.chams.sleeve);
	//					ImGui::ColorEdit4("sleeve chams color", reinterpret_cast<float*>(&config_system.get_config().colors.chams_sleeve), ImGuiColorEditFlags_NoInputs);
	//					if (config_system.get_config().visuals.chams.sleeve)
	//						ImGui::Checkbox("sleeve wireframe", &config_system.get_config().visuals.chams.sleeve_wireframe);
	//					
	//					ImGui::Spacing();
	//					ImGui::Checkbox("backtrack player", &config_system.get_config().visuals.chams.backtrack);

	//				}
	//				ImGui::EndChild(true);
	//			}

	//			ImGui::PopStyleVar();
	//			ImGui::PopStyleColor();
	//			ImGui::Columns();
	//			break;
	//		case 2:
	//			ImGui::Columns(2, NULL, false);
	//			ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
	//			ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
	//			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
	//			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

	//			ImGui::BeginChild("misc", ImVec2(339, 670), true); 
	//			{
	//				if (license_manager::checkModuleActive(globals::user_modules, MODULE_WALLHACK))
	//				{
	//					ImGui::Spacing();
	//					ImGui::Spacing();
	//					ImGui::Checkbox("active", &config_system.get_config().misc.global_active);
	//					ImGui::Checkbox("clantag", &config_system.get_config().misc.clantag);
	//					ImGui::Checkbox("radar", &config_system.get_config().misc.radar);

	//					ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_config().misc.global_active ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
	//					if (ImGui::BeginCombo("logs", "...", ImVec2(0, 65))) {
	//						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("player hurt"), &config_system.get_config().misc.logs.hurt, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
	//						ImGui::Selectable(("player bought"), &config_system.get_config().misc.logs.bouth, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
	//						ImGui::EndCombo();
	//					}
	//					ImGui::PopStyleColor();

	//					ImGui::Checkbox("hitmarker", &config_system.get_config().misc.hitmarker.enabled);
	//					if (config_system.get_config().misc.hitmarker.enabled) {
	//						ImGui::Combo("hitmarker sound", &config_system.get_config().misc.hitmarker.sound, "none\0one\0two\0three\0");
	//					}
	//					ImGui::Checkbox("anti screenshot", &config_system.get_config().misc.anti_screenshoot);
	//					ImGui::Checkbox("spectators", &config_system.get_config().misc.spectators);
	//					//ImGui::Checkbox("watermark", &config_system.get_config().watermark);
	//					ImGui::Checkbox("disable post processing", &config_system.get_config().misc.disable_post_processing);
	//					ImGui::Checkbox("recoil crosshair", &config_system.get_config().misc.recoil_crosshair);
	//					ImGui::Checkbox("rank reveal", &config_system.get_config().misc.radar);

	//					//ImGui::Checkbox("viewmodel offset", &config_system.get_config().viewmodel_offset);
	//					//if (config_system.get_config().viewmodel_offset) {
	//					//	ImGui::SliderInt("viewmodel x", &config_system.get_config().viewmodel_x, -10, 10);
	//					//	ImGui::SliderInt("viewmodel y", &config_system.get_config().viewmodel_y, -10, 10);
	//					//	ImGui::SliderInt("viewmodel z", &config_system.get_config().viewmodel_z, -10, 10);

	//					//}

	//					//ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_config().misc_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
	//					//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
	//					//if (ImGui::Button("dump steam id")) {
	//					//	utilities::dump_steam_id();
	//					//}
	//					if (ImGui::Button("hide name")) {
	//						utilities::change_name("\n\xAD\xAD\xAD");
	//					}
	//				}
	//			}
	//			ImGui::EndChild(true);
	//			ImGui::NextColumn();

	//			ImGui::BeginChild("BHOP", ImVec2(339, 670), true); {
	//				ImGui::Spacing();
	//				ImGui::Spacing();
	//				if (license_manager::checkModuleActive(globals::user_modules, MODULE_BUNNY_HOP))
	//				{
	//					ImGui::Checkbox("bunny hop", &config_system.get_config().bhop.enabled);
	//					if (config_system.get_config().bhop.enabled) {
	//						ImGui::Checkbox("auto strafe", &config_system.get_config().bhop.auto_strafe);
	//						//ImGui::SliderInt("hitchance", &config_system.get_config().bunny_hop_hitchance, 0, 100);
	//						//ImGui::SliderInt("minimum hops", &config_system.get_config().bunny_hop_minimum_value, 0, 20);
	//						//ImGui::SliderInt("maximum hops", &config_system.get_config().bunny_hop_maximum_value, 0, 20);
	//					}


	//					ImGui::Checkbox("edge jump", &config_system.get_config().bhop.edge_jump.enabled);
	//					if (config_system.get_config().bhop.edge_jump.enabled) {
	//						ImGui::Checkbox("duck in air", &config_system.get_config().bhop.edge_jump.duck_in_air);
	//					}
	//				}
	//				else
	//				{
	//					ImGui::Text("BHOP EXPIRED!");
	//				}
	//			}
	//			ImGui::EndChild(true);

	//			ImGui::PopStyleVar();
	//			ImGui::PopStyleColor();
	//			ImGui::Columns();
	//			break;
	//		case 3:
	//			ImGui::Columns(2, NULL, false);
	//			ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
	//			ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
	//			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
	//			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

	//			ImGui::BeginChild("knife", ImVec2(339, 332), true); {
	//				ImGui::Spacing();
	//				ImGui::Spacing();
	//				if (license_manager::checkModuleActive(globals::user_modules, MODULE_SKIN_CHANGER))
	//				{
	//					ImGui::Checkbox("enable", &config_system.get_config().skin_changer.enable_skinchanger);
	//					ImGui::Spacing();
	//					ImGui::Spacing();
	//					ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_config().skin_changer.enable_skinchanger ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
	//					ImGui::Combo("model", &config_system.get_config().skin_changer.knife_model, "default\0bayonet\0m9\0karambit\0bowie\0butterfly\0falchion\0flip\0gut\0huntsman\0shaddow daggers\0navaja\0stiletto\0talon\0ursus\0");
	//					ImGui::Combo(("skin"), &config_system.get_config().skin_changer.knife_skin_vector, [](void* data, int idx, const char** out_text) {
	//						*out_text = parser_skins[idx].name.c_str();
	//						return true;
	//					}, nullptr, parser_skins.size(), 10);
	//					config_system.get_config().skin_changer.knife_skin = parser_skins[config_system.get_config().skin_changer.knife_skin_vector].id;
	//					ImGui::Spacing();
	//					ImGui::Combo("condition", &config_system.get_config().skin_changer.knife_wear, "factory new\0minimal wear\0field-tested\0well-worn\0battle-scarred\0");
	//					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
	//					ImGui::Spacing();
	//					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
	//					if (ImGui::Button("force update")) {
	//						utilities::force_update();
	//					}
	//					ImGui::PopStyleColor();
	//				}
	//				else
	//				{
	//					expiredSkin = true;
	//					ImGui::Text("SKIN CHANGER EXPIRED!");
	//				}

	//			}
	//			ImGui::EndChild(true);

	//			ImGui::PopStyleVar();
	//			ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
	//			ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
	//			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

	//			if(!expiredSkin)
	//			{
	//				ImGui::BeginChild("gloves", ImVec2(339, 333), true); {
	//					ImGui::Spacing();
	//					ImGui::Spacing();
	//					ImGui::Checkbox("enable", &config_system.get_config().skin_changer.enable_glovechanger);
	//					ImGui::Spacing();
	//					ImGui::Spacing();
	//					ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_config().skin_changer.enable_glovechanger ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
	//					ImGui::Combo("collection", &config_system.get_config().skin_changer.glove_model, "default\0blood\0sport\0slick\0leather\0moto\0specin\0hydra\0");
	//					ImGui::Combo(("skin"), &config_system.get_config().skin_changer.glove_skin_vector, [](void* data, int idx, const char** out_text) {
	//						*out_text = parser_gloves[idx].name.c_str();
	//						return true;
	//					}, nullptr, parser_gloves.size(), 10);
	//					config_system.get_config().skin_changer.glove_skin = parser_gloves[config_system.get_config().skin_changer.glove_skin_vector].id;
	//					ImGui::Spacing();
	//					ImGui::Combo("condition", &config_system.get_config().skin_changer.glove_wear, "factory new\0minimal wear\0field-tested\0well-worn\0battle-scarred\0");
	//					ImGui::Spacing();
	//					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
	//					if (ImGui::Button("force update")) {
	//						utilities::force_update();
	//					}
	//					ImGui::PopStyleColor();
	//				}
	//				ImGui::EndChild(true);

	//				ImGui::NextColumn();

	//				ImGui::BeginChild((config_system.current_weapon_name + " settings").c_str(), ImVec2(339, 670), true); {
	//					ImGui::Spacing();
	//					ImGui::Spacing();
	//					ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_config().skin_changer.enable_skinchanger ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));

	//					ImGui::Combo((config_system.current_weapon_name + " skin").c_str(), &config_system.current_weapon.skin_info.paint_kit_vector, [](void* data, int idx, const char** out_text) {
	//						*out_text = parser_skins[idx].name.c_str();
	//						return true;
	//					}, nullptr, parser_skins.size(), 10);

	//					if(ImGui::Button("apply skin"))
	//					{
	//						config_system.current_weapon.skin_info.paint_kit = parser_skins[config_system.current_weapon.skin_info.paint_kit_vector].id;
	//						config_system.current_weapon.skin_info.setted = true;
	//						utilities::force_update();
	//					}
	//					ImGui::PopStyleColor();

	//				}
	//				ImGui::EndChild(true);
	//			}

	//			ImGui::PopStyleVar();
	//			ImGui::PopStyleColor();
	//			ImGui::Columns();
	//			break;

	//		case 4:
	//			ImGui::Columns(2, NULL, false);
	//			ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
	//			ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
	//			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
	//			//ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
	//			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
	//			ImGui::BeginChild(XorStr("config"), ImVec2(339, 333), true); {
	//				ImGui::Spacing();
	//				ImGui::Spacing();
	//				constexpr auto& config_items = config_system.get_configs();
	//				static int current_config = -1;

	//				if (static_cast<size_t>(current_config) >= config_items.size())
	//					current_config = -1;

	//				static char buffer[16];

	//				if (ImGui::ListBox("", &current_config, [](void* data, int idx, const char** out_text) {
	//					auto& vector = *static_cast<std::vector<std::string>*>(data);
	//					*out_text = vector[idx].c_str();
	//					return true;
	//					}, & config_items, config_items.size(), 5) && current_config != -1)
	//					strcpy(buffer, config_items[current_config].c_str());

	//					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
	//					ImGui::PushID(0);
	//					ImGui::PushItemWidth(178);
	//					if (ImGui::InputText("", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
	//						if (current_config != -1)
	//							config_system.rename(current_config, buffer);
	//					}
	//					ImGui::PopID();
	//					ImGui::NextColumn();

	//					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
	//					if (ImGui::Button((XorStr("create")))) {
	//						config_system.add(buffer);
	//					}

	//					ImGui::SameLine();

	//					if (ImGui::Button((XorStr("reset")))) {
	//						config_system.reset();
	//					}

	//					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
	//					if (current_config != -1) {
	//						if (ImGui::Button((XorStr("load")))) {
	//							config_system.load(current_config);
	//							load_config = true;
	//						}

	//						ImGui::SameLine();

	//						if (ImGui::Button(XorStr("save"))) {
	//							config_system.save(current_config);
	//							save_config = true;

	//						}

	//						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
	//						if (ImGui::Button((XorStr("remove")))) {
	//							config_system.remove(current_config);
	//						}
	//					}
	//			}
	//			ImGui::EndChild(true);

	//			ImGui::PopStyleVar();
	//			ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
	//			ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
	//			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

	//			ImGui::BeginChild(XorStr("settings"), ImVec2(339, 332), true); {
	//				ImGui::Spacing();
	//				ImGui::Spacing();
	//			}
	//			ImGui::EndChild(true);

	//			ImGui::NextColumn();

	//			ImGui::BeginChild("info", ImVec2(339, 670), true); {
	//				ImGui::Spacing();
	//				ImGui::Spacing();

	//				for(unsigned i  = 0; i < globals::user_modules.modules_count; i++)
	//				{
	//					std::string moduleName = "";
	//					std::string expireDate = std::to_string(globals::user_modules.modules_end_date[i]);
	//					switch(globals::user_modules.modules_ids[i])
	//					{
	//					case MODULE_AIM_BOT:
	//						moduleName = "[AIM BOT]";
	//						break;
	//					case MODULE_WALLHACK:
	//						moduleName = "[ESP & MISC]";
	//						break;
	//					case MODULE_BUNNY_HOP:
	//						moduleName = "[BUNNY HOP]";
	//						break;
	//					case MODULE_TRIGGER_BOT:
	//						moduleName = "[TRIGGER BOT]";
	//						break;
	//					case MODULE_SKIN_CHANGER:
	//						moduleName = "[SKIN CHANGER]";
	//						break;
	//					default:
	//						break;
	//					}
	//					if(globals::is_lifetime)
	//					{
	//						expireDate = std::string(XorStr("lifetime"));
	//					}
	//					else
	//					{
	//						tm* time = gmtime((time_t*)&globals::user_modules.modules_end_date[i]);
	//						if (time != nullptr)
	//						{
	//							expireDate =
	//								std::to_string(time->tm_mday) + "-" +
	//								std::to_string(time->tm_mon + 1) + "-" +
	//								std::to_string(time->tm_year + 1900) + " " +
	//								std::to_string(time->tm_hour + 1) + ":" +
	//								std::to_string(time->tm_min + 1) + ":" +
	//								std::to_string(time->tm_sec) + " ";
	//						}
	//					}
	//					ImGui::Text((moduleName + " expired :" + expireDate).c_str());
	//				}
	//				//char buffer[UNLEN + 1];
	//				//DWORD size;
	//				//size = sizeof(buffer);
	//				//GetUserName(buffer, &size);
	//				//char title[UNLEN];
	//				//char ch1[25] = "welcome, ";
	//				//char *ch = strcat(ch1, buffer);

	//				//ImGui::Text(ch);
	//				//ImGui::Text("build: " __DATE__ " / " __TIME__);
	//				//if (std::strstr(GetCommandLineA(), "-insecure")) {
	//				//	ImGui::Text("insecure mode!");
	//				//}
	//			}
	//			ImGui::EndChild(true);
	//			ImGui::PopStyleVar();
	//			ImGui::PopStyleColor();
	//			//ImGui::PopStyleColor();
	//			ImGui::Columns();
	//			break;
	//		}
	//	}

	//	ImGui::PopFont();

	//	ImGui::End();
	//}

void c_menu::run_popup() {
	ImGui::PushFont(font_menu);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

	if (save_config) {
		bool done = false;
		ImGui::begin_popup(XorStr("config saved."), 2000, &done);
		if (done)
			save_config = false;
	}

	if (load_config) {
		bool done = false;
		ImGui::begin_popup(XorStr("config loaded."), 2000, &done);
		if (done)
			load_config = false;
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	ImGui::PopFont();
}

void c_menu::run_visuals_preview() {

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

	static std::vector<esp_info_s> info;
	static bool enabled = true;

	ImGui::SetNextWindowSize(ImVec2(235, 400));

	ImGui::Begin(XorStr("ESP Preview"), &enabled, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar); {
		const auto cur_window = ImGui::GetCurrentWindow();
		const ImVec2 w_pos = cur_window->Pos;

		ImVec2 p = ImGui::GetCursorScreenPos();
		ImColor c = ImColor(32, 114, 247);

		//title bar
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x - 20, p.y + 15), ImVec2(p.x + ImGui::GetWindowWidth(), p.y - 40), ImColor(30, 30, 39));

		//draw gradient bellow title bar
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x - 20, p.y + 14), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + 16), ImColor(167, 24, 71, 255), ImColor(58, 31, 87, 255), ImColor(58, 31, 87, 255), ImColor(167, 24, 71, 255));

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 9); //góra, dół
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 8); //lewo prawo
		//render cheat name
		ImGui::Text(XorStr("visuals preview"));


		//if (config_system.get_config().player_box) {
		//	cur_window->DrawList->AddRect(ImVec2(w_pos.x + 40, w_pos.y + 60), ImVec2(w_pos.x + 200, w_pos.y + 360), ImGui::GetColorU32(ImGuiCol_Text));
		//}

		//if (config_system.get_config().player_health)
		//	cur_window->DrawList->AddRectFilled(ImVec2(w_pos.x + 34, w_pos.y + 60), ImVec2(w_pos.x + 36, w_pos.y + 360), ImGui::GetColorU32(ImVec4(83 / 255.f, 200 / 255.f, 84 / 255.f, 255 / 255.f)));

		//if (config_system.get_config().player_name)
		//	info.emplace_back(esp_info_s(XorStr("name"), color(255, 255, 255, 255), CENTER_UP));

		//if (config_system.get_config().player_weapon)
		//	info.emplace_back(esp_info_s(XorStr("awp"), color(255, 255, 255, 255), CENTER_DOWN));

		//if (config_system.get_config().player_flags_armor)
		//	info.emplace_back(esp_info_s(XorStr("hk"), color(255, 255, 255, 255), RIGHT));


		for (auto render : info) {
			const auto text_size = ImGui::CalcTextSize(render.f_name.c_str());

			auto pos = ImVec2(w_pos.x + 205, w_pos.y + 60);

			if (render.f_position == CENTER_DOWN) {
				pos = ImVec2(w_pos.x + (240 / 2) - text_size.x / 2, pos.y + 315 - text_size.y);
			}
			else if (render.f_position == CENTER_UP) {
				pos = ImVec2(w_pos.x + (240 / 2) - text_size.x / 2, pos.y - 5 - text_size.y);
			}

			cur_window->DrawList->AddText(pos, ImGui::GetColorU32(ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 255 / 255.f)), render.f_name.c_str());
		}
	}
	

	ImGui::PopStyleColor();
	ImGui::PopStyleVar();

	ImGui::End();

	info.clear();
}