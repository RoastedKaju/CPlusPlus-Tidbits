#pragma once

#include <iostream>
#include <fstream> // reading and writing to a file
#include <array>

namespace fs
{
	int fileStreamExample()
	{
		std::ifstream file("../example.txt", std::ios::ate);
		if (!file)
		{
			std::cout << "Failed to open file.\n";
			return EXIT_FAILURE;
		}
		auto sizeBytes = file.tellg();
		std::cout << "Size (In Characters): " << sizeBytes << '\n';
		// return to the start
		file.seekg(std::ios::beg);
		// In windows the char array is not auto null terminated, so we use std::string instead of char* array
		// or better use string stream
		//std::string buf(sizeBytes, '/0');
		//file.read(buf.data(), sizeBytes);

		std::stringstream ss;
		ss << file.rdbuf();

		// output the string
		std::cout << ss.str() << std::endl;

		// Close the file
		file.close();

		// Add a new line at the bottom of our file.
		// Open the file in append mode to insert at the end, by default its truncate mode
		std::ofstream fileOut("../example.txt", std::ios::binary | std::ios::app);
		if (!fileOut)
		{
			std::cout << "Failed to open file.\n";
			return EXIT_FAILURE;
		}

		std::string newLine = "Hell Yeah new line!!!\n";
		fileOut.write(newLine.c_str(), newLine.size());

		fileOut.close();

		return EXIT_SUCCESS;
	}
}