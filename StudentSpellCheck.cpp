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
	Node* ptr;
	for (int i = 0; i < dummy.trie_DS.size(); i++)
	{
		ptr = dummy.trie_DS[i];
		clearWords(ptr);
	}
}

bool StudentSpellCheck::load(std::string dictionaryFile) 
{
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

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) 
{
	suggestions.erase(suggestions.begin(), suggestions.end());

	string s = word;
	if (checkWord(s))
		return true;
	else
		return false;
	/*int count = 0;
	for (int i = 0; i < s.size(); i++)
	{
		s[i] = tolower(s[i]);
		
	}


	return true; */
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) 
{
	problems.erase(problems.begin(), problems.end());

	int start;
	int end;
	int char_count = 0;
	int iterator;
	string check;
	for (int i = 0; i < line.size(); i++)
	{
		if (isalpha(line[i]) || line[i] == '\'')
		{
			if (char_count == 0)
			{
				start = i;
				iterator = i;
				check += line[i];
				char_count++;
			}
			else if (i - 1 == iterator)
			{
				check += line[i];
				char_count++;
				iterator++;
			}
			if (i + 1 == line.size())
			{
				end = start + char_count - 1;
				if (!checkWord(check))	
				{
					Position pos;
					pos.start = start;
					pos.end = end;
					problems.push_back(pos);
				}
			}
		}
		else if (char_count != 0)
		{
			end = start + char_count - 1;
			if (!checkWord(check))
			{
				Position pos;
				pos.start = start;
				pos.end = end;
				problems.push_back(pos);	
			}
			char_count = 0;
			check = "";
		}
	}
}

void StudentSpellCheck::clearWords(Node* cur)
{
	if (cur->trie_DS.size() == 0)
		return;

	for (int i = 0; i < cur->trie_DS.size(); i++)
		clearWords(cur->trie_DS[i]);

	delete cur;
}

bool StudentSpellCheck::checkWord(std::string to_check)
{
	Node* ptr = &dummy;
	bool flag = false;
	int count = 0;
	for (int i = 0; i < to_check.size(); i++)
	{
		char cur_letter = tolower(to_check[i]);
		for (int j = 0; j < ptr->trie_DS.size(); j++)
		{
			if (cur_letter == ptr->trie_DS[j]->value)
			{
				ptr = ptr->trie_DS[j];
				flag = true;
				count++;
				break;
			}
		}
		if (!flag)
			break;
		else
			flag = false;
	}

	if (count == to_check.size())
	{
		for (int j = 0; j < ptr->trie_DS.size(); j++)
		{
			if ('.' == ptr->trie_DS[j]->value)
				return true;
		}
		return false;
	}
	else
		return false;
}
