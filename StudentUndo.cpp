#include "StudentUndo.h"
#include <stack>
#include <vector>
using namespace std;

Undo* createUndo()
{
	return new StudentUndo;
}

// stores operations that can be undone in a stack
void StudentUndo::submit(const Action action, int row, int col, char ch) 
{
	if (!stack_of_undos.empty())								// if the stack is not empty
	{
		int col_check = stack_of_undos.top()->m_col.top();
		Action action_check = stack_of_undos.top()->m_action.top();
		if (action == Action::INSERT)
		{
			if (col_check == (col - 1) && action_check == Action::INSERT)
			{
				stack_of_undos.top()->m_action.push(action);
				stack_of_undos.top()->m_col.push(col);
				stack_of_undos.top()->m_row.push(row);
				stack_of_undos.top()->m_ch.push(ch);
				stack_of_undos.top()->m_count++;
			}
			else
			{
				stack_of_undos.push(new UndoInfo);						// otherwise create a new UndoInfo struct that holds the relevant information
				stack_of_undos.top()->m_action.push(action);
				stack_of_undos.top()->m_col.push(col);
				stack_of_undos.top()->m_row.push(row);
				stack_of_undos.top()->m_ch.push(ch);
				stack_of_undos.top()->m_count++;
			}
		}
		if (action == Action::DELETE)
		{
			if (col_check == col || col_check == (col - 1))
			{
				if (action_check == Action::DELETE)
				{
					stack_of_undos.top()->m_action.push(action);
					stack_of_undos.top()->m_col.push(col);
					stack_of_undos.top()->m_row.push(row);
					stack_of_undos.top()->m_ch.push(ch);
					stack_of_undos.top()->m_count++;
				}
				else
				{
					stack_of_undos.push(new UndoInfo);						// otherwise create a new UndoInfo struct that holds the relevant information
					stack_of_undos.top()->m_action.push(action);
					stack_of_undos.top()->m_col.push(col);
					stack_of_undos.top()->m_row.push(row);
					stack_of_undos.top()->m_ch.push(ch);
					stack_of_undos.top()->m_count++;
				}
			}
		}
	}
	else
	{
		stack_of_undos.push(new UndoInfo);						// otherwise create a new UndoInfo struct that holds the relevant information
		stack_of_undos.top()->m_action.push(action);
		stack_of_undos.top()->m_col.push(col);
		stack_of_undos.top()->m_row.push(row);
		stack_of_undos.top()->m_ch.push(ch);
		stack_of_undos.top()->m_count++;
	}
}

// retrieves the lastest operation performed that can be undone
StudentUndo::Action StudentUndo::get(int &row, int &col, int& count, std::string& text) 
{
	if (stack_of_undos.empty())
		return Undo::Action::ERROR;

	count = 1;

	if (stack_of_undos.top()->m_action.top() == Action::INSERT)
	{
		row = getRow(stack_of_undos);
		col = getCol(stack_of_undos);
		if (stack_of_undos.top()->m_count != 1)
		{
			count = stack_of_undos.top()->m_count;
			int temp_count = stack_of_undos.top()->m_count;
			while (temp_count != 1)
			{
				stack_of_undos.top()->m_col.pop();
				temp_count--;
			}
			col = getCol(stack_of_undos);
		}

		stack_of_undos.pop();
		return Action::DELETE;
	}

	if (stack_of_undos.top()->m_action.top() == Action::DELETE)
	{
		row = getRow(stack_of_undos);
		col = getCol(stack_of_undos);
		if (stack_of_undos.top()->m_count != 1)
		{
			count = stack_of_undos.top()->m_count;
			int temp_count = stack_of_undos.top()->m_count;
			while (temp_count != 0)
			{
				col = getCol(stack_of_undos);
				stack_of_undos.top()->m_col.pop();

				size_t pos = 0;
				if (stack_of_undos.top()->m_col.empty() || (col - 1) == getCol(stack_of_undos))
				{
					text += getCh(stack_of_undos);
					stack_of_undos.top()->m_ch.pop();
					pos++;
				}
				else
				{
					text.insert(pos, 1, getCh(stack_of_undos));
					stack_of_undos.top()->m_ch.pop();
				}

				temp_count--;
			}
		}

		stack_of_undos.pop();
		return Action::INSERT;
	}

	if (stack_of_undos.top()->m_action.top() == Action::SPLIT)
	{
		row = getRow(stack_of_undos);
		col = getCol(stack_of_undos);

		stack_of_undos.pop();
		return Action::JOIN;
	}

	if (stack_of_undos.top()->m_action.top() == Action::JOIN)
	{
		row = getRow(stack_of_undos);
		col = getCol(stack_of_undos);

		stack_of_undos.pop();
		return Action::SPLIT;
	}
}

// removes all the undo operations that were stored
void StudentUndo::clear() 
{
	while (!stack_of_undos.empty())
	{
		delete stack_of_undos.top();
		stack_of_undos.pop();				
	}
}

int StudentUndo::getRow(stack<UndoInfo*>& undos)
{
	return undos.top()->m_row.top();
}

int StudentUndo::getCol(stack<UndoInfo*>& undos)
{
	return undos.top()->m_col.top();
}

char StudentUndo::getCh(stack<UndoInfo*>& undos)
{
	return undos.top()->m_ch.top();
}