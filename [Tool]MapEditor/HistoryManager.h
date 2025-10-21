#pragma once
#include <stack>

class CHistory;

class CHistoryManager
{
public:
	CHistoryManager(void);
	virtual ~CHistoryManager(void);
	void Undo();
	void Redo();
	void ClearUndo();
	void ClearRedo();
	bool IsUndoEmpty() const;
	bool IsRedoEmpty() const;
	template< class Unit >
	inline Unit& Allocate()
	{
		Unit* const unit = new Unit;
		mUndoStack.push(unit);

		return *unit;
	}

protected:
	// �۾��� ������ ����ȴ�
	std::stack< CHistory* > mUndoStack;
	// �۾� ����� ������ ����ȴ�
	std::stack< CHistory* > mRedoStack;
};
