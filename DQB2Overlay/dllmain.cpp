#include <cstdio>
#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include "kiero/kiero.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "input.hpp"
#include "blueprint.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef HRESULT(STDMETHODCALLTYPE* Present)(
	IDXGISwapChain* This,
	/* [in] */ UINT SyncInterval,
	/* [in] */ UINT Flags);

static bool g_Execute = false;
static bool g_Visible = true;
static HWND g_Hwnd;
static WNDPROC g_WndProc;
static Present g_Present;
static ID3D11Device* g_pDevice = NULL;
static ID3D11DeviceContext* g_pContext = NULL;
Input g_inputMenu(VK_INSERT);

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

void CreateBluePrintButton(int index,const char* const name)
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
		if (ImGui::Button("Count", size))
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
		if (ImGui::Button(" Count ", size))
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

void ExternalMenu()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Menu");
	GeneralMenu();
	ItemMenu();
	PlayerMenu();
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

LRESULT CALLBACK WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(g_WndProc, hWnd, uMsg, wParam, lParam);
}

bool InitDevice(IDXGISwapChain* pSwapChain)
{
	if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_pDevice))) return false;

	g_pDevice->GetImmediateContext(&g_pContext);
	DXGI_SWAP_CHAIN_DESC sd;
	pSwapChain->GetDesc(&sd);
	g_Hwnd = sd.OutputWindow;
	g_WndProc = (WNDPROC)SetWindowLongPtr(g_Hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\meiryo.ttc", 18.0f);
	ImGui_ImplWin32_Init(g_Hwnd);
	ImGui_ImplDX11_Init(g_pDevice, g_pContext);
	return true;
}

void CleanupDevice()
{
	kiero::bind(8, (void**)&g_Present, g_Present);
	if (g_pContext) g_pContext->Release();
	if (g_pDevice) g_pDevice->Release();
}

HRESULT STDMETHODCALLTYPE hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	static bool s_isInit = false;

	if (!g_Execute) return 0;
	if (!s_isInit)
	{
		s_isInit = InitDevice(pSwapChain);
	}

	if (s_isInit && g_Visible)
	{
		ExternalMenu();
	}

	return g_Present(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	if (kiero::init(kiero::RenderType::D3D11) != kiero::Status::Success) return 0;
	g_Execute = true;
	kiero::bind(8, (void**)&g_Present, hkPresent);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImportItem();
	ImportBlock();

	for (; g_Execute;)
	{
		g_inputMenu.Update();
		if (g_inputMenu.isPress())
		{
			g_Visible = !g_Visible;
		}
		Sleep(1);
	}

	SetWindowLongPtr(g_Hwnd, GWLP_WNDPROC, (LONG_PTR)g_WndProc);
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	CleanupDevice();
	kiero::shutdown();
	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
		break;
	}
	return TRUE;
}
