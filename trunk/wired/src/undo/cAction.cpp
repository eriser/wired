
#include "cAction.h"
#include "cActionManager.h"
#include "Visitor.h"

#include <iostream>

cAction::cAction() : mValid (false), mRegistered (false)
{
}


void cAction::NotifyActionManager()
{ 
	cActionManager::Global().RegisterActionManager (this);
	mRegistered = true;
}


void cAction::Accept (cActionVisitor& visitor)
{ 
	visitor.Visit (*this);
}
