
#if !defined(VISITOR_H)
#define VISITOR_H


// ----------------------------------------------------------------------------
// Forward declaration
// ----------------------------------------------------------------------------

class cAction;
class cMultiplyAction;
class cAdditionAction;

// ----------------------------------------------------------------------------
// Defintion classe cActionVisitor
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

public:
	virtual ~cRedoActionVisitor() {};
	static cRedoActionVisitor& Global();
	virtual void Visit (cAction& action);

};


class cUndoActionVisitor : public cActionVisitor
{
private:
	// Pointer sur l'unique objet de cette classe
	static cUndoActionVisitor* spSingleton;

	// Constructeur, prive pour gerer une seule instance de cette classe
	cUndoActionVisitor() 
	{};

public:
	// Destructeur
	virtual ~cUndoActionVisitor() 
	{};

    // Retourne l'unique instance de cette classe
	static cUndoActionVisitor& Global();

	// Fonction Visiteur
	virtual void Visit (cAction& action);
};


#endif
