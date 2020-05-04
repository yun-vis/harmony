
#ifndef ThreadLevelHigh_H
#define ThreadLevelHigh_H

#include <iostream>

#include "base/ThreadBase.h"

class ThreadLevelBorder : public ThreadBase {
private:

protected:

public:
	
	explicit ThreadLevelBorder( void );
	
	virtual ~ThreadLevelBorder();
	
	virtual const char *className( void ) const { return "ThreadLevelBorder"; }
	
	void force( void );
	
	void stress( void );
	
	void run( int id );
};

#endif // ThreadLevelHigh_H
