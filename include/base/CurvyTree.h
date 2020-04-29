//******************************************************************************
// CurvyTree.h
//	: header file for 2D CurvyTree coordinates
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2017
//
//******************************************************************************

#ifndef _CurvyTree_H
#define _CurvyTree_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <vector>
#include <iostream>

using namespace std;

#include "base/Line2.h"
#include "graph/UndirectedBaseGraph.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class CurvyTree {

protected:
	
	UndirectedBaseGraph _tree;
	
	vector< Line2 > _paths;        // steinter tree paths
	
	virtual void _init( void );    // initialization
	
	bool _isKeyOnPath( vector< UndirectedBaseGraph::vertex_descriptor > &pathVec,
	                   vector< UndirectedBaseGraph::vertex_descriptor > &keyVec );
	
	void _pathPartition( void );

public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
	CurvyTree();                        // constructor (default)
	CurvyTree( const CurvyTree &v );   // copy constructor
	virtual ~CurvyTree();                // destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
	void init( void ) { _init(); }
	
	UndirectedBaseGraph &tree( void ) { return _tree; }
	
	const UndirectedBaseGraph &tree( void ) const { return _tree; }
	
	vector< Line2 > &paths( void ) { return _paths; }
	
	const vector< Line2 > &paths( void ) const { return _paths; }

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
	void computeFineCurve( int num, double unit );

//------------------------------------------------------------------------------
//	Intersection check
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Friend functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
	friend ostream &operator<<( ostream &s, const CurvyTree &v );
	
	// Output
	friend istream &operator>>( istream &s, CurvyTree &v );
	
	// Input
	virtual const char *className( void ) const { return "CurvyTree"; }
	// Class name
};


#endif // _CurvyTree_H
