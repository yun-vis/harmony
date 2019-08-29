//==============================================================================
// Pathway.h
//  : header file for the map
//
//==============================================================================

#ifndef _PATHWAY_H        // begining of header file
#define _PATHWAY_H        // notifying that this file is included

//----------------------------------------------------------------------
//  Including header files
//----------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cassert>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <tinyxml.h>
//#include <tinystr.h>
#include <mutex>

using namespace std;

#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/graph/planar_canonical_ordering.hpp>
#include <boost/graph/is_straight_line_drawing.hpp>

#include <boost/graph/chrobak_payne_drawing.hpp>
#include <boost/graph/biconnected_components.hpp>
#include <boost/graph/make_biconnected_planar.hpp>
#include <boost/graph/make_maximal_planar.hpp>
#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/graphviz.hpp>
//#include <boost/graph/detail/read_graphviz_new.hpp>

#include "boost/create_dual_graph.hpp"
#include "base/Polygon2.h"
#include "graph/MetaboliteGraph.h"
#include "graph/UndirectedPropertyGraph.h"
#include "graph/DirectedBaseGraph.h"
#include "graph/ForceGraph.h"
#include "graph/SkeletonGraph.h"

class Pathway
{
private:

    double                     *_widthPtr, *_heightPtr;		// window width and height
	unsigned int				_nHyperEtoE;	// numbers of hyper edges to single egdes
	map< string, string >		_nV;			// distinct metabolites, duplicated nodes are not counted

	// metabolite graph
	MetaboliteGraph				_graph;			// directed
    ForceGraph					_layoutGraph;	// undirected

    // metabolite subgraph
    vector< MetaboliteGraph	>	_subGraph;		// directed
    vector< ForceGraph	>		_layoutSubGraph;// undirected

	SkeletonGraph				_skeletonGraph; // undirected, show connectivity and relative position of sub-system

    map< string, unsigned int > _metaFreq;		// map for recording the frequency of all metabolites
	map< string, string > 		_metaType;		// map for recording the types of all metabolites
	map< string, int > 			_dupliMap;		// map for recording the duplication of metabolites
	map< string, Subdomain * > 	_sub;			// map for recording the sub domain info
    vector< BioNetMetabolite >  _meta;			// metablite vector
    vector< BioNetReaction >    _react;			// reaction vector
	vector< vector < double > > _edgeW;			// edheWeight vector for dependGraph

    // ui
    bool                        _isCloneByThreshold;
    double                      _threshold;		// threshold value
	unsigned int 				_nZoneDiff;		// total different zone value

protected:

	mutex						_pathway_mutex;
	string 						_inputpath, _outputpath;	// input output files
	string 						_fileFreq, _fileType;		// metabolite frequency

	void loadMetaFreq( string filename );
	void loadMetaType( string filename );
    void loadXml( string filename );

	// graph
    MetaboliteGraph::vertex_descriptor findVertex( unsigned int id, MetaboliteGraph &g );
	bool findMetaboliteInGraph( string name, MetaboliteGraph::vertex_descriptor &existedVD );
    bool findMetaboliteInVec( string name, int &index );
	unsigned int retrieveFreq( string name );
	string retrieveMetaType( string name );
	unsigned int inout_degree( MetaboliteGraph::vertex_descriptor vd );
	bool isCloneMetaType( MetaboliteGraph::vertex_descriptor metaVD );

    void clearPathway( void );

	bool has_cycle_dfs( const UndirectedPropertyGraph & g, UndirectedPropertyGraph::vertex_descriptor u,
						default_color_type * color );
	bool has_cycle( const UndirectedPropertyGraph & g );
	bool loop_in_dual_graph_test( UndirectedPropertyGraph & propG );
	// void max_spaning_tree( void );
	bool planarity_test( vector< SkeletonGraphVVPair > &addedED, SkeletonGraphVVPair & newE );
	bool constrained_planarity_test( vector< SkeletonGraphVVPair > &addedED,
									 SkeletonGraphVVPair & newE, unsigned int maxDegree );
	void planar_graph_embedding( vector< SkeletonGraphVVPair > &addedED );
	void planar_max_filtered_graph( void );
	void computeZone( void );

public:

    Pathway();                      // default constructor
    Pathway( const Pathway & obj ); // Copy constructor
    virtual ~Pathway();             // Destructor

//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------
    const MetaboliteGraph &           g( void ) const 	 { return _graph; }
    MetaboliteGraph &                 g( void )          { return _graph; }
    const ForceGraph &              lg( void ) const 	 { return _layoutGraph; }
    ForceGraph &                    lg( void )           { return _layoutGraph; }
    const SkeletonGraph &           skeletonG( void ) const { return _skeletonGraph; }
    SkeletonGraph &                 skeletonG( void )       { return _skeletonGraph; }

	const vector< MetaboliteGraph >   & subG( void ) const  { return _subGraph; }
    vector< MetaboliteGraph > &         subG( void )        { return _subGraph; }
    const vector< ForceGraph > & 	    lsubG( void ) const { return _layoutSubGraph; }
    vector< ForceGraph > &           	lsubG( void )       { return _layoutSubGraph; }

	const map< string, unsigned int > &           metafreq( void ) const { return _metaFreq; }
    map< string, unsigned int > &                 metafreq( void )       { return _metaFreq; }
	const map< string, Subdomain * > &         	  subsys( void ) const 	 { return _sub; }
    map< string, Subdomain * > &                  subsys( void )       	 { return _sub; }

    const unsigned int		nVertices( void ) 	{ return num_vertices( _graph ); }
	const unsigned int		nEdges( void ) 		{ return num_edges( _graph ); }
	const unsigned int		nSubsys( void ) 	{ return _sub.size(); }

	const string &          inpath( void ) const   { return _inputpath; }
	string &                inpath( void )         { return _inputpath; }
	const string &          outpath( void ) const  { return _outputpath; }
	string &                outpath( void )        { return _outputpath; }

    const double *        	width( void ) const  { return _widthPtr; }
    double *                width( void )        { return _widthPtr; }
    const double *          height( void ) const { return _heightPtr; }
    double *                height( void )       { return _heightPtr; }

	const mutex &           pathwayMutex( void ) const { return _pathway_mutex; }
	mutex &                 pathwayMutex( void )       { return _pathway_mutex; }

//------------------------------------------------------------------------------
//  	Specific functions
//------------------------------------------------------------------------------
	void init( string pathIn, string pathOut,
               string fileFreq, string fileType, int clonedThreshold );
	void generate( void );
	void exportEdges( void );
	void initLayout( map< unsigned int, Polygon2 > &_polygonComplex );
	void initSubdomain( void );
	void loadPathway( void );
	void layoutPathway( void );
    void genGraph( void );
	void computeIdealAreaOfSubdomain( void );
    void genLayoutGraph( void );
    void genSubGraphs( void );
    void genLayoutSubGraphs( void );
	void genDependencyGraph( void );

	void pickColor( COLORTYPE colorType, unsigned int id, vector< double > &rgb );
	void loadDot( UndirectedPropertyGraph &graph, string filename );
	void exportDot( void );
	void normalization( void );

//------------------------------------------------------------------------------
//      Find conflicts
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      UI
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const Pathway & obj );
    // Output
    friend istream & operator >> ( istream & stream, Pathway & obj );
    // Input

    virtual const char * className( void ) const { return "Pathway"; }
    // Class name
};

#endif // _PATHWAY_H

// end of header file
// Do not add any stuff under this line.
