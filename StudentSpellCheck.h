#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>
using namespace std;

class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck();
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
    struct TrieNode
    {
        TrieNode(char c);
        ~TrieNode();
        TrieNode* children[27];
        char ch;
        bool isEnd;
        void addNode(string word);
        bool findNode(string word);
        void deleteTrie();
    };
    TrieNode* m_root;
};

#endif  // STUDENTSPELLCHECK_H_
