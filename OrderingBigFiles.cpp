#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <limits>

const int number_of_partitions = 10;

std::vector<char> read_partition(const std::string& file_name, int partition_number) {
    std::ifstream file(file_name, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_name << std::endl;
    }

    std::streamsize size = file.tellg();
    std::streamsize new_size = size / number_of_partitions;
    int start_pos = new_size * partition_number / number_of_partitions;

    file.seekg(start_pos, std::ios::beg);

    std::vector<char> buffer(new_size);
    if (file.read(buffer.data(), new_size)) {
        return buffer;
    } else {
        std::cerr << "Error reading from file: " << file_name << std::endl;
    }
}

std::vector<std::string> generatePartitionFiles(const std::string& inputFileName, const std::string& baseFileName) {
    std::vector<std::string> partitionFileNames;

    for (int i = 0; i < number_of_partitions; ++i) {
        std::string fileName = baseFileName + std::to_string(i) + ".txt";
        partitionFileNames.push_back(fileName);

        std::vector<char> buffer = read_partition(inputFileName, i);
        std::sort(buffer.begin(), buffer.end());

        std::ofstream myfile(fileName);
        if (!myfile.is_open()) {
            std::cerr << "Error opening file for writing: " << fileName << std::endl;
        }

        for (auto const& x : buffer) {
            std::cout << x;
            myfile << x;
        }

        myfile.close(); // Close the file explicitly
    }

    return partitionFileNames;
}

bool is_empty(std::ifstream& file) {
    return file.peek() == std::ifstream::traits_type::eof();
}

void interleaveFiles(const std::vector<std::string>& inputFiles, const std::string& outputFile) {
    const int numFiles = inputFiles.size();
    const int highValue = std::numeric_limits<int>::max();

    std::vector<std::ifstream> fileStreams;

    for (const std::string& fileName : inputFiles) {
        std::ifstream file(fileName, std::ios::in);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << fileName << std::endl;
        }

        fileStreams.push_back(std::move(file));
        std::cout << "Opened file: " << fileName << std::endl;
    }

    std::ofstream output(outputFile);
    if (!output.is_open()) {
        std::cerr << "Error opening output file for writing: " << outputFile << std::endl;
    }

    std::vector<int> positions(numFiles, 0);
    bool end = false;

    while (!end) {
        int menor = std::numeric_limits<int>::max();
        int pos_menor = 0;

        for (int i = 0; i < numFiles; ++i) {
            char character = ' ';
            if (!is_empty(fileStreams[i])) {
                fileStreams[i].get(character);
                fileStreams[i].seekg(fileStreams[i].tellg()-1);
                std::cout << "Character at position " << positions[i] << " in file " << i << ": " << character << std::endl;

                if (character - '0' < menor) {
                    menor = character - '0';
                    pos_menor = i;
                }
                

            }
        }

        if (menor == std::numeric_limits<int>::max()) {
            end = true;
        } else {
            output << menor;

            positions[pos_menor]++; 
            fileStreams[pos_menor].seekg(positions[pos_menor]);
        }
    }

}

int main() {
    std::vector<std::string> inputFiles = generatePartitionFiles("inputFile.txt", "partition");
    std::string outputFile = "sorted_output.txt";

    interleaveFiles(inputFiles, outputFile);

    return 0;
}
