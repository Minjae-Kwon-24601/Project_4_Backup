#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo) 
{
	m_row = 0;
	m_col = 0;							// set the cursor position to the top left
	m_text.push_back(new string);		// make the first line to prevent dereferencing a pointer without an object
	it = m_text.begin();				// point the iterator at the start of the text file
}

StudentTextEditor::~StudentTextEditor()
{
	for (it = m_text.begin(); it != m_text.end(); )
	{
		delete* it;
		it = m_text.erase(it);			// deletes the dynamically allocated memory and the nodes in the list container
	}
}

bool StudentTextEditor::load(std::string file) 
{
	ifstream infile(file);		// create an input file stream object
	if (!infile)		   
		return false;			// if the opening of the file fails return false
	else
	{
		string s;
		reset();
		m_text.push_back(new string);
		it = m_text.begin();					// point the iterator at the start of the text file
		while (getline(infile, s))				// as long as there are still lines to process, store them in s
		{
			size_t end_pos = s.size() - 1;
			if (s[end_pos] == '\r')				
				s.erase(end_pos);				// if the line has a carriage return character erase it
			*(*it) = s;
			m_text.push_back(new string);		
			it++;								// make a new line and increment the iterator
		}
		delete* it;
		m_text.erase(it);						// delete the last line as it is not part of the loaded text file
		it = m_text.begin();					// point the iterator back at the first line of the text file
		return true;
	}
}

bool StudentTextEditor::save(std::string file) 
{
	ofstream outfile(file);		// create an output file stream object
	if (!outfile)				
		return false;			// if the creation of the file fails return false
	else
	{
		for (list<string*>::iterator it = m_text.begin(); it != m_text.end(); it++)
			outfile << *(*it) + '\n';					// copy the lines with a newline added to the end into a text file until there are no more
		return true;
	}
}

void StudentTextEditor::reset() 
{
	for (it = m_text.begin(); it != m_text.end(); )
	{
		delete* it;
		it = m_text.erase(it);			// deletes the dynamically allocated memory and the nodes in the list container
	}

	m_row = 0;
	m_col = 0;								// reinitializes the text file to start at the top left again	
	
	getUndo()->clear();						// clears the undo state
}

void StudentTextEditor::move(Dir dir) 
{
	list<string*>::const_iterator itr = it;
	switch (dir)
	{
	case UP:
		if (m_row != 0)							// if it is not the first row
		{
			m_row--;							// move up
			it--;
			if (m_col > (*it)->size())			// if the line above is shorter than the current line
				m_col = (*it)->size();			// point the cursor just past the end of the line above
		}
		break;
	case DOWN:
		itr++;
		if (itr != m_text.end())				// if it is not the last row
		{
			m_row++;							// move down
			it++;
			if (m_col > (*it)->size())			// if the line below is shorter than the current line
				m_col = (*it)->size();			// point the cursor just past the end of the line below
		}
		break;
	case LEFT:
		if (m_row == 0 && m_col == 0)			// if the cursor is at the top left do nothing
			break;
		else if (m_col == 0)					// otherwise if the cursor is at the beginning of a line
		{
			it--;
			m_col = (*it)->size();
			m_row--;							// move it up and point the cursor just past the end of the line
		}
		else
		{
			m_col--;							// otherwise just move left
		}
		break;
	case RIGHT:
		itr++;
		if (m_col == (*it)->size() && itr == m_text.end())		// if the cursor points just past the very end of the text do nothing
			break;
		else if (m_col == (*it)->size())		// otherwise if the cursor is pointing just past the end of a line
		{
			it++;
			m_col = 0;
			m_row++;							// move it down and point the cursor at the beginning of the line
		}
		else
		{
			m_col++;							// otherwise just move right
		}
		break;
	case HOME:
		m_col = 0;								// point the cursor at the beginning of the line
		break;
	case END:
		m_col = (*it)->size();					// point the cursor just past the end of the line
		break;
	}
}

