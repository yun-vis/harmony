//******************************************************************************
// TreeVertexProperty.h
//	: header file for 2D coordinates
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef _TreeVertexProperty_H
#define _TreeVertexProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>
#include <vector>

using namespace std;

#include "base/Coord2.h"
#include "base/Common.h"
#include "graph/ForceGraph.h"
#include "graph/BaseVertexProperty.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------
#define VERTEX_CAPACITY (1)

//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class TreeVertexProperty : public BaseVertexProperty {

protected:
	
	void _init( void );

public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
	TreeVertexProperty();                // constructor (default)
	TreeVertexProperty( const TreeVertexProperty &v ) {
	}                    // copy constructor
	virtual ~TreeVertexProperty() {}        // destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
	int leafID;
	unsigned int level;      // hierarchical level
	
	Coord2 meanCoord;
	
	vector< ForceGraph::vertex_descriptor > leafVec;
	vector< unsigned int > child;      // vid of children

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
	
	void init( void ) { _init(); }

//------------------------------------------------------------------------------
//	Intersection check
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
	
	friend ostream &operator<<( ostream &s, const TreeVertexProperty &v );
	
	// Output
	friend istream &operator>>( istream &s, TreeVertexProperty &v );
	
	// Input
	virtual const char *className( void ) const { return "TreeVertexProperty"; }
	// class name
	
};

#endif // _TreeVertexProperty_H
