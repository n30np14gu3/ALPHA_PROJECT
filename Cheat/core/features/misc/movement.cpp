#include "../../../dependencies/common_includes.hpp"
#include "movement.hpp"

c_movement movement;

auto flags_backup = 0;

void c_movement::bunnyhop(c_usercmd* user_cmd) noexcept {
	if (!config_system.get_config().bhop.enabled)
		return;

	static bool bLastJumped = false;
	static bool bShouldFake = false;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!local_player)
		return;

	if (local_player->move_type() == movetype_ladder || local_player->move_type() == movetype_noclip)
		return;

	if (!bLastJumped && bShouldFake)
	{
		bShouldFake = false;
		user_cmd->buttons |= in_jump;
	}
	else if (user_cmd->buttons & in_jump)
	{
		if (local_player->flags() & fl_onground)
		{
			bLastJumped = true;
			bShouldFake = true;
		}
		else
		{
			user_cmd->buttons &= ~in_jump;
			bLastJumped = false;
		}
	}
	else
	{
		bLastJumped = false;
		bShouldFake = false;
	}
}

void c_movement::autostrafe(c_usercmd* user_cmd) noexcept
{
	if (!config_system.get_config().bhop.enabled || !config_system.get_config().bhop.auto_strafe)
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!local_player)
		return;

	if(local_player->flags() & fl_onground)
		return;

	if(user_cmd->buttons & in_forward || user_cmd->buttons & in_back || user_cmd->buttons & in_moveleft || user_cmd->buttons & in_moveright)
		return;

	if(user_cmd->mousedx <=1 && user_cmd->mousedx >= -1)
		return;

	user_cmd->sidemove = user_cmd->mousedx < 0.f ? -450.f : 450.f;
}

void c_movement::edge_jump_pre_prediction(c_usercmd* user_cmd) noexcept {
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!config_system.get_config().bhop.edge_jump.enabled)
		return;

	if (!GetAsyncKeyState(config_system.get_config().bhop.edge_jump.jump_key))
		return;

	if (!local_player)
		return;

	if (local_player->move_type() == movetype_ladder || local_player->move_type() == movetype_noclip)
		return;

	flags_backup = local_player->flags();
}

void c_movement::edge_jump_post_prediction(c_usercmd* user_cmd) noexcept {
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!config_system.get_config().bhop.edge_jump.enabled)
		return;

	if (!GetAsyncKeyState(config_system.get_config().bhop.edge_jump.jump_key))
		return;

	if (!local_player)
		return;

	if (local_player->move_type() == movetype_ladder || local_player->move_type() == movetype_noclip)
		return;

	if (flags_backup & fl_onground && !(local_player->flags() & fl_onground))
		user_cmd->buttons |= in_jump;

	if (!(local_player->flags() & fl_onground) && config_system.get_config().bhop.edge_jump.duck_in_air)
		user_cmd->buttons |= in_duck;
}
