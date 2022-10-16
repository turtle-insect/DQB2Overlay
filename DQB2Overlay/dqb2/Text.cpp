#include <filesystem>
#include <fstream>
#include <sstream>
#include "Text.hpp"

int s2i(std::string& word)
{
	auto ss = std::istringstream(word);
	int value;
	ss >> value;
	return value;
}

std::vector<std::string> split(std::string& text, char delimiter)
{
	std::vector<std::string> words;
	if (text.length() == 0) return words;

	size_t offset = 0;
	for (;;)
	{
		size_t index = text.find(delimiter, offset);
		if (index == std::string::npos)
		{
			words.push_back(text.substr(offset));
			break;
		}
		words.push_back(text.substr(offset, index - offset));
		offset = index + 1;
	}

	return words;
}

void ReadText(std::string filename, oneline_func func)
{
	if (std::filesystem::exists(filename) == false) return;

	std::ifstream reading_file;
	reading_file.open(filename, std::ios::in);

	std::string oneline;
	for (; std::getline(reading_file, oneline);)
	{
		func(oneline);
	}
}
