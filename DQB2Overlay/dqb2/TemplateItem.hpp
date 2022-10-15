#pragma once

#include <string>
#include <vector>
#include "Item.hpp"

struct TemplateItem
{
	std::vector<Item> items;
};

void ImportTemplate();
int GetTemplateItemCount();
char* GetTemplateItemNames();
std::vector<TemplateItem>& GetTemplateItems();
