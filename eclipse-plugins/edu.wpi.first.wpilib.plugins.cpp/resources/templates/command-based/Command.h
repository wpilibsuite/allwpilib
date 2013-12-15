#ifndef $classname_H
#define $classname_H

#include "../CommandBase.h"

/**
 *
 *
 * @author ExampleAuthor
 */
class $classname: public CommandBase {
public:
	$classname();
	virtual void Initialize();
	virtual void Execute();
	virtual bool IsFinished();
	virtual void End();
	virtual void Interrupted();
};

#endif
