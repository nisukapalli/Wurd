#include "StudentUndo.h"

Undo* createUndo()
{
	return new StudentUndo;
}




void StudentUndo::submit(const Action action, int row, int col, char ch) {
	
    //make new undo operation
    Operation lastOperation;
    lastOperation.m_action = action;
    lastOperation.m_text = ch;
    lastOperation.m_row = row;
    lastOperation.m_col = col;
    
    if (ch == '\t')
    {
        lastOperation.m_text = "    ";
    }
    
    //find consecutive delete operations
    if (action == Undo::Action::DELETE && ! m_operations.empty())
    {
        //find consecutive delete operations
        Operation op = m_operations.top();
        if (op.m_action == action && op.m_row == row && op.m_col == col)
        {
            //combine strings
            lastOperation.m_text = op.m_text + lastOperation.m_text;
            
            //remove last operation
            m_operations.pop();
        }
        
        //find consecutive backspace operations
        else if (op.m_action == action && op.m_row == row
            && op.m_col == col+1)
        {
         //combine strings
         lastOperation.m_text += op.m_text;
         
         //remove last operation
         m_operations.pop();
        }
    }
    
    //find consecutive insert operations
    else if (action == Undo::Action::INSERT && ! m_operations.empty())
    {
        //if previous operation is at previous position
        Operation op = m_operations.top();
        if (op.m_action == action && op.m_row == row
            && op.m_col == col-1)
        {
            //combine strings
            lastOperation.m_text = op.m_text + lastOperation.m_text;
            
            //remove last operation
            m_operations.pop();
        }
    }
    
    //push new operation onto stack
    m_operations.push(lastOperation);
    
    //O(1)
}




StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text) {
    
    if (m_operations.empty())
    {
        return Undo::Action::ERROR;
    }
    Operation lastOperation = m_operations.top();
    Action lastAction = lastOperation.m_action;
    
    //action to return
    Action returnAction;
    switch (lastAction)
    {
        case Undo::Action::INSERT:
        {
            //delete the inserted chars
            returnAction = Undo::Action::DELETE;
            row = lastOperation.m_row;
            col = lastOperation.m_col;
            count = (int) lastOperation.m_text.size();
            text = "";
            
            //remove from stack
            m_operations.pop();
            
            break;
        }
        case Undo::Action::DELETE:
        {
            //insert the deleted chars
            returnAction = Undo::Action::INSERT;
            row = lastOperation.m_row;
            col = lastOperation.m_col;
            count = 1;
            text = lastOperation.m_text;
            
            //remove from stack
            m_operations.pop();
            
            break;
        }
        case Undo::Action::SPLIT:
        {
            //join the split lines
            returnAction = Undo::Action::JOIN;
            row = lastOperation.m_row;
            col = lastOperation.m_col;
            count = 1;
            text = "";
            
            //remove from stack
            m_operations.pop();
            
            break;
        }
        case Undo::Action::JOIN:
        {
            //split the joined lines
            returnAction = Undo::Action::SPLIT;
            row = lastOperation.m_row;
            col = lastOperation.m_col;
            count = 1;
            text = "";
            
            //remove from stack
            m_operations.pop();
            
            break;
        }
        default:
        {
            returnAction = Undo::Action::ERROR;
            break;
        }
    }
    
    //O(1)
    
    return returnAction;
}




void StudentUndo::clear() {
	
    
    while ( ! m_operations.empty())
    {
       m_operations.pop();
    }
    
    //O(N)
}
