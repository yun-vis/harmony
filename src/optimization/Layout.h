//==============================================================================
// Layout.h
//	: header file for constrained problems for Layout layouts
//	(MIP -> Layout)
//
//	This class is for computing the Layout layout of the network.
//
//------------------------------------------------------------------------------
//
//				Date: Mon Mar 11 00:04:57 2017
//
//==============================================================================

#ifndef _Layout_H        // beginning of header file
#define _Layout_H        // notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include "core/Common.h"
#include "graph/SkeletonGraph.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	External function declaration
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class Layout {

private:
	int _width;
	int _height;

protected:
	
	SkeletonGraph *_skeletonGraph;
	double _err;
	vector< vector< SkeletonGraph::vertex_descriptor > > _childSubGraph;

public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
	Layout();            // default constructor
	// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
	~Layout();            // destructor

//------------------------------------------------------------------------------
//	Initialization functions
//------------------------------------------------------------------------------
	
	virtual void _init( SkeletonGraph *__skeletonGraph );

//------------------------------------------------------------------------------
// 	Specific functions
//------------------------------------------------------------------------------
	void up_degrees( SkeletonGraph::vertex_descriptor &vdC,
	                 int &degrees, bool &isAllVisited );
	
	bool subGraphConnected( SkeletonGraph::vertex_descriptor &vdA, SkeletonGraph::vertex_descriptor &vdB, bool &visit );
	
	void findChildSubGraph( SkeletonGraph::vertex_descriptor &vdTarget );
	
	void computeChildSubGraph( void );
	
	void radialPlacement( void );

//------------------------------------------------------------------------------
//	Referring to members
//------------------------------------------------------------------------------
	const double err( void ) const { return _err; }

//------------------------------------------------------------------------------
//      Initialization functions
//------------------------------------------------------------------------------
	void init( SkeletonGraph *__skeletonGraph, int __width, int __height ) {
		_init( __skeletonGraph );
		_width = __width;
		_height = __height;
	}

//------------------------------------------------------------------------------
//	Assignment operators
//------------------------------------------------------------------------------
	Layout &operator=( const Layout &obj );
	// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
	friend ostream &operator<<( ostream &stream, const Layout &obj );
	
	// output
	friend istream &operator>>( istream &stream, Layout &obj );
	// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
	virtual const char *className( void ) const { return "Layout"; }
	// class name
};


#endif // _Layout_H

// end of header file
// Do not add any stuff under this line.
