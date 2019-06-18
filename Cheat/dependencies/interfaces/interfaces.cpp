#include "interfaces.hpp"
#include "../common_includes.hpp"
#include "../../SDK/crypto/XorStr.h"

i_base_client_dll* interfaces::client = nullptr;
i_input* interfaces::input = nullptr;
i_client_entity_list* interfaces::entity_list = nullptr;
iv_engine_client* interfaces::engine = nullptr;
i_client_mode* interfaces::clientmode = nullptr;
i_client_state* interfaces::clientstate = nullptr;
i_panel* interfaces::panel = nullptr;
i_surface* interfaces::surface = nullptr;
c_global_vars_base* interfaces::globals = nullptr;
i_material_system* interfaces::material_system = nullptr;
iv_model_info* interfaces::model_info = nullptr;
iv_model_render* interfaces::model_render = nullptr;
i_render_view* interfaces::render_view = nullptr;
iv_effects* interfaces::effects = nullptr;
i_console* interfaces::console = nullptr;
i_localize* interfaces::localize = nullptr;
i_game_event_manager2* interfaces::event_manager = nullptr;
i_inputsytem* interfaces::inputsystem = nullptr;
iv_debug_overlay* interfaces::debug_overlay = nullptr;
IDirect3DDevice9* interfaces::directx = nullptr;
iv_engine_sound* interfaces::engine_sound = nullptr;
trace *interfaces::trace_ray = nullptr;
glow_manager_t *interfaces::glow_manager = nullptr;
player_game_movement *interfaces::game_movement = nullptr;
player_prediction *interfaces::prediction = nullptr;
player_move_helper *interfaces::move_helper = nullptr;
i_view_render_beams *interfaces::render_beams = nullptr;

void interfaces::initialize() {
	client = reinterpret_cast<i_base_client_dll*>(utilities::game::capture_interface(XorStr("client_panorama.dll"), XorStr("VClient018")));
	entity_list = reinterpret_cast<i_client_entity_list*>(utilities::game::capture_interface(XorStr("client_panorama.dll"), XorStr("VClientEntityList003")));
	engine = reinterpret_cast<iv_engine_client*>(utilities::game::capture_interface(XorStr("engine.dll"), XorStr("VEngineClient014")));
	panel = reinterpret_cast<i_panel*>(utilities::game::capture_interface(XorStr("vgui2.dll"), XorStr("VGUI_Panel009")));
	surface = reinterpret_cast<i_surface*>(utilities::game::capture_interface(XorStr("vguimatsurface.dll"), XorStr("VGUI_Surface031")));
	material_system = reinterpret_cast<i_material_system*>(utilities::game::capture_interface(XorStr("materialsystem.dll"), XorStr("VMaterialSystem080")));
	model_info = reinterpret_cast<iv_model_info*>(utilities::game::capture_interface(XorStr("engine.dll"), "VModelInfoClient004"));
	model_render = reinterpret_cast<iv_model_render*>(utilities::game::capture_interface(XorStr("engine.dll"), XorStr("VEngineModel016")));
	render_view = reinterpret_cast<i_render_view*>(utilities::game::capture_interface(XorStr("engine.dll"), XorStr("VEngineRenderView014")));
	console = reinterpret_cast<i_console*>(utilities::game::capture_interface(XorStr("vstdlib.dll"), XorStr("VEngineCvar007")));
	localize = reinterpret_cast<i_localize*>(utilities::game::capture_interface(XorStr("localize.dll"), XorStr("Localize_001")));
	event_manager = reinterpret_cast<i_game_event_manager2*>(utilities::game::capture_interface(XorStr("engine.dll"), XorStr("GAMEEVENTSMANAGER002")));
	debug_overlay = reinterpret_cast<iv_debug_overlay*>(utilities::game::capture_interface(XorStr("engine.dll"), XorStr("VDebugOverlay004")));
	effects = reinterpret_cast<iv_effects*>(utilities::game::capture_interface(XorStr("engine.dll"), XorStr("VEngineEffects001")));
	inputsystem = reinterpret_cast<i_inputsytem*>(utilities::game::capture_interface(XorStr("inputsystem.dll"), XorStr("InputSystemVersion001")));
	trace_ray = reinterpret_cast<trace*>(utilities::game::capture_interface(XorStr("engine.dll"), XorStr("EngineTraceClient004")));
	game_movement = reinterpret_cast<player_game_movement*>(utilities::game::capture_interface(XorStr("client_panorama.dll"), XorStr("GameMovement001")));
	prediction = reinterpret_cast<player_prediction*>(utilities::game::capture_interface(XorStr("client_panorama.dll"), XorStr("VClientPrediction001")));
	engine_sound = reinterpret_cast<iv_engine_sound*>(utilities::game::capture_interface(XorStr("engine.dll"), XorStr("IEngineSoundClient003")));
	//std::cout << "\n" << inputsystem;
	clientmode = **reinterpret_cast<i_client_mode***>((*reinterpret_cast<uintptr_t**>(client))[10] + 5);
	clientstate = **(i_client_state***)(utilities::pattern_scan(GetModuleHandleA(XorStr("engine.dll")), XorStr("A1 ? ? ? ? 8B 80 ? ? ? ? C3")) + 1);
	globals = **reinterpret_cast<c_global_vars_base***>((*reinterpret_cast<uintptr_t**>(client)[0] + 27));
	directx = **(IDirect3DDevice9***)(utilities::pattern_scan(GetModuleHandleA(XorStr("shaderapidx9.dll")), XorStr("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 1);
	input = *(i_input**)(utilities::pattern_scan(GetModuleHandleA(XorStr("client_panorama.dll")), XorStr("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10")) + 1);
	render_beams = *reinterpret_cast<i_view_render_beams**>((utilities::pattern_scan(GetModuleHandleA(XorStr("client_panorama.dll")), XorStr("B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9")) + 1));
	glow_manager = (glow_manager_t *)(*(uintptr_t *)(utilities::pattern_scan(GetModuleHandleA(XorStr("client_panorama.dll")), XorStr("0F 11 05 ? ? ? ? 83 C8 01 C7 05 ? ? ? ? 00 00 00 00")) + 3));
	move_helper = **(player_move_helper***)(utilities::pattern_scan(GetModuleHandleA(XorStr("client_panorama.dll")), XorStr("8B 0D ? ? ? ? 8B 46 08 68")) + 2); //test
}