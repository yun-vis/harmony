
#ifndef ThreadLevelMiddle_H
#define ThreadLevelMiddle_H

#include <iostream>

#include "base/ThreadBase.h"

class ThreadLevelMiddle : public ThreadBase
{
private:


protected:

public:

    explicit ThreadLevelMiddle( void );
    virtual ~ThreadLevelMiddle();

    virtual const char * className( void ) const { return "ThreadLevelMiddle"; }

    void force( void );
    void stress( void );
    void run( int id );
    //void run( int id, string &name );

};
#endif // ThreadLevelMiddle_H
