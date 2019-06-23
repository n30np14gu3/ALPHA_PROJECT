#include <algorithm>
#include "parser.hpp"
#include "../../../source-sdk/valve parser/valve_parser.h"

c_kit_parser kit_parser;

std::vector<paint_kit> parser_skins;
std::vector<paint_kit> parser_gloves;

class CCStrike15ItemSchema;
class CCStrike15ItemSystem;


template <typename Key, typename Value>
struct node_t {
	int previous_id;
	int next_id;
	void* _unknown_ptr;
	int _unknown;
	Key key;
	Value value;
};


template <typename key, typename value>
struct head_t {
	node_t<key, value>* memory;
	int allocation_count;
	int grow_size;
	int start_element;
	int next_available;
	int _unknown;
	int last_element;
};

struct string_t {
	char* buffer;
	int capacity;
	int grow_size;
	int length;
};

struct paint_kit_t {
	int id;
	string_t name;
	string_t description;
	string_t item_name;
	string_t material_name;
	string_t image_inventory;
	char pad_0x0054[0x8C];
};

void* get_export(const char* module_name, const char* export_name) {
	HMODULE mod;
	while (!((mod = GetModuleHandleA(module_name))))
		Sleep(100);
	return reinterpret_cast<void*>(GetProcAddress(mod, export_name));
}

void c_kit_parser::setup() noexcept 
{
	valve_parser::Document items_document;
	bool item_document_loaded = items_document.Load((char*)".\\csgo\\scripts\\items\\items_game.txt", valve_parser::ENCODING::UTF8);
	if (!item_document_loaded)
		return;

	valve_parser::Document csgo_document;
	bool csgo_document_loaded = csgo_document.Load((char*)".\\csgo\\resource\\csgo_english.txt", valve_parser::ENCODING::UTF16_LE);
	if (!csgo_document_loaded)
		return;

	auto items = items_document.BreadthFirstSearch((wchar_t*)(L"items"));
	if (!items || !items->ToObject())
		return;

	auto paint_kits = items_document.BreadthFirstSearch((wchar_t*)L"paint_kits");
	if (!paint_kits || !paint_kits->ToObject())
		return;

	auto paint_kits_rarity = items_document.BreadthFirstSearch((wchar_t*)L"paint_kits_rarity");
	if (!paint_kits_rarity || !paint_kits_rarity->ToObject())
		return;

	auto weapon_icons = items_document.BreadthFirstSearch((wchar_t*)L"weapon_icons");
	if (!weapon_icons || !weapon_icons->ToObject())
		return;

	auto tokens = csgo_document.BreadthFirstSearch((wchar_t*)L"Tokens");
	if (!tokens || !tokens->ToObject())
		return;

	for (auto child : paint_kits->children)
	{
		if (child->ToObject())
		{
			std::wstring paintKitId = child->ToObject()->name.toString();
			int paint_kit_id = atoi(std::string(paintKitId.begin(), paintKitId.end()).c_str());
			std::wstring paint_kit_name = child->ToObject()->GetKeyByName((wchar_t*)L"name")->ToKeyValue()->Value.toString();

			if (paint_kit_id == 0 || paint_kit_id == 9001) continue;

			std::wstring paint_kit_description_tag = child->ToObject()->GetKeyByName((wchar_t*)L"description_tag")->ToKeyValue()->Value.toString();

			paint_kit_description_tag.erase(paint_kit_description_tag.begin());
			std::transform(paint_kit_description_tag.begin(), paint_kit_description_tag.end(), paint_kit_description_tag.begin(), towlower);

			if(paint_kit_id < 10000)
				parser_skins.push_back({ paint_kit_id, std::string(paint_kit_name.begin(), paint_kit_name.end()) });
			else
				parser_gloves.push_back({ paint_kit_id, std::string(paint_kit_name.begin(), paint_kit_name.end()) });

		}
		std::sort(parser_skins.begin(), parser_skins.end());
		std::sort(parser_gloves.begin(), parser_gloves.end());
	}
}
