
#include "cActionManager.h"
#include "Visitor.h"
#include <iostream>



cActionManager* cActionManager::spSingleton = 0;

cActionManager::cActionManager() : mRedoCount (0), mUndoCount (0)
{
}

cActionManager::~cActionManager()
{
	//TODO : Supprimer les actions puis faire un clear() des listes
	//TODO : Voir comment gerer le cycle de vie des actions
}

cActionManager& cActionManager::Global()
{
	if (spSingleton == 0)
		spSingleton = new cActionManager();
	return *spSingleton;
}

void cActionManager::AddAction(tStackKind stack, cAction& action)
{
  if (stack == UNDO)
    {
      mUndoList.push_front (&action);
      mUndoCount++;
    }
  else
    if (stack == REDO)
      {
	mRedoList.push_front (&action);
	mRedoCount++;
      }
}

void cActionManager::RemoveTopAction(tStackKind stack)
{
  if ( stack == REDO )
    {
      mRedoList.pop_front();
      mRedoCount--;
    }
  else
    if ( stack == UNDO )
      {
	mUndoList.pop_front();
	mUndoCount--;
      }
  if ((mRedoCount < 1) && (!mRedoList.empty()))
    mRedoList.clear();
  if ((mUndoCount < 1) && (!mUndoList.empty()))
    mUndoList.clear();
}

void cActionManager::RegisterActionManager(cAction* action) 
{ 
	if (action->IsRegistered() == false)
	{
		AddAction( UNDO, *action );
	}
}

bool cActionManager::Redo()
{ 
	if (CanRedo())
	{
		cAction* action = mRedoList.front();
		mRedoList.push_back(action);
		RemoveTopAction(REDO);
		action->Accept( cRedoActionVisitor::Global() );
		AddAction( UNDO, *action );
		return true;
	}
	return false;
}

bool cActionManager::Undo()
{ 
	if (CanUndo())
	{
		cAction* action = mUndoList.front();
		mUndoList.push_back(action);
		RemoveTopAction(UNDO);
		action->Accept( cUndoActionVisitor::Global() );
		AddAction( REDO, *action );
		return true;
	}
	return false;
}
