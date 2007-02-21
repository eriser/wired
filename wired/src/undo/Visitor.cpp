// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991


#include "Visitor.h"
#include "cAction.h"

/********************   class cRedoActionVisitor   ********************/

cRedoActionVisitor* cRedoActionVisitor::spSingleton = 0;

cRedoActionVisitor& cRedoActionVisitor::Global()
{
	if (spSingleton == 0)
		spSingleton = new cRedoActionVisitor();
	return *spSingleton;
}

void cRedoActionVisitor::Visit (cAction& action)
{ 
	action.Redo(); 
}

cRedoActionVisitor			cRedoActionVisitor::operator=(const cRedoActionVisitor& right)
{
	if (this != &right)
	{
		spSingleton = right.spSingleton;
	}
	return *this;
}

/********************   class cUndoActionVisitor   ********************/

cUndoActionVisitor* cUndoActionVisitor::spSingleton = 0;

cUndoActionVisitor& cUndoActionVisitor::Global()
{
	if (spSingleton == 0)
		spSingleton = new cUndoActionVisitor();
	return *spSingleton;
}

void cUndoActionVisitor::Visit (cAction& action)
{ 
	action.Undo(); 
}

cUndoActionVisitor			cUndoActionVisitor::operator=(const cUndoActionVisitor& right)
{
	if (this != &right)
	{
		spSingleton = right.spSingleton;
	}
	return *this;
}
