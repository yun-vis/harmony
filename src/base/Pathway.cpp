//==============================================================================
// Pathway.cpp
//  : program file for the map
//
//------------------------------------------------------------------------------
//
//              Date: Mon Dec 29 04:28:26 2016
//
//==============================================================================

//------------------------------------------------------------------------------
//      Including Header Files
//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstring>
#include <cmath>

using namespace std;


#include <QtWidgets/QDialog>
#include <QtWidgets/QFileDialog>

#include "base/Pathway.h"

//#define STRAIGHT_LINE_DRAWING

struct coord_t
{
  std::size_t x;
  std::size_t y;
};

//------------------------------------------------------------------------------
//      Public functions
//------------------------------------------------------------------------------
//
//  Pathway::Pathway -- default constructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Pathway::Pathway( void )
{
}

//
//  Pathway::Pathway -- copy constructor
//
//  Inputs
//  obj : object of this class
//
//  Outputs
//  none
//
Pathway::Pathway( const Pathway & obj )
{
}


//------------------------------------------------------------------------------
//      Destructor
//------------------------------------------------------------------------------
//
//  Pathway::~Pathway --    destructor
//
//  Inputs
//  none
//
//  Outputs
//  none
//
Pathway::~Pathway( void )
{
}

//------------------------------------------------------------------------------
//  Specific functions
//------------------------------------------------------------------------------
//
//  Pathway::init --    initialization
//
//  Inputs
//  pathname: name of the main path
//
//  Outputs
//  node
//
void Pathway::init( string pathIn, string pathOut, string fileFreq, string fileType )
{
	_inputpath = pathIn;
	_outputpath = pathOut;
    _fileFreq = fileFreq;
    _fileType = fileType;
	_nHyperEtoE = 0;
}

//
//  Pathway::initLayout --    initialization of the layout
//
//  Inputs
//  node
//
//  Outputs
//  node
//
void Pathway::initLayout( map< unsigned int, Polygon2 > &polygonComplex )
{
	// create edge object from the spanning tree and add it to the scene
    for( unsigned int i = 0; i < _layoutSubGraph.size(); i++ ) {

		map< unsigned int, Polygon2 >::iterator itP = polygonComplex.begin();
		double minX = INFINITY, maxX = -INFINITY, minY = INFINITY, maxY = -INFINITY;

		// compute boundary
		advance( itP, i );
		Coord2 lt( INFINITY, -INFINITY ), rb( -INFINITY, INFINITY ), center( 0.0, 0.0 );
		vector< Coord2 > &coordVec = itP->second.elements();
		for( unsigned int k = 0; k < coordVec.size(); k++ ){
			if( coordVec[k].x() < lt.x() ) lt.x() = coordVec[k].x();
			if( coordVec[k].x() > rb.x() ) rb.x() = coordVec[k].x();
			if( coordVec[k].y() > lt.y() ) lt.y() = coordVec[k].y();
			if( coordVec[k].y() < rb.y() ) rb.y() = coordVec[k].y();
			center += coordVec[k];
		}
		center /= (double) coordVec.size();

        // initialization
        BGL_FORALL_VERTICES( vd, _layoutSubGraph[i], ForceGraph ) {

			if( maxX < _layoutSubGraph[i][vd].coordPtr->x() ) maxX = _layoutSubGraph[i][vd].coordPtr->x();
			if( minX > _layoutSubGraph[i][vd].coordPtr->x() ) minX = _layoutSubGraph[i][vd].coordPtr->x();
			if( maxY < _layoutSubGraph[i][vd].coordPtr->y() ) maxY = _layoutSubGraph[i][vd].coordPtr->y();
			if( minY > _layoutSubGraph[i][vd].coordPtr->y() ) minY = _layoutSubGraph[i][vd].coordPtr->y();
        }
        // normalization
        double scale = 0.1;
        double half_w = scale*(rb.x()-lt.x());
        double half_h = scale*(lt.y()-rb.y());
        // cerr << "c = " << center;
		BGL_FORALL_VERTICES( vd, _layoutSubGraph[i], ForceGraph ) {

			//_layoutSubGraph[i][vd].coordPtr->x() = center.x();
			//_layoutSubGraph[i][vd].coordPtr->y() = center.y();
			_layoutSubGraph[i][vd].coordPtr->x() = half_w * (_layoutSubGraph[i][vd].coordPtr->x() - minX)/( maxX - minX )+center.x() - scale * half_w;
			_layoutSubGraph[i][vd].coordPtr->y() = half_h * (_layoutSubGraph[i][vd].coordPtr->y() - minY)/( maxY - minY )+center.y() - scale * half_h;
		}
    }
}

//
//  Pathway::findMetabolite --    find the metabolite and return its vertex descriptor
//
//  Inputs
//  name: name of the metabolite
//
//  Outputs
//  MetaboliteGraph::vertex_descriptor: return vertex descriptor
//
bool Pathway::findMetaboliteInGraph( string name, MetaboliteGraph::vertex_descriptor &existedVD )
{
	BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph )
	{
		//cerr << "name =" << _graph[ vd ].metaPtr->name << endl;
		if( _graph[ vd ].type == "metabolite" ){
			if( ( _graph[ vd ].metaPtr != NULL ) && ( _graph[ vd ].metaPtr->name == name ) ){
				// cerr << " hit name = " << name << endl;
                existedVD = vd;
				return true;
			}
		}
	}
	return false;
}

//
//  Pathway::findMetabolite --    find the metabolite and return its index
//
//  Inputs
//  name: name of the metabolite
//  index:
//
//  Outputs
//  bool: if is founded then return true, else false
//
bool Pathway::findMetaboliteInVec( string name, int &index )
{
	for( unsigned int i = 0; i < _meta.size(); i++ ){

		if( _meta[i].name == name ){
			index = i;
			// cerr << "  name = " << name << endl;
			return true;
		}
	}
	return false;
}

//
//  Pathway::retrieveFreq --    return the recorded number the metabolite
//
//  Inputs
//  name: name of the metabolite
//
//  Outputs
//  none
//
unsigned int Pathway::retrieveFreq( string name )
{
	for( map< string, unsigned int >::iterator it = _metaFreq.begin(); it != _metaFreq.end(); ++it ){

		if( it->first == name )
			return it->second;
	}

	return 0;
}

//
//  Pathway::retrieveMetaType --    return the recorded number the metabolite
//
//  Inputs
//  name: name of the metabolite
//
//  Outputs
//  none
//
string Pathway::retrieveMetaType( string name )
{
	for( map< string, string >::iterator it = _metaType.begin(); it != _metaType.end(); ++it ){

		if( it->first == name )
			return it->second;
	}

	return "unknown";
}

