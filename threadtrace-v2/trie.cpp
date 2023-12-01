#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>

static std::vector<std::string> commonLongestPrefix;
static std::unordered_set<int> completedBranches;

class TrieNode {
public:
    std::unordered_map<char, TrieNode*> children;
    bool isEndOfWord;
    bool Traversed;

    TrieNode() : isEndOfWord(false), Traversed(false) {}
};

class Trie {
private:
    TrieNode* root;

public:
    Trie() : root(new TrieNode()) {}

    TrieNode* getRoot() {
        return this->root;
    }

    void insert(const std::string& word) {
        TrieNode* node = root;
        for (char ch : word) {
            if (node->children.find(ch) == node->children.end()) {
                node->children[ch] = new TrieNode();
            }
            node = node->children[ch];
        }
        node->isEndOfWord = true;
    }

    std::vector<std::string> findAllWordsWithPrefix(const std::string& prefix) {
        TrieNode* node = root;
        std::vector<std::string> result;

        for (char ch : prefix) {
            if (node->children.find(ch) == node->children.end()) {
                return result;
            }
            node = node->children[ch];
        }

        getAllWordsFromNode(node, prefix, result);

        return result;
    }

private:
    void getAllWordsFromNode(TrieNode* node, const std::string& currentPrefix, std::vector<std::string>& result) {
        if (node->isEndOfWord) {
            result.push_back(currentPrefix);
        }

        for (auto& entry : node->children) {
            TrieNode* childNode = entry.second;
            getAllWordsFromNode(childNode, currentPrefix + entry.first, result);
        }
    }
};

// DFS前缀树，记录最接近叶子节点的分支的字符串
void traverseTrieDFS(TrieNode* node, std::string commonPrefix, int currentBranchSerialNumber, TrieNode* rootNode) {
    if (!node) {
        return;
    }
    node->Traversed = true;
    // TODO:剪枝似乎还能优化
    for (auto& entry : node->children) {
        char ch = entry.first;
        if (!completedBranches.empty()) {
            traverseTrieDFS(entry.second, commonPrefix + ch, currentBranchSerialNumber + 1, rootNode);
        }
        else {
            traverseTrieDFS(entry.second, commonPrefix + ch, currentBranchSerialNumber, rootNode);
        }
    }

    if (node->children.size() > 1 && node->Traversed && completedBranches.find(currentBranchSerialNumber) == completedBranches.end()) {
        commonLongestPrefix.push_back(commonPrefix);
        completedBranches.insert(currentBranchSerialNumber);
    }
}

// int main() {
//     std::vector<std::string> words = {
//         "HikariCP-myPool-1",
//         "HikariCP-myPool-2",
//         "Druid-ConnectionPool-Create-9",
//         "Druid-ConnectionPool-Create-3",
//         "pool-1-thread-1",
//         "pool-1-thread-2",
//         "pool-2-thread-1",
//         "pool-2-thread-2",
//         "rhg uaeiuh"
//     };

//     Trie trie;
//     for (const std::string& word : words) {
//         trie.insert(word);
//     }

//     int currentBranchSerialNumber = 1;
//     traverseTrieDFS(trie.getRoot(), "", currentBranchSerialNumber, trie.getRoot());

//     std::vector<std::vector<std::string>> result;
//     std::unordered_set<std::string> resultSetWithCommonPrefix;

//     for (const std::string& prefix : commonLongestPrefix) {
//         std::vector<std::string> allWordsWithPrefix = trie.findAllWordsWithPrefix(prefix);
//         result.push_back(allWordsWithPrefix);
//         resultSetWithCommonPrefix.insert(allWordsWithPrefix.begin(), allWordsWithPrefix.end());
//     }

//     std::unordered_set<std::string> allWordsSet(words.begin(), words.end());

//     for (const std::string& word : resultSetWithCommonPrefix) {
//         allWordsSet.erase(word);
//     }

//     if (!allWordsSet.empty()) {
//         result.push_back(std::vector<std::string>(allWordsSet.begin(), allWordsSet.end()));
//     }

//     for (size_t i = 0; i < result.size(); ++i) {
//         std::string title = (i == result.size() - 1) ? "Others :" : "Group " + std::to_string(i + 1) + ":";
//         std::cout << title << std::endl;

//         const std::vector<std::string>& group = result[i];
//         for (const std::string& item : group) {
//             std::cout << "  " << item << std::endl;
//         }

//         if (i != result.size() - 1) {
//             std::cout << std::endl;  // Add a newline between groups for better readability
//         }
//     }

//     return 0;
// }
