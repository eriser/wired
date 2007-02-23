// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991


#if !defined(VISITOR_H)
#define VISITOR_H


// ----------------------------------------------------------------------------
// Forward declaration
// ----------------------------------------------------------------------------

class cAction;
class cMultiplyAction;
class cAdditionAction;

// ----------------------------------------------------------------------------
// Definition classe cActionVisitor
// ----------------------------------------------------------------------------

class cActionVisitor
{
public:
	virtual void Visit (cAction& action) 
	{};
};


class cRedoActionVisitor : public cActionVisitor
{
private:
	// Pointer sur l'unique objet de cette classe
	static cRedoActionVisitor* spSingleton;

	// Constructeur, prive pour gerer une seule instance de cette classe 
	cRedoActionVisitor() {};
	cRedoActionVisitor(const cRedoActionVisitor& copy) {*this = copy;};

public:
	virtual ~cRedoActionVisitor() {if (spSingleton) delete spSingleton;};
	static cRedoActionVisitor& Global();
	virtual void Visit (cAction& action);

	// Operateur =
	cRedoActionVisitor		operator=(const cRedoActionVisitor& right);

};


class cUndoActionVisitor : public cActionVisitor
{
private:
	// Pointer sur l'unique objet de cette classe
	static cUndoActionVisitor* spSingleton;

	// Constructeur, prive pour gerer une seule instance de cette classe
	cUndoActionVisitor() {};
	cUndoActionVisitor(const cUndoActionVisitor& copy) {*this = copy;};

public:
	// Destructeur
	virtual ~cUndoActionVisitor() {if (spSingleton) delete spSingleton;};

    // Retourne l'unique instance de cette classe
	static cUndoActionVisitor& Global();

	// Fonction Visiteur
	virtual void Visit (cAction& action);
	
	// Operateur =
	cUndoActionVisitor		operator=(const cUndoActionVisitor& right);

};


#endif