//
//  Pathway::isCloneMetaType --    return true if is defined as duplication type
//
//  Inputs
//  name: vertex_descriptor of the metabolite
//
//  Outputs
//  isClone: return true if is defined as a cloned metabolite
//
bool Pathway::isCloneMetaType( MetaboliteGraph::vertex_descriptor metaVD )
{
	bool isClone = true;

	if( //( _graph[ metaVD ].metaPtr->metaType == "7_WATER" ) ||
		//( _graph[ metaVD ].metaPtr->metaType == "6_ION" ) ||
		//( _graph[ metaVD ].metaPtr->metaType == "5_COFACTOR_ORANGE" ) ||
		//( _graph[ metaVD ].metaPtr->metaType == "5_COFACTOR_DARK" ) ||
		//( _graph[ metaVD ].metaPtr->metaType == "5_COFACTOR_LIGHT" ) ||
		//( _graph[ metaVD ].metaPtr->metaType == "4_ANTIOXIDANT_AND_FREERADICALS" ) ||
		//( _graph[ metaVD ].metaPtr->metaType == "3_PHOSPHATE" ) ||
		//( _graph[ metaVD ].metaPtr->metaType == "2_NUCLEOTIDE" ) ||
		//( _graph[ metaVD ].metaPtr->metaType == "unknown" ) ||
		( _graph[ metaVD ].metaPtr->metaType == "1_METABOLITE" )
		){

		isClone = false;
	}

	return isClone;
}

//
//  Pathway::generate --    generate the pathway
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::generate( void )
{
    loadPathway();
    initSubdomain();
    genGraph();
    genSubGraphs();
    randomGraphLayout( (DirectedBaseGraph &) _graph );
    //fruchtermanGraphLayout( (DirectedBaseGraph &) _graph );
    // normalization();
    //initLayout();
}

//
//  Pathway::loadXml --    loadxml file
//
//  Inputs
//  inputname: filename
//
//  Outputs
//  none
//
void Pathway::loadXml( string inputname )
{
	string filename = _inputpath + inputname; // + ".xml";
#ifdef DEBUG
	cerr << "inputename = " << inputname << endl;
#endif // DEBUG

    TiXmlDocument xmlDoc( filename.c_str() );

	xmlDoc.LoadFile();

	if( xmlDoc.ErrorId() > 0 ) {
        cerr << "ErrorId = " << xmlDoc.ErrorDesc() << " : " << filename << endl;
        return;
    }

	TiXmlElement* rootMetabolites = xmlDoc.RootElement();
    TiXmlElement* rootReactions = rootMetabolites->NextSiblingElement();
    TiXmlElement* rootSubsystems = rootReactions->NextSiblingElement();
    //cerr << "rootMetabolites = " << rootMetabolites->Value() << endl;
    //cerr << "rootReactions = " << rootReactions->Value() << endl;

	if( !rootMetabolites || !rootReactions ) return;

    // reading metabolites
    for( TiXmlElement* metabolite = rootMetabolites->FirstChildElement(); metabolite; metabolite = metabolite->NextSiblingElement() ){

        BioNetMetabolite newMeta;
        //cerr << "metabolite = " << metabolite->Value() << endl;

        for( TiXmlElement* child = metabolite->FirstChildElement(); child; child = child->NextSiblingElement() ){

            string strname = child->Value();
            if( strname == "name" ){
                //cerr << strname << " = " << child->GetText() << endl;

                if( child->GetText() != NULL ){
                    newMeta.name = child->GetText();
                    _nV.insert( pair< string, string >( newMeta.name, newMeta.name )  );
                }
                else
                    newMeta.description = "";
            }
            if( strname == "formula" ){
                if( child->GetText() != NULL )
                    newMeta.formula = child->GetText();
                else
                    newMeta.description = "";
            }
            if( strname == "description" ){
                if( child->GetText() != NULL )
                    newMeta.description = child->GetText();
                else
                    newMeta.description = "";
            }

        }
		newMeta.freq = retrieveFreq( newMeta.name );
		newMeta.metaType = retrieveMetaType( newMeta.name );

        QFont font = QFont( "Arial", DEFAULT_FONT_SIZE, QFont::Bold, false );
		QFontMetrics fm( font );
		newMeta.namePixelWidth = fm.width( QString::fromStdString( newMeta.name ) );
        newMeta.namePixelHeight = fm.height();

        bool isExisted = false;
        // check if existed
        for( unsigned int i = 0; i < _meta.size(); i++ ){
            if( newMeta.name == _meta[i].name ){
                isExisted = true;
                //cerr << "i = " << i << ", " << _meta[i].name << endl;
            }
        }
        if( isExisted == false ) _meta.push_back( newMeta );
    }

    // reading reactions
    for( TiXmlElement* reaction = rootReactions->FirstChildElement(); reaction; reaction = reaction->NextSiblingElement() ){

        BioNetReaction newReact;
        //cerr << "reaction = " << reaction->Value() << endl;

        for( TiXmlElement* child = reaction->FirstChildElement(); child; child = child->NextSiblingElement() ){

            string strname = child->Value();
            //cerr << "child = " << strname << endl;
            if( strname == "id" ){
                newReact.abbr = child->GetText();
            }
            if( strname == "name" ){
                newReact.name = child->GetText();
            }
            if( strname == "reactant" ){
                newReact.reactantVec.push_back( child->GetText() );
                _nHyperEtoE++;
            }
            if( strname == "product" ){
                newReact.productVec.push_back( child->GetText() );
				_nHyperEtoE++;
            }
            if( strname == "reversible" ){
                newReact.reversible = child->GetText();
            }
            if( strname == "subsystem" ){

				string subsystemname = child->GetText();
                map< string, Subdomain * >::iterator it = _sub.find( subsystemname );
                if( it != _sub.end() ){
                    it->second->reactNum++;
					newReact.subsystem = it->second;
                }
                else{
                    newReact.subsystem = new Subdomain;
					//newReact.subsystem->name = subsystemname;
					//Subdomain domain;
					newReact.subsystem->id		= _sub.size();
					newReact.subsystem->name	= subsystemname;
					newReact.subsystem->reactNum = 1;
					newReact.subsystem->area 	= 0.0;
					newReact.subsystem->center.x() 	= 0.0;			// center point of the subdomain
					newReact.subsystem->center.y() 	= 0.0;			// center point of the subdomain
					newReact.subsystem->width 	= 0.0;				// width df the domain
					newReact.subsystem->height 	= 0.0;				// height of the domain
                    newReact.subsystem->ratio 	= DEFAULT_ASPECT;	// aspect ratio of the subdomain
                    _sub.insert( pair< string, Subdomain * >( subsystemname, newReact.subsystem ) );
                }
#ifdef DEBUG
                if( subsystemname == "Alanine and aspartate metabolism" ){
                    cerr << " newReact.abbr = " << newReact.abbr << " addr = " << newReact.subsystem << endl;
                }
#endif // DEBUG
            }
        }

        // add name pixel length
        QFont font = QFont( "Arial", DEFAULT_FONT_SIZE, QFont::Bold, false );
        QFontMetrics fm( font );
        newReact.namePixelWidth = fm.width( QString::fromStdString( newReact.abbr ) );
        newReact.namePixelHeight = fm.height();

        _react.push_back( newReact );
    }

    // reading subsystems
    if ( rootSubsystems != NULL ) {

        for( TiXmlElement* subsys = rootSubsystems->FirstChildElement(); subsys; subsys = subsys->NextSiblingElement() ){

            string id, name, color, x, y;

            for( TiXmlElement* child = subsys->FirstChildElement(); child; child = child->NextSiblingElement() ){

                string strname = child->Value();
                //cerr << "child = " << strname << endl;
                if( strname == "id" ){
                    id = child->GetText();
                }
                if( strname == "name" ){
                    name = child->GetText();
                }
                if( strname == "x" ){
                    x = child->GetText();
                }
                if( strname == "y" ){
                    y = child->GetText();
                }
                if( strname == "color" ){
                    color = child->GetText();
                }
            }

            map< string, Subdomain * >::iterator itS = _sub.find( name );
            itS->second->defaultCenter.x() = atof( x.c_str() );
            itS->second->defaultCenter.y() = atof( y.c_str() );
            itS->second->defaultColor = color;

#ifdef DEBUG
            cerr << "name = " << name << endl;
        cerr << itS->second->defaultCenter;
#endif // DEBUG
        }
    }

#ifdef DEBUG
    cerr << " nMetabolites = " << _meta.size() << endl;
	cerr << " nReactions = " << _react.size() << endl;
    cerr << " nSubsystems = " << _sub.size() << endl;
#endif // DEBUG
}

