//==============================================================================
// Similarity.h
//	: program file for networks
//
//------------------------------------------------------------------------------
//
//				Date: Sat Jul 30 23:07:36 2016
//
//==============================================================================

#ifndef _Similarity_H		// beginning of header file
#define _Similarity_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <QImage>
#include "base/Polygon2.h"
#include "base/QuardTree.h"
#include "base/Config.h"
#include "graph/ForceGraph.h"
#include "optimization/Voronoi.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Similarity {

  private:

    unsigned int    _id;
    ForceGraph      * _g1Ptr, * _g2Ptr;

  protected:

    void		    _init	    ( ForceGraph *__g1Ptr, ForceGraph *__g2Ptr );
    void            _clear      ( void );

    bool            _isSimilar  ( void );

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    Similarity();			// default constructor
    Similarity( const Similarity & obj );
				// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~Similarity();			// destructor

//------------------------------------------------------------------------------
//	Referencing to members
//------------------------------------------------------------------------------
    const unsigned int &    id( void )      const   { return _id; }
    unsigned int &          id( void )              { return _id; }


//------------------------------------------------------------------------------
//	Fundamental functions
//------------------------------------------------------------------------------
    void init( ForceGraph *__g1Ptr, ForceGraph *__g2Ptr ) {
        _init( __g1Ptr, __g2Ptr );
    }
    void clear( void )				{ _clear(); }

//------------------------------------------------------------------------------
//	Similarity functions
//------------------------------------------------------------------------------
    bool isSimilar( void )          { return _isSimilar(); }

//------------------------------------------------------------------------------
//	Specific functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------
    Similarity & operator = ( const Similarity & obj );
				// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Similarity & obj );
				// output
    friend istream & operator >> ( istream & stream, Similarity & obj );
				// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "Similarity"; }
				// class name
};


#endif // _Similarity_H

// end of header file
// Do not add any stuff under this line.
