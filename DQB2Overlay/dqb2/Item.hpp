#pragma once

#include <string>
#include <vector>

struct Item
{
	int id;
	int count;
};

struct ItemInfo
{
	std::string name;
	bool link;
};

void ImportItem();
ItemInfo* SearchInfo(int id);
std::vector<int> SearchID(std::string& name);