//
//  Pathway::genGraph --    generate the graph representation
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::genGraph( void )
{
	unsigned int nVertices = 0, nEdges = 0;

	_graph[ graph_bundle ].centerPtr	= new Coord2( 0.0, 0.0 );
	_graph[ graph_bundle ].widthPtr		= new double( DEFAULT_WIDTH );
	_graph[ graph_bundle ].heightPtr    = new double( DEFAULT_HEIGHT );

	for( unsigned int i = 0; i < _react.size(); i++ ){

		// add reaction vertex
		MetaboliteGraph::vertex_descriptor reactVD 		= add_vertex( _graph );

		_graph[ reactVD ].id 			= nVertices;
		_graph[ reactVD ].groupID		= nVertices;
        _graph[ reactVD ].initID		= nVertices;
        //_graph[ reactVD ].isSelected	= false;
        //_graph[ reactVD ].isNeighbor	= false;
        _graph[ reactVD ].isClonedPtr   = NULL;
		_graph[ reactVD ].type 			= "reaction";
		_graph[ reactVD ].coordPtr		= new Coord2;
		_graph[ reactVD ].coordPtr->x() = rand() % DEFAULT_WIDTH - DEFAULT_WIDTH/2.0;
		_graph[ reactVD ].coordPtr->y() = rand() % DEFAULT_HEIGHT - DEFAULT_HEIGHT/2.0;
        _graph[ reactVD ].widthPtr      = new double (10.0);
        _graph[ reactVD ].heightPtr		= new double (10.0);
        _graph[ reactVD ].metaPtr		= NULL;
		_graph[ reactVD ].reactPtr		= &_react[i];
        _graph[ reactVD ].namePtr  		= &_react[i].abbr;
        _graph[ reactVD ].namePixelWidthPtr  = &_react[i].namePixelWidth;
        _graph[ reactVD ].namePixelHeightPtr = &_react[i].namePixelHeight;

		nVertices++;

		// add reactants
		for( unsigned int j = 0; j < _react[i].reactantVec.size(); j++ ){

			string name = _react[i].reactantVec[j];

			string buf; // have a buffer string
			stringstream ss( name ); // insert the string into a stream

			// a vector to hold our words
			vector< string > tokens;
			int stoichiometry = 1;
			while (ss >> buf) tokens.push_back( buf );
			// cerr << " tokens.size() = " << tokens.size() << endl;
			assert( tokens.size() <= 2 );
			if( tokens.size() > 1 ) stoichiometry = stoi( tokens[0] );

			int index = -1;
			findMetaboliteInVec( tokens[ tokens.size()-1 ], index );
			assert( index != -1 );

			// add metabolite vertex
			MetaboliteGraph::vertex_descriptor metaVD;
			bool isExist = findMetaboliteInGraph( tokens[ tokens.size()-1 ], metaVD );
			//cerr << "metabolite = " << tokens[ tokens.size()-1 ] << endl;
            bool flag = false;
			flag = !isExist || ( isExist && isCloneMetaType( metaVD ) ) || ( stoichiometry > 1 );


#ifdef DEBUG
			if( name == "13dpg[c]" ){
				cerr << "reactant flag = " << flag << " isExist = " << isExist
					 << " stoichiometry = " << stoichiometry << " dul = " << _meta[ index ].metaDul
					 << " isCloneMetaType = " << isCloneMetaType( metaVD ) << " index = " << index << endl;
			}
#endif // DEBUG
			if( flag ){

				if( isExist == true ) _meta[ index ].isCloned = true;

				metaVD 	= add_vertex( _graph );
				_graph[ metaVD ].id 			= nVertices;
                _graph[ metaVD ].groupID	    = nVertices;
				_graph[ metaVD ].initID			= nVertices;
                _graph[ metaVD ].stoichiometry  = stoichiometry;
				//_graph[ metaVD ].isSelected		= false;
                //_graph[ metaVD ].isNeighbor	    = false;
                _graph[ metaVD ].isClonedPtr    = &_meta[ index ].isCloned;
				_graph[ metaVD ].type 			= "metabolite";
				_graph[ metaVD ].coordPtr		= new Coord2;
				_graph[ metaVD ].coordPtr->x() 	= rand() % DEFAULT_WIDTH - DEFAULT_WIDTH/2.0;
				_graph[ metaVD ].coordPtr->y() 	= rand() % DEFAULT_HEIGHT - DEFAULT_HEIGHT/2.0;
                _graph[ metaVD ].widthPtr	    = new double (10.0);
                _graph[ metaVD ].heightPtr		= new double (10.0);
				_graph[ metaVD ].metaPtr		= &_meta[ index ];
				_graph[ metaVD ].subsystems.insert( pair< string, Subdomain* >( _react[i].subsystem->name, _react[i].subsystem ) );
				_graph[ metaVD ].metaPtr->subsystems.insert( pair< string, Subdomain* >( _react[i].subsystem->name, _react[i].subsystem ) );
				_graph[ metaVD ].reactPtr		= NULL;
                _graph[ metaVD ].namePtr  		= &_meta[index].name;
                _graph[ metaVD ].namePixelWidthPtr  = &_meta[index].namePixelWidth;
                _graph[ metaVD ].namePixelHeightPtr = &_meta[index].namePixelHeight;

				//if( index == 64 ) cerr << "index(64) = " << name << endl;
				_meta[ index ].metaDul++;
				nVertices++;
			}
			else {
				//cerr << "type = " << _graph[ metaVD ].metaPtr->metaType << endl;
				_graph[ metaVD ].subsystems.insert( pair< string, Subdomain* >( _react[i].subsystem->name, _react[i].subsystem ) );
				_graph[ metaVD ].metaPtr->subsystems.insert( pair< string, Subdomain* >( _react[i].subsystem->name, _react[i].subsystem ) );
			}

			// add edges
			pair< MetaboliteGraph::edge_descriptor, unsigned int > foreE = add_edge( metaVD, reactVD, _graph );
			MetaboliteGraph::edge_descriptor foreED = foreE.first;
			_graph[ foreED ].id 		= nEdges;
			_graph[ foreED ].type		= "reactant";
			_graph[ foreED ].weight 	= stoichiometry;
			_graph[ foreED ].fore		= true;

			nEdges++;

			if( _react[i].reversible == "true" ){

				pair< MetaboliteGraph::edge_descriptor, unsigned int > backE = add_edge( reactVD, metaVD, _graph );
				MetaboliteGraph::edge_descriptor backED = backE.first;
				_graph[ backED ].id 	= nEdges;
				_graph[ backED ].type 	= "product";
				_graph[ backED ].weight = stoichiometry;
				_graph[ backED ].fore 	= false;

				nEdges++;
			}
		}

		// add products
		for( unsigned int j = 0; j < _react[i].productVec.size(); j++ ){

			string name = _react[i].productVec[j];

			string buf; // have a buffer string
			stringstream ss( name ); // insert the string into a stream

			// a vector to hold our words
			vector< string > tokens;
			int stoichiometry = 1;
			while (ss >> buf) tokens.push_back( buf );
			// cerr << " tokens.size() = " << tokens.size() << endl;
			assert( tokens.size() <= 2 );
			if( tokens.size() > 1 ) stoichiometry = stoi( tokens[0] );

			int index = -1;
			findMetaboliteInVec( tokens[ tokens.size()-1 ], index );
			assert( index != -1 );

			// add metabolite vertex
			MetaboliteGraph::vertex_descriptor metaVD;
			bool isExist = findMetaboliteInGraph( tokens[ tokens.size()-1 ], metaVD );
            bool flag = false;
			flag = !isExist || ( isExist && isCloneMetaType( metaVD ) ) || ( stoichiometry > 1 );

#ifdef DEBUG
            if( name == "13dpg[c]" ){
				cerr << "product name = " << name << " index = " << index << endl;
				cerr << "product flag = " << flag << " isExist = " << isExist
					 << " stoichiometry = " << stoichiometry << " dul = " << _meta[ index ].metaDul
					 << " isCloneMetaType = " << isCloneMetaType( metaVD ) << endl;
			}
#endif // DEBUG
			if( flag ){

            	if( isExist == true ) _meta[ index ].isCloned = true;

				metaVD 	= add_vertex( _graph );

				_graph[ metaVD ].id 			= nVertices;
                _graph[ metaVD ].groupID	    = nVertices;
				_graph[ metaVD ].initID			= nVertices;
                _graph[ metaVD ].stoichiometry  = stoichiometry;
				//_graph[ metaVD ].isSelected		= false;
                //_graph[ metaVD ].isNeighbor		= false;
				_graph[ metaVD ].isClonedPtr    = &_meta[ index ].isCloned;
  				_graph[ metaVD ].type 			= "metabolite";
				_graph[ metaVD ].coordPtr 		= new Coord2;
				_graph[ metaVD ].coordPtr->x() 	= rand() % DEFAULT_WIDTH - DEFAULT_WIDTH/2.0;
				_graph[ metaVD ].coordPtr->y() 	= rand() % DEFAULT_HEIGHT - DEFAULT_HEIGHT/2.0;
                _graph[ metaVD ].widthPtr	    = new double(10.0);
                _graph[ metaVD ].heightPtr		= new double(10.0);
                _graph[ metaVD ].metaPtr		= &_meta[ index ];
				_graph[ metaVD ].subsystems.insert( pair< string, Subdomain* >( _react[i].subsystem->name, _react[i].subsystem ) );
				_graph[ metaVD ].metaPtr->subsystems.insert( pair< string, Subdomain* >( _react[i].subsystem->name, _react[i].subsystem ) );
				_graph[ metaVD ].reactPtr		= NULL;
                _graph[ metaVD ].namePtr  		= &_meta[index].name;
                _graph[ metaVD ].namePixelWidthPtr  = &_meta[index].namePixelWidth;
                _graph[ metaVD ].namePixelHeightPtr = &_meta[index].namePixelHeight;

				_meta[ index ].metaDul++;
				nVertices++;
			}
			else {
				//cerr << "type = " << _graph[ metaVD ].metaPtr->metaType << endl;
				_graph[ metaVD ].subsystems.insert( pair< string, Subdomain* >( _react[i].subsystem->name, _react[i].subsystem ) );
				_graph[ metaVD ].metaPtr->subsystems.insert( pair< string, Subdomain* >( _react[i].subsystem->name, _react[i].subsystem ) );
			}

			// add edges
			pair< MetaboliteGraph::edge_descriptor, unsigned int > foreE = add_edge( reactVD, metaVD, _graph );
			MetaboliteGraph::edge_descriptor foreED = foreE.first;
			_graph[ foreED ].id 	= nEdges;
			_graph[ foreED ].type 	= "product";
			_graph[ foreED ].weight = stoichiometry;
			_graph[ foreED ].fore 	= true;

			nEdges++;

			if( _react[i].reversible == "true" ){

				pair< MetaboliteGraph::edge_descriptor, unsigned int > backE = add_edge( metaVD, reactVD, _graph );
				MetaboliteGraph::edge_descriptor backED = backE.first;
				_graph[ backED ].id 	= nEdges;
				_graph[ backED ].type 	= "reactant";
				_graph[ backED ].weight = stoichiometry;
				_graph[ backED ].fore 	= false;

				nEdges++;
			}
		}
	}

	// update vertex degree information
	unsigned int numMeta = 0;
	BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph ) {

		//MetaboliteGraph::degree_size_type      degreeOut         = out_degree( vd, _graph );
		//MetaboliteGraph::degree_size_type      degreeIn          = in_degree( vd, _graph );
		//_graph[ vd ].degree = degreeOut+degreeIn;

		unsigned int overlapped = inout_degree( vd );
		unsigned int totalDegree = (out_degree( vd, _graph ) + in_degree( vd, _graph )) - overlapped;
		_graph[ vd ].degree = totalDegree;

		//cerr << "vertexID = " << _graph[ vd ].id << " degreeOut = " << degreeOut << " degreeIn = " << degreeIn << endl;
		if( _graph[ vd ].type == "metabolite" &&  _graph[ vd ].metaPtr->metaType == "1_METABOLITE" )
			numMeta++;

#ifdef  DEBUG
		if( _graph[ vd ].type == "metabolite" ){
			cerr << "trial " << _graph[ vd ].metaPtr->subsystems.size() << " ?= " << _graph[ vd ].subsystems.size() << endl;
		}
#endif  // DEBUG
	}

	cerr << "nMetabolites = " << _meta.size() << endl;
	cerr << "nReactions = " << _react.size() << endl;
	cerr << "nHyperEtoE = " << _nHyperEtoE << endl;
	cerr << "noDuplicatedV = " << _nV.size()+_react.size() << endl;
    cerr << "density_before = " << 100.0*(double)_nHyperEtoE/(double)((_nV.size()+_react.size())*(_nV.size()+_react.size()-1)) << endl;
    cerr << "density_after = " << 100.0*(double)num_edges( _graph )/(double)(num_vertices( _graph )*(num_vertices( _graph )-1)) << endl;

	cerr << "num of vertices = " << num_vertices( _graph ) << endl;
	cerr << "num of edges = " << num_edges( _graph ) << endl;
	cerr << "num of 1_METABOLITE = " << numMeta << endl;

    // printGraph( _graph );
	genLayoutGraph();
}

