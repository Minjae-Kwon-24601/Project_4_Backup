#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>

class StudentSpellCheck : public SpellCheck {
public:
	StudentSpellCheck();
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
	struct Node
	{
		char value;
		std::vector<Node*> trie_DS;
	};

	Node dummy;
	void clearWords(Node* cur);
	bool checkWord(std::string to_check);
};

#endif  // STUDENTSPELLCHECK_H_
