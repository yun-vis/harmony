//******************************************************************************
// BaseEdgeProperty.h
//	: header file for 2D coordinaes
//
//------------------------------------------------------------------------------
//
//	Ver 1.00		Date: Tue Dec 27 23:16:12 2011
//
//******************************************************************************

#ifndef _BaseEdgeProperty_H
#define _BaseEdgeProperty_H

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>
#include <vector>

using namespace std;

#include "core/Coord2.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class BaseEdgeProperty {

protected:
	
	void _init( void );


public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
	BaseEdgeProperty();                // constructor (default)
	BaseEdgeProperty( const BaseEdgeProperty &e ) {
		id = e.id;
		weight = e.weight;
		visit = e.visit;
		visitedTimes = e.visitedTimes;
		label = e.label;
		
		isFore = e.isFore;
		isBack = e.isBack;
	}                    // copy constructor
	virtual ~BaseEdgeProperty() {}        // destructor

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Reference to elements
//------------------------------------------------------------------------------
	unsigned int id;
	
	double angle;
	double weight;
	bool visit;
	int visitedTimes;
	int label;
	
	bool isFore;
	bool isBack;
	
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
	friend ostream &operator<<( ostream &s, const BaseEdgeProperty &v );
	
	// Output
	friend istream &operator>>( istream &s, BaseEdgeProperty &v );
	
	// Input
	virtual const char *className( void ) const { return "BaseEdgeProperty"; }
	// class name
	
};

#endif // _BaseEdgeProperty_H
