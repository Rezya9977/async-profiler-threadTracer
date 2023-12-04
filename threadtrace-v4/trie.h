#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>


class Trie
{
private:
    class TrieNode
    {
    public:
        std::unordered_map<char, TrieNode*> children;
        bool is_end_of_string;
        bool traversed;

        TrieNode() : is_end_of_string(false), traversed(false) {}
    };

    TrieNode* root;

    void getAllWordsFromNode(TrieNode* node, const std::string& currentPrefix, std::vector<std::string>& result)
    {
        if (node->is_end_of_string)
        {
            result.push_back(currentPrefix);
        }

        for (auto& entry : node->children)
        {
            TrieNode* childNode = entry.second;
            getAllWordsFromNode(childNode, currentPrefix + entry.first, result);
        }
    }

public:
    Trie();
    // TODO:析构函数
    ~Trie() {};

    TrieNode* getRoot() const;

    void insert(const std::string& word);

    std::vector<std::string> findStringWithCommonPrefix(const std::string& prefix);
    // DFS前缀树，记录最接近叶子节点的分支的字符串
    void traverseTrieDFS(
        TrieNode* node, std::string commonPrefix, int currentBranchSerialNumber,
        TrieNode* rootNode, std::vector<std::string>& commonLongestPrefix,
        std::unordered_set<int>& completedBranches);
};