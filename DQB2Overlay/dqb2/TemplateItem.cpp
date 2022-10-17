#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <Windows.h>
#include "Text.hpp"
#include "TemplateItem.hpp"

typedef std::vector<TemplateItem> vecTemplateItem;

static vecTemplateItem s_template_items;
static char* s_template_names = nullptr;

void ImportTemplate()
{
	static std::string dir = "info/template/";
	if (std::filesystem::exists(dir) == false) return;

	std::vector<std::string> template_names;
	for (const auto& file : std::filesystem::directory_iterator(dir))
	{
		auto path = file.path().generic_string();
		auto path_split = split(path, '/');
		if (path_split.size() < 1) continue;

		template_names.push_back(path_split[path_split.size() - 1]);

		std::ifstream reading_file;
		reading_file.open(path, std::ios::in);
		std::string oneline;

		TemplateItem temp;
		for (; std::getline(reading_file, oneline);)
		{
			if (oneline.length() < 3) continue;
			if (oneline[0] == '#') continue;

			std::vector<std::string> words = split(oneline, '\t');
			if (words.size() < 2) continue;

			int id = s2i(words[0]);
			if (SearchInfo(id) == nullptr) continue;

			Item item;
			item.id = id;
			item.count = s2i(words[1]);
			if (item.count > 999) item.count = 999;

			temp.items.push_back(item);
		}

		s_template_items.push_back(temp);
	}

	size_t length = 1;
	for (auto name : template_names)
	{
		length += name.length() + 1;
	}
	s_template_names = new char[length];
	ZeroMemory(s_template_names, length);
	length = 0;
	for (auto name : template_names)
	{
		memcpy(s_template_names + length, name.c_str(), name.length());
		length += name.length() + 1;
	}
}

int GetTemplateItemCount()
{
	return (int)s_template_items.size();
}

char* GetTemplateItemNames()
{
	return s_template_names;
}

std::vector<TemplateItem>& GetTemplateItems()
{
	return s_template_items;
}
