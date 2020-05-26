
#ifndef ThreadLevelDetailed_H
#define ThreadLevelDetailed_H

#include <iostream>

#include "core/ThreadBase.h"

class ThreadLevelDetail : public ThreadBase {
private:


protected:

public:
	
	explicit ThreadLevelDetail( void );
	
	virtual ~ThreadLevelDetail();
	
	virtual const char *className( void ) const { return "ThreadLevelDetail"; }
	
	void force( void );
	
	void stress( void );
	
	void run( int id );
};

#endif // ThreadLevelDetailed_H
