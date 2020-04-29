//==============================================================================
// Boundary.h
//  : header file for the Boundary network
//
//==============================================================================

#ifndef _Boundary_H        // beginning of header file
#define _Boundary_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstring>
#include <cmath>

using namespace std;

#include "base/Grid2.h"
#include "base/Polygon2.h"
#include "graph/BoundaryGraph.h"
#include "graph/ForceGraph.h"

//------------------------------------------------------------------------------
//	Defining data types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Defining macros
//------------------------------------------------------------------------------
class Boundary {
protected:
	
	// skeletonForceGraph
	BoundaryGraph _boundary;          // inner + outer boundary
	Polygon2 *_contour;          // boundary of the graph
	double *_content_width;    // content width
	double *_content_height;    // content height
	
	// boundary graph
	vector< vector< BoundaryGraph::edge_descriptor > > _line;
	vector< vector< BoundaryGraph::vertex_descriptor > > _lineSta;
	unsigned int _nVertices;
	unsigned int _nEdges;
	unsigned int _nLines;
	unsigned int _nSbeforeSim;   // station no. before simplification
	unsigned int _nEbeforeSim;   // edge no. before simplification
	double _meanVSize;     // mean area size of the node
	double _distAlpha;     // unit distance of a focused edge
	double _distBeta;      // unit distance of an edge
	
	// for recording the information if nodes or edged are deleted.
	vector< unsigned int > _removedVertices;   // ID of removed vertices
	vector< Common::UIDPair > _removedEdges;      // VID pair of removed edges
	vector< double > _removedWeights;    // weight removed edges
	
	// detecting close vertex-edge pairs which have high possibility to overlap
	VEMap _VEconflict;    // vertex-edge pair that is too close to each other
	vector< double > _ratioR;        // ratio of vertices projected on the edge, r*v1 + (r-1)*v2


//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
	void _init( void );
	
	void _clear( void );

public:
	
	Boundary();                        // default constructor
	Boundary( const Boundary &obj );     // Copy constructor
	virtual ~Boundary();               // Destructor

//------------------------------------------------------------------------------
//	Reference to members
//------------------------------------------------------------------------------
	const BoundaryGraph &boundary( void ) const { return _boundary; }
	
	BoundaryGraph &boundary( void ) { return _boundary; }
	
	const Polygon2 *contour( void ) const { return _contour; }
	
	Polygon2 *contour( void ) { return _contour; }
	
	const double *contentWidth( void ) const { return _content_width; }
	
	double *contentWidth( void ) { return _content_width; }
	
	const double *contentHeight( void ) const { return _content_height; }
	
	double *contentHeight( void ) { return _content_height; }
	
	// boundary graph
	const unsigned int &nVertices( void ) const { return _nVertices; }
	
	unsigned int &nVertices( void ) { return _nVertices; }
	
	const unsigned int &nEdges( void ) const { return _nEdges; }
	
	unsigned int &nEdges( void ) { return _nEdges; }
	
	const unsigned int &nLines( void ) const { return _nLines; }
	
	unsigned int &nLines( void ) { return _nLines; }
	
	const double &meanVSize( void ) const { return _meanVSize; }
	
	const double &dAlpha( void ) const { return _distAlpha; }
	
	const double &dBeta( void ) const { return _distBeta; }
	
	const vector< vector< BoundaryGraph::edge_descriptor > > &line( void ) const { return _line; }
	
	vector< vector< BoundaryGraph::edge_descriptor > > &line( void ) { return _line; }
	
	const vector< vector< BoundaryGraph::vertex_descriptor > > &lineSta( void ) const { return _lineSta; }
	
	vector< vector< BoundaryGraph::vertex_descriptor > > &lineSta( void ) { return _lineSta; }
	
	const vector< unsigned int > &removedVertices( void ) const { return _removedVertices; }
	
	const vector< Common::UIDPair > &removedEdges( void ) const { return _removedEdges; }
	
	const vector< double > &removedWeights( void ) const { return _removedWeights; }
	
	const VEMap &VEconflict( void ) const { return _VEconflict; }
	
	VEMap &VEconflict( void ) { return _VEconflict; }
	
	const vector< double > &ratioR( void ) const { return _ratioR; }

//------------------------------------------------------------------------------
//      Find conflicts
//------------------------------------------------------------------------------
	void clearConflicts( void ) {
		_VEconflict.clear();
	}
	
	bool checkVEConflicts( void );

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
	void adjustsize( const int &width, const int &height );   // normalize the LevelBorder size

//------------------------------------------------------------------------------
//  File I/O
//------------------------------------------------------------------------------
	void init( void ) { _init(); }
	
	void clear( void ) { _clear(); }

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
	friend ostream &operator<<( ostream &stream, const Boundary &obj );
	
	// Output
	friend istream &operator>>( istream &stream, Boundary &obj );
	// Input
	
	virtual const char *className( void ) const { return "Boundary"; }
	// Class name
};

#endif // _Boundary_H

// end of header file
// Do not add any stuff under this line.

