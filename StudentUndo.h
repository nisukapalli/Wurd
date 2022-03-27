#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <stack>
using namespace std;

class StudentUndo : public Undo {
public:

	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:
    struct Operation
    {
        Operation()
        {
            m_text = "";
            m_row = 0;
            m_col = 0;
        }
        Action m_action;
        string m_text;
        int m_row;
        int m_col;
    };
    stack<Operation> m_operations;
};

#endif // STUDENTUNDO_H_
