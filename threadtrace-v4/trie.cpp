#include "trie.h"
#include <algorithm>

Trie::Trie() : root(new TrieNode()) {}

Trie::~Trie()
{
    deleteNode(this->root);
}

void Trie::deleteNode(TrieNode *node)
{
    if (node == NULL)
    {
        return;
    }
    std::unordered_map<char, TrieNode*> children = node->children;
    for (std::unordered_map<char, TrieNode*>::iterator it = children.begin(); it != children.end(); ++it) {
        deleteNode(it->second);
    }
    delete node;
}
Trie::TrieNode *Trie::getRoot() const
{
    return this->root;
}

void Trie::insert(const std::string &word)
{
    TrieNode *node = root;
    for (char ch : word)
    {
        if (node->children.find(ch) == node->children.end())
        {
            node->children[ch] = new TrieNode();
        }
        node = node->children[ch];
    }
    node->is_end_of_string = true;
}

std::vector<std::string> Trie::findStringWithCommonPrefix(const std::string &prefix)
{
    TrieNode *node = root;
    std::vector<std::string> result;

    for (char ch : prefix)
    {
        if (node->children.find(ch) == node->children.end())
        {
            return result;
        }
        node = node->children[ch];
    }

    getAllWordsFromNode(node, prefix, result);

    return result;
}

// DFS前缀树，记录最接近叶子节点的分支的前缀字符串
void Trie::traverseTrieDFS(
    TrieNode *node, std::string common_prefix, int current_branch_serial_number,
    TrieNode *root, std::vector<std::string> &common_longest_prefix,
    std::unordered_set<int> &completed_branches)
{
    if (!node)
    {
        return;
    }
    node->traversed = true;
    // TODO:剪枝似乎还能优化
    for (auto &entry : node->children)
    {
        char ch = entry.first;
        if (completed_branches.find(current_branch_serial_number) != completed_branches.end())
        {
            traverseTrieDFS(entry.second, common_prefix + ch, current_branch_serial_number + 1, root, common_longest_prefix, completed_branches);
        }
        else
        {
            traverseTrieDFS(entry.second, common_prefix + ch, current_branch_serial_number, root, common_longest_prefix, completed_branches);
        }
    }

    if (node->children.size() > 1 && node->traversed && completed_branches.find(current_branch_serial_number) == completed_branches.end())
    {
        common_longest_prefix.push_back(common_prefix);
        completed_branches.insert(current_branch_serial_number);
    }
}

// // 用假数据测试一下
// int main() {
//     std::vector<std::string> thread_names = {
//         "HikariCP-myPool-1",
//         "HikariCP-myPool-2",
//         "Druid-ConnectionPool-Create-9",
//         "Druid-ConnectionPool-Create-3",
//         "pool-1-thread-1",
//         "pool-1-thread-2",
//         "pool-2-thread-1",
//         "pool-2-thread-2",
//         "rhg uaeiuh",
//     };

//     Trie trie;
//     for (const std::string& item : thread_names) {
//         trie.insert(item);
//     }
//     // 是否保留排序操作有待考量
//     // std::sort(thread_names.begin(), thread_names.end(), [](const std::string& a, const std::string& b) {
//     //     return a.length() > b.length();
//     // });

//     int current_branch_serial_number = 1;
//     std::vector<std::string> common_longest_prefix;
//     std::unordered_set<int> completed_branches;
//     trie.traverseTrieDFS(trie.getRoot(), "", current_branch_serial_number, trie.getRoot(),
//                                                 common_longest_prefix, completed_branches);

//     std::vector<std::vector<std::string>> result;
//     std::unordered_set<std::string> resultSetWithCommonPrefix;

//     for (const std::string& prefix : common_longest_prefix) {
//         std::vector<std::string> common_prefix_strings = trie.findStringWithCommonPrefix(prefix);
//         result.push_back(common_prefix_strings);
//         resultSetWithCommonPrefix.insert(common_prefix_strings.begin(), common_prefix_strings.end());
//     }

//     std::unordered_set<std::string> allWordsSet(thread_names.begin(), thread_names.end());

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
