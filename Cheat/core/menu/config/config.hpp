#pragma once
#include <vector>
#include <filesystem>

class alpha_config
{
public:
	struct color_t
	{
		float r;
		float g;
		float b;
		float a;
	};

	struct legit_weapon_settings
	{
		bool enable{ false };
		bool silent{ false };

		bool nearest{ false };
		int hitbox{ false };

		float fov{ false };
		float smooth{ false };

		float rcs_x{ false };
		float rcs_y{ false };

	};

	struct trigger_weapon_settings
	{
		bool enable{ false };
		bool on_key{ false };
		int key_id{ 0 };
		bool rcs{ false };
		bool hitbox_head{ false };
		bool hitbox_body{ false };
		bool hitbox_arms{ false };
		bool hitbox_legs{ false };
		int delay{ 0 };
	};

	struct backtrack_settings
	{
		bool enable{ false };
		float backtrack_ms{ 0.f };
	};

	struct weapon_skin_info
	{
		int paint_kit{ 0 };
		int paint_kit_vector{ 0 };
		bool setted{ false };
	};

	struct skin_changer_settings
	{
		bool enable_skinchanger{ false };
		bool enable_glovechanger{ false };

		int knife_model{ 0 };
		
		int knife_skin{ 0 };
		int knife_skin_vector{ 0 };
		int knife_wear{ 0 };

		int glove_model{ 0 };
		int glove_skin{ 0 };
		int glove_skin_vector{ 0 };
		int glove_wear{ 0 };

	};

	struct visuals_player_flags
	{
		bool bombcarrier{ false };
		bool is_flashed{ false };
		bool is_scoped{ false };
		bool is_defusing{ false };
		bool position_callout{ false };
		bool defuse_kit{ false };
		bool armor{ false };
		bool money{ false };
	};

	struct visuals_player_settings
	{
		bool name{ false };
		bool box{ false };
		bool health{ false };

		bool weapon_name{ false };
		bool weapon_icon{ false };

		bool footsteps{ false };

		bool skeleton{ false };

		bool force_crosshair{ false };
		int fov{ 0 };


		visuals_player_flags flags;
	};

	struct visuals_removals
	{
		bool fog{ false };
		bool smoke{ false };
		bool flash{ false };
		bool sleeves{ false };
		bool hands{ false };
		bool skope_overlay{ false };
	};

	struct visuals_world
	{
		bool planted_bomb{ false };

		bool dropped_weapons_name{ false };
		bool dropped_weapons_icon{ false };

		bool projectiles{ false };
		bool misc_entities{ false };
		bool danger_zone{ false };
	};

	struct visuals_helpers
	{
		bool grenade_prediction{ false };
		bool damage_indicator{ false };
	};

	struct visuals_glow
	{
		bool enabled{ false };
		bool enemy{ false };
		bool temmmate{ false };
		bool planted_bomb{ false };
		bool dropped_weapons{ false };
		bool nades{ false };

	};

	struct visuals_chams
	{
		bool enabled{ false };
		bool smoke_check{ false };
		int material{ 0 };

		bool enemy{ false };
		bool enemy_xoz{ false };

		bool temmmate{ false };
		bool temmate_xoz{ false };

		bool weapon{ false };
		bool weapon_wireframe{ false };

		bool hands{ false };
		bool hands_wireframe{ false };

		bool sleeve{ false };
		bool sleeve_wireframe{ false };
		bool backtrack{ false };
	};

	struct visuals_settings
	{
		bool global_enabled{ false };
		bool show_team{ false };
		bool visible_only{ false };

		visuals_player_settings player{};

		bool ambient_light{ false };

		bool night_mode{ false };
		int brightness{ 0 };


		visuals_removals removals{};

		visuals_world world{};

		visuals_helpers helpers{};

		visuals_glow glow{};

		visuals_chams chams{};
	};

	struct misc_logs
	{
		bool hurt{ false };
		bool bouth{ false };
	};

	struct misc_hitmarker
	{
		bool enabled{ false };
		int sound{ 0 };
	};

	struct bhop_edge_jump
	{
		bool enabled{ false };
		bool duck_in_air{ false };
		int jump_key{ 0 };
	};

	struct bhop_settings
	{
		bool enabled{ false };
		bool auto_strafe{ false };
		bhop_edge_jump edge_jump{};
	};

	struct misc_settings
	{
		bool global_active{ false };
		bool clantag{ false };
		bool radar{ false };
		misc_logs logs{};
		misc_hitmarker hitmarker{};
		bool anti_screenshoot{ false };
		bool spectators{ false };
		bool disable_post_processing{ false };
		bool recoil_crosshair{ false };
		bool rank_reveal{ false };
	};

	struct weapon_settings
	{
		size_t weapon_id{ 0 };
		legit_weapon_settings legit_settings{};
		trigger_weapon_settings trigger_settings{};
		weapon_skin_info skin_info{};
		weapon_settings() = default;
		weapon_settings(size_t id) : weapon_id(id) {}
		bool operator () (const struct weapon_settings& m) const
		{
			return m.weapon_id == weapon_id;
		}
	};

	struct colors_settings
	{
		color_t esp_box{};
		color_t esp_name{};
		color_t esp_weapon_icon{};
		color_t esp_footsteps{};

		color_t chams_enemy_visible{};
		color_t chams_enemy_invisible{};
		color_t chams_team_visible{};
		color_t chams_team_ivisible{};
		color_t chams_hand_weapon{};
		color_t chams_hands{};
		color_t chams_sleeve{};

		color_t glow_enemy{};
		color_t glow_team{};
		color_t bomb{};
		color_t weapons{};
		color_t nades{};

		color_t sky{};
		color_t ambient{};

		color_t damage_indicator{};
	};

	typedef std::vector<weapon_settings> weapon_info_vec;

	struct rcs_standalone_settings
	{
		bool enabled{ false };
		float rcs_x{ 0 };
		float rcs_y{ 0 };
	};
	struct legit_bot_settings
	{
		bool smoke_check{ false };
		bool scope_aim{ false };
		bool distance_based_fov{ false };
		bool team_check{ false };
		int kill_delay{ 0 };
		bool auto_pistol{ false };
		backtrack_settings backtrack{};
		rcs_standalone_settings rcs_standalone{};
		
	};

	struct config_base
	{
		colors_settings colors;
		legit_bot_settings legit;
		skin_changer_settings skin_changer;
		visuals_settings visuals;
		misc_settings misc;
		bhop_settings bhop;
		weapon_info_vec weapons;
	};

	weapon_settings& get_active_weapon(size_t weapon_id);
	config_base& get_config();

	alpha_config();
	void run(const char*) noexcept;
	void load(size_t) noexcept;
	void save(size_t) const noexcept;
	void add(const char*) noexcept;
	void remove(size_t) noexcept;
	void rename(size_t, const char*) noexcept;
	void reset() noexcept;
	constexpr auto& get_configs() noexcept {
		return configs;
	}

	weapon_settings* current_weapon;
	int current_wepon_id{ 0 };
	std::string current_weapon_name;

private:
	std::filesystem::path path;
	config_base item{};
	std::vector<std::string> configs;
};

extern alpha_config config_system;