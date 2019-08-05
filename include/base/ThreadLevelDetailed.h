
#ifndef ThreadLevelDetailed_H
#define ThreadLevelDetailed_H

#include <iostream>

#include "base/ThreadBase.h"

class ThreadLevelDetailed : public ThreadBase
{
private:


protected:

public:

    explicit ThreadLevelDetailed( void );
    virtual ~ThreadLevelDetailed();

    virtual const char * className( void ) const { return "ThreadLevelDetailed"; }

    void force( void );
    void stress( void );
    void run( int id );
    //void run( int id, string &name );

};
#endif // ThreadLevelDetailed_H
