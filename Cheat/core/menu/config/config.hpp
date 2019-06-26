#pragma once

#include <filesystem>


class c_config final {
public:
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

	struct {
		bool backtrack{ false };
		bool backtrack_visualize{ false };
		bool scope_aim{ false };
		bool smoke_check{ false };
		bool aim_distance_based_fov{ false };
		bool aim_team_check{ false };

		bool bomb_planted{ false };
		bool dropped_weapons{ false };
		bool backtrack_skeleton{ false };
		bool nade_pred{ false };
		bool visuals_preview{ false };
		bool sound_footstep{ false };
		bool skeleton{ false };
		bool danger_zone_dropped{ false };
		bool visuals_enabled{ false };
		bool anti_screenshot{ false };
		bool remove_smoke{ false };
		bool remove_fog{ false };
		bool visuals_glow{ false };
		bool visuals_glow_enemy{ false };
		bool visuals_glow_team{ false };
		bool visuals_glow_planted{ false };
		bool visuals_glow_weapons{ false };
		bool visuals_glow_nades{ false };
		bool player_name{ false };
		bool player_dormant{ false };
		bool player_ammo{ false };
		bool player_flags{ false };
		bool player_flags_armor{ false };
		bool player_flags_money{ false };
		bool player_flags_scoped{ false };
		bool player_flags_c4{ false };
		bool player_flags_kit = false;
		bool player_flags_defuse = false;
		bool player_flags_pos = false;
		bool player_flags_flashed{ false };
		bool player_box{ false };
		bool player_health{ false };
		bool player_weapon{ false };
		bool player_weapon_icon{ false };
		bool player_armor{ false };
		float clr_chams_vis[4]{ 1.0f,1.0f,1.0f,255.0f };
		float clr_chams_invis[4]{ 1.0f,1.0f,1.0f,255.0f };
		float clr_glow[4]{ 1.0f,1.0f,1.0f,255.0f };
		float clr_glow_team[4]{ 1.0f,1.0f,1.0f,255.0f };
		float clr_glow_planted[4]{ 1.0f,1.0f,1.0f,255.0f };
		float clr_glow_dropped[4]{ 1.0f,1.0f,1.0f,255.0f };
		float clr_glow_dropped_nade[4]{ 1.0f,1.0f,1.0f,255.0f };
		float clr_sky[4]{ 1.0f,1.0f,1.0f,255.0f };
		float clr_ambient[4]{ 1.0f,1.0f,1.0f,255.0f };
		float clr_box[4]{ 1.0f,1.0f,1.0f,255.0f };
		float clr_name[4]{ 1.0f,1.0f,1.0f,255.0f };
		float clr_weapon[4]{ 1.0f,1.0f,1.0f,255.0f };
		float clr_weapon_icon[4]{ 1.0f,1.0f,1.0f,255.0f };
		float clr_footstep[4]{ 1.0f,1.0f,1.0f,255.0f };
		bool chams{ false };
		bool remove_scope{ false };
		int vis_chams_type{ 0 };
		int viewmodel_fov{ 0 };
		int viewmodel_x{ 0 };
		int viewmodel_y{ 0 };
		int viewmodel_z{ 0 };
		int knife_wear{ 0 };
		int glove_wear{ 0 };
		int fov{ 0 };
		int nightmode_brightness{ 100 };
		bool reduce_flash{ false };
		bool remove_sleeves{ false };
		bool remove_hands{ false };
		bool entity_esp{ false };
		bool projectiles{ false };
		float flash_alpha{ 255.f };
		bool force_crosshair{ false };
		bool visuals_team_check{ false };
		bool vis_chams_vis{ false };
		bool vis_chams_invis{ false };
		bool nightmode{ false };
		bool ambient{ false };
		bool spectators_list{ false };
		bool watermark{ false };
		bool misc_enabled{ false };
		bool misc_save_config{ false };
		bool misc_load_config{ false };
		bool bunny_hop_auto_stafe{ false };
		bool bunny_hop{ false };
		bool clan_tag{ false };
		bool radar{ false };
		bool hitmarker{ false };
		bool viewmodel_offset{ false };
		bool logs_player_hurt{ false };
		bool logs_player_bought{ false };
		bool skinchanger_enable{ false };
		bool glovechanger_enable{ false };
		int hitmarker_sound{ 0 };
		int knife_model{ 0 };
		int knife_skin{ 0 };
		int glove_model{ 0 };
		int glove_skin{ 0 };
		int config_selection{ 0 };
		int keybinds_selection{ 0 };
		bool edge_jump{ false };
		bool edge_jump_duck_in_air{ false };
		int edge_jump_key{ 0 };

		int paint_kit_vector_index_knife{ 0 };
		int paint_kit_index_knife{ 0 };
		int paint_kit_vector_index_glove{ 0 };
		int paint_kit_index_glove{ 0 };
		float backtrack_ms{ 0.0f };

		int aim_key{ 0 };
		bool vis_chams_smoke_check{ false };
		bool disable_post_processing{ false };
		bool recoil_crosshair{ false };
		bool rank_reveal{ false };
		bool vis_chams_vis_teammate{ false };
		bool vis_chams_invis_teammate{ false };
		float clr_chams_vis_teammate[4]{ 1.0f,1.0f,1.0f,255.0f };
		float clr_chams_invis_teammate[4]{ 1.0f,1.0f,1.0f,255.0f };
		int aimbot_delay_after_kill{ 0 };
		bool aimbot_auto_pistol{ false };
		float rcs_standalone_x{ 0.0f };
		float rcs_standalone_y{ 0.0f };
		bool rcs_standalone{ false };
		bool weapon_chams{ false };
		bool hand_chams{ false };
		bool sleeve_chams{ false };
		float clr_weapon_chams[4]{ 1.0f,1.0f,1.0f,255.0f };
		float clr_hand_chams[4]{ 1.0f,1.0f,1.0f,255.0f };
		float clr_sleeve_chams[4]{ 1.0f,1.0f,1.0f,255.0f };
		bool visuals_visible_only{ false };
		bool visuals_on_key{ false };
		int visuals_key{ 0 };
		bool damage_indicator{ false };
		float clr_damage_indicator[4] = { 1.0f,1.0f,1.0f,255.0f };
		bool dropped_weapons_icon{ false };

		struct
		{
			int paint_kit;
			int paint_kit_vecotor;
			bool setted;
		}weapon_skin_info[530]{};

		struct
		{
			bool enable;
			bool silent;

			bool nearest;
			int hitbox;

			float fov;
			float smooth;

			float rcs_x;
			float rcs_y;
		}aim_bot_settings[530]{};


		struct
		{
			bool enable;
			bool on_key;
			int key_id;
			bool rcs;
			bool hitbox_head;
			bool hitbox_body;
			bool hitbox_arms;
			bool hitbox_legs;
			int delay;
		}trigger_bot[530]{};

	} item;

	int active_weapon = 0;
	std::string weapon_name = "";

private:
	std::filesystem::path path;
	std::vector<std::string> configs;
};

extern c_config config_system;