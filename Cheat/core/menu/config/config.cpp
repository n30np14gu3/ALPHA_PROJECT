#include <fstream>
#include <ShlObj.h>
#include "config.hpp"

#include "../../../SDK/jsonip/helper.hpp"
#include "../../../SDK/jsonip/writer.hpp"
#include "../../../SDK/jsonip/parse.hpp"

alpha_config config_system;

alpha_config::weapon_settings& alpha_config::get_active_weapon(size_t weapon_id)
{
	auto it = std::find_if(item.weapons.begin(), item.weapons.end(), weapon_settings(weapon_id));
	if (it != item.weapons.end())
		return *it;

	item.weapons.push_back({ weapon_id });
	return item.weapons[item.weapons.size() - 1];
}

alpha_config::config_base& alpha_config::get_config()
{
	return item;
}

void alpha_config::reset() noexcept {
	item = { };
	current_wepon_id = 0;
	current_weapon_name = "";
}

alpha_config::alpha_config()
{
	current_weapon = &get_active_weapon(0);
	current_wepon_id = 0;
	current_weapon_name = "";
}

void alpha_config::run(const char* name) noexcept {
	PWSTR pathToDocuments;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pathToDocuments))) {
		path = pathToDocuments;
		path /= name;
	}

	if (!std::filesystem::is_directory(path)) {
		std::filesystem::remove(path);
		std::filesystem::create_directory(path);
	}

	std::transform(std::filesystem::directory_iterator{ path },
		std::filesystem::directory_iterator{ },
		std::back_inserter(configs),
		[](const auto & entry) { return entry.path().filename().string(); });
}

void alpha_config::load(size_t id) noexcept {
	if (!std::filesystem::is_directory(path)) {
		std::filesystem::remove(path);
		std::filesystem::create_directory(path);
	}

	std::ifstream in{ path / configs[id] };

	if (!in.good())
		return;

	jsonip::parse(item, in);
	in.close();

}

void alpha_config::save(size_t id) const noexcept {
	if (!std::filesystem::is_directory(path)) {
		std::filesystem::remove(path);
		std::filesystem::create_directory(path);
	}

	std::ofstream out{ path / configs[id] };

	if (!out.good())
		return;

	jsonip::write(out, item, true);
	out.close();
}

void alpha_config::add(const char* name) noexcept {
	if (*name && std::find(std::cbegin(configs), std::cend(configs), name) == std::cend(configs))
		configs.emplace_back(name);
}

void alpha_config::remove(size_t id) noexcept {
	std::filesystem::remove(path / configs[id]);
	configs.erase(configs.cbegin() + id);
}

void alpha_config::rename(size_t item, const char* newName) noexcept {
	std::filesystem::rename(path / configs[item], path / newName);
	configs[item] = newName;
}


