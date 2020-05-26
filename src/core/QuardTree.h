//******************************************************************************
// QuardTree.h
//	: header file for QuardTree
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Nov 19 02:36:37 2018
//
//******************************************************************************

#ifndef _QuardTree_H
#define _QuardTree_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <cmath>
#include <iostream>

using namespace std;

#include "graph/TreeGraph.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class QuardTree {

protected:
	
	ForceGraph *_forceGraphPtr;
	TreeGraph _tree;
	TreeGraph::vertex_descriptor _root;
	
	void _init( ForceGraph *__forceGraphPtr, double __width, double __height );
	
	void _clear( void );
	
	// display
	int _width;
	int _height;

public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
	QuardTree();                        // constructor (default)
	virtual ~QuardTree();                // destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
	const TreeGraph &tree( void ) const { return _tree; }
	
	TreeGraph &tree( void ) { return _tree; }
	
	const TreeGraph::vertex_descriptor &root( void ) const { return _root; }
	
	TreeGraph::vertex_descriptor &root( void ) { return _root; }


//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
	void subdivide( TreeGraph::vertex_descriptor &vdC,
	                vector< TreeGraph::vertex_descriptor > &vdFour );
	
	bool containVertex( Coord2 &coord,
	                    TreeGraph::vertex_descriptor &vdC );
	
	bool insertVertex( ForceGraph::vertex_descriptor &vdF,
	                   TreeGraph::vertex_descriptor &vdC );
	
	void init( ForceGraph *__forceGraphPtr, double __width, double __height ) {
		_init( __forceGraphPtr, __width, __height );
	}
	
	void clear( void ) {
		_clear();
	}
	
	void reset( ForceGraph *__forceGraphPtr, double __width, double __height ) {
		_clear();
		_init( __forceGraphPtr, __width, __height );
	}

//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
	friend ostream &operator<<( ostream &s, const QuardTree &v );
	
	friend istream &operator>>( istream &s, QuardTree &v );
	
	virtual const char *className( void ) const { return "QuardTree"; }
	
};


#endif // _QuardTree_H
