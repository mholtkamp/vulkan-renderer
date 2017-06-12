#include "Utilities.h"

#include <iostream>
#include <fstream>

using namespace std;

std::vector<char> ReadFile(const std::string& filename)
{
	ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw exception("Failed to open file.");
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);
	file.seekg(0);

	file.read(buffer.data(), fileSize);

	return buffer;
}