#pragma region SERIALIZATION_HELPER

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::color_t,
	(float, r)
	(float, g)
	(float, b)
	(float, a)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::legit_weapon_settings,
	(bool, enable)
	(bool, silent)
	(bool, nearest)
	(int, hitbox)
	(float, fov)
	(float, smooth)
	(float, rcs_x)
	(float, rcs_y)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::trigger_weapon_settings,
	(bool, enable)
	(bool, on_key)
	(int, key_id)
	(bool, rcs)
	(bool, hitbox_head)
	(bool, hitbox_body)
	(bool, hitbox_arms)
	(bool, hitbox_legs)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::weapon_skin_info,
	(int, paint_kit)
	(int, paint_kit_vector)
	(bool, setted)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::skin_changer_settings,
	(bool, enable_skinchanger)
	(bool, enable_glovechanger)

	(int, knife_model)
	(int, knife_skin)
	(int, knife_wear)

	(int, glove_model)
	(int, glove_skin)
	(int, glove_wear)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::colors_settings,
	(alpha_config::color_t, esp_box)
	(alpha_config::color_t, esp_name)
	(alpha_config::color_t, esp_weapon_icon)
	(alpha_config::color_t, esp_footsteps)

	(alpha_config::color_t, chams_enemy_visible)
	(alpha_config::color_t, chams_enemy_invisible)
	(alpha_config::color_t, chams_team_visible)
	(alpha_config::color_t, chams_team_ivisible)
	(alpha_config::color_t, chams_hand_weapon)
	(alpha_config::color_t, chams_hands)
	(alpha_config::color_t, chams_sleeve)

	(alpha_config::color_t, glow_enemy)
	(alpha_config::color_t, glow_team)
	(alpha_config::color_t, bomb)
	(alpha_config::color_t, weapons)
	(alpha_config::color_t, nades)

	(alpha_config::color_t, sky)
	(alpha_config::color_t, ambient)

	(alpha_config::color_t, damage_indicator)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::visuals_player_flags,
	(bool, bombcarrier)
	(bool, is_flashed)
	(bool, is_scoped)
	(bool, is_defusing)
	(bool, position_callout)
	(bool, defuse_kit)
	(bool, armor)
	(bool, money)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::visuals_player_settings,
	(bool, name)
	(bool, box)
	(bool, health)

	(bool, weapon_name)
	(bool, weapon_icon)

	(bool, footsteps)

	(bool, skeleton)

	(bool, force_crosshair)

	(int, fov)


	(alpha_config::visuals_player_flags, flags)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::visuals_removals,
	(bool, fog)
	(bool, smoke)
	(bool, flash)
	(bool, sleeves)
	(bool, hands)
	(bool, skope_overlay)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::visuals_world,
	(bool, planted_bomb)
	(bool, dropped_weapons_name)
	(bool, dropped_weapons_icon)
	(bool, projectiles)
	(bool, misc_entities)
	(bool, danger_zone)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::visuals_helpers,
	(bool, grenade_prediction)
	(bool, damage_indicator)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::visuals_glow,
	(bool, enabled)
	(bool, enemy)
	(bool, temmmate)
	(bool, planted_bomb)
	(bool, dropped_weapons)
	(bool, dropped_weapons)
	(bool, nades)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::visuals_chams,
	(bool, enabled)
	(bool, smoke_check)
	(int, material)

	(bool, enemy)
	(bool, enemy_xoz)

	(bool, temmmate)
	(bool, temmate_xoz)

	(bool, weapon)
	(bool, weapon_wireframe)

	(bool, hands)
	(bool, hands_wireframe)

	(bool, sleeve)
	(bool, sleeve_wireframe)

	(bool, backtrack)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::visuals_settings,
	(bool, global_enabled)
	(bool, show_team)
	(bool, visible_only)

	(alpha_config::visuals_player_settings, player)


	(bool, ambient_light)

	(bool, night_mode)
	(int, brightness)

	(alpha_config::visuals_removals, removals)
	(alpha_config::visuals_world, world)
	(alpha_config::visuals_helpers, helpers)
	(alpha_config::visuals_glow, glow)
	(alpha_config::visuals_chams, chams)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::misc_logs,
	(bool, hurt)
	(bool, bouth)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::misc_hitmarker,
	(bool, enabled)
	(int, sound)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::bhop_edge_jump,
	(bool, enabled)
	(bool, duck_in_air)
	(int, jump_key)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::bhop_settings,
	(bool, enabled)
	(bool, auto_strafe)
	(alpha_config::bhop_edge_jump, edge_jump)
)


BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::misc_settings,

	(bool, global_active)
	(bool, clantag)
	(bool, radar)
	(alpha_config::misc_logs, logs)
	(alpha_config::misc_hitmarker, hitmarker)
	(bool, anti_screenshoot)
	(bool, spectators)
	(bool, disable_post_processing)
	(bool, recoil_crosshair)
	(bool, rank_reveal)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::backtrack_settings,
	(bool, enable)
	(float, backtrack_ms)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::weapon_settings,
	(size_t, weapon_id)
	(alpha_config::legit_weapon_settings, legit_settings)
	(alpha_config::trigger_weapon_settings, trigger_settings)
	(alpha_config::weapon_skin_info, skin_info)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::rcs_standalone_settings,
	(bool, enabled)
	(float, rcs_x)
	(float, rcs_y)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::legit_bot_settings,
	(bool, smoke_check)
	(bool, scope_aim)
	(bool, distance_based_fov)
	(bool, team_check)
	(bool, auto_pistol)
	(int, kill_delay)
	(alpha_config::backtrack_settings, backtrack)
	(alpha_config::rcs_standalone_settings, rcs_standalone)
)

BOOST_FUSION_ADAPT_STRUCT(
	alpha_config::config_base,
	(alpha_config::colors_settings, colors),
	(alpha_config::legit_bot_settings, legit)
	(alpha_config::skin_changer_settings, skin_changer)
	(alpha_config::visuals_settings, visuals)
	(alpha_config::misc_settings, misc)
	(alpha_config::bhop_settings, bhop)
	(alpha_config::weapon_info_vec, weapons)
)
#pragma endregion