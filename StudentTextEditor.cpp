#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}




StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo) {
     
     m_text.push_back("");
     m_currentRow = m_text.begin();
     m_numlines = 0;
     m_row = 0;
     m_col = 0;
     
     //O(1)
}




StudentTextEditor::~StudentTextEditor()
{
    //reset contents
    reset();
    
    //O(N+U), N = number of rows in editor , U = number of items in undo stack
}




bool StudentTextEditor::load(std::string file) {
    
    ifstream infile(file);
    if ( ! infile )
    {
        //failed to load
        return false;
    }
    
    //reset contents
    reset();
    
    string s;
    while(getline(infile, s))
    {
        //remove carriage return
        if (s.size() > 0 && s[s.size()-1] == '\r')
        {
            s.pop_back();
        }
        m_text.push_back(s);
        m_numlines++;
    }
    
    //reset editing position to first row and col
    m_currentRow = m_text.begin();
    m_row = 0;
    m_col = 0;
    
    //O(M+N+U), M = number of chars in editor,
    //N = number of chars in new file, U = number of items in undo stack
    
    //successfully loaded
	return true;
}




bool StudentTextEditor::save(std::string file) {
    
    ofstream outfile(file);
    if ( ! outfile)
    {
        //failed to open file
        return false;
    }
    
    string tempStr = "";
    m_currentRow = m_text.begin();
    while (m_currentRow != m_text.end())
    {
        //write current line to file
        tempStr = *m_currentRow;
        tempStr += '\n';
        outfile << tempStr;
    }
    
    //O(M), M = number of chars in editor
    
    //successfully saved lines
    return true;
}


void StudentTextEditor::reset() {
    
    //clear text editor
    m_text.clear();
    
    //reset editing position
    m_currentRow = m_text.begin();
    m_numlines = 0;
    m_row = 0;
    m_col = 0;
    getUndo()->clear();
    
    //O(N+U), N = number of rows in editor, U = number of items in undo stack
}




void StudentTextEditor::insert(char ch) {
    
    if (ch == '\n')
    {
        enter();
    }
    else if (ch == '\b')
    {
        backspace();
    }
    else if (ch == '\t')
    {
        m_currentRow->insert(m_col, "    ");
        
        //move editing position
        for (int i = 0; i < 4; i++)
        {
            m_col++;
        }
        
        getUndo()->submit(Undo::Action::INSERT, m_row, m_col, '\t');
    }
    else
    {
        string temp = "";
        temp += ch;
        m_currentRow->insert(m_col, temp);
        
        //move editing position
        m_col++;
        
        getUndo()->submit(Undo::Action::INSERT, m_row, m_col, ch);
    }
    
    //O(L), L = length of line
}




void StudentTextEditor::enter() {

    //divide into 2 lines
    string newLine = m_currentRow->substr(m_col);
    string splitLine = m_currentRow->substr(0, m_col);
    
    getUndo()->submit(Undo::Action::SPLIT, m_row, m_col, '\n');
    
    //erase old row
    m_currentRow = m_text.erase(m_currentRow);
    //add split rows
    m_currentRow = m_text.insert(m_currentRow, newLine);
    m_currentRow = m_text.insert(m_currentRow, splitLine);
    
    //move editing position to beginning of new line
    incRow();
    m_col = 0;
    m_numlines++;
    
    //O(L), L = length of line
}




void StudentTextEditor::del() {
    
    string str;
    
    //if not at end of line
    if (m_col < m_currentRow->size())
    {
        char c = (*m_currentRow)[m_col];
        
        //copy the rest of row
        str = m_currentRow->substr(m_col+1);
        *m_currentRow = m_currentRow->substr(0, m_col) + str;
        
        getUndo()->submit(Undo::Action::DELETE, m_row, m_col, c);
    }
    
    //if at end of line
    else if (m_col == m_currentRow->size() && m_row < m_text.size()-1)
    {
        //append next row to current row
        int currentSize = (int) m_currentRow->size();
        str = *(next(m_currentRow));
        *m_currentRow += str;
        
        //delete next row
        m_text.erase(next(m_currentRow));
        m_col = currentSize;
        m_numlines--;
        
        getUndo()->submit(Undo::Action::JOIN, m_row, m_col, '\n');
    }
    
    //O(L) for middle of line, L = length of line
    //O(L1+L2) for end of line, L1 = length of current line, L2 = length of next line
}




