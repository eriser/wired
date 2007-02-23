// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991


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

cAction			cAction::operator=(const cAction& right)
{
	if (this != &right)
	{
		m_Id = right.m_Id;
		mRegistered = right.mRegistered;
		mValid = right.mValid;
	}
	return *this;
}
