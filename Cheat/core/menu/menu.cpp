#include "menu.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "../features/misc/logs.hpp"
#include "../features/skinchanger/parser.hpp"
#include "../../SDK/crypto/XorStr.h"

c_menu menu;

#define UNLEN 256
IDirect3DStateBlock9 *state_block;
bool reverse = false;
int offset = 0;
bool show_popup = false;
bool save_config = false;
bool load_config = false;
namespace ImGui {
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
		ImGui::Begin(XorStr("##PopUpWindow"), &show_popup, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImVec2 p = ImGui::GetCursorScreenPos();

		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x - 15, p.y - 13), ImVec2(p.x + ImGui::GetWindowWidth(), p.y - 16), ImColor(167, 24, 71, 255), ImColor(58, 31, 87, 255), ImColor(58, 31, 87, 255), ImColor(167, 24, 71, 255));

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

void c_menu::run() {
	static int page = 0;

	if (opened) {
		//ImGui::GetStyle().Colors[ImGuiCol_CheckMark] = ImVec4(167 / 255.f, 24 / 255.f, 71 / 255.f, 1.f);
		//ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = ImVec4(167 / 255.f, 24 / 255.f, 71 / 255.f, 1.f);
		//ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive] = ImVec4(167 / 255.f, 24 / 71, 247 / 255.f, 1.f);

		ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiSetCond_FirstUseEver);
		ImGui::Begin(XorStr("alpha project"), &opened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar); {
			ImVec2 p = ImGui::GetCursorScreenPos();

			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x, p.y + 30), ImVec2(p.x + ImGui::GetWindowWidth(), p.y - 3), ImColor(26, 35, 46));
			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x, p.y + 32), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + +30), ImColor(25, 25, 80, 255), ImColor(25, 25, 80, 255), ImColor(25, 25, 80, 255), ImColor(26, 35, 46, 255));
			ImGui::PushFont(font_menu);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7); 
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 7);
			std::string windowName = XorStr("ALPHA | CHEAT [beta]");
			ImGui::Text(windowName.c_str());
			ImGui::SameLine();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 9);
			if (ImGui::ButtonT(XorStr("legit bot"), ImVec2(50, 30), page, 0, false, false)) page = 0; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT(XorStr("visuals"), ImVec2(40, 30), page, 1, false, false)) page = 1; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT(XorStr("misc"), ImVec2(40, 30), page, 2, false, false)) page = 2; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT(XorStr("skins"), ImVec2(40, 30), page, 3, false, false)) page = 3; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT(XorStr("config"), ImVec2(40, 30), page, 4, false, false)) page = 4; ImGui::SameLine(0, 0);
			ImGui::PopFont();

			ImGui::PushFont(font_menu);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 45);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 222); 

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
			//ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

			if (config_system.item.visuals_preview) {
				run_visuals_preview();
			}

			ImGui::PopStyleColor();
			//ImGui::PopStyleColor();
			ImGui::PopStyleVar();

			static int test = 0;
			switch (page) {
			case 0:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				//push window color for child
				//ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				//push border color for child
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
				ImGui::BeginChild(XorStr("aimbot"), ImVec2(279, 268), true); {
					ImGui::Checkbox(XorStr("active"), &config_system.item.aim_enabled);
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.aim_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::Combo(XorStr("mode"), &config_system.item.aim_mode, XorStr("hitbox\0nearest hitbox")); //todo add custom bone selection - designer
					ImGui::PopStyleColor();
					ImGui::Checkbox(XorStr("dynamic fov"), &config_system.item.aim_distance_based_fov);
					ImGui::Checkbox(XorStr("silent aim"), &config_system.item.aim_silent);
					ImGui::Checkbox(XorStr("scope aim"), &config_system.item.scope_aim);
					ImGui::Checkbox(XorStr("smoke aim"), &config_system.item.smoke_check);
					ImGui::Checkbox(XorStr("friendly fire"), &config_system.item.aim_team_check);
					ImGui::Checkbox(XorStr("auto pistol"), &config_system.item.aimbot_auto_pistol);
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.aim_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::SliderInt(XorStr("kill delay"), &config_system.item.aimbot_delay_after_kill, 0, 350);
					ImGui::PopStyleColor();

				} ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild(XorStr("backtrack"), ImVec2(279, 267), true); {
					ImGui::Checkbox(XorStr("active"), &config_system.item.backtrack);
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.backtrack ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::SliderFloat(XorStr("records (ms)"), &config_system.item.backtrack_ms, 1.0f, 200.0f, XorStr("%.2f"));
					ImGui::PopStyleColor();
				}
				ImGui::EndChild(true);

				ImGui::NextColumn();

				ImGui::BeginChild(XorStr("settings"), ImVec2(279, 543), true); {
					if (ImGui::ButtonT(XorStr("pistol"), ImVec2(50, 30), test, 0, false, ImColor(0, 0, 0))) test = 0; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT(XorStr("rifle"), ImVec2(50, 30), test, 1, false, false)) test = 1; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT(XorStr("sniper"), ImVec2(50, 30), test, 2, false, false)) test = 2; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT(XorStr("smg"), ImVec2(50, 30), test, 3, false, false)) test = 3; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT(XorStr("heavy"), ImVec2(50, 30), test, 4, false, false)) test = 4;

					ImGui::PushFont(font_menu);

					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.aim_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					switch (test) {
					case 0:
						if (config_system.item.aim_mode == 0) {
							ImGui::Combo(XorStr("pistol hitbox"), &config_system.item.aim_bone_pistol, XorStr("head\0neck\0chest\0stomach\0pelvis"));
						}
						ImGui::SliderFloat(XorStr("pistol fov"), &config_system.item.aim_fov_pistol, 0.0f, 180.0f, XorStr("%.2f"));
						ImGui::SliderFloat(XorStr("pistol smooth"), &config_system.item.aim_smooth_pistol, 1.f, 10.f, XorStr("%.2f"));
						ImGui::SliderFloat(XorStr("pistol rcs x"), &config_system.item.rcs_x_pistol, 0.0f, 1.0f, XorStr("%.2f"));
						ImGui::SliderFloat(XorStr("pistol rcs y"), &config_system.item.rcs_y_pistol, 0.0f, 1.0f, XorStr("%.2f"));
						break;
					case 1:
						if (config_system.item.aim_mode == 0) {
							ImGui::Combo(XorStr("rifle hitbox"), &config_system.item.aim_bone_rifle, XorStr("head\0neck\0chest\0stomach\0pelvis"));
						}
						ImGui::SliderFloat(XorStr("rifle fov"), &config_system.item.aim_fov_rifle, 0.0f, 180.0f, XorStr("%.2f"));
						ImGui::SliderFloat(XorStr("rifle smooth"), &config_system.item.aim_smooth_rifle, 1.f, 10.f, XorStr("%.2f"));
						ImGui::SliderFloat(XorStr("rifle rcs x"), &config_system.item.rcs_x_rifle, 0.0f, 1.0f, XorStr("%.2f"));
						ImGui::SliderFloat(XorStr("rifle rcs y"), &config_system.item.rcs_y_rifle, 0.0f, 1.0f, XorStr("%.2f"));
						break;
					case 2:
						if (config_system.item.aim_mode == 0) {
							ImGui::Combo(XorStr("sniper hitbox"), &config_system.item.aim_bone_sniper, XorStr("head\0neck\0chest\0stomach\0pelvis"));
						}
						ImGui::SliderFloat(XorStr("sniper fov"), &config_system.item.aim_fov_sniper, 0.0f, 180.0f, XorStr("%.2f"));
						ImGui::SliderFloat(XorStr("sniper smooth"), &config_system.item.aim_smooth_sniper, 1.f, 10.f, "%.2f");
						ImGui::SliderFloat(XorStr("sniper rcs x"), &config_system.item.rcs_x_sniper, 0.0f, 1.0f, XorStr("%.2f"));
						ImGui::SliderFloat(XorStr("sniper rcs y"), &config_system.item.rcs_y_sniper, 0.0f, 1.0f, XorStr("%.2f"));
						break;
					case 3:
						if (config_system.item.aim_mode == 0) {
							ImGui::Combo(XorStr("smg hitbox"), &config_system.item.aim_bone_smg, XorStr("head\0neck\0chest\0stomach\0pelvis"));
						}

						ImGui::SliderFloat(XorStr("smg fov"), &config_system.item.aim_fov_smg, 0.0f, 180.0f, XorStr("%.2f"));
						ImGui::SliderFloat(XorStr("smg smooth"), &config_system.item.aim_smooth_smg, 1.f, 10.f, XorStr("%.2f"));
						ImGui::SliderFloat(XorStr("smg rcs x"), &config_system.item.rcs_x_smg, 0.0f, 1.0f, XorStr("%.2f"));
						ImGui::SliderFloat(XorStr("smg rcs y"), &config_system.item.rcs_y_smg, 0.0f, 1.0f, XorStr("%.2f"));
						break;
					case 4:
						if (config_system.item.aim_mode == 0) {
							ImGui::Combo(XorStr("heavy hitbox"), &config_system.item.aim_bone_heavy, XorStr("head\0neck\0chest\0stomach\0pelvis"));
						}
						ImGui::SliderFloat(XorStr("heavy fov"), &config_system.item.aim_fov_heavy, 0.0f, 180.0f, XorStr("%.2f"));
						ImGui::SliderFloat(XorStr("heavy smooth"), &config_system.item.aim_smooth_heavy, 1.f, 10.f, XorStr("%.2f"));
						ImGui::SliderFloat(XorStr("heavy rcs x"), &config_system.item.rcs_x_heavy, 0.0f, 1.0f, XorStr("%.2f"));
						ImGui::SliderFloat(XorStr("heavy rcs y"), &config_system.item.rcs_y_heavy, 0.0f, 1.0f, XorStr("%.2f"));
						break;
					}
					ImGui::PopStyleColor();

					ImGui::PopFont();
				} ImGui::EndChild(true);
				ImGui::PopStyleColor();
				//ImGui::PopStyleColor();
				ImGui::PopStyleVar();

				break;
			case 1:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				//ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

				ImGui::BeginChild(XorStr("player"), ImVec2(279, 268), true); {
					ImGui::Checkbox(XorStr("active"), &config_system.item.visuals_enabled);
					if (config_system.item.visuals_enabled) 
					{
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 7);
						ImGui::Checkbox(XorStr("teammate"), &config_system.item.visuals_team_check);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 7);
						ImGui::Checkbox(XorStr("visible only"), &config_system.item.visuals_visible_only);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 7);
						ImGui::Checkbox(XorStr("on key"), &config_system.item.visuals_on_key);
					}

					ImGui::Checkbox(XorStr("name"), &config_system.item.player_name);
					ImGui::ColorEdit4(XorStr("name color"), config_system.item.clr_name, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox(XorStr("box"), &config_system.item.player_box);
					ImGui::ColorEdit4(XorStr("box color"), config_system.item.clr_box, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox(XorStr("health"), &config_system.item.player_health);
					ImGui::Checkbox(XorStr("weapon"), &config_system.item.player_weapon);
					ImGui::ColorEdit4(XorStr("weapon color"), config_system.item.clr_weapon, ImGuiColorEditFlags_NoInputs);

					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.visuals_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					if (ImGui::BeginCombo(XorStr("flags"), XorStr("..."), ImVec2(0, 105)))
					{
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(XorStr("armor"), &config_system.item.player_flags_armor, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(XorStr("bomb"), &config_system.item.player_flags_c4, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(XorStr("flashed"), &config_system.item.player_flags_flashed, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(XorStr("money"), &config_system.item.player_flags_money, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(XorStr("scoped"), &config_system.item.player_flags_scoped, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);

						ImGui::EndCombo();
					}
					ImGui::PopStyleColor();
					ImGui::Checkbox(XorStr("footstep"), &config_system.item.sound_footstep);
					ImGui::ColorEdit4(XorStr("footstep color"), config_system.item.clr_footstep, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox(XorStr("skeleton"), &config_system.item.skeleton);
					//ImGui::Checkbox(XorStr("preview visuals"), &config_system.item.visuals_preview);
				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild(XorStr("preview visuals"), ImVec2(279, 267), true); {
					ImGui::Checkbox(XorStr("force crosshair"), &config_system.item.force_crosshair);
					ImGui::Checkbox(XorStr("modulate crosshair color"), &config_system.item.crosshair_color);
					ImGui::ColorEdit4(XorStr("##crosshair color"), config_system.item.clr_crosshair, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox(XorStr("modulate crosshair outline color"), &config_system.item.crosshair_outline_color);
					ImGui::ColorEdit4(XorStr("##crosshair outline color"), config_system.item.clr_crosshair_outline, ImGuiColorEditFlags_NoInputs);

					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.visuals_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::SliderInt(XorStr("viewmodel field of view"), &config_system.item.viewmodel_fov, 0, 135);
					ImGui::SliderInt(XorStr("field of view"), &config_system.item.fov, 0, 60);
					ImGui::PopStyleColor();
					ImGui::Checkbox(XorStr("night mode"), &config_system.item.nightmode);
					ImGui::ColorEdit4(XorStr("sky color"), config_system.item.clr_sky, ImGuiColorEditFlags_NoInputs);
					if (config_system.item.nightmode) {
						ImGui::SliderInt(XorStr("brightness"), &config_system.item.nightmode_brightness, 0, 100);
					}

					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.visuals_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					if (ImGui::BeginCombo(XorStr("removals"), XorStr("..."), ImVec2(0, 105))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(XorStr("smoke"), &config_system.item.remove_smoke, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(XorStr("flash"), &config_system.item.reduce_flash, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(XorStr("sleeves"), &config_system.item.remove_sleeves, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(XorStr("hands"), &config_system.item.remove_hands, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(XorStr("scope overlay"), &config_system.item.remove_scope, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}

					if (ImGui::BeginCombo(XorStr("world"), XorStr("..."), ImVec2(0, 105))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable((XorStr("planted bomb")), &config_system.item.bomb_planted, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable((XorStr("dropped weapons")), &config_system.item.dropped_weapons, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable((XorStr("projectiles")), &config_system.item.projectiles, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable((XorStr("misc entities")), &config_system.item.entity_esp, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable((XorStr("danger zone")), &config_system.item.danger_zone_dropped, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}
					ImGui::PopStyleColor();
				}
				ImGui::EndChild(true);

				ImGui::NextColumn();

				ImGui::BeginChild(XorStr("glow"), ImVec2(279, 268), true); {
					ImGui::Checkbox(XorStr("active"), &config_system.item.visuals_glow);
					ImGui::Checkbox(XorStr("enemy"), &config_system.item.visuals_glow_enemy);
					ImGui::ColorEdit4(XorStr("glow color"), config_system.item.clr_glow, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox(XorStr("teammate"), &config_system.item.visuals_glow_team);
					ImGui::ColorEdit4(XorStr("glow color team"), config_system.item.clr_glow_team, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox(XorStr("planted bomb"), &config_system.item.visuals_glow_planted);
					ImGui::ColorEdit4(XorStr("glow color planted"), config_system.item.clr_glow_planted, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox(XorStr("dropped weapons"), &config_system.item.visuals_glow_weapons);
					ImGui::ColorEdit4(XorStr("glow color weapons"), config_system.item.clr_glow_dropped, ImGuiColorEditFlags_NoInputs);

				}
				ImGui::EndChild(true);

				ImGui::Spacing();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);

				ImGui::BeginChild(XorStr("chams"), ImVec2(279, 267), true); {
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.visuals_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::Combo(XorStr("chams type"), &config_system.item.vis_chams_type, XorStr("textured\0flat\0metallic\0pulsating"));
					ImGui::PopStyleColor();
					ImGui::Checkbox(XorStr("enemy"), &config_system.item.vis_chams_vis);
					ImGui::ColorEdit4(XorStr("enemy color"), config_system.item.clr_chams_vis, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox(XorStr("enemy (behind wall)"), &config_system.item.vis_chams_invis);
					ImGui::ColorEdit4(XorStr("enemy (behind wall) color"), config_system.item.clr_chams_invis, ImGuiColorEditFlags_NoInputs);

					ImGui::Checkbox(XorStr("teammate"), &config_system.item.vis_chams_vis_teammate);
					ImGui::ColorEdit4(XorStr("teammate color"), config_system.item.clr_chams_vis_teammate, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox(XorStr("teammate (behind wall)"), &config_system.item.vis_chams_invis_teammate);
					ImGui::ColorEdit4(XorStr("teammate (behind wall) color"), config_system.item.clr_chams_invis_teammate, ImGuiColorEditFlags_NoInputs);

#ifdef debug_build

					ImGui::Checkbox("backtrack", &config_system.item.backtrack_chams);
#endif
					ImGui::Checkbox(XorStr("smoke check"), &config_system.item.vis_chams_smoke_check);
				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				//ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::Columns();
				break;
			case 2:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				//ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

				ImGui::BeginChild(XorStr("misc"), ImVec2(279, 543), true); {
					ImGui::Checkbox(XorStr("active"), &config_system.item.misc_enabled);
					ImGui::Checkbox(XorStr("clantag spammer"), &config_system.item.clan_tag);
					ImGui::Checkbox(XorStr("engine radar"), &config_system.item.radar);

					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.misc_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					if (ImGui::BeginCombo(XorStr("logs"), XorStr("..."), ImVec2(0, 65))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable((XorStr("player hurt")), &config_system.item.logs_player_hurt, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable((XorStr("player bought")), &config_system.item.logs_player_bought, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable((XorStr("config system")), &config_system.item.logs_config_system, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}
					ImGui::PopStyleColor();

					ImGui::Checkbox(XorStr("hitmarker"), &config_system.item.hitmarker);
					if (config_system.item.hitmarker) {
						ImGui::Combo(XorStr("hitmarker sound"), &config_system.item.hitmarker_sound, XorStr("none\0one\0two\0three"));
					}
					ImGui::Checkbox(XorStr("anti screenshot"), &config_system.item.anti_screenshot);
					ImGui::Checkbox(XorStr("spectators"), &config_system.item.spectators_list);
					ImGui::Checkbox(XorStr("watermark"), &config_system.item.watermark);
					ImGui::Checkbox(XorStr("disable post processing"), &config_system.item.disable_post_processing);
					ImGui::Checkbox(XorStr("recoil crosshair"), &config_system.item.recoil_crosshair);
					ImGui::Checkbox(XorStr("rank reveal"), &config_system.item.rank_reveal);

					ImGui::Checkbox(XorStr("viewmodel offset"), &config_system.item.viewmodel_offset);
					if (config_system.item.viewmodel_offset) {
						ImGui::SliderInt(XorStr("viewmodel x"), &config_system.item.viewmodel_x, -10, 10);
						ImGui::SliderInt(XorStr("viewmodel y"), &config_system.item.viewmodel_y, -10, 10);
						ImGui::SliderInt(XorStr("viewmodel z"), &config_system.item.viewmodel_z, -10, 10);

					}

					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.misc_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
					if (ImGui::Button(XorStr("dump steam id"), ImVec2(84, 18))) {
						utilities::dump_steam_id();
					}

					ImGui::SameLine();
					if (ImGui::Button(XorStr("hide name"), ImVec2(84, 18))) {
						utilities::change_name(XorStr("\n\xAD\xAD\xAD"));
					}
					ImGui::PopStyleColor();

				}
				ImGui::EndChild(true);
				ImGui::NextColumn();

				ImGui::BeginChild(XorStr("movement"), ImVec2(279, 543), true); {
					ImGui::Checkbox(XorStr("bunny hop"), &config_system.item.bunny_hop);
					if (config_system.item.bunny_hop) {
						ImGui::SliderInt(XorStr("hitchance"), &config_system.item.bunny_hop_hitchance, 0, 100);
						ImGui::SliderInt(XorStr("minimum hops"), &config_system.item.bunny_hop_minimum_value, 0, 20);
						ImGui::SliderInt(XorStr("maximum hops"), &config_system.item.bunny_hop_maximum_value, 0, 20);
					}


					ImGui::Checkbox(XorStr("edge jump"), &config_system.item.edge_jump);
					if (config_system.item.edge_jump) {
						ImGui::Checkbox(XorStr("duck in air"), &config_system.item.edge_jump_duck_in_air);
					}
				}
				ImGui::EndChild(true);
				ImGui::PopStyleVar();
				//ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::Columns();
				break;
			case 3:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				//ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
				ImGui::BeginChild(XorStr("knife"), ImVec2(279, 268), true); {
					ImGui::Checkbox(XorStr("enable"), &config_system.item.skinchanger_enable);
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.skinchanger_enable ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::Combo(XorStr("knife"), &config_system.item.knife_model, XorStr("default\0bayonet\0m9\0karambit\0bowie\0butterfly\0falchion\0flip\0gut\0huntsman\0shaddow daggers\0navaja\0stiletto\0talon\0ursus"));
					ImGui::Combo(XorStr("condition"), &config_system.item.knife_wear, XorStr("factory new\0minimal wear\0field-tested\0well-worn\0battle-scarred"));

					ImGui::Combo(XorStr("skin"), &config_system.item.paint_kit_vector_index_knife, [](void* data, int idx, const char** out_text) {
						*out_text = parser_skins[idx].name.c_str();
						return true;
					}, nullptr, parser_skins.size(), 10);
					config_system.item.paint_kit_index_knife = parser_skins[config_system.item.paint_kit_vector_index_knife].id;
					ImGui::PopStyleColor();

					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
					if (ImGui::Button(XorStr("force update"), ImVec2(84, 18))) {
						utilities::force_update();
					}

				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild(XorStr("gloves"), ImVec2(279, 267), true); {
					ImGui::Checkbox(XorStr("enable"), &config_system.item.glovechanger_enable);
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.glovechanger_enable ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::Combo(XorStr("type"), &config_system.item.glove_model, XorStr("default\0blood\0sport\0slick\0leather\0moto\0specin\0hydra"));
					ImGui::Combo(XorStr("skin"), &config_system.item.paint_kit_vector_index_glove, [](void* data, int idx, const char** out_text) {
						*out_text = parser_gloves[idx].name.c_str();
						return true;
						}, nullptr, parser_gloves.size(), 10);
					config_system.item.paint_kit_index_glove = parser_gloves[config_system.item.paint_kit_vector_index_glove].id;
					ImGui::Combo(XorStr("condition"), &config_system.item.glove_wear, XorStr("factory new\0minimal wear\0field-tested\0well-worn\0battle-scarred"));
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
					if (ImGui::Button(XorStr("force update"), ImVec2(84, 18))) {
						utilities::force_update();
					}
					ImGui::PopStyleColor();

				}
				ImGui::EndChild(true);

				ImGui::NextColumn();

				ImGui::BeginChild(XorStr("weapons"), ImVec2(279, 543), true); {
					static int weapons_page = 0;
					if (ImGui::ButtonT(XorStr("pistol"), ImVec2(50, 30), weapons_page, 0, false, ImColor(0, 0, 0))) weapons_page = 0; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT(XorStr("rifle"), ImVec2(50, 30), weapons_page, 1, false, false)) weapons_page = 1; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT(XorStr("sniper"), ImVec2(50, 30), weapons_page, 2, false, false)) weapons_page = 2; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT(XorStr("smg"), ImVec2(50, 30), weapons_page, 3, false, false)) weapons_page = 3; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT(XorStr("heavy"), ImVec2(50, 30), weapons_page, 4, false, false)) weapons_page = 4;

					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.skinchanger_enable ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					switch (weapons_page) {
					case 0:
						ImGui::Combo((XorStr("p2000")), &config_system.item.paint_kit_vector_index_p2000, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_p2000 = parser_skins[config_system.item.paint_kit_vector_index_p2000].id;


						ImGui::Combo((XorStr("usp-s")), &config_system.item.paint_kit_vector_index_usp, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_usp = parser_skins[config_system.item.paint_kit_vector_index_usp].id;

						ImGui::Combo((XorStr("glock")), &config_system.item.paint_kit_vector_index_glock, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_glock = parser_skins[config_system.item.paint_kit_vector_index_glock].id;

						ImGui::Combo((XorStr("p250")), &config_system.item.paint_kit_vector_index_p250, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_p250 = parser_skins[config_system.item.paint_kit_vector_index_p250].id;

						ImGui::Combo((XorStr("five-seven")), &config_system.item.paint_kit_vector_index_fiveseven, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_fiveseven = parser_skins[config_system.item.paint_kit_vector_index_fiveseven].id;

						ImGui::Combo((XorStr("tec9")), &config_system.item.paint_kit_vector_index_tec, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_tec = parser_skins[config_system.item.paint_kit_vector_index_tec].id;

						ImGui::Combo((XorStr("cz75a")), &config_system.item.paint_kit_vector_index_cz, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_cz = parser_skins[config_system.item.paint_kit_vector_index_cz].id;

						ImGui::Combo((XorStr("duals")), &config_system.item.paint_kit_vector_index_duals, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_duals = parser_skins[config_system.item.paint_kit_vector_index_duals].id;

						ImGui::Combo((XorStr("deagle")), &config_system.item.paint_kit_vector_index_deagle, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_deagle = parser_skins[config_system.item.paint_kit_vector_index_deagle].id;

						ImGui::Combo((XorStr("revolver")), &config_system.item.paint_kit_vector_index_revolver, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_revolver = parser_skins[config_system.item.paint_kit_vector_index_revolver].id;

						break;
					case 1:
						ImGui::Combo((XorStr("famas")), &config_system.item.paint_kit_vector_index_famas, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_famas = parser_skins[config_system.item.paint_kit_vector_index_famas].id;

						ImGui::Combo((XorStr("galil")), &config_system.item.paint_kit_vector_index_galil, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_galil = parser_skins[config_system.item.paint_kit_vector_index_galil].id;

						ImGui::Combo((XorStr("m4a4")), &config_system.item.paint_kit_vector_index_m4a4, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_m4a4 = parser_skins[config_system.item.paint_kit_vector_index_m4a4].id;

						ImGui::Combo(XorStr("m4a1"), &config_system.item.paint_kit_vector_index_m4a1, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_m4a1 = parser_skins[config_system.item.paint_kit_vector_index_m4a1].id;

						ImGui::Combo(XorStr("ak47"), &config_system.item.paint_kit_vector_index_ak47, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_ak47 = parser_skins[config_system.item.paint_kit_vector_index_ak47].id;

						ImGui::Combo(XorStr("sg 553"), &config_system.item.paint_kit_vector_index_sg553, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_sg553 = parser_skins[config_system.item.paint_kit_vector_index_sg553].id;

						ImGui::Combo(XorStr("aug"), &config_system.item.paint_kit_vector_index_aug, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_aug = parser_skins[config_system.item.paint_kit_vector_index_aug].id;

						break;
					case 2:
						ImGui::Combo(XorStr("ssg08"), &config_system.item.paint_kit_vector_index_ssg08, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_ssg08 = parser_skins[config_system.item.paint_kit_vector_index_ssg08].id;

						ImGui::Combo(XorStr("awp"), &config_system.item.paint_kit_vector_index_awp, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_awp = parser_skins[config_system.item.paint_kit_vector_index_awp].id;

						ImGui::Combo(XorStr("scar20"), &config_system.item.paint_kit_vector_index_scar, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_scar = parser_skins[config_system.item.paint_kit_vector_index_scar].id;

						ImGui::Combo(XorStr("g3sg1"), &config_system.item.paint_kit_vector_index_g3sg1, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_g3sg1 = parser_skins[config_system.item.paint_kit_vector_index_g3sg1].id;

						break;
					case 3:
						break;
					case 4:
						break;

					}
					ImGui::PopStyleColor();
				}
				ImGui::EndChild(true);
				ImGui::PopStyleVar();
				//ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::Columns();
				break;

			case 4:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				//ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
				ImGui::BeginChild(XorStr("config"), ImVec2(279, 268), true); {
					constexpr auto& config_items = config_system.get_configs();
					static int current_config = -1;

					if (static_cast<size_t>(current_config) >= config_items.size())
						current_config = -1;

					static char buffer[16];

					if (ImGui::ListBox("", &current_config, [](void* data, int idx, const char** out_text) {
						auto& vector = *static_cast<std::vector<std::string>*>(data);
						*out_text = vector[idx].c_str();
						return true;
						}, & config_items, config_items.size(), 5) && current_config != -1)
						strcpy(buffer, config_items[current_config].c_str());

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
						ImGui::PushID(0);
						ImGui::PushItemWidth(178);
						if (ImGui::InputText("", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
							if (current_config != -1)
								config_system.rename(current_config, buffer);
						}
						ImGui::PopID();
						ImGui::NextColumn();

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
						if (ImGui::Button((XorStr("create")), ImVec2(85, 20))) {
							config_system.add(buffer);
						}

						ImGui::SameLine();

						if (ImGui::Button((XorStr("reset")), ImVec2(85, 20))) {
							config_system.reset();
						}

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
						if (current_config != -1) {
							if (ImGui::Button((XorStr("load")), ImVec2(85, 20))) {
								config_system.load(current_config);

								load_config = true;

								if (config_system.item.logs_config_system) {
									utilities::console_warning(XorStr("[config system] "));
									interfaces::console->console_printf(config_items[current_config].c_str());
									interfaces::console->console_printf(XorStr(" loaded. \n"));
									std::stringstream ss;
									ss << config_items[current_config].c_str() << XorStr(" loaded.");
									event_logs.add(ss.str(), color(167, 255, 255, 255));
								}

							}

							ImGui::SameLine();

							if (ImGui::Button(XorStr("save"), ImVec2(85, 20))) {
								config_system.save(current_config);

								save_config = true;

								if (config_system.item.logs_config_system) {
									utilities::console_warning(XorStr("[config system] "));
									interfaces::console->console_printf(config_items[current_config].c_str());
									interfaces::console->console_printf(XorStr(" saved. \n"));
									std::stringstream ss;
									ss << config_items[current_config].c_str() << " saved.";
									event_logs.add(ss.str(), color(167, 255, 255, 255));
								}

							}

							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
							if (ImGui::Button((XorStr("remove")), ImVec2(85, 20))) {
								config_system.remove(current_config);
							}
						}
				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild(XorStr("settings"), ImVec2(279, 267), true); {
					ImGui::Combo(XorStr("keybinds"), &config_system.item.keybinds_selection, XorStr("edge jump\0aimbot key\0visuals key"));

					if (config_system.item.keybinds_selection == 0) {
						ImGui::Hotkey(XorStr("##edge jump key"), &config_system.item.edge_jump_key, ImVec2(100, 20));
					}

					else if (config_system.item.keybinds_selection == 1) {
						ImGui::Hotkey(XorStr("##aimbot key"), &config_system.item.aim_key, ImVec2(100, 20));
					}

					else if (config_system.item.keybinds_selection == 2) {
						ImGui::Hotkey(XorStr("##visuals key"), &config_system.item.visuals_key, ImVec2(100, 20));
					}
				}
				ImGui::EndChild(true);

				ImGui::NextColumn();

				/*ImGui::BeginChild("info", ImVec2(279, 543), true); {
					char buffer[UNLEN + 1];
					DWORD size;
					size = sizeof(buffer);
					GetUserName(buffer, &size);
					char title[UNLEN];
					char ch1[25] = "welcome, ";
					char *ch = strcat(ch1, buffer);

					ImGui::Text(ch);
					ImGui::Text("build: " __DATE__ " / " __TIME__);
					if (std::strstr(GetCommandLineA(), "-insecure")) {
						ImGui::Text("insecure mode!");
					}
				}
				ImGui::EndChild(true);*/
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				//ImGui::PopStyleColor();
				ImGui::Columns();
				break;
			}
		}

		ImGui::PopFont();

		ImGui::End();
	}
}

void c_menu::run_popup() {
	ImGui::PushFont(font_menu);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
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
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	ImGui::PopFont();
}

void c_menu::run_visuals_preview() {
	static std::vector<esp_info_s> info;
	static bool enabled = true;

	ImGui::SetNextWindowSize(ImVec2(235, 400));

	ImGui::Begin(XorStr("ESP Preview"), &enabled, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar); {
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


		if (config_system.item.player_box) {
			cur_window->DrawList->AddRect(ImVec2(w_pos.x + 40, w_pos.y + 60), ImVec2(w_pos.x + 200, w_pos.y + 360), ImGui::GetColorU32(ImGuiCol_Text));
		}

		if (config_system.item.player_health)
			cur_window->DrawList->AddRectFilled(ImVec2(w_pos.x + 34, w_pos.y + 60), ImVec2(w_pos.x + 36, w_pos.y + 360), ImGui::GetColorU32(ImVec4(83 / 255.f, 200 / 255.f, 84 / 255.f, 255 / 255.f)));

		if (config_system.item.player_name)
			info.emplace_back(esp_info_s(XorStr("name"), color(255, 255, 255, 255), CENTER_UP));

		if (config_system.item.player_weapon)
			info.emplace_back(esp_info_s(XorStr("awp"), color(255, 255, 255, 255), CENTER_DOWN));

		if (config_system.item.player_flags_armor)
			info.emplace_back(esp_info_s(XorStr("hk"), color(255, 255, 255, 255), RIGHT));


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
	ImGui::End();

	info.clear();
}
