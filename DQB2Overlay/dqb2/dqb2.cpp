#include <Windows.h>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx11.h"
#include "TemplateItem.hpp"
#include "blueprint.hpp"
#include "input.hpp"
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

void DQBMenu::Initialize()
{
	ImportItem();
	ImportBlock();
	ImportTemplate();

	Append(new DQBGeneralMenu());
	Append(new DQBItemMenu());
	Append(new DQBBluePrintMenu());
	Append(new DQBPlayerMenu());
}

void DQBMenu::Update()
{
	static Input s_input(VK_INSERT);

	s_input.Update();
	if (s_input.isPress())
	{
		m_Visible = !m_Visible;
	}
}

void DQBMenu::LoadDevice(HWND hwnd, ID3D11Device* pDevice)
{
	m_pDevice = pDevice;
	m_pDevice->GetImmediateContext(&m_pContext);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.AddKeyEvent(ImGuiKey_GamepadFaceDown, true);
	ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\meiryo.ttc", 36.0f);
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);
}

void DQBMenu::CreateMenu()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (m_Visible)
	{
		ImGui::Begin("Menu");
		IMenuAction::CreateMenu();
		ImGui::End();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}

void DQBMenu::Finalize()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (m_pDevice)
	{
		m_pDevice->Release();
		m_pDevice = nullptr;
	}

	if (m_pContext)
	{
		m_pContext->Release();
		m_pContext = nullptr;
	}

	Clear();
}

void DQBGeneralMenu::Initialize()
{
	Append(new DQBBuilderHeart());
	Append(new DQBTimeofDay());
}

void DQBGeneralMenu::CreateMenu()
{
	if (ImGui::CollapsingHeader("General"))
	{
		IMenuAction::CreateMenu();
	}
}

void DQBBuilderHeart::CreateMenu()
{
	ImGui::Text("Builder Heart");
	uintptr_t address = GetBaseAddress();
	int* heart = (int*)(address + 0x7E5744);
	ImGui::SliderInt(" ", heart, 0, 99999);
}

void DQBTimeofDay::CreateMenu()
{
	ImGui::Text("Time of Day");
	uintptr_t address = GetBaseAddress();
	float* timer = (float*)(address + 0x7E57C8);
	ImGui::SliderFloat("  ", timer, 0, 1200);
}

void DQBItemMenu::Initialize()
{
	Append(new DQBItemControl());
	Append(new DQBImportItemTemplate());
}

void DQBItemMenu::CreateMenu()
{
	if (ImGui::CollapsingHeader("Item"))
	{
		IMenuAction::CreateMenu();
	}
}

void DQBItemControl::CreateMenu()
{
	ImGui::Text("Count");
	ImGui::SliderInt("    ", &m_Count, 1, 999);

	ImGui::Text("Inventory");
	ImVec2 size = s_ButtonSize;
	if (ImGui::Button("Change", size))
	{
		ChangeItemCount(0xB88650, 15);
	}
	ImGui::SameLine();
	if (ImGui::Button("Clear", size))
	{
		ClearItemCount(0xB88650, 15);
	}

	ImGui::Text("Bag");
	if (ImGui::Button(" Change ", size))
	{
		ChangeItemCount(0xB8DF74, 420);
	}
	ImGui::SameLine();
	if (ImGui::Button(" Clear ", size))
	{
		ClearItemCount(0xB8DF74, 420);
	}
}

void DQBItemControl::ChangeItemCount(uintptr_t address, size_t size)
{
	address += GetBaseAddress();
	for (size_t index = 0; index < size; index++)
	{
		short id = *(short*)(address + index * 4);
		if (id == 0) continue;

		*(short*)(address + index * 4 + 2) = m_Count;
	}
}

void DQBItemControl::ClearItemCount(uintptr_t address, size_t size)
{
	address += GetBaseAddress();
	for (int index = 0; index < size; index++)
	{
		*(short*)(address + index * 4) = 0;
		*(short*)(address + index * 4 + 2) = 0;
	}
}

