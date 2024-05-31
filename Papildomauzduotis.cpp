#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <regex>

// Structure to hold a word and its occurrences
struct WordInfo {
    int count;
    std::vector<int> lines;
};

// Function to remove punctuation from a word, except for characters that can appear in URLs
std::string cleanWord(const std::string& word) {
    std::string cleaned;
    for (char ch : word) {
        if (ch != '–' && ch != '-' && (!std::ispunct(static_cast<unsigned char>(ch)) || ch == '.' || ch == '/' || ch == ':')) {
            cleaned += ch;
        }
        else if (std::string("אטזכבנרû").find(ch) != std::string::npos) {
            cleaned += ch;
        }
    }
    return cleaned;
}



// Function to check if a word is a URL
bool isUrl(const std::string& word) {
    const std::regex urlRegex(R"((http://|https://|www\.|[a-zA-Z0-9.-]+\.(com|org|net|edu|gov|io|co|lt)))");
    return std::regex_search(word, urlRegex);
}

int main() {
    std::ifstream inputFile("text.txt");
    std::ofstream wordOutput("zodziai.txt");
    std::ofstream urlOutput("nuorodos.txt");

    if (!inputFile) {
        std::cerr << "Error: Unable to open input file" << std::endl;
        return 1;
    }

    std::unordered_map<std::string, WordInfo> wordMap;
    std::unordered_map<std::string, WordInfo> urlMap;
    std::string line;
    int lineNumber = 0;

    while (std::getline(inputFile, line)) {
        ++lineNumber;
        std::istringstream lineStream(line);
        std::string word;

        while (lineStream >> word) {
            std::string cleanedWord = cleanWord(word);
            if (!cleanedWord.empty()) {
                if (isUrl(cleanedWord)) {
                    auto it = urlMap.find(cleanedWord);
                    if (it != urlMap.end()) {
                        it->second.count++;
                        it->second.lines.push_back(lineNumber);
                    }
                    else {
                        urlMap[cleanedWord] = { 1, {lineNumber} };
                    }
                }
                else {
                    auto it = wordMap.find(cleanedWord);
                    if (it != wordMap.end()) {
                        it->second.count++;
                        it->second.lines.push_back(lineNumber);
                    }
                    else {
                        wordMap[cleanedWord] = { 1, {lineNumber} };
                    }
                }
            }
        }
    }

    inputFile.close();

    // Output words that appear more than once along with their counts and line numbers
    wordOutput << "Pasikartojantys zodziai:\n";
    for (const auto& entry : wordMap) {
        if (entry.second.count > 1) {
            wordOutput << entry.first << ": " << entry.second.count << " [teksto eilutese: ";
            for (const int& lin : entry.second.lines) {
                wordOutput << lin << " ";
            }
            wordOutput << "]\n";
        }
    }

    // Output all URLs
    urlOutput << "Nuorodos:\n";
    for (const auto& entry : urlMap) {
        urlOutput << entry.first << "\n";
    }
    wordOutput.close();
    urlOutput.close();
    return 0;
}