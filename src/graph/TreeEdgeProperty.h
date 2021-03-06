//******************************************************************************
// TreeEdgeProperty.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef _TreeEdgeProperty_H
#define _TreeEdgeProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>
#include <vector>

using namespace std;

#include "core/Coord2.h"
#include "graph/BaseEdgeProperty.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class TreeEdgeProperty : public BaseEdgeProperty {

protected:
	
	void _init( void );


public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
	TreeEdgeProperty();                // constructor (default)
	TreeEdgeProperty( const TreeEdgeProperty &e ) {
		id = e.id;
		weight = e.weight;
	}                    // copy constructor
	virtual ~TreeEdgeProperty() {}        // destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
	
	void init( void ) { _init(); }

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Intersection check
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
	friend ostream &operator<<( ostream &s, const TreeEdgeProperty &v );
	
	// Output
	friend istream &operator>>( istream &s, TreeEdgeProperty &v );
	
	// Input
	virtual const char *className( void ) const { return "TreeEdgeProperty"; }
	// class name
	
};

#endif // _TreeEdgeProperty_H