//
//  Pathway::genLayoutGraph --    generate the undirected graph representation
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::genLayoutGraph( void )
{
    BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph ){

        ForceGraph::vertex_descriptor vdL = add_vertex( _layoutGraph );
        _layoutGraph[ vdL ].id              = _graph[ vd ].id;
        _layoutGraph[ vdL ].groupID		    = _graph[ vd ].groupID;

		_layoutGraph[ vdL ].coordPtr        = _graph[ vd ].coordPtr;
        _layoutGraph[ vdL ].prevCoordPtr 	= new Coord2( _graph[ vd ].coordPtr->x(), _graph[ vd ].coordPtr->y() );
        _layoutGraph[ vdL ].forcePtr 		= new Coord2( 0, 0 );
        _layoutGraph[ vdL ].placePtr 		= new Coord2( 0, 0 );
        _layoutGraph[ vdL ].shiftPtr 		= new Coord2( 0, 0 );

        _layoutGraph[ vdL ].widthPtr        = _graph[ vd ].widthPtr;
        _layoutGraph[ vdL ].heightPtr       = _graph[ vd ].heightPtr;
    }

	// add edges
    BGL_FORALL_EDGES( ed, _graph, MetaboliteGraph ){

        MetaboliteGraph::vertex_descriptor vdS = source( ed, _graph );
        MetaboliteGraph::vertex_descriptor vdT = target( ed, _graph );
        ForceGraph::vertex_descriptor vdSL = vertex( _graph[ vdS ].id, _layoutGraph );
        ForceGraph::vertex_descriptor vdTL = vertex( _graph[ vdT ].id, _layoutGraph );

		bool found = false;
        ForceGraph::edge_descriptor oldED;
		tie( oldED, found ) = edge( vdSL, vdTL, _layoutGraph );

		if( found == false ){
			pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdSL, vdTL, _layoutGraph );
            ForceGraph::edge_descriptor edL = foreE.first;
			_layoutGraph[ edL ].id      = _graph[ ed ].id;
			_layoutGraph[ edL ].weight  = _graph[ ed ].weight;
			_layoutGraph[ edL ].isFore  = true;
		}
		else{
			if( _graph[ vdS ].id == _layoutGraph[ vdTL ].id && _graph[ vdT ].id == _layoutGraph[ vdSL ].id ){
				_layoutGraph[ oldED ].isBack  = true;
			}
		}
    }

