#include "aimbot.hpp"
#include "../../../source-sdk/sdk.hpp"
#include "../../../dependencies/common_includes.hpp"
#include "../../features/backtrack/backtrack.hpp"

c_aimbot aimbot;

int c_aimbot::get_nearest_bone(player_t* entity, c_usercmd* user_cmd) noexcept {
	
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return false;

	float best_dist = 360.f;
	int aimbone;

	matrix_t matrix[MAXSTUDIOBONES];

	if (!entity->setup_bones(matrix, 128, BONE_USED_BY_HITBOX, 0.0f))
		return -1;

	studio_hdr_t* studio_model = interfaces::model_info->get_studio_model(entity->model());
	if (!studio_model)
		return -1;

	studio_hitbox_set_t* set = studio_model->hitbox_set(entity->hitbox_set());
	if (!set)
		return -1;

	for (int i = 0; i < set->hitbox_count; i++) {
		if (i >= hitbox_max)
			continue;

		studio_box_t* hitbox = set->hitbox(i);

		if (!hitbox)
			continue;

		auto angle = math.calculate_angle(local_player->get_eye_pos(), vec3_t(matrix[hitbox->bone][0][3], matrix[hitbox->bone][1][3], matrix[hitbox->bone][2][3]), user_cmd->viewangles);
		auto this_dist = std::hypotf(angle.x, angle.y);

		if (best_dist > this_dist) {
			best_dist = this_dist;
			aimbone = hitbox->bone;
			continue;
		}
	}
	return aimbone;
}

void c_aimbot::weapon_settings(weapon_t* weapon) noexcept {
	if (!weapon)
		return;
	
	hitbox_id = config_system.current_weapon->legit_settings.hitbox;
	aim_smooth = config_system.current_weapon->legit_settings.smooth;
	aim_fov = config_system.current_weapon->legit_settings.fov;
	rcs_x = config_system.current_weapon->legit_settings.rcs_x;
	rcs_y = config_system.current_weapon->legit_settings.rcs_y;
}

int c_aimbot::find_target(c_usercmd* user_cmd) noexcept {
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return false;

	auto best_fov = aim_fov;
	auto best_target = 0;

	for (int i = 1; i <= interfaces::globals->max_clients; i++) {
		auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));
		auto entity_bone_pos = entity->get_bone_position(get_nearest_bone(entity, user_cmd));
		auto local_eye_pos = local_player->get_eye_pos();
		auto distance = local_eye_pos.distance_to(entity_bone_pos);

		if (!entity || entity == local_player || entity->dormant() || !entity->is_alive() || entity->has_gun_game_immunity())
			continue;

		angle = math.calculate_angle(local_eye_pos, entity_bone_pos, user_cmd->viewangles);
		auto fov = config_system.get_config().legit.distance_based_fov ? math.distance_based_fov(distance, math.calculate_angle_alternative(local_eye_pos, entity_bone_pos), user_cmd) : std::hypotf(angle.x, angle.y);
		if (fov < best_fov) {
			best_fov = fov;
			best_target = i;
		}
	}
	return best_target;
}

void c_aimbot::event_player_death(i_game_event* event) noexcept {
	if (!interfaces::engine->is_connected() && !interfaces::engine->is_in_game())
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player || !local_player->is_alive())
		return;

	auto attacker = interfaces::entity_list->get_client_entity(interfaces::engine->get_player_for_user_id(event->get_int("attacker")));
	if (!attacker)
		return;

	if (attacker == local_player)
		kill_delay = interfaces::globals->tick_count + config_system.get_config().legit.kill_delay;
}

void c_aimbot::auto_pistol(c_usercmd* user_cmd) {
	if (!config_system.get_config().legit.auto_pistol)
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;

	auto weapon = local_player->active_weapon();
	if (!weapon)
		return;

	static bool was_firing = false;

	if (aimbot.is_pistol(weapon) && weapon->item_definition_index() != WEAPON_REVOLVER) {
		if (user_cmd->buttons & in_attack && !aimbot.is_knife(weapon) && !aimbot.is_grenade(weapon)) {
			if (was_firing) {
				user_cmd->buttons &= ~in_attack;
			}
		}

		was_firing = user_cmd->buttons & in_attack ? true : false;
	}
}

