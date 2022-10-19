#pragma once

#include <Windows.h>
#include <d3d11.h>
#include "IMenuAction.hpp"

class DQBMenu : public IMenuAction
{
public:
	DQBMenu()
		: m_Visible(true) {}
	void ChangeVisible() {m_Visible = !m_Visible;}
	void LoadDevice(HWND hwnd, ID3D11Device* pDevice);
	virtual void Initialize();
	virtual void CreateMenu();
	virtual void Finalize();

private:
	bool m_Visible;
	ID3D11Device* m_pDevice = NULL;
	ID3D11DeviceContext* m_pContext = NULL;
};

class DQBGeneralMenu : public IMenuAction
{
public:
	virtual void Initialize();
	virtual void CreateMenu();
};

class DQBBuilderHeart : public IMenuAction
{
public:
	virtual void CreateMenu();
};

class DQBTimeofDay : public IMenuAction
{
public:
	virtual void CreateMenu();
};

class DQBItemMenu : public IMenuAction
{
public:
	virtual void Initialize();
	virtual void CreateMenu();
};

class DQBItemControl : public IMenuAction
{
public:
	DQBItemControl()
		: m_Count(777) {}
	virtual void CreateMenu();

private:
	void ChangeItemCount(uintptr_t address, size_t size);
	void ClearItemCount(uintptr_t address, size_t size);
	int m_Count;
};

class DQBImportItemTemplate : public IMenuAction
{
public:
	DQBImportItemTemplate()
		: m_SelectedIndex(0){}
	virtual void CreateMenu();

private:
	int m_SelectedIndex;
};

class DQBBluePrintMenu : public IMenuAction
{
public:
	virtual void Initialize();
	virtual void CreateMenu();
};

class DQBBluePrintControl : public IMenuAction
{
public:
	virtual void CreateMenu();

private:
	void ImportBluePrintItem(const char* name, int color_number, size_t index);
	void ImportBluePrintItem(const char* name, size_t index);
	void ClearBluePrint(const char* name, int color_number, size_t index);
};

class DQBPlayerMenu : public IMenuAction
{
public:
	virtual void Initialize();
	virtual void CreateMenu();
};

class DQBPlayerPosition : public IMenuAction
{
public:
	virtual void CreateMenu();
};

class DQBPlayerMoonJump : public IMenuAction
{
public:
	DQBPlayerMoonJump()
		: m_isMoonJump(false)
	{}
	virtual void CreateMenu();
	virtual void ExecuteAction();

private:
	bool m_isMoonJump;
};
