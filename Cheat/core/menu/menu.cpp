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

void c_menu::run() {
	static int page = 0;

	if (opened) 
	{
		static bool expiredSkin = false;
		static bool expiredEsp = false;
		ImGui::SetNextWindowSize(ImVec2(700, 730), ImGuiSetCond_FirstUseEver);
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

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5);
			if (ImGui::ButtonT(XorStr("legit bot"), ImVec2(60, 30), page, 0, false, false)) page = 0; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT(XorStr("visuals"), ImVec2(50, 30), page, 1, false, false)) page = 1; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT(XorStr("misc"), ImVec2(40, 30), page, 2, false, false)) page = 2; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT(XorStr("skin changer"), ImVec2(100, 30), page, 3, false, false)) page = 3; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT(XorStr("config & info"), ImVec2(90, 30), page, 4, false, false)) page = 4; ImGui::SameLine(0, 0);
			ImGui::PopFont();

			ImGui::PushFont(font_menu);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 45);
			//ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 222);

			//if (config_system.get_config().visuals_preview) {
			//	run_visuals_preview();
			//}

			static int test = 0;
			switch (page) 
			{
			case 0:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
				ImGui::BeginChild(XorStr("aimbot [globals]"), ImVec2(339, 300), true); {
					if (license_manager::checkModuleActive(globals::user_modules, MODULE_AIM_BOT))
					{
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Checkbox("dynamic fov", &config_system.get_config().legit.distance_based_fov);
						ImGui::Checkbox("aim in scope only", &config_system.get_config().legit.scope_aim);
						ImGui::Checkbox("smoke check", &config_system.get_config().legit.smoke_check);
						ImGui::Checkbox("friendly fire", &config_system.get_config().legit.team_check);
						ImGui::Checkbox("auto pistol", &config_system.get_config().legit.auto_pistol);
						ImGui::SliderInt("kill delay", &config_system.get_config().legit.kill_delay, 0, 350);
					}
					else
					{
						ImGui::Text("AIM BOT EXPIRED!");
					}
				} ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(18, 18));

				ImGui::BeginChild("extras", ImVec2(339, 360), true); 
				{
					if (license_manager::checkModuleActive(globals::user_modules, MODULE_AIM_BOT))
					{
						ImGui::Spacing();
						ImGui::Checkbox("backtrack", &config_system.get_config().legit.backtrack.enable);
						ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_config().legit.backtrack.enable ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
						if (config_system.get_config().legit.backtrack.enable)
						{
							ImGui::SliderFloat("records (ms)", &config_system.get_config().legit.backtrack.backtrack_ms, 1.0f, 200.0f, "%.2f");
						}
						ImGui::PopStyleColor();
						ImGui::Checkbox("standalone rcs", &config_system.get_config().legit.rcs_standalone.enabled);
						if (config_system.get_config().legit.rcs_standalone.enabled) {
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
							ImGui::SliderFloat("rcs amount x", &config_system.get_config().legit.rcs_standalone.rcs_x, 0.0f, 1.0f, "%.2f");
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
							ImGui::SliderFloat("rcs amount y", &config_system.get_config().legit.rcs_standalone.rcs_y, 0.0f, 1.0f, "%.2f");
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
						}
					}
				}
				ImGui::EndChild(true);

				ImGui::NextColumn();

				ImGui::BeginChild(("settings [" + std::string(config_system.current_weapon_name) + "]").c_str(), ImVec2(339, 670), true); {

					if (license_manager::checkModuleActive(globals::user_modules, MODULE_AIM_BOT))
					{
						ImGui::PushFont(font_menu);

						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Checkbox("aimbot enable", &config_system.get_active_weapon(config_system.current_wepon_id).legit_settings.enable);
						if(config_system.get_active_weapon(config_system.current_wepon_id).legit_settings.enable)
						{
							ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_active_weapon(config_system.current_wepon_id).legit_settings.enable ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));

							ImGui::Checkbox("silent", &config_system.get_active_weapon(config_system.current_wepon_id).legit_settings.silent);
							ImGui::Checkbox("nearest", &config_system.get_active_weapon(config_system.current_wepon_id).legit_settings.nearest);


							if (!config_system.get_active_weapon(config_system.current_wepon_id).legit_settings.nearest)
							{
								ImGui::Combo(XorStr("hitbox"), &config_system.get_active_weapon(config_system.current_wepon_id).legit_settings.hitbox, XorStr("head\0neck\0chest\0stomach\0pelvis\0"));
							}
							ImGui::SliderFloat(XorStr("fov"), &config_system.get_active_weapon(config_system.current_wepon_id).legit_settings.fov, 0.0f, 180.0f, XorStr("%.2f"));
							ImGui::SliderFloat(XorStr("smooth"), &config_system.get_active_weapon(config_system.current_wepon_id).legit_settings.smooth, 1.f, 10.f, XorStr("%.2f"));
							ImGui::SliderFloat(XorStr("rcs x"), &config_system.get_active_weapon(config_system.current_wepon_id).legit_settings.rcs_x, 0.0f, 1.0f, XorStr("%.2f"));
							ImGui::SliderFloat(XorStr("rcs y"), &config_system.get_active_weapon(config_system.current_wepon_id).legit_settings.rcs_y, 0.0f, 1.0f, XorStr("%.2f"));

							ImGui::PopStyleColor();
						}
						ImGui::Spacing();
						ImGui::Spacing();


						if (license_manager::checkModuleActive(globals::user_modules, MODULE_TRIGGER_BOT))
						{
							ImGui::Checkbox("triggerbot enable", &config_system.get_active_weapon(config_system.current_wepon_id).trigger_settings.enable);
							if(config_system.get_active_weapon(config_system.current_wepon_id).trigger_settings.enable)
							{
								ImGui::Checkbox("on key", &config_system.get_active_weapon(config_system.current_wepon_id).trigger_settings.on_key);
								if(config_system.get_active_weapon(config_system.current_wepon_id).trigger_settings.on_key)
								{
									ImGui::Hotkey("##triger key", &config_system.get_active_weapon(config_system.current_wepon_id).trigger_settings.key_id, ImVec2(130, 30));
								}
								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
								if (ImGui::BeginCombo("hitboxes", "...", ImVec2(0, 105))) {
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
									ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
									ImGui::Selectable(("head"), &config_system.get_active_weapon(config_system.current_wepon_id).trigger_settings.hitbox_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
									ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
									ImGui::Selectable(("body"), &config_system.get_active_weapon(config_system.current_wepon_id).trigger_settings.hitbox_body, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
									ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
									ImGui::Selectable(("arms"), &config_system.get_active_weapon(config_system.current_wepon_id).trigger_settings.hitbox_arms, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
									ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
									ImGui::Selectable(("legs"), &config_system.get_active_weapon(config_system.current_wepon_id).trigger_settings.hitbox_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
									ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
									ImGui::EndCombo();
									ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
								}
								ImGui::SliderInt("trigger delay", &config_system.get_active_weapon(config_system.current_wepon_id).trigger_settings.delay, 1, 50);
								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
								ImGui::Checkbox("trigger recoil", &config_system.get_active_weapon(config_system.current_wepon_id).trigger_settings.rcs);
								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
							}

						}
						else
						{
							ImGui::Text("TRIGGER BOT EXPIRED!");
						}
						ImGui::PopFont();
					}
					
				} ImGui::EndChild(true);
				ImGui::PopStyleColor();
				ImGui::PopStyleVar();
				break;
			case 1:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
				ImGui::BeginChild("player", ImVec2(339, 340), true); 
				{
					ImGui::Spacing();
					ImGui::Spacing();
					if (license_manager::checkModuleActive(globals::user_modules, MODULE_WALLHACK))
					{
						ImGui::Checkbox("global active", &config_system.get_config().visuals.global_enabled);
						if (config_system.get_config().visuals.global_enabled) {
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 7);
							ImGui::Checkbox("teammate", &config_system.get_config().visuals.show_team);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 7);
							ImGui::Checkbox("visible only", &config_system.get_config().visuals.visible_only);
						}
						ImGui::Checkbox("name", &config_system.get_config().visuals.player.name);
						ImGui::ColorEdit4("name color", reinterpret_cast<float*>(&config_system.get_config().colors.esp_name), ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox("box", &config_system.get_config().visuals.player.box);
						ImGui::ColorEdit4("box color", reinterpret_cast<float*>(&config_system.get_config().colors.esp_box), ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox("health", &config_system.get_config().visuals.player.health);
						ImGui::Checkbox("weapon name", &config_system.get_config().visuals.player.weapon_name);
						ImGui::ColorEdit4("weapon color", reinterpret_cast<float*>(&config_system.get_config().colors.weapons), ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox("weapon icon", &config_system.get_config().visuals.player.weapon_icon);
						ImGui::ColorEdit4("weapon icon color", reinterpret_cast<float*>(&config_system.get_config().colors.esp_weapon_icon), ImGuiColorEditFlags_NoInputs);

						ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_config().visuals.global_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
						if (ImGui::BeginCombo("flags", "...", ImVec2(0, 105))) {
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("bombcarrier"), &config_system.get_config().visuals.player.flags.bombcarrier, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("is flashed"), &config_system.get_config().visuals.player.flags.is_flashed, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("is scoped"), &config_system.get_config().visuals.player.flags.is_scoped, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("is defusing"), &config_system.get_config().visuals.player.flags.is_defusing, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("position callout"), &config_system.get_config().visuals.player.flags.position_callout, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("defusekit"), &config_system.get_config().visuals.player.flags.defuse_kit, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("armor"), &config_system.get_config().visuals.player.flags.armor, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("money $$"), &config_system.get_config().visuals.player.flags.money, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);

							ImGui::EndCombo();
						}
						ImGui::PopStyleColor();
						ImGui::Checkbox("footstep", &config_system.get_config().visuals.player.footsteps);
						ImGui::ColorEdit4("footstep color", reinterpret_cast<float*>(&config_system.get_config().colors.esp_footsteps), ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox("skeleton", &config_system.get_config().visuals.player.skeleton);
						//ImGui::Checkbox("backtrack skeleton", &config_system.get_config().backtrack_skeleton);
					}
					else
					{
						expiredEsp = true;
						ImGui::Text("ESP EXPIRED!");
					}
				}
				ImGui::EndChild(true);

				if(!expiredEsp)
				{
					ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
					ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();

					ImGui::BeginChild("effects", ImVec2(339, 330), true); {
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Checkbox("force crosshair", &config_system.get_config().visuals.player.force_crosshair);
						ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_config().visuals.global_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
						//ImGui::SliderInt("viewmodel field of view", &config_system.get_config().viewmodel_fov, 0, 135);
						ImGui::SliderInt("field of view", &config_system.get_config().visuals.player.fov, 0, 60);
						ImGui::Checkbox("ambient light", &config_system.get_config().visuals.ambient_light);
						ImGui::ColorEdit4("ambient light color", reinterpret_cast<float*>(&config_system.get_config().colors.ambient), ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox("night mode", &config_system.get_config().visuals.night_mode);
						ImGui::ColorEdit4("sky color", reinterpret_cast<float*>(&config_system.get_config().colors.sky), ImGuiColorEditFlags_NoInputs);
						if (config_system.get_config().visuals.night_mode) {
							ImGui::SliderInt("brightness", &config_system.get_config().visuals.brightness, 0, 100);
						}

						if (ImGui::BeginCombo("removals", "...", ImVec2(0, 105))) {
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("fog"), &config_system.get_config().visuals.removals.fog, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("smoke"), &config_system.get_config().visuals.removals.smoke, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("flash"), &config_system.get_config().visuals.removals.flash, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("sleeves"), &config_system.get_config().visuals.removals.sleeves, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("hands"), &config_system.get_config().visuals.removals.hands, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("scope overlay"), &config_system.get_config().visuals.removals.skope_overlay, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::EndCombo();
						}

						if (ImGui::BeginCombo("world", "...", ImVec2(0, 105))) {
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("planted bomb"), &config_system.get_config().visuals.world.planted_bomb, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("dropped weapons name"), &config_system.get_config().visuals.world.dropped_weapons_name, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("dropped weapons icon"), &config_system.get_config().visuals.world.dropped_weapons_icon, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("projectiles"), &config_system.get_config().visuals.world.projectiles, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("misc entities"), &config_system.get_config().visuals.world.misc_entities, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("danger zone"), &config_system.get_config().visuals.world.danger_zone, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::EndCombo();
						}
						ImGui::PopStyleColor();
					}
					ImGui::EndChild(true);

					ImGui::NextColumn();

					ImGui::BeginChild("helpers", ImVec2(339, 110), true); {
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Checkbox("grenade predition", &config_system.get_config().visuals.helpers.grenade_prediction);

						ImGui::Checkbox("damage indicator", &config_system.get_config().visuals.helpers.damage_indicator);
						ImGui::ColorEdit4("damage indicator color ", reinterpret_cast<float*>(&config_system.get_config().colors.damage_indicator), ImGuiColorEditFlags_NoInputs);
					}
					ImGui::EndChild(true);

					ImGui::BeginChild("glow", ImVec2(339, 200), true); {
						ImGui::Spacing();
						ImGui::Spacing();

						ImGui::Checkbox("active", &config_system.get_config().visuals.glow.enabled);
						ImGui::Checkbox("enemy", &config_system.get_config().visuals.glow.enemy);
						ImGui::ColorEdit4("glow color", reinterpret_cast<float*>(&config_system.get_config().colors.glow_enemy), ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox("teammate", &config_system.get_config().visuals.glow.temmmate);
						ImGui::ColorEdit4("glow color team", reinterpret_cast<float*>(&config_system.get_config().colors.glow_team), ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox("planted bomb", &config_system.get_config().visuals.glow.planted_bomb);
						ImGui::ColorEdit4("glow color planted", reinterpret_cast<float*>(&config_system.get_config().colors.nades), ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox("dropped weapons", &config_system.get_config().visuals.glow.dropped_weapons);
						//ImGui::ColorEdit4("glow color weapons", reinterpret_cast<float*>(&config_system.get_config().colors.nades), ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox("dropped nades", &config_system.get_config().visuals.glow.nades);
						//ImGui::ColorEdit4("glow color nades", reinterpret_cast<float*>(&config_system.get_config().colors.nades), ImGuiColorEditFlags_NoInputs);

					}
					ImGui::EndChild(true);

					ImGui::Spacing();

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);

					ImGui::BeginChild("chams", ImVec2(339, 350), true); {
						ImGui::Spacing();
						ImGui::Spacing();

						ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_config().visuals.global_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
						ImGui::Combo("material", &config_system.get_config().visuals.chams.material, "textured\0flat\0metallic\0pulsating\0");
						ImGui::PopStyleColor();
						ImGui::Spacing();
						ImGui::Checkbox("smoke check", &config_system.get_config().visuals.chams.smoke_check);
						ImGui::Spacing();
						ImGui::Checkbox("enemy", &config_system.get_config().visuals.chams.enemy);
						ImGui::ColorEdit4("enemy color", reinterpret_cast<float*>(&config_system.get_config().colors.chams_enemy_visible), ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox("enemy (behind wall)", &config_system.get_config().visuals.chams.enemy_xoz);
						ImGui::ColorEdit4("enemy (behind wall) color", reinterpret_cast<float*>(&config_system.get_config().colors.chams_enemy_invisible), ImGuiColorEditFlags_NoInputs);

						ImGui::Checkbox("teammate", &config_system.get_config().visuals.chams.temmmate);
						ImGui::ColorEdit4("teammate color", reinterpret_cast<float*>(&config_system.get_config().colors.chams_team_visible), ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox("teammate (behind wall)", &config_system.get_config().visuals.chams.temmate_xoz);
						ImGui::ColorEdit4("teammate (behind wall) color", reinterpret_cast<float*>(&config_system.get_config().colors.chams_enemy_visible), ImGuiColorEditFlags_NoInputs);

						ImGui::Checkbox("weapon chams", &config_system.get_config().visuals.chams.weapon);
						ImGui::ColorEdit4("weapon chams color", reinterpret_cast<float*>(&config_system.get_config().colors.chams_hand_weapon), ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox("hand chams", &config_system.get_config().visuals.chams.hands);
						ImGui::ColorEdit4("hand chams color", reinterpret_cast<float*>(&config_system.get_config().colors.chams_hands), ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox("sleeve chams", &config_system.get_config().visuals.chams.sleeve);
						ImGui::ColorEdit4("sleeve chams color", reinterpret_cast<float*>(&config_system.get_config().colors.chams_sleeve), ImGuiColorEditFlags_NoInputs);
						ImGui::Spacing();
						ImGui::Checkbox("backtrack player", &config_system.get_config().visuals.chams.backtrack);

					}
					ImGui::EndChild(true);
				}

				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::Columns();
				break;
			case 2:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

				ImGui::BeginChild("misc", ImVec2(339, 670), true); 
				{
					if (license_manager::checkModuleActive(globals::user_modules, MODULE_WALLHACK))
					{
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Checkbox("active", &config_system.get_config().misc.global_active);
						ImGui::Checkbox("clantag", &config_system.get_config().misc.clantag);
						ImGui::Checkbox("radar", &config_system.get_config().misc.radar);

						ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_config().misc.global_active ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
						if (ImGui::BeginCombo("logs", "...", ImVec2(0, 65))) {
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("player hurt"), &config_system.get_config().misc.logs.hurt, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("player bought"), &config_system.get_config().misc.logs.bouth, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::EndCombo();
						}
						ImGui::PopStyleColor();

						ImGui::Checkbox("hitmarker", &config_system.get_config().misc.hitmarker.enabled);
						if (config_system.get_config().misc.hitmarker.enabled) {
							ImGui::Combo("hitmarker sound", &config_system.get_config().misc.hitmarker.sound, "none\0one\0two\0three\0");
						}
						ImGui::Checkbox("anti screenshot", &config_system.get_config().misc.anti_screenshoot);
						ImGui::Checkbox("spectators", &config_system.get_config().misc.spectators);
						//ImGui::Checkbox("watermark", &config_system.get_config().watermark);
						ImGui::Checkbox("disable post processing", &config_system.get_config().misc.disable_post_processing);
						ImGui::Checkbox("recoil crosshair", &config_system.get_config().misc.recoil_crosshair);
						ImGui::Checkbox("rank reveal", &config_system.get_config().misc.radar);

						//ImGui::Checkbox("viewmodel offset", &config_system.get_config().viewmodel_offset);
						//if (config_system.get_config().viewmodel_offset) {
						//	ImGui::SliderInt("viewmodel x", &config_system.get_config().viewmodel_x, -10, 10);
						//	ImGui::SliderInt("viewmodel y", &config_system.get_config().viewmodel_y, -10, 10);
						//	ImGui::SliderInt("viewmodel z", &config_system.get_config().viewmodel_z, -10, 10);

						//}

						//ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_config().misc_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
						//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
						//if (ImGui::Button("dump steam id")) {
						//	utilities::dump_steam_id();
						//}
						if (ImGui::Button("hide name")) {
							utilities::change_name("\n\xAD\xAD\xAD");
						}
					}
				}
				ImGui::EndChild(true);
				ImGui::NextColumn();

				ImGui::BeginChild("BHOP", ImVec2(339, 670), true); {
					ImGui::Spacing();
					ImGui::Spacing();
					if (license_manager::checkModuleActive(globals::user_modules, MODULE_BUNNY_HOP))
					{
						ImGui::Checkbox("bunny hop", &config_system.get_config().bhop.enabled);
						if (config_system.get_config().bhop.enabled) {
							ImGui::Checkbox("auto strafe", &config_system.get_config().bhop.auto_strafe);
							//ImGui::SliderInt("hitchance", &config_system.get_config().bunny_hop_hitchance, 0, 100);
							//ImGui::SliderInt("minimum hops", &config_system.get_config().bunny_hop_minimum_value, 0, 20);
							//ImGui::SliderInt("maximum hops", &config_system.get_config().bunny_hop_maximum_value, 0, 20);
						}


						ImGui::Checkbox("edge jump", &config_system.get_config().bhop.edge_jump.enabled);
						if (config_system.get_config().bhop.edge_jump.enabled) {
							ImGui::Checkbox("duck in air", &config_system.get_config().bhop.edge_jump.duck_in_air);
						}
					}
					else
					{
						ImGui::Text("BHOP EXPIRED!");
					}
				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::Columns();
				break;
			case 3:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

				ImGui::BeginChild("knife", ImVec2(339, 332), true); {
					ImGui::Spacing();
					ImGui::Spacing();
					if (license_manager::checkModuleActive(globals::user_modules, MODULE_SKIN_CHANGER))
					{
						ImGui::Checkbox("enable", &config_system.get_config().skin_changer.enable_skinchanger);
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_config().skin_changer.enable_skinchanger ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
						ImGui::Combo("model", &config_system.get_config().skin_changer.knife_model, "default\0bayonet\0m9\0karambit\0bowie\0butterfly\0falchion\0flip\0gut\0huntsman\0shaddow daggers\0navaja\0stiletto\0talon\0ursus\0");
						ImGui::Combo(("skin"), &config_system.get_config().skin_changer.knife_skin_vector, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.get_config().skin_changer.knife_skin = parser_skins[config_system.get_config().skin_changer.knife_skin_vector].id;
						ImGui::Spacing();
						ImGui::Combo("condition", &config_system.get_config().skin_changer.knife_wear, "factory new\0minimal wear\0field-tested\0well-worn\0battle-scarred\0");
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
						ImGui::Spacing();
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
						if (ImGui::Button("force update")) {
							utilities::force_update();
						}
						ImGui::PopStyleColor();
					}
					else
					{
						expiredSkin = true;
						ImGui::Text("SKIN CHANGER EXPIRED!");
					}

				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				if(!expiredSkin)
				{
					ImGui::BeginChild("gloves", ImVec2(339, 333), true); {
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Checkbox("enable", &config_system.get_config().skin_changer.enable_glovechanger);
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_config().skin_changer.enable_glovechanger ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
						ImGui::Combo("collection", &config_system.get_config().skin_changer.glove_model, "default\0blood\0sport\0slick\0leather\0moto\0specin\0hydra\0");
						ImGui::Combo(("skin"), &config_system.get_config().skin_changer.glove_skin_vector, [](void* data, int idx, const char** out_text) {
							*out_text = parser_gloves[idx].name.c_str();
							return true;
						}, nullptr, parser_gloves.size(), 10);
						config_system.get_config().skin_changer.glove_skin = parser_gloves[config_system.get_config().skin_changer.glove_skin_vector].id;
						ImGui::Spacing();
						ImGui::Combo("condition", &config_system.get_config().skin_changer.glove_wear, "factory new\0minimal wear\0field-tested\0well-worn\0battle-scarred\0");
						ImGui::Spacing();
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
						if (ImGui::Button("force update")) {
							utilities::force_update();
						}
						ImGui::PopStyleColor();
					}
					ImGui::EndChild(true);

					ImGui::NextColumn();

					ImGui::BeginChild((config_system.current_weapon_name + " settings").c_str(), ImVec2(339, 670), true); {
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::PushStyleColor(ImGuiCol_Text, config_system.get_config().skin_changer.enable_skinchanger ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));

						ImGui::Combo((config_system.current_weapon_name + " skin").c_str(), &config_system.get_active_weapon(config_system.current_wepon_id).skin_info.paint_kit_vector, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);

						if(ImGui::Button("apply skin"))
						{
							config_system.get_active_weapon(config_system.current_wepon_id).skin_info.paint_kit = parser_skins[config_system.get_active_weapon(config_system.current_wepon_id).skin_info.paint_kit_vector].id;
							config_system.get_active_weapon(config_system.current_wepon_id).skin_info.setted = true;
							utilities::force_update();
						}
						ImGui::PopStyleColor();

					}
					ImGui::EndChild(true);
				}

				ImGui::PopStyleVar();
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
				ImGui::BeginChild(XorStr("config"), ImVec2(339, 333), true); {
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
						if (ImGui::Button((XorStr("create")))) {
							config_system.add(buffer);
						}

						ImGui::SameLine();

						if (ImGui::Button((XorStr("reset")))) {
							config_system.reset();
						}

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
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

							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
							if (ImGui::Button((XorStr("remove")))) {
								config_system.remove(current_config);
							}
						}
				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild(XorStr("settings"), ImVec2(339, 332), true); {
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Hotkey(XorStr("edge jump key"), &config_system.get_config().bhop.edge_jump.jump_key, ImVec2(140, 30));
				}
				ImGui::EndChild(true);

				ImGui::NextColumn();

				ImGui::BeginChild("info", ImVec2(339, 670), true); {
					ImGui::Spacing();
					ImGui::Spacing();

					for(unsigned i  = 0; i < globals::user_modules.modules_count; i++)
					{
						std::string moduleName = "";
						std::string expireDate = std::to_string(globals::user_modules.modules_end_date[i]);
						switch(globals::user_modules.modules_ids[i])
						{
						case MODULE_AIM_BOT:
							moduleName = "[AIM BOT]";
							break;
						case MODULE_WALLHACK:
							moduleName = "[ESP & MISC]";
							break;
						case MODULE_BUNNY_HOP:
							moduleName = "[BUNNY HOP]";
							break;
						case MODULE_TRIGGER_BOT:
							moduleName = "[TRIGGER BOT]";
							break;
						case MODULE_SKIN_CHANGER:
							moduleName = "[SKIN CHANGER]";
							break;
						default:
							break;
						}
						if(globals::is_lifetime)
						{
							expireDate = std::string(XorStr("lifetime"));
						}
						else
						{
							tm* time = gmtime((time_t*)&globals::user_modules.modules_end_date[i]);
							if (time != nullptr)
							{
								expireDate =
									std::to_string(time->tm_mday) + "-" +
									std::to_string(time->tm_mon + 1) + "-" +
									std::to_string(time->tm_year + 1900) + " " +
									std::to_string(time->tm_hour + 1) + ":" +
									std::to_string(time->tm_min + 1) + ":" +
									std::to_string(time->tm_sec) + " ";
							}
						}
						ImGui::Text((moduleName + " expired :" + expireDate).c_str());
					}
					//char buffer[UNLEN + 1];
					//DWORD size;
					//size = sizeof(buffer);
					//GetUserName(buffer, &size);
					//char title[UNLEN];
					//char ch1[25] = "welcome, ";
					//char *ch = strcat(ch1, buffer);

					//ImGui::Text(ch);
					//ImGui::Text("build: " __DATE__ " / " __TIME__);
					//if (std::strstr(GetCommandLineA(), "-insecure")) {
					//	ImGui::Text("insecure mode!");
					//}
				}
				ImGui::EndChild(true);
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
