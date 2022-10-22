#include <cstdio>
#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include "kiero/kiero.h"
#include "imgui/imgui.h"

#include "dqb2/dqb2.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef HRESULT(STDMETHODCALLTYPE* Present)(
	IDXGISwapChain* This,
	/* [in] */ UINT SyncInterval,
	/* [in] */ UINT Flags);

static HWND g_Hwnd;
static WNDPROC g_WndProc;
static Present g_Present;
DQBMenu g_Menu;


LRESULT CALLBACK WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(g_WndProc, hWnd, uMsg, wParam, lParam);
}

bool InitDevice(IDXGISwapChain* pSwapChain)
{
	ID3D11Device* pDevice;
	if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice))) return false;

	DXGI_SWAP_CHAIN_DESC sd;
	pSwapChain->GetDesc(&sd);
	g_Hwnd = sd.OutputWindow;
	g_WndProc = (WNDPROC)SetWindowLongPtr(g_Hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);

	g_Menu.LoadDevice(g_Hwnd, pDevice);
	return true;
}

HRESULT STDMETHODCALLTYPE hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	static bool s_isInit = false;

	if (!s_isInit)
	{
		s_isInit = InitDevice(pSwapChain);
	}

	if (s_isInit)
	{
		g_Menu.CreateMenu();
		g_Menu.ExecuteAction();
	}

	return g_Present(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	if (kiero::init(kiero::RenderType::D3D11) != kiero::Status::Success) return 0;
	kiero::bind(8, (void**)&g_Present, hkPresent);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	g_Menu.Initialize();

	for (;;)
	{
		g_Menu.Update();
		Sleep(1);
	}

	g_Menu.Finalize();
	SetWindowLongPtr(g_Hwnd, GWLP_WNDPROC, (LONG_PTR)g_WndProc);
	kiero::unbind(8);
	kiero::shutdown();
	return 0;
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