void DQBImportItemTemplate::CreateMenu()
{
	int count = GetTemplateItemCount();
	if (count == 0) return;

	ImGui::Text("Template Item");
	ImGui::SameLine();
	HelpMarker("Retrieve the items listed in the text\nfile in [/info/template] to inventory");
	ImGui::Combo("   ", &m_SelectedIndex, GetTemplateItemNames(), count);

	if (ImGui::Button("Import", s_ButtonSize))
	{
		std::vector<TemplateItem>& templateItems = GetTemplateItems();
		uintptr_t address = GetBaseAddress() + 0xB88650;
		auto items = templateItems[m_SelectedIndex].items;
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

void DQBBluePrintMenu::Initialize()
{
	Append(new DQBBluePrintControl());
}

void DQBBluePrintMenu::CreateMenu()
{
	if (ImGui::CollapsingHeader("BluePrint"))
	{
		IMenuAction::CreateMenu();
	}
}

void DQBBluePrintControl::CreateMenu()
{
	struct stColorBluePrint
	{
		size_t index;
		int color_number;
		const char* name;
	};
	static stColorBluePrint s_ImportColorBluePrint[]
		=
	{
		{4, 0, "red"},
		{5, 4, "blue"},
		{6, 2, "green"},
		{7, 1, "yellow"},
	};
	static stColorBluePrint s_ClearColorBluePrint[]
	=
	{
		{4, 0, " red "},
		{5, 4, " blue "},
		{6, 2, " green "},
		{7, 1, " yellow "},
	};

	struct stOnlineBluePrint
	{
		size_t index;
		const char* name;
	};
	static stOnlineBluePrint s_ImportOnlineBluePrint[]
		=
	{
		{0, "online1"},
		{1, "online2"},
		{2, "online3"},
		{3, "online4"},
	};

	ImGui::Text("Import in Bag");
	ImGui::SameLine();
	HelpMarker("Store items needed for BluePrint's in the bag");
	for (size_t index = 0; index < 4; index++)
	{
		if (index > 0) ImGui::SameLine();

		stColorBluePrint* p = s_ImportColorBluePrint + index;
		ImportBluePrintItem(p->name, p->color_number, p->index);
	}
	for (size_t index = 0; index < 4; index++)
	{
		if (index > 0) ImGui::SameLine();

		stOnlineBluePrint* p = s_ImportOnlineBluePrint + index;
		ImportBluePrintItem(p->name, p->index);
	}

	ImGui::Text("Clear");
	for (size_t index = 0; index < 4; index++)
	{
		if (index > 0) ImGui::SameLine();

		stColorBluePrint* p = s_ClearColorBluePrint + index;
		ClearBluePrint(p->name, p->color_number, p->index);
	}
}

void DQBBluePrintControl::ImportBluePrintItem(const char* name, int color_number, size_t index)
{
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(color_number / 7.0f, 0.6f, 0.6f));
	ImportBluePrintItem(name, index);
	ImGui::PopStyleColor(1);
}

void DQBBluePrintControl::ImportBluePrintItem(const char* name, size_t index)
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

void DQBBluePrintControl::ClearBluePrint(const char* name, int color_number, size_t index)
{
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(color_number / 7.0f, 0.6f, 0.6f));
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
	ImGui::PopStyleColor(1);
}

void DQBPlayerMenu::Initialize()
{
	Append(new DQBPlayerPosition());
	Append(new DQBPlayerMoonJump());
}

void DQBPlayerMenu::CreateMenu()
{
	if (ImGui::CollapsingHeader("Player"))
	{
		IMenuAction::CreateMenu();
	}
}

void DQBPlayerPosition::CreateMenu()
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

void DQBPlayerMoonJump::CreateMenu()
{
	ImGui::Checkbox("Moon Jump", &m_isMoonJump);
}

void DQBPlayerMoonJump::ExecuteAction()
{
	if (m_isMoonJump && ImGui::IsKeyDown(ImGuiKey_GamepadFaceDown))
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
