#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "DEBUG: Program started" << std::endl;

    if (argc < 2 || argv[1] == nullptr) {
        std::cerr << "Error: No filename provided!" << std::endl;
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::string file_name = argv[1];
    std::cout << "DEBUG: Processing file: " << file_name << std::endl;

    std::ifstream ifile(file_name);
    if (!ifile) {
        std::cerr << "Error: Cannot open file '" << file_name << "'" << std::endl;
        return 1;
    }

    std::cout << "DEBUG: Successfully opened file" << std::endl;

    std::string input;
    int line_count = 0;  // Counter to prevent infinite loops

    while (ifile >> input) {
        std::cout << "Processing: " << input << std::endl;
        line_count++;

        if (line_count > 100000) {  // Arbitrary safety limit
            std::cerr << "Error: Too many iterations, possible infinite loop!" << std::endl;
            break;
        }
    }

    std::cout << "DEBUG: Finished processing" << std::endl;
    return 0;
}
