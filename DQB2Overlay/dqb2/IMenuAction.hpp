#pragma once

#include <vector>

class IMenuAction
{
public:
	virtual ~IMenuAction()
	{
		Clear();
	}
	virtual void Initialize() {}
	virtual void CreateMenu()
	{
		for (auto pMenu : m_Menus)
		{
			pMenu->CreateMenu();
		}
	}

	virtual void ExecuteAction()
	{
		for (auto pMenu : m_Menus)
		{
			pMenu->ExecuteAction();
		}
	}
	virtual void Finalize() {};

protected:
	void Append(IMenuAction* pMenu)
	{
		m_Menus.push_back(pMenu);
		pMenu->Initialize();
	}
	void Clear()
	{
		for (auto pMenu : m_Menus)
		{
			pMenu->Finalize();
			pMenu->Clear();
			delete pMenu;
		}
		m_Menus.clear();
	}

private:
	std::vector<IMenuAction*> m_Menus;
};
