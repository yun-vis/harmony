
#ifndef ThreadLevelCenter_H
#define ThreadLevelCenter_H

#include <iostream>

#include "core/ThreadBase.h"

class ThreadLevelCellCenter : public ThreadBase {
private:


protected:

public:
	
	explicit ThreadLevelCellCenter( void );
	
	virtual ~ThreadLevelCellCenter();
	
	virtual const char *className( void ) const { return "ThreadLevelCellCenter"; }
	
	void force( void );
	
	void stress( void );
	
	void run( int id );
};

#endif // ThreadLevelCenter_H
