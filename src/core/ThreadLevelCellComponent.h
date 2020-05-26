
#ifndef ThreadLevelMiddle_H
#define ThreadLevelMiddle_H

#include <iostream>

#include "core/ThreadBase.h"

class ThreadLevelCellComponent : public ThreadBase {
private:


protected:

public:
	
	explicit ThreadLevelCellComponent( void );
	
	virtual ~ThreadLevelCellComponent();
	
	virtual const char *className( void ) const { return "ThreadLevelCellComponent"; }
	
	void force( void );
	
	void stress( void );
	
	void run( int id );
};

#endif // ThreadLevelMiddle_H