#ifdef DEBUG
	cerr << "printing the layout graph..." << endl;
    printGraph( _layoutGraph );
#endif // DEBUG
}


unsigned int Pathway::inout_degree( MetaboliteGraph::vertex_descriptor vd )
{
	unsigned int num = 0;

	MetaboliteGraph::out_edge_iterator eo, eo_end;
	for( tie( eo, eo_end ) = out_edges( vd, _graph ); eo != eo_end; ++eo ){

		MetaboliteGraph::edge_descriptor ed = *eo;
		MetaboliteGraph::vertex_descriptor vdS = source( ed, _graph );
		MetaboliteGraph::vertex_descriptor vdT = target( ed, _graph );

		bool found = false;
		MetaboliteGraph::edge_descriptor oldED;
		tie( oldED, found ) = edge( vdT, vdS, _graph );

		if( found == true ) num++;
	}

	return num;
}

//
//  Pathway::loadMetaFreq --    load txt file that record the number of metabolites occurred in the metabolic network
//
//  Inputs
//  filename: input filename
//
//  Outputs
//  none
//
void Pathway::loadMetaFreq( string filename )
{
	ifstream ifs( filename.c_str() );

	// cerr << "file name = " << filename << endl;

	if ( !ifs ) {
		cerr << "Cannot open the target file : " << filename << endl;
		assert( false );
	}

	while( !ifs.eof() ){

		string metabolite;
		getline( ifs, metabolite );

		string buf; // have a buffer string
		stringstream ss( metabolite ); // insert the string into a stream

		// a vector to hold our words
		vector< string > tokens;
		while (ss >> buf) tokens.push_back( buf );

		_metaFreq.insert( pair< string, unsigned int >( tokens[0], stoi( tokens[1] ) ) );
        //cerr << tokens[0] << " = " << tokens[1] << endl;
	}
	//cerr << "_metaFreq.size() = " << _metaFreq.size() << endl;
}

//
//  Pathway::loadMetaType --    load txt file that record metabolite types
//
//  Inputs
//  filename: input filename
//
//  Outputs
//  none
//
void Pathway::loadMetaType( string filename )
{
	ifstream ifs( filename.c_str() );

	if ( !ifs ) {
		cerr << "Cannot open the target file : " << filename << endl;
		assert( false );
	}

	while( !ifs.eof() ){

		string metabolite;
		getline( ifs, metabolite );

		string buf; // have a buffer string
		stringstream ss( metabolite ); // insert the string into a stream

		// a vector to hold our words
		vector< string > tokens;
		while (ss >> buf) tokens.push_back( buf );

		_metaType.insert( pair< string, string >( tokens[0], tokens[1] ) );
        //cerr << tokens[0] << " = " << tokens[1] << endl;
	}
	//cerr << "_metaType.size() = " << _metaType.size() << endl;
}

