
#include "Visitor.h"
#include "cAction.h"

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
