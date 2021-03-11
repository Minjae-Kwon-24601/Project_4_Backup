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
		char ch_check = stack_of_undos.top()->m_ch.top();
		Action action_check = stack_of_undos.top()->m_action.top();				
		if (action == Action::INSERT)																// check if the last action was an insertion
		{
			if (col_check == (col - 1) && action_check == Action::INSERT)							// if it was a insertion into the position after 
			{																						// the last insertion batch it together
				stack_of_undos.top()->m_action.push(action);
				stack_of_undos.top()->m_col.push(col);
				stack_of_undos.top()->m_row.push(row);
				stack_of_undos.top()->m_ch.push(ch);
				stack_of_undos.top()->m_count++;
			}
			else if (ch == '\t' && col_check == (col - 4) && action_check == Action::INSERT)		// otherwise if it was an tab insertion into the 
			{																						// position after the last insertion also batch it
				stack_of_undos.top()->m_action.push(action);
				stack_of_undos.top()->m_col.push(col);
				stack_of_undos.top()->m_row.push(row);
				stack_of_undos.top()->m_ch.push(ch);
				stack_of_undos.top()->m_count++;
			}
			else
				createNewUndo(stack_of_undos, action, row, col, ch);								// otherwise make a new undo submission
		}
		else if (action == Action::DELETE)															// check if the last action was an deletion
		{
			if ((col_check == col || col_check == (col + 1)) && action_check == Action::DELETE)		// if the last action was a backspace or a delete 
			{																						// in the correction position batch it together

				stack_of_undos.top()->m_action.push(action);
				stack_of_undos.top()->m_col.push(col);
				stack_of_undos.top()->m_row.push(row);
				stack_of_undos.top()->m_ch.push(ch);
				stack_of_undos.top()->m_count++;
			}
			else
				createNewUndo(stack_of_undos, action, row, col, ch);								// otherwise make a new undo submission
		}
		else
			createNewUndo(stack_of_undos, action, row, col, ch);									// otherwise make a new undo submission
	}
	else
		createNewUndo(stack_of_undos, action, row, col, ch);	// otherwise make a new undo submission		
}

// retrieves the lastest operation performed that can be undone
StudentUndo::Action StudentUndo::get(int &row, int &col, int& count, std::string& text) 
{
	if (stack_of_undos.empty())												// if there are no more undo submissions return with error
		return Undo::Action::ERROR;

	count = 1;																// make the DEFAULT count equal to 1

	if (stack_of_undos.top()->m_action.top() == Action::INSERT)				// if the last undo submission was an insertion
	{
		row = getRow(stack_of_undos);
		if (stack_of_undos.top()->m_ch.top() == '\t')						// if it was an tab, correct the position of the cursor and count
		{
			count += 3;
			col = getCol(stack_of_undos) - 4;
		}
		else
			col = getCol(stack_of_undos) - 1;								// otherwise move the cursor back by one

		if (stack_of_undos.top()->m_count != 1)
		{
			int temp_count = stack_of_undos.top()->m_count;
			count = stack_of_undos.top()->m_count;
			while (temp_count != 0)											// until the stack is empty
			{
				if (getCh(stack_of_undos) == '\t')							// check if the character is a tab to give the correct count and col position
				{
					count += 3;
					col = getCol(stack_of_undos) - 4;
				}
				else
					col = getCol(stack_of_undos) - 1;						// otherwise move the col position back one

				stack_of_undos.top()->m_ch.pop();
				stack_of_undos.top()->m_col.pop();
				temp_count--;
			}
		}

		delete stack_of_undos.top();
		stack_of_undos.pop();
		return Action::DELETE;												// and return the action to delete
	}

	if (stack_of_undos.top()->m_action.top() == Action::DELETE)				// if the last undo submission was a deletion
	{
		row = getRow(stack_of_undos);
		col = getCol(stack_of_undos);
		if (stack_of_undos.top()->m_count == 1)
			text += getCh(stack_of_undos);
		else
		{
			count = stack_of_undos.top()->m_count;
			int temp_count = stack_of_undos.top()->m_count;
			size_t pos = 0;
			text += getCh(stack_of_undos);
			stack_of_undos.top()->m_ch.pop();
			while (temp_count != 1)
			{
				int temp_col = getCol(stack_of_undos);
				stack_of_undos.top()->m_col.pop();

				if ((temp_col + 1) == getCol(stack_of_undos))
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

		delete stack_of_undos.top();
		stack_of_undos.pop();
		return Action::INSERT;											// and return the action to insert
	}

	if (stack_of_undos.top()->m_action.top() == Action::SPLIT)			// if the last undo submission was a split
	{
		row = getRow(stack_of_undos);
		col = getCol(stack_of_undos);									// get the correct row and col position

		delete stack_of_undos.top();
		stack_of_undos.pop();
		return Action::JOIN;											// and return the action to join
	}

	if (stack_of_undos.top()->m_action.top() == Action::JOIN)			// if the last undo submission was a split
	{
		row = getRow(stack_of_undos);
		col = getCol(stack_of_undos);									// get the correct row and col position

		delete stack_of_undos.top();
		stack_of_undos.pop();
		return Action::SPLIT;											// and return the action to split
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

// create a new UndoInfo struct that holds the relevant information
void StudentUndo::createNewUndo(stack<UndoInfo*>& undos, const Action action, int row, int col, char ch)
{
	undos.push(new UndoInfo);						
	undos.top()->m_action.push(action);
	undos.top()->m_col.push(col);
	undos.top()->m_row.push(row);
	undos.top()->m_ch.push(ch);
	undos.top()->m_count++;
}

// returns the row of the last submitted undo
int StudentUndo::getRow(stack<UndoInfo*>& undos)
{
	return undos.top()->m_row.top();
}

// returns the col of the last submitted undo
int StudentUndo::getCol(stack<UndoInfo*>& undos)
{
	return undos.top()->m_col.top();
}

// returns the char of the last submitted undo
char StudentUndo::getCh(stack<UndoInfo*>& undos)
{
	return undos.top()->m_ch.top();
}
