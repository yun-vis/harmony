#include <ctime>

using namespace std;

#include "core/QuardTree.h"

//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
//
//  QuardTree::QuardTree --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
QuardTree::QuardTree() {
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
//
//  QuardTree::~QuardTree --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
QuardTree::~QuardTree() {

}

//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------
//
//  QuareTree::_init -- initialize the graph.
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void QuardTree::_init( ForceGraph *__forceGraphPtr, double __width, double __height ) {
	_forceGraphPtr = __forceGraphPtr,
			_width = __width;
	_height = __height;
	
	_tree.clear();
	_root = add_vertex( _tree );
	
	_tree[ _root ].id = 0;
	_tree[ _root ].weight = 0;
	_tree[ _root ].leafID = -1;
	_tree[ _root ].level = 0;
	
	_tree[ _root ].coordPtr = new Coord2( 0.0, 0.0 );
	_tree[ _root ].widthPtr = new double( _width );
	_tree[ _root ].heightPtr = new double( _height );
	
	_tree[ _root ].leafVec.clear();
	_tree[ _root ].child.clear();
}

//
//  QuareTree::_init -- initialize the graph.
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void QuardTree::_clear( void ) {
	cerr << "nT = " << num_vertices( _tree ) << endl;
	
	int nV = num_vertices( _tree );
	if( nV != 0 ) {
		if( _tree[ _root ].coordPtr != NULL ) delete _tree[ _root ].coordPtr;
		if( _tree[ _root ].widthPtr != NULL ) delete _tree[ _root ].widthPtr;
		if( _tree[ _root ].heightPtr != NULL ) delete _tree[ _root ].heightPtr;
		_tree[ _root ].leafVec.clear();
		_tree[ _root ].child.clear();
		_tree.clear();
	}
}

//------------------------------------------------------------------------------
//	Special functions
//------------------------------------------------------------------------------
//
//  QuardTree::subdivide --	subdivide the vertex into 4 sub vertices
//
//  Inputs
//	TreeGraph::vertex_descriptor
//
//  Outputs
//	none
//
void QuardTree::subdivide( TreeGraph::vertex_descriptor &vdC,
                           vector< TreeGraph::vertex_descriptor > &vdFour ) {
	ForceGraph &g = *_forceGraphPtr;
	
	for( unsigned int i = 0; i < 4; i++ ) {
		
		TreeGraph::vertex_descriptor vdNew = add_vertex( _tree );
		_tree[ vdNew ].id = num_vertices( _tree ) - 1;
		_tree[ vdNew ].weight = 0;
		_tree[ vdNew ].leafID = -1;
		_tree[ vdNew ].level = _tree[ vdC ].level + 1;
		
		double x = 0.0, y = 0.0;
		if( i == 0 ) {       // left-bottom
			x = _tree[ vdC ].coordPtr->x() - 0.25 * *_tree[ vdC ].widthPtr;
			y = _tree[ vdC ].coordPtr->y() - 0.25 * *_tree[ vdC ].heightPtr;
		}
		if( i == 1 ) {       // right-bottom
			x = _tree[ vdC ].coordPtr->x() + 0.25 * *_tree[ vdC ].widthPtr;
			y = _tree[ vdC ].coordPtr->y() - 0.25 * *_tree[ vdC ].heightPtr;
		}
		if( i == 2 ) {       // right-top
			x = _tree[ vdC ].coordPtr->x() + 0.25 * *_tree[ vdC ].widthPtr;
			y = _tree[ vdC ].coordPtr->y() + 0.25 * *_tree[ vdC ].heightPtr;
		}
		if( i == 3 ) {       // left-top
			x = _tree[ vdC ].coordPtr->x() - 0.25 * *_tree[ vdC ].widthPtr;
			y = _tree[ vdC ].coordPtr->y() + 0.25 * *_tree[ vdC ].heightPtr;
		}
		
		_tree[ vdNew ].coordPtr = new Coord2( x, y );
		_tree[ vdNew ].widthPtr = new double( 0.5 * *_tree[ vdC ].widthPtr );
		_tree[ vdNew ].heightPtr = new double( 0.5 * *_tree[ vdC ].heightPtr );
		_tree[ vdNew ].leafVec.clear();
		_tree[ vdNew ].child.clear();
		
		_tree[ vdC ].child.push_back( _tree[ vdNew ].id );
		vdFour.push_back( vdNew );
		
		pair< TreeGraph::edge_descriptor, unsigned int > foreE = add_edge( vdC, vdNew, _tree );
		TreeGraph::edge_descriptor edNew = foreE.first;
		_tree[ edNew ].id = num_edges( _tree ) - 1;
	}
	
	// redistribute the collected vertices
	for( unsigned int i = 0; i < _tree[ vdC ].leafVec.size(); i++ ) {
		
		assert( _tree[ vdC ].child.size() == 4 );
		for( unsigned int j = 0; j < _tree[ vdC ].child.size(); j++ ) {
			
			TreeGraph::vertex_descriptor vdT = vertex( _tree[ vdC ].child[ j ], _tree );
			if( containVertex( *g[ _tree[ vdC ].leafVec[ i ] ].coordPtr, vdT ) ) {
				_tree[ vdT ].leafVec.push_back( _tree[ vdC ].leafVec[ i ] );
			}
		}
	}
	// cerr << "*" << _tree[vdC].leafVec.size() << endl;
}

//
//  QuardTree::containVertex --	check if the vertex is in the range
//
//  Inputs
//	ForceGraph::vertex_descriptor
//  TreeGraph::vertex_descriptor
//
//  Outputs
//	none
//
bool QuardTree::containVertex( Coord2 &coord,
                               TreeGraph::vertex_descriptor &vdC ) {
	double r = _tree[ vdC ].coordPtr->x() + 0.5 * *_tree[ vdC ].widthPtr;
	double l = _tree[ vdC ].coordPtr->x() - 0.5 * *_tree[ vdC ].widthPtr;
	double t = _tree[ vdC ].coordPtr->y() + 0.5 * *_tree[ vdC ].heightPtr;
	double b = _tree[ vdC ].coordPtr->y() - 0.5 * *_tree[ vdC ].heightPtr;

#ifdef DEBUG
	cerr << "r = " << r << " l = " << l << " t = " << t << " b = " << b << endl;
	cerr << coord;
	cerr << "in = " << (coord.x() > l && coord.x() < r && coord.y() > b && coord.y() < t ) << endl;
#endif // DEBUG
	
	if( coord.x() > l && coord.x() < r &&
	    coord.y() > b && coord.y() < t )
		return true;
	
	return false;
}

//
//  QuardTree::insertVertex --	insert a new vertex to the quardtree, split if necessary
//
//  Inputs
//	ForceGraph::vertex_descriptor
//  TreeGraph::vertex_descriptor
//
//  Outputs
//	none
//
bool QuardTree::insertVertex( ForceGraph::vertex_descriptor &vdF,
                              TreeGraph::vertex_descriptor &vdC ) {
	// TreeGraph::vertex_descriptor vdC = _root;
	ForceGraph &g = *_forceGraphPtr;
	
	//cerr << "fid " << g[vdF].id << ", cid " << _tree[vdC].id << endl;
	// Ignore objects that do not belong in this quad tree
	if( !containVertex( *g[ vdF ].coordPtr, vdC ) ) {
		return false; // object cannot be added
	}
	
	// If there is space in this quad tree, add the object here
	// cerr << _tree[vdC].id << " vdVec = " << _tree[vdC].leafVec.size() << endl;
	if( _tree[ vdC ].leafVec.size() < VERTEX_CAPACITY ) {
		_tree[ vdC ].leafVec.push_back( vdF );
		g[ vdF ].leafID = _tree[ vdC ].id;
		_tree[ vdC ].meanCoord += *g[ vdF ].coordPtr;
		return true;
	}
	
	// Otherwise, subdivide and then add the point to whichever node will accept it
	vector< TreeGraph::vertex_descriptor > vdFour;
	//cerr << "size = " << _tree[vdC].child.size() << endl;
#ifdef DEBUG
	cerr << "before cid = " << _tree[vdC].id
		 << " vdFour.size() = " << vdFour.size()
		 << " childSize = " << _tree[vdC].child.size() << endl;
#endif // DEBUG
	if( _tree[ vdC ].child.size() == 0 ) {
		
		subdivide( vdC, vdFour );
#ifdef DEBUG
		cerr << "subdividing..." << endl;
		for( unsigned int i = 0; i < vdFour.size(); i++ ){
			cerr << i << ": " << _tree[vdFour[i]].id << endl;
		}
#endif // DEBUG
	}

#ifdef DEBUG
	cerr << "cid = " << _tree[vdC].id
		 << " vdFour.size() = " << vdFour.size()
		 << " childSize = " << _tree[vdC].child.size() << endl;
	for( unsigned int i = 0; i < _tree[vdC].child.size(); i++ ) {
		cerr << _tree[vdC].child[i] << " ";
	}
	cerr << endl;
#endif // DEBUG
	
	for( unsigned int i = 0; i < _tree[ vdC ].child.size(); i++ ) {
		unsigned int id = _tree[ vdC ].child[ i ];
		TreeGraph::vertex_descriptor vdT = vertex( id, _tree );
		if( insertVertex( vdF, vdT ) ) {
			_tree[ vdC ].leafVec.push_back( vdF );
			return true;
		}
	}
	
	// Otherwise, the point cannot be inserted for some unknown reason (this should never happen)
	return false;
}