//
//  Pathway::loadPathway --    load xml file from a specified folder
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::loadPathway( void )
{
    clearPathway();

	// different member versions of find in the same order as above:
	//std::size_t found = _inputpath.find( "Ecoli_Palsson2011_iJO1366" );

#ifdef DEBUG
	// load the metabolite frequecy data
	cerr << "_inputpath = " << _inputpath << endl;
	cerr << "_fileFreq = " << _fileFreq << endl;
	cerr << "_fileType = " << _fileType << endl;
#endif // DEBUG

	//loadMetaFreq( (QDir::currentPath()+QString( "/../xml/" ) + QString( "ecoli_metabolite_frequency.txt" )).toStdString() );
	loadMetaFreq( _fileFreq );
	loadMetaType( _fileType );

	// init dupliMap
	for( map< string, unsigned int >::iterator it = _metaFreq.begin(); it != _metaFreq.end(); ++it ) {

		_dupliMap.insert( pair< string, int >( it->first, 0 ) );
	}

	QDir d( QString::fromStdString( _inputpath ) );
	const QList< QString > namelist = d.entryList();
	QList< QString >::const_iterator itL = namelist.begin();
	itL++;  // skip dir .
	itL++;  // skip dir ..

	// sort by ascii
	map< string, string > namemap;
	for( ; itL != namelist.end(); itL++ ) {

        string str = (*itL).toStdString();
		string substr = str.substr ( 0, str.length()-4 );
		namemap.insert( pair< string, string >( substr, substr ) );
	}

	map< string, string >::iterator itName = namemap.begin();
	for( ; itName != namemap.end(); itName++ ){

		// cerr << "name list = " << itName->first << endl;
		loadXml( itName->first + ".xml" );
	}
}


//
//  Pathway::initSubdomain --    initialize the size for subdomain
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::initSubdomain( void )
{
	double totalArea = DEFAULT_WIDTH * DEFAULT_HEIGHT;
	double totalNum = 0.0;
    double mag = 0.3;

	// find the sum of total area
	for( map< string, Subdomain * >::iterator it = _sub.begin(); it != _sub.end(); ++it ){
		totalNum += it->second->reactNum;
		//cerr << " _sub[ " << it->first << " ] = " << it->second.reactNum << endl;
        //cerr << " _subID[ " << it->first << " ] = " << it->second->id << endl;
	}

	// find the appropriate size for each Subdomain
	// cerr << "totalArea = " << totalArea << " totalNum = " << totalNum << endl;
    double minX = INFINITY, maxX = -INFINITY, minY = INFINITY, maxY = -INFINITY;
	for( map< string, Subdomain * >::iterator it = _sub.begin(); it != _sub.end(); ++it ){
		double subArea = totalArea * (double)it->second->reactNum / totalNum * mag;

		it->second->area = subArea;		// best size for drawing
		//it->second.width 	= DEFAULT_ASPECT * sqrt( subArea/DEFAULT_ASPECT );
        it->second->width 	= sqrt( subArea );
										// width df the domain
        //it->second.height 	= sqrt( subArea/DEFAULT_ASPECT );
		it->second->height 	= sqrt( subArea );
										// height of the domain
		it->second->ratio 	= DEFAULT_ASPECT;
										// aspect ratio of the subdomain

		it->second->center.x() 	= it->second->defaultCenter.x();
		// left-button point of the subdomain
		it->second->center.y() 	= -it->second->defaultCenter.y();
		// left-button point of the subdomain

		if( minX > it->second->center.x() ) minX = it->second->center.x();
		if( maxX < it->second->center.x() ) maxX = it->second->center.x();
		if( minY > it->second->center.y() ) minY = it->second->center.y();
		if( maxY < it->second->center.y() ) maxY = it->second->center.y();

#ifdef  DEBUG
        cerr << it->second->center;
        cerr << " _sub[ " << it->first << " ] = " << it->second->id << endl;
		cerr << " _sub[ " << it->first << " ] = " << subArea << endl;
		cerr << it->second.width << " ?= " << it->second.height << endl
			 << (double)DEFAULT_ASPECT * (double)sqrt( subArea/DEFAULT_ASPECT ) << endl
			 << DEFAULT_ASPECT << endl;
#endif  // DEBUG
	}
}

//
//  Pathway::layoutPathway --    layout the pathway
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::layoutPathway( void )
{
	randomGraphLayout( (DirectedBaseGraph &) _graph );
	fruchtermanGraphLayout( (DirectedBaseGraph &) _graph );
}

//
//  Pathway::genSubGraphs --    generate the sbusystem graph representation
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::genSubGraphs( void )
{
#ifdef  DEBUG
	BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph )
	{
		if( _graph[ vd ].type == "metabolite" ){
			int size = _graph[ vd ].subsystems.size();
			if( size > 1 ){
				cerr << " id = " << _graph[ vd ].id
				 	 << " subnum = " << _graph[ vd ].subsystems.size() << endl;
			}
		}
	}