void c_aimbot::rcs_standalone(c_usercmd* user_cmd) noexcept {
	if (!config_system.get_config().legit.rcs_standalone.enabled)
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;
	
	auto weapon = local_player->active_weapon();
	if (!weapon || !weapon->clip1_count() || is_bomb(weapon) || is_knife(weapon) || is_grenade(weapon))
		return;
	
	static vec3_t old_punch = { 0.0f, 0.0f, 0.0f };
	auto recoil_scale = interfaces::console->get_convar("weapon_recoil_scale");
	vec3_t aim_punch = local_player->aim_punch_angle() * recoil_scale->get_float();

	rcs_x = config_system.get_config().legit.rcs_standalone.rcs_x;
	rcs_y = config_system.get_config().legit.rcs_standalone.rcs_y;

	aim_punch.x *= rcs_x;
	aim_punch.y *= rcs_y;


	auto rcs = user_cmd->viewangles += (old_punch - aim_punch);
	interfaces::engine->set_view_angles(rcs);

	old_punch = aim_punch;
}

void c_aimbot::do_psilent(vec3_t angle, player_t* pLocal, c_usercmd* cmd) noexcept
{
	float flNextPrimaryAttack = pLocal->active_weapon()->next_primary_attack();
	float flServerTime = pLocal->get_tick_base() * interfaces::globals->interval_per_tick;
	vec3_t localAngle = interfaces::engine->get_view_angles();
	bool BulletTime = true;
	if (flNextPrimaryAttack > flServerTime)
		BulletTime = false;
	if(cmd->buttons & in_attack && BulletTime)
	{
		*interfaces::bSendPacket = false;
		cmd->viewangles += angle;
	}else
	{
		cmd->viewangles = localAngle;
		std::chrono::microseconds(2);
		*interfaces::bSendPacket = true;
		cmd->buttons &= ~in_attack;
	}
}

void c_aimbot::apply_player_weapon(c_usercmd*) noexcept
{
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;

	auto weapon = local_player->active_weapon();
	if(!weapon)
		return;

	if(is_pistol(weapon) || is_rifle(weapon) || is_sniper(weapon) || is_smg(weapon) || is_heavy(weapon) || is_heavy(weapon))
	{
		config_system.current_wepon_id = weapon->item_definition_index();
		config_system.current_weapon_name = std::string(weapon->weapon_name_definition());
	}
	else
	{
		config_system.current_wepon_id = 0;
		config_system.current_weapon_name = "UNKNOWN";
	}
	config_system.current_weapon = &config_system.get_active_weapon(config_system.current_wepon_id);
}

void c_aimbot::run(c_usercmd* user_cmd) noexcept 
{

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;

	if (kill_delay >= interfaces::globals->tick_count)
		return;

	auto weapon = local_player->active_weapon();
	weapon_settings(weapon);
	auto_pistol(user_cmd);
	rcs_standalone(user_cmd);
	
	if (config_system.current_weapon->legit_settings.enable && user_cmd->buttons & in_attack) {
		if (auto target = find_target(user_cmd)) {
			auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(target));

			if (!weapon || !weapon->clip1_count())
				return;

			if (!local_player->can_see_player_pos(entity, entity->get_eye_pos()))
				return;

			if (!config_system.get_config().legit.team_check && entity->is_in_local_team())
				return;

			if (!config_system.get_config().legit.smoke_check && utilities::is_behind_smoke(local_player->get_eye_pos(), entity->get_hitbox_position(entity, hitbox_head)))
				return;

			if (is_knife(weapon) || is_grenade(weapon)|| is_bomb(weapon))
				return;

			if (is_sniper(weapon) && !local_player->is_scoped() && !config_system.get_config().legit.scope_aim)
				return;

			auto recoil_scale = interfaces::console->get_convar("weapon_recoil_scale");
			auto aim_punch = local_player->aim_punch_angle() * recoil_scale->get_float();
			aim_punch.x *= rcs_x;
			aim_punch.y *= rcs_y;

			switch (config_system.current_weapon->legit_settings.nearest) {
			case 0:
				angle = math.calculate_angle(local_player->get_eye_pos(), entity->get_hitbox_position(entity, hitbox_id), user_cmd->viewangles + aim_punch);
				break;
			case 1:
				angle = math.calculate_angle(local_player->get_eye_pos(), entity->get_bone_position(get_nearest_bone(entity, user_cmd)), user_cmd->viewangles + aim_punch);
				break;
			}

			if (!config_system.current_weapon->legit_settings.silent) {
				angle /= aim_smooth;
				user_cmd->viewangles += angle;
				interfaces::engine->set_view_angles(user_cmd->viewangles);
			}
			else
			{
				do_psilent(angle, local_player, user_cmd);
			}
		}
	}
} 