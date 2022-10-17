#include <vector>
#include <map>
#include <string>
#include "Text.hpp"
#include "blueprint.hpp"

// https://github.com/turtle-insect/DQB2ProcessEditor/blob/main/DQB2ProcessEditor/Info.cs

typedef std::map<int, std::map<int, std::string>*> mapBlock;
static mapBlock s_blocks;

void AppendBlock(std::string& oneline)
{
	if (oneline.length() < 3) return;
	if (oneline[0] == '#') return;

	auto words = split(oneline, '\t');
	if (words.size() < 3) return;

	int category = s2i(words[0]);
	int id = s2i(words[1]);
	std::string name = words[2];

	if (id == 0 && category == 0) return;

	std::map<int, std::string>* blocks;
	auto finder = s_blocks.find(category);
	if (finder == s_blocks.end())
	{
		blocks = new std::map<int, std::string>();
		s_blocks.insert(std::make_pair(category, blocks));
	}
	else
	{
		blocks = finder->second;
	}

	if (blocks->find(id) != blocks->end()) return;
	blocks->insert(std::make_pair(id, name));
}

void ImportBlock()
{
	for (auto ite = s_blocks.begin(); ite != s_blocks.end(); ++ite)
	{
		delete ite->second;
	}
	s_blocks.clear();
	ReadText("info/block.txt", AppendBlock);
}

std::vector<int> Search(int category, int id)
{
	std::vector<int> item_ids;
	if (s_blocks.find(category) == s_blocks.end()) return item_ids;

	auto blocks = s_blocks[category];
	auto finder = blocks->find(id);
	if (finder == blocks->end()) return item_ids;

	std::string& name = finder->second;
	if (name.length() == 0) return item_ids;

	return SearchID(name);
}

std::vector<Item> CreateBluePrintItem(uintptr_t address)
{
	std::vector<Item> items;
	size_t size = *(short*)(address + 0x30000);
	size *= *(short*)(address + 0x30002);
	size *= *(short*)(address + 0x30004);

	std::map<int, int> blocks;
	for (size_t index = 0; index < size; index++)
	{
		int key = *(int*)(address + index * 6);
		if (key == 0) continue;

		auto finder = blocks.find(key);
		if (finder == blocks.end())
		{
			blocks.insert(std::make_pair(key, 0));
		}
		blocks[key]++;
	}

	for (auto block_ite = blocks.begin(); block_ite != blocks.end(); ++block_ite)
	{
		int key = block_ite->first;
		int id = key & 0xFFFF;
		int category = (key >> 16) & 0xFFFF;
		if (id == 0)
		{
			id = category;
			category = 0;
		}

		std::vector<int> categorys = { category, 1780, 2047 };
		std::vector<int> item_ids;
		for (auto category_ite = categorys.begin(); category_ite != categorys.end(); ++category_ite)
		{
			auto item_ids = Search(*category_ite, id);
			for (auto itemid_ite = item_ids.begin(); itemid_ite != item_ids.end(); ++itemid_ite)
			{
				Item item;
				item.id = *itemid_ite;
				item.count = block_ite->second;
				items.push_back(item);
			}

			if (item_ids.size() > 0) break;
		}
	}

	return items;
}