#endif  // DEBUG

	map< string, Subdomain * >::iterator iter;
	for( iter = _sub.begin(); iter != _sub.end(); iter++ ){

		MetaboliteGraph g;
		map< unsigned int, unsigned int > idMap;
		unsigned int nVertices = 0, nEdges = 0;

		g[ graph_bundle ].centerPtr	= &iter->second->center;
		g[ graph_bundle ].widthPtr	= &iter->second->width;
		g[ graph_bundle ].heightPtr	= &iter->second->height;

		BGL_FORALL_EDGES( ed, _graph, MetaboliteGraph )
		{
			MetaboliteGraph::vertex_descriptor vdS = source( ed, _graph );
			MetaboliteGraph::vertex_descriptor vdT = target( ed, _graph );
			MetaboliteGraph::vertex_descriptor newSVD = NULL, newTVD = NULL;

			if( _graph[ vdS ].type == "metabolite" ){
				if( ( _graph[ vdS ].metaPtr != NULL ) && ( _graph[ vdS ].subsystems.size() == 1 ) &&
					( _graph[ vdS ].subsystems.begin()->first == iter->first ) ){

					map< unsigned int, unsigned int >::iterator iter = idMap.find( _graph[ vdS ].id );
					if( iter != idMap.end() ){
						newSVD = vertex( iter->second, g );
					}
					else{
						newSVD = add_vertex( g );
						g[ newSVD ].id              = nVertices;
                        g[ newSVD ].initID          = _graph[ vdS ].id;
						g[ newSVD ].namePtr  		= &_graph[ vdS ].metaPtr->name;
                        g[ newSVD ].namePixelWidthPtr  	= &_graph[ vdS ].metaPtr->namePixelWidth;
                        g[ newSVD ].namePixelHeightPtr  = &_graph[ vdS ].metaPtr->namePixelHeight;
						g[ newSVD ].groupID         = _graph[ vdS ].groupID;
						g[ newSVD ].type        	= _graph[ vdS ].type;
                        g[ newSVD ].coordPtr    	= _graph[ vdS ].coordPtr;
                        g[ newSVD ].widthPtr       	= _graph[ vdS ].widthPtr;
                        g[ newSVD ].heightPtr      	= _graph[ vdS ].heightPtr;
                        idMap.insert( pair< unsigned int, unsigned int >( _graph[ vdS ].id, nVertices ) );
						nVertices++;
					}
				}
			}
			else if( _graph[ vdS ].type == "reaction" ){

				if( _graph[ vdS ].reactPtr->subsystem->name == iter->first ){

					map< unsigned int, unsigned int >::iterator iter = idMap.find( _graph[ vdS ].id );
					if( iter != idMap.end() ){
						newSVD = vertex( iter->second, g );
					}
					else{
						newSVD = add_vertex( g );
                        g[ newSVD ].id              = nVertices;
                        g[ newSVD ].initID          = _graph[ vdS ].id;
						g[ newSVD ].namePtr  		= &_graph[ vdS ].reactPtr->abbr;
                        g[ newSVD ].namePixelWidthPtr  	= &_graph[ vdS ].reactPtr->namePixelWidth;
                        g[ newSVD ].namePixelHeightPtr  = &_graph[ vdS ].reactPtr->namePixelHeight;
						g[ newSVD ].groupID         = _graph[ vdS ].groupID;
                        g[ newSVD ].type        	= _graph[ vdS ].type;
						g[ newSVD ].coordPtr    	= _graph[ vdS ].coordPtr;
                        g[ newSVD ].widthPtr       	= _graph[ vdS ].widthPtr;
                        g[ newSVD ].heightPtr      	= _graph[ vdS ].heightPtr;
                        idMap.insert( pair< unsigned int, unsigned int >( _graph[ vdS ].id, nVertices ) );
						nVertices++;
					}
				}
			}

			if( _graph[ vdT ].type == "metabolite" ){
				if( ( _graph[ vdT ].metaPtr != NULL ) && ( _graph[ vdT ].subsystems.size() == 1 ) &&
					( _graph[ vdT ].subsystems.begin()->first == iter->first ) ){

  				    map< unsigned int, unsigned int >::iterator iter = idMap.find( _graph[ vdT ].id );
					if( iter != idMap.end() ){
						newTVD = vertex( iter->second, g );
					}
					else{
						newTVD = add_vertex( g );
                        g[ newTVD ].id              = nVertices;
                        g[ newTVD ].initID          = _graph[ vdT ].id;
						g[ newTVD ].namePtr  		= &_graph[ vdT ].metaPtr->name;
                        g[ newTVD ].namePixelWidthPtr  	= &_graph[ vdT ].metaPtr->namePixelWidth;
                        g[ newTVD ].namePixelHeightPtr  = &_graph[ vdT ].metaPtr->namePixelHeight;
						g[ newTVD ].groupID         = _graph[ vdT ].groupID;
						g[ newTVD ].type       		= _graph[ vdT ].type;
						g[ newTVD ].coordPtr    	= _graph[ vdT ].coordPtr;
                        g[ newTVD ].widthPtr       	= _graph[ vdT ].widthPtr;
                        g[ newTVD ].heightPtr      	= _graph[ vdT ].heightPtr;
                        idMap.insert( pair< unsigned int, unsigned int >( _graph[ vdT ].id, nVertices ) );
						nVertices++;
					}
				}
			}
			else if( _graph[ vdT ].type == "reaction" ){

				if( _graph[ vdT ].reactPtr->subsystem->name == iter->first ){

					map< unsigned int, unsigned int >::iterator iter = idMap.find( _graph[ vdT ].id );
					if( iter != idMap.end() ){
						newTVD = vertex( iter->second, g );
					}
					else{
						newTVD = add_vertex( g );
                        g[ newTVD ].id              = nVertices;
                        g[ newTVD ].initID          = _graph[ vdT ].id;
						g[ newTVD ].namePtr  		= &_graph[ vdT ].reactPtr->abbr;
                        g[ newTVD ].namePixelWidthPtr  	= &_graph[ vdT ].reactPtr->namePixelWidth;
                        g[ newTVD ].namePixelHeightPtr  = &_graph[ vdT ].reactPtr->namePixelHeight;
						g[ newTVD ].groupID         = _graph[ vdT ].groupID;
                        g[ newTVD ].type          	= _graph[ vdT ].type;
						g[ newTVD ].coordPtr    	= _graph[ vdT ].coordPtr;
                        g[ newTVD ].widthPtr       	= _graph[ vdT ].widthPtr;
                        g[ newTVD ].heightPtr      	= _graph[ vdT ].heightPtr;
                        idMap.insert( pair< unsigned int, unsigned int >( _graph[ vdT ].id, nVertices ) );
						nVertices++;
					}
				}
			}

			if( newSVD != NULL && newTVD != NULL ){

				//cerr << "name =" << iter->first << endl;

				pair< MetaboliteGraph::edge_descriptor, unsigned int > foreE = add_edge( newSVD, newTVD, g );
				MetaboliteGraph::edge_descriptor foreED = foreE.first;

				//g[ foreED ].id = nEdges;
				g[ foreED ].id = _graph[ ed ].id;
				//put( edge_myid, g, foreED, nEdges );
				nEdges++;
			}

		}
		//cerr << "_sub: " << iter->first << iter->second->name << endl;
		//printGraph( g );
		_subGraph.push_back( g );

#ifdef  DEBUG
		cerr << "center = " << *g[ graph_bundle ].centerPtr
			 << " w = " << g[ graph_bundle ].width
			 << " h = " << g[ graph_bundle ].height << endl;
#endif  // DEBUG
	}

	unsigned int index = 0;
	for( iter = _sub.begin(); iter != _sub.end(); iter++ ){

		_subGraph[ index ][ graph_bundle ].centerPtr = &iter->second->center;
		_subGraph[ index ][ graph_bundle ].widthPtr = &iter->second->width;
		_subGraph[ index ][ graph_bundle ].heightPtr = &iter->second->height;
		index++;
	}

#ifdef DEBUG
    for( unsigned int i = 0; i < _sub.size(); i++ ) {

        BGL_FORALL_VERTICES( vd, _subGraph[i], MetaboliteGraph ){
            cerr << "id = " << _subGraph[i][vd].id
                 << " w = " << *_subGraph[i][vd].namePixelWidthPtr
                 << " h = " << *_subGraph[i][vd].namePixelHeightPtr << endl;
        }
    }
    BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph )
    {
        // out edge
        MetaboliteGraph::out_edge_iterator eo, eo_end;
        if( _graph[ vd ].id == 45 ){
            cerr << "45: ";
            for( tie( eo, eo_end ) = out_edges( vd, _graph ); eo != eo_end; ++eo ){

                MetaboliteGraph::edge_descriptor ed = *eo;
                MetaboliteGraph::vertex_descriptor vdT = target( ed, _graph );
                cerr << _graph[ vdT ].id << " ";
            }
            cerr << endl;
        }
    }
