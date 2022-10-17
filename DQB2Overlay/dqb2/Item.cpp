#include <map>
#include "Text.hpp"
#include "Item.hpp"

typedef std::map<int, ItemInfo> mapItem;
static mapItem s_items;

void AppendItem(std::string& oneline)
{
	if (oneline.length() < 3) return;
	if (oneline[0] == '#') return;

	auto words = split(oneline, '\t');
	if (words.size() < 5) return;

	int id = s2i(words[0]);
	if (id == 0) return;
	if (s_items.find(id) != s_items.end()) return;

	ItemInfo info;
	info.name = words[1];
	info.link = words[4] == "TRUE";

	s_items.insert(std::make_pair(id, info));
}

void ImportItem()
{
	s_items.clear();
	ReadText("info/item.txt", AppendItem);
}

ItemInfo* SearchInfo(int id)
{
	auto finder = s_items.find(id);

	if (finder == s_items.end())
	{
		return nullptr;
	}

	return &finder->second;
}

std::vector<int> SearchID(std::string& name)
{
	std::vector<int> ids;
	for (auto item : s_items)
	{
		if (item.second.name == name)
		{
			if (item.second.link)
			{
				ids.clear();
				ids.push_back(item.first);
				break;
			}

			ids.push_back(item.first);
		}
	}

	return ids;
}
