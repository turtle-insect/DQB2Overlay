#include <Windows.h>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"
#include "TemplateItem.hpp"
#include "blueprint.hpp"
#include "dqb2.hpp"

uintptr_t GetBaseAddress()
{
	struct
	{
		const TCHAR* name;
		uintptr_t distance;
	}static s_app[]
		=
	{
		{L"DQB2.exe", 0x137E490},
		{L"DQB2_EU.exe", 0x13AF558},
		{L"DQB2_AS.exe", 0x139D3F8},
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

void ItemTemplateItem()
{
	static int current_index = 0;
	
	ImGui::Text("Template Item");
	ImGui::Combo("   ", &current_index, GetTemplateItemNames(), GetTemplateItemCount());
	ImVec2 size = { 75, 25 };
	if (ImGui::Button("Import", size))
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
	ImVec2 size = { 75, 25 };
	if (ImGui::Button(name, size))
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
	ImVec2 size = { 75, 25 };
	if (ImGui::Button(name, size))
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
	if (ImGui::CollapsingHeader("Item"))
	{
		ImGui::Text("Inventory");
		ImVec2 size = { 160, 25 };
		if (ImGui::Button("All 777", size))
		{
			uintptr_t address = GetBaseAddress() + 0xB88650;
			for (int index = 0; index < 15; index++)
			{
				short id = *(short*)(address + index * 4);
				if (id == 0) continue;

				*(short*)(address + index * 4 + 2) = 777;
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
		if (ImGui::Button(" All 777 ", size))
		{
			uintptr_t address = GetBaseAddress() + 0xB8DF74;
			for (int index = 0; index < 420; index++)
			{
				short id = *(short*)(address + index * 4);
				if (id == 0) continue;

				*(short*)(address + index * 4 + 2) = 777;
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

void PlayerMenu()
{
	if (ImGui::CollapsingHeader("Player"))
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