void StudentTextEditor::backspace() {
    
    string str;
    
    //if at beginning of first line
    if (m_row == 0 && m_currentRow->size() == 0)
    {
        return;
    }
    
    //if not at end of line
    else if (m_col > 0)
    {
        char c = (*m_currentRow)[m_col-1];
        
        str = m_currentRow->substr(m_col);
        *m_currentRow = m_currentRow->substr(0, m_col-1) + str;
        m_col--;
        
        getUndo()->submit(Undo::Action::DELETE, m_row, m_col, c);
    }
    
    //if at beginning of line or line is empty
    else if (m_row > 0 || m_currentRow->size() == 0)
    {
        //append previous row
        list<string>::iterator prevRow = prev(m_currentRow);
        int prevSize = (int) prevRow->size();
        str = *prevRow + *m_currentRow;
        *prevRow = str;
        m_currentRow = m_text.erase(m_currentRow);
        
        //go to end of previous row
        decRow();
        m_col = prevSize;
        m_numlines--;
        
        getUndo()->submit(Undo::Action::JOIN, m_row, m_col, '\n');
    }
    
    //O(L) for middle of line, L = length of line
    //O(L1+L2) for end of line, L1 = length of current line, L2 = length of next line
}




void StudentTextEditor::move(Dir dir) {
	
    switch (dir)
    {
        case UP:
            if (m_row > 0)
            {
                decRow();
                
                //if previous row is a blank line
                if ((*m_currentRow)[0] == '\n')
                {
                    m_col = 0;
                }
                //go to previous row's last char
                else if (m_col > m_currentRow->size())
                {
                    moveToLastCol();
                }
            }
            break;
        case DOWN:
            if (m_row < m_numlines)
            {
                incRow();
                
                //if next row is a blank line
                if ((*m_currentRow)[0] == '\n')
                {
                    m_col = 0;
                }
                //go to next row's last char
                else if (m_col > m_currentRow->size())
                {
                    moveToLastCol();
                }
            }
            break;
        case LEFT:
            if (m_col > 0)
            {
                m_col--;
            }
            else if (m_row > 0)
            {
                //go to previous row's last char
                decRow();
                moveToLastCol();
            }
            break;
        case RIGHT:
            if (m_col < m_currentRow->size())
            {
                m_col++;
            }
            else
            {
                if (m_row < m_numlines)
                {
                    //go to next row's first char
                    incRow();
                    m_col = 0;
                }
            }
            break;
        case HOME:
            //go to first char
            m_col = 0;;
            break;
        case END:
            //go to position after last char
            moveToLastCol();
            break;
    }
    
    //O(1)
}




void StudentTextEditor::getPos(int& row, int& col) const {
	
    row = m_row;
    col = m_col;
    
    //O(1)
}




int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
    
    if (startRow < 0 || numRows < 0 || startRow > m_text.size())
    {
        return -1;
    }
    
    //move iterator to startRow
    list<string>::const_iterator it = m_text.begin();
    for (int i = 0; i < startRow; i++)
    {
        it++;
    }
    
    //clear previous data
    lines.clear();
    
    //count number of lines
    int count = 0;
    for (int i = 0; i < numRows && it != m_text.end(); i++)
    {
        lines.push_back(*it);
        count++;
        it++;
    }
    
    //O(oldR + abs(current row number - startRow) + numRows * L)
    //oldR = lines.size(), L = average line length
    
	return count;
}




void StudentTextEditor::undo() {
	
    //get last action
    int row;
    int col;
    int count;
    string text;
    Undo::Action lastAction = getUndo()->get(row, col, count, text);
    
    //if undo stack is empty
    if (lastAction == Undo::ERROR)
    {
        return;
    }
    
    //move iterator to current row
    m_currentRow = m_text.begin();
    for (int i = 0; i < row; i++)
    {
        m_currentRow++;
    }
    m_row = row;
    m_col = col;
    
    switch (lastAction)
    {
        case Undo::INSERT:
        {
            
            //insert text at position
            m_currentRow->insert(m_col, text);
            
            break;
        }
        case Undo::DELETE:
        {
            //delete text at position
            string str = m_currentRow->substr(m_col);
            *m_currentRow = m_currentRow->substr(0, m_col-count) + str;
            m_col -= count;
            
            break;
        }
        case Undo::SPLIT:
        {
            //split line
            string splitLine = m_currentRow->substr(0, m_col);
            string newLine = m_currentRow->substr(m_col);
            
            //erase old row
            m_currentRow = m_text.erase(m_currentRow);
            
            //add split rows
            m_currentRow = m_text.insert(m_currentRow, newLine);
            m_currentRow = m_text.insert(m_currentRow, splitLine);
            m_numlines++;
            
            break;
        }
        case Undo::JOIN:
        {
            //append next row to current row
            int currentSize = (int) m_currentRow->size();
            string str = *(next(m_currentRow));
            *m_currentRow += str;
            
            //delete next row
            m_text.erase(next(m_currentRow));
            m_col = currentSize;
            m_numlines--;
            
            break;
        }
        default:
        {
            break;
        }
    }
    
    //O(undo operation)
}




//=====================================================================




void StudentTextEditor::incRow()
{
    m_currentRow++;
    m_row++;
    
    //O(1)
}




void StudentTextEditor::decRow()
{
    m_currentRow--;
    m_row--;
    
    //O(1)
}




void StudentTextEditor::moveToLastCol()
{
    m_col = (int) m_currentRow->size();
    
    //O(1)
}