void StudentTextEditor::del() 
{
	list<string*>::const_iterator itr = it;
	itr++;
	if (m_col == (*it)->size() && itr == m_text.end())		// if the cursor points just past the very end of the text do nothing
		return;
	else
	{
		if (m_col < (*it)->size())
		{
			(*it)->erase(m_col, 1);							// deletes the char that the cursor is pointing at
		}
		else if (m_col == (*it)->size())					// if the cursor points just past the end of the line
		{
			string temp = *(*it);							// store the current line in a string
			delete* it;
			it = m_text.erase(it);							// delete the line and move to the next line								
			(*it)->insert(0, temp);							// concatenate the stored string onto the beginning of the line
		}

		// MAKE SURE TO NOTIFY UNDO LATER
	}
}

void StudentTextEditor::backspace() 
{
	if (m_row == 0 && m_col == 0)						// if the cursor is at the top left do nothing
	{
		return;
	}
	else
	{
		if (m_col > 0)
		{
			(*it)->erase(m_col - 1, 1);					// deletes the char at the position before the cursor
			m_col--;									// and then decrements the position of the cursor
		}
		else if (m_row > 0 && m_col == 0)				// if it is not the first row and the cursor is at the beginning of the line
		{
			string temp = *(*it);						// store the line in a string
			delete* it;
			it = m_text.erase(it);
			it--;
			m_row--;									
			m_col = (*it)->size();						// delete the current line and point the cursor just past the end of the line above
			*(*it) += temp;								// concatenate the stored string onto the line above
		}

		// MAKE SURE TO NOTIFY UNDO LATER
	}
}

void StudentTextEditor::insert(char ch) 
{
	if (ch == 9)								// if the character is a tab key
	{
		(*it)->insert(m_col, 4, ch);			// insert 4 spaces at the position of the cursor
		m_col = m_col + 4;						// and move the cursor 4 spots to the right
	}
	else
	{
		(*it)->insert(m_col, 1, ch);			// otherwise insert a character at the position of the cursor
		m_col++;								// increments the position of the cursor
	}

	// MAKE SURE TO NOTIFY UNDO LATER
}

void StudentTextEditor::enter() 
{
	string substring = (*it)->substr(m_col);	// store the substring from the cursor to the end of the current line
	(*it)->erase(m_col);						// erase that substring from the current line so it can be added to the next line
	it++;
	m_text.insert(it, new string);				// insert a new line 
	it--;
	m_row++;
	if (substring != "")						// if the substring is not empty
	{
		*(*it) += substring;					// add it to the beginning of the new line
		m_col = 0;								// point the cursor at the beginning of the line
	}
	else
		m_col = 0;

	// MAKE SURE TO NOTIFY UNDO LATER
}

void StudentTextEditor::getPos(int& row, int& col) const 
{
	row = m_row;
	col = m_col;			// sets the parameters equal to the current m_row and m_col
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const 
{
	if (startRow < 0 || numRows < 0 || startRow > m_row)
		return -1;
	else if (startRow == (m_row + 1))			// if the startRow equals the number of lines return 0
		return 0;
	else
	{
		for (vector<string>::iterator itr_1 = lines.begin(); itr_1 != lines.end(); )
		{
			itr_1 = lines.erase(itr_1);			// erases any previous items in the vector
		}

		int i = 0;
		list<string*>::const_iterator itr_2 = m_text.begin();
		for (; i < startRow; i++)
			itr_2++;							// makes itr_2 point to the correct position

		i = 1;
		while (itr_2 != m_text.end() && i <= numRows)
		{
			lines.push_back(*(*itr_2));			// copies the desired lines from m_text into the "lines" vector
			itr_2++;
			i++;
		}
		
		return --i;								// returns the number of lines that were copied into the "lines" vector
	}
}

void StudentTextEditor::undo() {
	// TODO
}
