#include <Windows.h>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"
#include "TemplateItem.hpp"
#include "blueprint.hpp"
#include "dqb2.hpp"

static ImVec2 s_ButtonSize = { 150, 50 };

uintptr_t GetBaseAddress()
{
	struct
	{
		const TCHAR* name;
		uintptr_t distance;
	}static s_app[]
		=
	{
		{TEXT("DQB2.exe"), 0x137E490},
		{TEXT("DQB2_EU.exe"), 0x13AF558},
		{TEXT("DQB2_AS.exe"), 0x139D3F8},
	};

	uintptr_t address = 0;
	for (int index = 0; index < 3; index++)
	{
		address = (uintptr_t)GetModuleHandle(s_app[index].name);
		if (address)
		{
			address += s_app[index].distance;
			break;
		}
	}

	if (address == 0) return 0;

	address = *(uintptr_t*)address;
	address += 0x60;
	address = *(uintptr_t*)address;
	return address;

}

void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void ItemTemplateItem()
{
	static int current_index = 0;
	
	int count = GetTemplateItemCount();
	if (count == 0) return;

	ImGui::Text("Template Item");
	ImGui::SameLine();
	HelpMarker("Retrieve the items listed in the text\nfile in [/info/template] to inventory");
	ImGui::Combo("   ", &current_index, GetTemplateItemNames(), count);
	
	if (ImGui::Button("Import", s_ButtonSize))
	{
		std::vector<TemplateItem>& templateItems = GetTemplateItems();
		uintptr_t address = GetBaseAddress() + 0xB88650;
		auto items = templateItems[current_index].items;
		auto item_ite = items.begin();
		for (int index = 0; item_ite != items.end() && index < 15; index++)
		{
			short id = *(short*)(address + index * 4);
			if (id != 0) continue;

			*(short*)(address + index * 4) = item_ite->id;
			*(short*)(address + index * 4 + 2) = item_ite->count;
			++item_ite;
		}
	}
}

void GeneralBuilderHeart()
{
	ImGui::Text("Builder Heart");
	uintptr_t address = GetBaseAddress();
	int* heart = (int*)(address + 0x7E5744);
	ImGui::SliderInt(" ", heart, 0, 99999);
}

void GeneralTimeofDay()
{
	ImGui::Text("Time of Day");
	uintptr_t address = GetBaseAddress();
	float* timer = (float*)(address + 0x7E57C8);
	ImGui::SliderFloat("  ", timer, 0, 1200);
}

void BluePrintClearButton(int index, const char* const name)
{
	if (ImGui::Button(name, s_ButtonSize))
	{
		uintptr_t address = GetBaseAddress();
		address += 0x167030 + index * 0x30008;

		char* p = (char*)address;
		for (int index = 0; index < 0x30008; index++)
		{
			*p = 0;
			p++;
		}
	}
}

void BluePrintImportButton(int index, const char* const name)
{
	if (ImGui::Button(name, s_ButtonSize))
	{
		uintptr_t address = GetBaseAddress();

		auto items = CreateBluePrintItem(address + 0x167030 + index * 0x30008);
		auto item_ite = items.begin();
		address = GetBaseAddress() + 0xB8DF74;
		for (int index = 0; item_ite != items.end() && index < 420; index++)
		{
			short id = *(short*)(address + index * 4);
			if (id != 0) continue;

			*(short*)(address + index * 4) = item_ite->id;
			if (item_ite->count > 999)
			{
				*(short*)(address + index * 4 + 2) = 999;
				item_ite->count -= 999;
			}
			else if (item_ite->count)
			{
				*(short*)(address + index * 4 + 2) = item_ite->count;
				++item_ite;
			}
		}
	}
}

