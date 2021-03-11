#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <stack>
#include <vector>

class StudentUndo : public Undo {
public:
	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:
	struct UndoInfo
	{
		std::stack<Action> m_action;
		std::stack<int> m_row;
		std::stack<int> m_col;
		std::stack<char> m_ch;
		int m_count = 0;
	};

	std::stack<UndoInfo*> stack_of_undos;
	void createNewUndo(std::stack<UndoInfo*>& undos, const Action action, int row, int col, char ch);
	int getRow(std::stack<UndoInfo*>& undos);
	int getCol(std::stack<UndoInfo*>& undos);
	char getCh(std::stack<UndoInfo*>& undos);
};

#endif // STUDENTUNDO_H_
