#include "events.hpp"
#include "../misc/logs.hpp"
#include "../misc/hitmarker.hpp"
#include "../visuals/sound.hpp"
#include "../aimbot/aimbot.hpp"
#include "../../../SDK/crypto/XorStr.h"
#pragma comment(lib, "Winmm.lib")

c_hooked_events events;

void c_hooked_events::fire_game_event(i_game_event* event) noexcept {
	auto event_name = event->get_name();

	if (!strcmp(event_name, XorStr("player_hurt"))) {
		hitmarker.event(event);
		event_logs.event_player_hurt(event);
		sound_esp.event_player_hurt(event);
	}

	else if (strstr(event->get_name(), XorStr("item_purchase"))) {
		event_logs.event_item_purchase(event);
	}

	else if (strstr(event->get_name(), XorStr("player_footstep"))) {
		sound_esp.event_player_footstep(event);
	}

	else if (!strcmp(event_name, XorStr("player_death"))) {
		aimbot.event_player_death(event);
	}
}

int c_hooked_events::get_event_debug_id(void) noexcept {
	return EVENT_DEBUG_ID_INIT;
}

void c_hooked_events::setup() noexcept {
	m_i_debug_id = EVENT_DEBUG_ID_INIT;
	interfaces::event_manager->add_listener(this, XorStr("player_hurt"), false);
	interfaces::event_manager->add_listener(this, XorStr("item_purchase"), false);
	interfaces::event_manager->add_listener(this, XorStr("player_footstep"), false);
	interfaces::event_manager->add_listener(this, XorStr("player_death"), false);
}

void c_hooked_events::release() noexcept {
	interfaces::event_manager->remove_listener(this);
}
