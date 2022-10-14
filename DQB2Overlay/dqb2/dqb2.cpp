#include <Windows.h>
#include "blueprint.hpp"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"
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

void CreateBluePrintButton(int index, const char* const name)
{
	ImVec2 size = { 70, 20 };
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

void GeneralBluePrint()
{
	ImGui::Text("BluePrint");
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
	CreateBluePrintButton(4, "red");
	ImGui::PopStyleColor(1);
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.6f));
	CreateBluePrintButton(5, "blue");
	ImGui::PopStyleColor(1);
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
	CreateBluePrintButton(6, "green");
	ImGui::PopStyleColor(1);
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
	CreateBluePrintButton(7, "yellow");
	ImGui::PopStyleColor(1);

	CreateBluePrintButton(0, "online1");
	ImGui::SameLine();
	CreateBluePrintButton(1, "online2");
	ImGui::SameLine();
	CreateBluePrintButton(2, "online3");
	ImGui::SameLine();
	CreateBluePrintButton(3, "online4");
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
		ImVec2 size = { 100, 20 };
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

		GeneralBluePrint();
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
}

void ExternalMenu()
{
	ImGui::Begin("Menu");
	GeneralMenu();
	ItemMenu();
	PlayerMenu();
	ImGui::End();
}