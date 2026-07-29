#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

int main() {
    std::ifstream input_file("example.txt", std::ios::binary | std::ios::ate);
    if (!input_file) {
        std::printf("Failed to open file\n");
        return EXIT_FAILURE;
    }

    const auto size_bytes = input_file.tellg();
    std::printf("Size (In Bytes) %llu\n", static_cast<unsigned long long>(size_bytes));

    input_file.seekg(std::ios::beg);

    std::stringstream string_stream;
    string_stream << input_file.rdbuf();

    std::printf("%s\n", string_stream.str().c_str());

    input_file.close();

    // Add a new line at the bottom of our file
    // Open the file in append mode if you don't want to remove the existing data
    std::ofstream output_file("example.txt", std::ios::binary | std::ios::app);
    if (!output_file) {
        std::printf("Failed to open file\n");
        return EXIT_FAILURE;
    }

    std::string new_line = "Hell yeah new line!\n";
    output_file << new_line;

    output_file.close();

    return EXIT_SUCCESS;
}