void BluePrintMenu()
{
	if (ImGui::CollapsingHeader("BluePrint"))
	{
		ImGui::Text("Import in Bag");
		ImGui::SameLine();
		HelpMarker("Store items needed for BluePrint's in the bag");
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
		BluePrintImportButton(4, "red");
		ImGui::PopStyleColor(1);
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.6f));
		BluePrintImportButton(5, "blue");
		ImGui::PopStyleColor(1);
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
		BluePrintImportButton(6, "green");
		ImGui::PopStyleColor(1);
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
		BluePrintImportButton(7, "yellow");
		ImGui::PopStyleColor(1);

		BluePrintImportButton(0, "online1");
		ImGui::SameLine();
		BluePrintImportButton(1, "online2");
		ImGui::SameLine();
		BluePrintImportButton(2, "online3");
		ImGui::SameLine();
		BluePrintImportButton(3, "online4");

		ImGui::Text("Clear");
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
		BluePrintClearButton(4, " red ");
		ImGui::PopStyleColor(1);
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.6f));
		BluePrintClearButton(5, " blue ");
		ImGui::PopStyleColor(1);
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
		BluePrintClearButton(6, " green ");
		ImGui::PopStyleColor(1);
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
		BluePrintClearButton(7, " yellow ");
		ImGui::PopStyleColor(1);
	}
}

void GeneralMenu()
{
	if (ImGui::CollapsingHeader("General"))
	{
		GeneralBuilderHeart();
		GeneralTimeofDay();
	}
}

void ItemMenu()
{
	static int s_count = 777;
	if (ImGui::CollapsingHeader("Item"))
	{
		ImGui::Text("Count");
		ImGui::SliderInt("    ", &s_count, 1, 999);
		ImGui::Text("Inventory");
		ImVec2 size = s_ButtonSize;
		size.x *= 2;
		if (ImGui::Button("Change", size))
		{
			uintptr_t address = GetBaseAddress() + 0xB88650;
			for (int index = 0; index < 15; index++)
			{
				short id = *(short*)(address + index * 4);
				if (id == 0) continue;

				*(short*)(address + index * 4 + 2) = s_count;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear", size))
		{
			uintptr_t address = GetBaseAddress() + 0xB88650;
			for (int index = 0; index < 15; index++)
			{
				*(short*)(address + index * 4) = 0;
				*(short*)(address + index * 4 + 2) = 0;
			}
		}

		ImGui::Text("Bag");
		if (ImGui::Button(" Change ", size))
		{
			uintptr_t address = GetBaseAddress() + 0xB8DF74;
			for (int index = 0; index < 420; index++)
			{
				short id = *(short*)(address + index * 4);
				if (id == 0) continue;

				*(short*)(address + index * 4 + 2) = s_count;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(" Clear ", size))
		{
			uintptr_t address = GetBaseAddress() + 0xB8DF74;
			for (int index = 0; index < 420; index++)
			{
				*(short*)(address + index * 4) = 0;
				*(short*)(address + index * 4 + 2) = 0;
			}
		}

		ItemTemplateItem();
	}
}

void PlayerPosition()
{
	uintptr_t address = GetBaseAddress();
	uintptr_t pos = address + 0x954B0;
	char description[] = "Pos X";

	for (int index = 0; index < 3; index++)
	{
		float value = *(float*)pos;
		ImGui::Text(description);
		ImGui::SameLine();
		ImGui::Text("%3.2f", value);
		pos += 4;
		description[4]++;
	}
}

void PlayerMoonJump()
{
	static bool s_isMoonJump = false;
	ImGui::Checkbox("Moon Jump", &s_isMoonJump);
	if (s_isMoonJump && ImGui::IsKeyDown(ImGuiKey_GamepadFaceDown))
	{
		uintptr_t address = (uintptr_t)GetModuleHandle(TEXT("DQB2.exe"));
		if (address)
		{
			address += 0x133A8B8;
			address = *(uintptr_t*)address;
			address += 0x58;
			address = *(uintptr_t*)address;
			address += 0x198;
			address = *(uintptr_t*)address;
			address += 0x14;
			*(float*)address = 0.25f;
		}
	}
}

void PlayerMenu()
{
	if (ImGui::CollapsingHeader("Player"))
	{
		PlayerPosition();
		PlayerMoonJump();
	}
}

void Initialize()
{
	ImportItem();
	ImportBlock();
	ImportTemplate();
}

void ExternalMenu()
{
	ImGui::Begin("Menu");
	GeneralMenu();
	ItemMenu();
	BluePrintMenu();
	PlayerMenu();
	ImGui::End();
}