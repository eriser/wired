
#include "cAction.h"
#include "cActionManager.h"
#include "Visitor.h"

#include <iostream>

cAction::cAction() : mValid (false), mRegistered (false)
{
}

void	cAction::NotifyActionManager()
{ 
	cActionManager::Global().RegisterActionManager (this);
	mRegistered = true;
}

void	cAction::Accept (cActionVisitor& visitor)
{ 
	visitor.Visit (*this);
}

void	cAction::Dump(bool alone)
{
	if (alone == true)
		std::cout << "    Dumping cAction : " << this << std::endl;
	std::cout << "      bool mValid : " << mValid << std::endl;
	std::cout << "      bool mRegistered : " << mRegistered << std::endl;
	if (alone == true)
		std::cout << "    End Dumping cAction" << std::endl;
}
