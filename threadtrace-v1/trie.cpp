#include <iostream>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

class Trie {
public:
    Trie() {}

    // 在前缀树插入一个单词
    void insert(const std::string& word) {
        Trie* cur = this;
        for (char c : word) {
            // 如果当前字符不在子节点中，则创建一个新的 Trie 节点
            if (cur->data_.find(c) == cur->data_.end()) {
                cur->data_[c] = std::unique_ptr<Trie>(new Trie());
            }
            // 移动指针到子节点
            cur = cur->data_[c].get();
        }
        // 标记当前节点表示一个单词的结束
        cur->is_end_ = true;
    }

    // 返回word是否在前缀树中
    // 常量函数，用于在不修改对象的情况下访问 Trie 的元素
    bool search(const std::string& word) const {
        const Trie* cur = this;  // 指针常量，防止修改指针指向的内容
        for (char c : word) {
            // 如果当前字符不在子节点中，说明单词不存在
            if (cur->data_.find(c) == cur->data_.end()) {
                return false;
            }
            // 移动指针到子节点
            cur = cur->data_.at(c).get();
        }
        // 检查当前节点是否表示一个完整的单词
        return cur->is_end_;
    }

    // 返回前缀树中是否有此前缀
    bool startsWith(const std::string& prefix) const {
        const Trie* cur = this;
        for (char c : prefix) {
            // 如果当前字符不在子节点中，说明前缀不存在
            if (cur->data_.find(c) == cur->data_.end()) {
                return false;
            }
            // 移动指针到子节点
            cur = cur->data_.at(c).get();
        }
        // 前缀存在
        return true;
    }

    // 找到某个前缀的所有单词
    std::vector<std::string> findAllWordsWithPrefix(const std::string& prefix) const {
        const Trie* cur = this;
        for (char c : prefix) {
            // 如果当前字符不在子节点中，说明前缀不存在
            if (cur->data_.find(c) == cur->data_.end()) {
                return {};  // 返回空向量表示没有匹配的前缀
            }
            // 移动指针到子节点
            cur = cur->data_.at(c).get();
        }
        // 递归获取所有以当前节点为前缀的单词
        return getAllWordsFromNode(cur, prefix);
    }

    // 深度优先搜索，获取所有叶子节点对应的单词
    void dfs(const Trie *node, const std::string &prefix, std::vector<std::string> &result) const {
        if (node->is_end_) {
            result.push_back(prefix);
        }
        for (const auto &entry : node->data_) {
            char ch = entry.first;
            const Trie *child = entry.second.get();
            dfs(child, prefix + ch, result);
        }
    }

    // 获取所有叶子节点对应的单词
    std::vector<std::string> getAllWords() const {
        std::vector<std::string> result;
        dfs(this, "", result);
        return result;
    }

    // 根据单词的最长前缀进行分组
    std::map<std::string, std::vector<std::string>> groupByLongestPrefix() const {
        std::vector<std::string> allWords = getAllWords();
        std::map<std::string, std::vector<std::string>> groupedWords;

        for (const auto &word : allWords) {
            for (size_t i = 1; i <= word.size(); ++i) {
                std::string prefix = word.substr(0, i);
                groupedWords[prefix].push_back(word);
            }
        }

        return groupedWords;
    }


private:
    std::unordered_map<char, std::unique_ptr<Trie>> data_;  // 存储子节点的容器
    bool is_end_{ false };  // 标记当前节点是否表示一个单词的结束

    // 递归获取以当前节点为前缀的所有单词
    std::vector<std::string> getAllWordsFromNode(const Trie* node, const std::string& prefix) const {
        std::vector<std::string> words;
        if (node->is_end_) {
            words.push_back(prefix);
        }
        for (const auto& entry : node->data_) {
            char ch = entry.first;
            const Trie* child = entry.second.get();
            for (const std::string& suffix : getAllWordsFromNode(child, prefix + ch)) {
                words.push_back(suffix);
            }
        }
        return words;
    }
};