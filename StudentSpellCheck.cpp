#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cctype>
using namespace std;




StudentSpellCheck::TrieNode::TrieNode(char c)
{
    
    for (int i = 0; i < 27; i++)
    {
        children[i] = nullptr;
    }
    ch = c;
    isEnd = false;
    
}



StudentSpellCheck::TrieNode::~TrieNode()
{
    
    deleteTrie();
    
}




void StudentSpellCheck::TrieNode::addNode(string word)
{
    
    TrieNode* node = this;
    
    for (int i = 0; i < word.length(); i++)
    {
        //apostrophe
        if (word[i] == '\'')
        {
            if (node->children[26] == nullptr)
            {
                node->children[26] = new TrieNode('\'');
            }
            //go to next node
            node = node->children[26];
        }
        //letter
        else
        {
            char c = tolower(word[i]);
            if (node->children[c - 'a'] == nullptr)
            {
                node->children[c - 'a'] = new TrieNode(word[i]);
            }
            //go to next node
            node = node->children[c - 'a'];
        }
    }
    
    //O(1)
    
    //end of word
    node->isEnd = true;
}




bool StudentSpellCheck::TrieNode::findNode(string word)
{
    //if root is nullptr
    TrieNode* node = this;
    if (node == nullptr)
    {
        return false;
    }
    for (int i = 0; i < word.length(); i++)
    {
        //apostrophe
        if (word[i] == '\'')
        {
            if (node->children[26] == nullptr)
            {
                return false;
            }
            //go to next node
            node = node->children[26];
        }
        //letter
        else
        {
            //make letters lowercase
            char c = tolower(word[i]);
            if (node->children[c - 'a'] == nullptr)
            {
                return false;
            }
            //go to next node
            node = node->children[c - 'a'];
        }
    }
    
    //if it is an entire word
    return node->isEnd;
}




void StudentSpellCheck::TrieNode::deleteTrie()
{
    
    TrieNode* cur = this;
    
    //delete children nodes
    for (int i = 0; i < 27; i++)
    {
        delete cur->children[i];
    }
    
}




//=====================================================================




SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}




StudentSpellCheck::StudentSpellCheck() {
    
    m_root = new TrieNode(' ');
    
}




StudentSpellCheck::~StudentSpellCheck() {
	
    if (m_root != nullptr)
    {
        delete m_root;
    }
    m_root = nullptr;
    
}




bool StudentSpellCheck::load(std::string dictionaryFile) {
    
    ifstream infile(dictionaryFile);
    if ( ! infile )
    {
        //failed to load
        return false;
    }
    
    //load onto trie
    string s = "";
    while(getline(infile, s))
    {
        m_root->addNode(s);
    }
    
    //O(N)
    
    //successfully loaded
    return true;
}




bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	
    //make word lowercase
    string lowercaseWord = "";
    for (int i = 0; i < word.size(); i++)
    {
        lowercaseWord += tolower(word[i]);
    }
    
    //number of suggestions
    int count = 0;
    
    //if word is in dictionary
    if (m_root->findNode(lowercaseWord))
    {
        return true;
    }
    else
    {
        //clear suggestions
        suggestions.clear();
        
        string alphabet = "abcdefghijklmnopqrstuvwxyz";
        
        for (int i = 0; i < lowercaseWord.size(); i++)
        {
            string str;
            
            //find a letter that makes it a word
            for (int j = 0; j < 26; j++)
            {
                str = "";
                
                //first char in word
                if (i == 0)
                {
                    str = alphabet[j] + lowercaseWord.substr(i+1);
                }
                //last char in word
                else if (i == lowercaseWord.size()-1)
                {
                    str = lowercaseWord.substr(0, i) + alphabet[j];
                }
                else
                {
                    str = lowercaseWord.substr(0, i) + alphabet[j] + lowercaseWord.substr(i+1);
                }
                
                //if new word is in dictionary
                if (m_root->findNode(str))
                {
                    //add to suggestions
                    suggestions.push_back(str);
                    count++;
                    
                    //if we have enough suggestions
                    if (count == max_suggestions)
                    {
                        break;
                    }
                }
            }
            
            //if apostrophe makes it a word
            str = "";
            
            //first char in word
            if (i == 0)
            {
                str = '\'' + lowercaseWord.substr(i+1);
            }
            //last char in word
            else if (i == lowercaseWord.size()-1)
            {
                str = lowercaseWord.substr(0, i) + '\'';
            }
            else
            {
                str = lowercaseWord.substr(0, i) + '\'' + lowercaseWord.substr(i+1);
            }
            
            //if new word is in dictionary
            if (m_root->findNode(str))
            {
                //add to suggestions
                suggestions.push_back(str);
                count++;
                
                //if we have enough suggestions
                if (count == max_suggestions)
                {
                    break;
                }
            }
        }
        return false;
    }
    
    //O(oldS + L^2 + maxSuggestions), oldS = suggestions.size(), L = length of word
}




void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	
    //start and end of word
    int startIndex;
    int endIndex;
    
    int i = 0;
    while (i < line.size())
    {
        //make word lowercase
        string lowercaseStr = "";
        
        //if char is a letter or apostrophe
        if (isalpha(line[i]) || line[i] == '\'')
        {
            lowercaseStr += tolower(line[i]);
            startIndex = i;
            endIndex = i;
            
            //go to next char
            i++;
            
            while (isalpha(line[i]) || line[i] == '\'')
            {
                //if char is a letter
                if (isalpha(line[i]))
                {
                    lowercaseStr += tolower(line[i]);
                }
                //if char is an apostrophe
                else
                {
                    lowercaseStr += line[i];
                }
                
                //increment endIndex until false
                endIndex++;
                i++;
            }
            
            //if word is not in dictionary
            if ( ! m_root->findNode(lowercaseStr))
            {
                SpellCheck::Position p;
                p.start = startIndex;
                p.end = endIndex;
                problems.push_back(p);
            }
            
            //continue from next char
            i++;
        }
        else
        {
            i++;
        }
    }
    
    //O(oldP + S + W*L), oldP = problems.size(), S = length of line,
    //W = number of words in line, L = max length of word
}
