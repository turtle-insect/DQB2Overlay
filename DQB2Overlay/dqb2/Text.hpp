#pragma once

#include <string>
#include <vector>

typedef void (*oneline_func)(std::string&);

int s2i(std::string& word);
std::vector<std::string> split(std::string& text, char delimiter);
void ReadText(std::string filename, oneline_func func);