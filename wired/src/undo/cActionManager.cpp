
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

void	cActionManager::AddEffectAction(PlugStartInfo* startInfo, PluginLoader* plugLoader, bool shouldAdd)
{
	cCreateEffectAction* action = new cCreateEffectAction(startInfo, plugLoader, shouldAdd);
	action->Do();
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
		mUndoList.push_front (action);
      	mUndoCount++;
    }
  	else
    	if (stack == REDO)
      	{
			mRedoList.push_front (action);
			mRedoCount++;
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
		AddAction(UNDO, action);
	//Dump();
}

bool	cActionManager::Redo()
{ 
	if (CanRedo())
	{
		cAction* action = mRedoList.front();
		//mRedoList.push_back(action);					// Why ?
		RemoveTopAction(REDO);
		action->Accept( cRedoActionVisitor::Global() );
		AddAction(UNDO, action);
		return true;
	}
	return false;
}

bool	cActionManager::Undo()
{ 
	if (CanUndo())
	{
		cAction* action = mUndoList.front();
		//mUndoList.push_back(action);					// Why ?
		RemoveTopAction(UNDO);
		action->Accept(cUndoActionVisitor::Global());
		AddAction(REDO, action);
		return true;
	}
	return false;
}

std::list<string>		cActionManager::getListActions(int *separatorIndex)
{
	std::list<string>				result;
	tActionList::const_iterator		iter;
	
	*separatorIndex = 0;
	for (iter = mUndoList.begin(); iter != mUndoList.end(); iter++, (*separatorIndex)++)
		result.push_back(UNDO_LABEL + (*iter)->getHistoryLabel());
	for (iter = mRedoList.begin(); iter != mRedoList.end(); iter++)
		result.push_back(REDO_LABEL + (*iter)->getHistoryLabel());
	return result;
}

void	cActionManager::DumptActionList(const tActionList& actionList, const std::string& listName)
{
	tActionList::const_iterator	iter;
	
	std::cout << "  Dumping tActionList " << listName << std::endl;
	for (iter = actionList.begin(); iter != actionList.end(); iter++)
		(*iter)->Dump();
	std::cout << "    size() : " << actionList.size() << std::endl;
	std::cout << "  End Dumping tActionList" << std::endl;
}

void	cActionManager::Dump()
{
	std::cout << "Dumping cActionManager : " << spSingleton << std::endl;
	DumptActionList(mUndoList, "mUndoList");
	std::cout << "  int mUndoCount : " << mUndoCount << std::endl;
	DumptActionList(mRedoList, "mRedoList");
	std::cout << "  int mRedoCount : " << mRedoCount << std::endl;
	std::cout << "End Dumping cActionManager" << std::endl;
}