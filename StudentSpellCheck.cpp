#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

StudentSpellCheck::StudentSpellCheck()
{

}

StudentSpellCheck::~StudentSpellCheck() 
{

}

bool StudentSpellCheck::load(std::string dictionaryFile) {
	ifstream infile(dictionaryFile);		// create an input file stream object
	if (!infile)
		return false;			// if the opening of the file fails return false
	else
	{
		string s;
		
		while (getline(infile, s))				// as long as there are still lines to process, store them in s
		{
			for (int i = 0; i < s.size(); i++)
			{
				s[i] = tolower(s[i]);
				if (!isalpha(s[i]) && s[i] != '\'')
					s.erase(i, 1);
			}

			Node* p = &dummy;
			for (int i = 0; i < s.size(); i++)
			{
				int j = 0;
				for (; j < p->trie_DS.size(); j++)
				{
					if (p->trie_DS[j]->value == s[i])
					{
						break;
					}
				}
				if (j == p->trie_DS.size())
				{
					p->trie_DS.push_back(new Node);
					p = p->trie_DS[j];
					p->value = s[i];
				}
				else
					p = p->trie_DS[j];
			}
			p->trie_DS.push_back(new Node);
			int last_pos = p->trie_DS.size() - 1;
			p = p->trie_DS[last_pos];
			p->value = '.';
		}
		return true;
	}
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	return true; // TODO
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	// TODO
}
