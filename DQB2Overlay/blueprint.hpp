#pragma once
#include<vector>
struct ItemInfo
{
	int id;
	int count;
};

std::vector<ItemInfo> CreateBluePrintItem(uintptr_t address);
void ImportItem();
void ImportBlock();