#endif // DEBUG
	genLayoutSubGraphs();
}

//
//  Pathway::genLayoutSubGraphs --    generate the undirected sbusystem graph representation
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::genLayoutSubGraphs( void )
{
	_layoutSubGraph.clear();
	_layoutSubGraph.resize( _subGraph.size() );
	map< unsigned int, unsigned int > indexMap;

	cerr << "subG size = " << _subGraph.size() << endl;
	for( unsigned int i = 0; i < _subGraph.size(); i++ ){

		unsigned int nv = 0;
		BGL_FORALL_VERTICES( vd, _subGraph[i], MetaboliteGraph ){

            ForceGraph::vertex_descriptor vdL = add_vertex( _layoutSubGraph[i] );
			_layoutSubGraph[i][ vdL ].id            = nv;
			_layoutSubGraph[i][ vdL ].namePtr 		= _subGraph[i][ vd ].namePtr;
            _layoutSubGraph[i][ vdL ].namePixelWidthPtr     = _subGraph[i][ vd ].namePixelWidthPtr;
            _layoutSubGraph[i][ vdL ].namePixelHeightPtr    = _subGraph[i][ vd ].namePixelHeightPtr;
			_layoutSubGraph[i][ vdL ].groupID       = _subGraph[i][ vd ].groupID;

			_layoutSubGraph[i][ vdL ].coordPtr      = _subGraph[i][ vd ].coordPtr;
			_layoutSubGraph[i][ vdL ].prevCoordPtr 	= new Coord2( _subGraph[i][ vd ].coordPtr->x(), _subGraph[i][ vd ].coordPtr->y() );
			_layoutSubGraph[i][ vdL ].forcePtr 		= new Coord2( 0, 0 );
			_layoutSubGraph[i][ vdL ].placePtr 		= new Coord2( 0, 0 );
			_layoutSubGraph[i][ vdL ].shiftPtr 		= new Coord2( 0, 0 );

			_layoutSubGraph[i][ vdL ].widthPtr      = _subGraph[i][ vd ].widthPtr;
			_layoutSubGraph[i][ vdL ].heightPtr     = _subGraph[i][ vd ].heightPtr;
			indexMap.insert( pair< unsigned int, unsigned int >( _subGraph[i][ vd ].groupID, nv ) );
			nv++;
		}

		BGL_FORALL_EDGES( ed, _subGraph[i], MetaboliteGraph ){

			MetaboliteGraph::vertex_descriptor vdS = source( ed, _subGraph[i] );
			MetaboliteGraph::vertex_descriptor vdT = target( ed, _subGraph[i] );
			unsigned int idS = indexMap.find( _subGraph[i][ vdS ].groupID )->second;
			unsigned int idT = indexMap.find( _subGraph[i][ vdT ].groupID )->second;
            ForceGraph::vertex_descriptor vdSL = vertex( idS, _layoutSubGraph[i] );
            ForceGraph::vertex_descriptor vdTL = vertex( idT, _layoutSubGraph[i] );

			//cerr << "idS = " << idS << " idT = " << idT << endl;
	 		bool found = false;
            ForceGraph::edge_descriptor oldED;
			tie( oldED, found ) = edge( vdSL, vdTL, _layoutSubGraph[i] );

			if( found == false ){

				pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdSL, vdTL, _layoutSubGraph[i] );
                ForceGraph::edge_descriptor edL = foreE.first;
				_layoutSubGraph[i][ edL ].id 		= _subGraph[i][ ed ].id;
				_layoutSubGraph[i][ edL ].weight 	= _subGraph[i][ ed ].weight;
				_layoutSubGraph[i][ edL ].isFore  	= true;
				_layoutSubGraph[i][ edL ].isBack  	= false;
            }
            else{
				_layoutSubGraph[i][ oldED ].isBack  = true;
            }
		}

#ifdef  DEBUG
		printGraph( _layoutSubGraph[i] );
#endif // DEBUG
	}
}


//
//  Pathway::clearPathway --    clear all the pathway dataset
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::clearPathway( void )
{
	_metaFreq.clear();
	_metaType.clear();
    _sub.clear();
    _meta.clear();
    _react.clear();
	_edgeW.clear();

	// clear graph
	clearGraph( _graph );
	clearGraph( _layoutGraph );
	for( unsigned int i = 0; i < _subGraph.size(); i++ ){
		clearGraph( _subGraph[i] );
	}
	for( unsigned int i = 0; i < _layoutSubGraph.size(); i++ ){
		clearGraph( _layoutSubGraph[i] );
	}
    _subGraph.clear();
    _layoutSubGraph.clear();

	for( map< string, Subdomain * >:: iterator it = _sub.begin(); it != _sub.end(); it++ ){
		it->second->treeLeaves.clear();
	}
}

//------------------------------------------------------------------------------
//      I/O
//------------------------------------------------------------------------------
//
//  operator << --      output
//
//  Inputs
//      stream  : output stream
//      obj     : object of this class
//
//  Outputs
//      output stream
//
ostream & operator << ( ostream & stream, const Pathway & obj )
{
    MetaboliteGraph _graph = obj.g();

	map< string, unsigned int > _metaFreq = obj.metafreq();
	map< string, Subdomain * > _sub = obj.subsys();

	stream << "-- pathway --" << endl;

#ifdef  SKIP
	stream << " _metaFreq" << endl;
	for( map< string, unsigned int >::iterator it = _metaFreq.begin(); it != _metaFreq.end(); ++it ){
		stream << " metafreq[ " << it->first << " ] = " << it->second << endl;
	}
#endif  // SKIP
	stream << " subsys" << endl;
	for( map< string, Subdomain * >::iterator it = _sub.begin(); it != _sub.end(); ++it ){
		stream << " _sub[ " << it->first << " ] = " << it->second->reactNum << endl;
		stream << " _sub[ " << it->first << " ] = " << it->second->width << endl;
		stream << " _sub[ " << it->first << " ] = " << it->second->height << endl;
	}

	stream << "-- graph --" << endl;
	printGraph( _graph );

    return stream;
}

//
//  operator >> --      input
//
//  Inputs
//      stream  : input stream
//      obj     : object of this class
//
//  Outputs
//      input stream
//
istream & operator >> ( istream & stream, Pathway & obj )
{
    // do nothing
    return stream;
}

// end of header file
// Do not add any stuff under this line.
