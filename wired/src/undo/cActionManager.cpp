// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "cActionManager.h"
#include "Visitor.h"
#include <iostream>
#include "Settings.h"


cActionManager* cActionManager::spSingleton = 0;

cActionManager::cActionManager() : mRedoCount (0), mUndoCount (0)
{
	nextID = BEGIN_HISTORY_ID;
}

cActionManager::~cActionManager()
{
	//TODO : Supprimer les actions puis faire un clear() des listes
	//TODO : Voir comment gerer le cycle de vie des actions
	mUndoList.clear();
	mRedoList.clear();
	if (spSingleton) delete spSingleton;
}

cActionManager& cActionManager::Global()
{
	if (spSingleton == 0)
		spSingleton = new cActionManager();
	return *spSingleton;
}

void	cActionManager::AddImportWaveAction(const wxString& path, trackType kind, bool selectFromIndex)
{
	cImportWaveAction* action = new cImportWaveAction(path, kind, selectFromIndex);
	action->Do();
}

void	cActionManager::AddEditWaveAction(const wxString& path, trackType kind, bool selectFromIndex)
{
	cImportWaveAction* action = new cImportWaveAction(path, kind, selectFromIndex);
	action->AddWaveToEditor();
}

void	cActionManager::AddChangeParamsEffectAction(Plugin* plugin, bool shouldSave)
{
	cChangeParamsEffectAction* action = new cChangeParamsEffectAction(plugin, shouldSave);
	action->Do();
}

void	cActionManager::AddAction(tStackKind stack, cAction *action)
{
	if (stack == UNDO)
	{
		if (mUndoCount == WiredSettings->maxUndoRedoDepth)
		{
			mUndoList.push_front (action);
    	  	mUndoList.pop_back();
		}
		else
		{
			mUndoList.push_front (action);
    	  	mUndoCount++;
		}
    }
  	else
    	if (stack == REDO)
      	{
      		if (mUndoCount == WiredSettings->maxUndoRedoDepth)
			{
				mRedoList.push_front (action);
    		  	mRedoList.pop_back();
			}
			else
			{
				mRedoList.push_front (action);
				mRedoCount++;
			}
      	}
}

void	cActionManager::RemoveTopAction(tStackKind stack)
{
  	if (stack == REDO)
    {
      	mRedoList.pop_front();
      	mRedoCount--;
    }
  	else
    	if (stack == UNDO)
      	{
			mUndoList.pop_front();
			mUndoCount--;
      	}
  	if ((mRedoCount < 1) && (!mRedoList.empty()))
    	mRedoList.clear();
  	if ((mUndoCount < 1) && (!mUndoList.empty()))
    	mUndoList.clear();
}

void	cActionManager::RegisterActionManager(cAction* action) 
{ 
	if (action->IsRegistered() == false)
	{
		action->m_Id = nextID++;
		AddAction(UNDO, action);
	}
	//Dump();
}

bool	cActionManager::Redo()
{ 
	if (CanRedo())
	{
		cAction* action = mRedoList.front();
		RemoveTopAction(REDO);
		action->Accept( cRedoActionVisitor::Global() );
		AddAction(UNDO, action);
		return true;
	}
	return false;
}

bool	cActionManager::Redo(int id)
{ 
	while (42)
	{
		if (CanRedo())
		{
			cAction* action = mRedoList.front();
			RemoveTopAction(REDO);
			action->Accept( cRedoActionVisitor::Global() );
			AddAction(UNDO, action);
			if (action->m_Id == id)
				return true;
		}
		else
			return false;
	}
	return false;
}

bool	cActionManager::Undo()
{ 
	if (CanUndo())
	{
		cAction* action = mUndoList.front();
		RemoveTopAction(UNDO);
		action->Accept(cUndoActionVisitor::Global());
		AddAction(REDO, action);
		return true;
	}
	return false;
}

bool	cActionManager::Undo(int id)
{
	while (42)
	{
		if (CanUndo())
		{
			cAction* action = mUndoList.front();
			RemoveTopAction(UNDO);
			action->Accept(cUndoActionVisitor::Global());
			AddAction(REDO, action);
			if (action->m_Id == id)
				return true;
		}
		else
			return false;
	}
	return false;
}

std::list<t_menuInfo*>		cActionManager::getListActions(int *separatorIndex)
{
	std::list<t_menuInfo*>				result;
	tActionList::const_iterator			iter;
	
	*separatorIndex = 0;
	for (iter = mUndoList.begin(); iter != mUndoList.end(); iter++, (*separatorIndex)++)
	{
		t_menuInfo *menuInfo = new t_menuInfo;
		menuInfo->label = UNDO_LABEL + (*iter)->getHistoryLabel();
		menuInfo->id = (*iter)->m_Id;
		result.push_back(menuInfo);
	}
	for (iter = mRedoList.begin(); iter != mRedoList.end(); iter++)
	{
		t_menuInfo *menuInfo = new t_menuInfo;
		menuInfo->label = REDO_LABEL + (*iter)->getHistoryLabel();
		menuInfo->id = (*iter)->m_Id;
		result.push_back(menuInfo);
	}
	return result;
}

cActionManager			cActionManager::operator=(const cActionManager& right)
{
	if (this != &right)
	{
		spSingleton = right.spSingleton;
		nextID = right.nextID;
		mUndoList = right.mUndoList;
		mUndoCount = right.mUndoCount;
		mRedoList = right.mRedoList;
		mRedoCount = right.mRedoCount;
	}
	return *this;
}

void	cActionManager::DumptActionList(const tActionList& actionList, const wxString& listName)
{
	tActionList::const_iterator	iter;
	
	std::cout << "  Dumping tActionList " << listName.mb_str() << std::endl;
	for (iter = actionList.begin(); iter != actionList.end(); iter++)
		(*iter)->Dump();
	std::cout << "    size() : " << actionList.size() << std::endl;
	std::cout << "  End Dumping tActionList" << std::endl;
}

void	cActionManager::Dump()
{
	std::cout << "Dumping cActionManager : " << spSingleton << std::endl;
	DumptActionList(mUndoList, wxT("mUndoList"));
	std::cout << "  int mUndoCount : " << mUndoCount << std::endl;
	DumptActionList(mRedoList, wxT("mRedoList"));
	std::cout << "  int mRedoCount : " << mRedoCount << std::endl;
	std::cout << "End Dumping cActionManager" << std::endl;
}
