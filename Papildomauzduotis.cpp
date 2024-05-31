#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <regex>

// Structure to hold a word and its occurrences
struct WordInfo {
    std::string word;
    int count;
    std::vector<int> lines;
};

// Function to remove punctuation from a word, except for characters that can appear in URLs
std::string cleanWord(const std::string& word) {
    std::string cleaned;
    for (char ch : word) {
        if (!std::ispunct(static_cast<unsigned char>(ch)) || ch == '.' || ch == '/' || ch == ':' || ch == '–') {
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

// Function to find a word in a vector
int findWordIndex(const std::vector<WordInfo>& wordList, const std::string& word) {
    for (size_t i = 0; i < wordList.size(); ++i) {
        if (wordList[i].word == word) {
            return i;
        }
    }
    return -1;
}

int main() {
    std::ifstream inputFile("text.txt");
    std::ofstream wordOutput("zodziai.txt");
    std::ofstream urlOutput("nuorodos.txt");

    if (!inputFile) {
        std::cerr << "Error: Unable to open input file" << std::endl;
        return 1;
    }

    std::vector<WordInfo> wordList;
    std::vector<WordInfo> urlList;
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
                    int index = findWordIndex(urlList, cleanedWord);
                    if (index != -1) {
                        urlList[index].count++;
                        urlList[index].lines.push_back(lineNumber);
                    }
                    else {
                        urlList.push_back({ cleanedWord, 1, {lineNumber} });
                    }
                }
                else {
                    int index = findWordIndex(wordList, cleanedWord);
                    if (index != -1) {
                        wordList[index].count++;
                        wordList[index].lines.push_back(lineNumber);
                    }
                    else {
                        wordList.push_back({ cleanedWord, 1, {lineNumber} });
                    }
                }
            }
        }
    }

    inputFile.close();

    // Output words that appear more than once along with their counts and line numbers
    wordOutput << "Pasikartojantys zodziai:\n";
    for (const auto& iv : wordList) {
        if (iv.count > 1) {
            wordOutput << iv.word << ": " << iv.count << " [teksto eilutese: ";
            for (const int& lin : iv.lines) {
                wordOutput << lin << " ";
            }
            wordOutput << "]\n";
        }
    }

    // Output all URLs
    urlOutput << "Nuorodos:\n";
    for (const auto& entry : urlList) {
        urlOutput << entry.word << "\n";
    }
    wordOutput.close();
    urlOutput.close();
    return 0;
}
