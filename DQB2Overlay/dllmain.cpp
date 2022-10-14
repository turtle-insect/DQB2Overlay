#include <cstdio>
#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include "kiero/kiero.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "input.hpp"
#include "dqb2/dqb2.hpp"

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
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ExternalMenu();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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

	Initialize();

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
	kiero::unbind(8);
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
