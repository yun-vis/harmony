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
#include <QtWidgets/QDialog>
#include <QtWidgets/QFileDialog>

#include "base/Pathway.h"
#include "base/Color.h"
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
    _widthPtr = new double (0.0);
    _heightPtr = new double (0.0);
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
void Pathway::init( string pathIn, string pathOut,
					string fileFreq, string fileType, int clonedThreshold )
{
	_inputpath = pathIn;
	_outputpath = pathOut;
    _fileFreq = fileFreq;
    _fileType = fileType;
    if( clonedThreshold == 0 ){
		_isCloneByThreshold = false;
		_threshold = 0;
    }
    else{
		_isCloneByThreshold = true;
		_threshold = clonedThreshold;
    }
	_nHyperEtoE = 0;
}

//
//  Pathway::findVertex --    find a vertex
//
//  Inputs
//  node
//
//  Outputs
//  node
//
MetaboliteGraph::vertex_descriptor Pathway::findVertex( unsigned int id, MetaboliteGraph &g )
{
    MetaboliteGraph::vertex_descriptor vdNull = NULL;

	BGL_FORALL_VERTICES( vd, g, MetaboliteGraph )
	{
		//cerr << g[vd].id << " == " << id << endl;
		if( g[vd].id == id ) return vd;
	}

	return vdNull;
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
		Coord2 &centroid = itP->second.centroid();

        // initialization
        BGL_FORALL_VERTICES( vd, _layoutSubGraph[i], ForceGraph ) {

			if( maxX < _layoutSubGraph[i][vd].coordPtr->x() ) maxX = _layoutSubGraph[i][vd].coordPtr->x();
			if( minX > _layoutSubGraph[i][vd].coordPtr->x() ) minX = _layoutSubGraph[i][vd].coordPtr->x();
			if( maxY < _layoutSubGraph[i][vd].coordPtr->y() ) maxY = _layoutSubGraph[i][vd].coordPtr->y();
			if( minY > _layoutSubGraph[i][vd].coordPtr->y() ) minY = _layoutSubGraph[i][vd].coordPtr->y();
        }

#ifdef DEBUG
		cerr << "centroid = " << centroid;
		cerr << "minX = " << minX << " maxX = " << maxX << " minY = " << minY << " maxY = " << maxY << endl;
		cerr << "c = " << center;
#endif // DEBUG

		// normalization
        double scale = 0.05;
        double half_w = scale*( maxX-minX );
        double half_h = scale*( maxY-minY );
		BGL_FORALL_VERTICES( vd, _layoutSubGraph[i], ForceGraph ) {

			//_layoutSubGraph[i][vd].coordPtr->x() = centroid.x();
			//_layoutSubGraph[i][vd].coordPtr->y() = centroid.y();
			_layoutSubGraph[i][vd].coordPtr->x() = half_w * (_layoutSubGraph[i][vd].coordPtr->x() - minX)/( maxX - minX )+centroid.x() - scale * half_w;
			_layoutSubGraph[i][vd].coordPtr->y() = half_h * (_layoutSubGraph[i][vd].coordPtr->y() - minY)/( maxY - minY )+centroid.y() - scale * half_h;
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
		( _graph[ metaVD ].metaPtr->metaType == "2_NUCLEOTIDE" ) ||
		//( _graph[ metaVD ].metaPtr->metaType == "unknown" ) ||
		( _graph[ metaVD ].metaPtr->metaType == "1_METABOLITE" )
		){

		isClone = false;
	}

	//return false;
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
	computeIdealAreaOfSubdomain();
	genDependencyGraph();
    //randomGraphLayout( (DirectedBaseGraph &) _graph );
    //fruchtermanGraphLayout( (DirectedBaseGraph &) _graph );
    // normalization();
    //initLayout();
}


//
//  Pathway::exportEdges --    export graph edges
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::exportEdges( void )
{
    // graph
	ofstream ofs( "../data/biological_yun.txt" );
	ForceGraph g;

	// copy graphs
    unsigned int index = 0;
    for( unsigned int i = 0; i < _subGraph.size(); i++ ){

        BGL_FORALL_VERTICES( vd, _subGraph[i], MetaboliteGraph ) {

            ForceGraph::vertex_descriptor vdNew = add_vertex( g );
            g[vdNew].id = _subGraph[i][vd].id+index;
            g[vdNew].label = i;
        }
        index += num_vertices( _subGraph[i] );
    }

    index = 0;
    for( unsigned int i = 0; i < _layoutSubGraph.size(); i++ ){
        BGL_FORALL_EDGES( ed, _layoutSubGraph[i], ForceGraph ) {

            ForceGraph::vertex_descriptor vdS = source( ed, _layoutSubGraph[i] );
            ForceGraph::vertex_descriptor vdT = target( ed, _layoutSubGraph[i] );
            unsigned int idS = _layoutSubGraph[i][vdS].id + index;
            unsigned int idT = _layoutSubGraph[i][vdT].id + index;
            ForceGraph::vertex_descriptor vdNS = vertex( idS, g );
            ForceGraph::vertex_descriptor vdNT = vertex( idT, g );

            //cerr << "idS = " << g[vdNS].id << " idT = " << g[vdNT].id << endl;

            pair< ForceGraph::edge_descriptor, unsigned int > foreE = add_edge( vdNS, vdNT, g );
            ForceGraph::edge_descriptor foreED = foreE.first;
            g[ foreED ].id 		= _layoutSubGraph[i][ed].id+index;
            //cerr << "eid = " << g[ foreED ].id << endl;
        }
        index += num_vertices( _layoutSubGraph[i] );
    }

    BGL_FORALL_EDGES( ed, g, ForceGraph ) {

		ForceGraph::vertex_descriptor vdS = source( ed, g );
		ForceGraph::vertex_descriptor vdT = target( ed, g );

		cerr << "eid = " << g[ed].id << endl;
        cerr << g[vdS].id << " " << g[vdT].id << endl;
		ofs << g[vdS].id << " " << g[vdT].id << endl;
	}

	ofs.close();

	// distance matrix
	ofstream ofsm( "../data/matrix_biological_yun.txt" );

	index = 0;
    BGL_FORALL_VERTICES( vdO, g, ForceGraph ) {

        BGL_FORALL_VERTICES( vdI, g, ForceGraph ) {

                bool foundF = false, foundB = false;
                ForceGraph::edge_descriptor oldED;
                tie( oldED, foundF ) = edge( vdO, vdI, g );
                tie( oldED, foundB ) = edge( vdI, vdO, g );

                if( foundF || foundB ) ofsm << "1,";
                else ofsm << "10000,";
        }
        ofsm << endl;
    }

	ofsm.close();

    // distance matrix
    ofstream ofsc( "../data/biological_yun_communities.txt" );

    BGL_FORALL_VERTICES( vd, g, ForceGraph ) {

        ofsc << g[vd].id << "\t" << g[vd].label << endl;
    }

    ofsc.close();
}

void Pathway::pickColor( COLORTYPE colorType, unsigned int id, vector< double > &rgb )
{
    switch( colorType ){

        case COLOR_PREDEFINED:
        {
            rgb.resize( 3 );

            map< string, Subdomain * >::iterator it = _sub.begin();
            advance( it, id );

            QColor color;
            color.setNamedColor( QString::fromStdString( it->second->defaultColor ) );
            rgb[ 0 ] = color.redF();
            rgb[ 1 ] = color.greenF();
            rgb[ 2 ] = color.blueF();

        }
            break;
        case COLOR_MONOTONE:
            pickMonotoneColor( id, rgb );
            break;
        case COLOR_PASTEL:
            pickPastelColor( id, rgb );
            break;
        case COLOR_BREWER:
            pickBrewerColor( id, rgb );
            break;
        default:
            cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
            break;
    }
}

void Pathway::loadDot( UndirectedPropertyGraph &graph, string filename )
{
    VertexIndexMap              vertexIndex     = get( vertex_index, graph );
    VertexPosMap                vertexPos       = get( vertex_mypos, graph );
	VertexLabelMap              vertexLabel     = get( vertex_mylabel, graph );
	VertexColorMap              vertexColor     = get( vertex_mycolor, graph );
    VertexXMap                  vertexX         = get( vertex_myx, graph );
    VertexYMap                  vertexY         = get( vertex_myy, graph );
    EdgeIndexMap                edgeIndex       = get( edge_index, graph );
    EdgeWeightMap               edgeWeight      = get( edge_weight, graph );

    dynamic_properties dp( boost::ignore_other_properties );

    //dp.property( "fontsize" , vertexIndex );
    dp.property( "label",  vertexLabel );
    dp.property( "pos",  vertexPos );
	dp.property( "clustercolor",  vertexColor );
    dp.property( "weight", edgeWeight );

    ifstream ifs( filename );
    read_graphviz( ifs, graph, dp );
    boost::print_graph( graph );

	BGL_FORALL_VERTICES( vd, graph, UndirectedPropertyGraph ) {

	    string str = vertexPos[vd];
	    vector< double > vect;

        stringstream ss( str );

        double v;
        while( ss >> v ) {
            vect.push_back( v );

            if ( ss.peek() == ',' )
                ss.ignore();
        }

        //for ( int i=0; i< vect.size(); i++)
        //    cout << vect.at(i) << endl;

		vertexX[vd] = vect.at(0);
        vertexY[vd] = vect.at(1);
#ifdef DEBUG
        cerr << "id = " << vertexIndex[vd] << ", label = " << vertexLabel[vd]
             << ", color = " << vertexColor[vd]
             << ", pos = " << vertexPos[vd]
             << ", x = " << vertexX[vd]
             << ", y = " << vertexY[vd]
             << endl;
#endif // DEBUG
	}

	map< string, unsigned int > colormap;
	BGL_FORALL_VERTICES( vd, graph, UndirectedPropertyGraph ) {

		map< string, unsigned int >::iterator it = colormap.find( vertexColor[vd] );
		if( it == colormap.end() ){
			colormap.insert( pair< string, unsigned int >( vertexColor[vd], 1 ) );
		}
		else{
			it->second++;
		}
	}
}

void Pathway::exportDot( void )
{
	// graph
	ofstream ofs( "../dot/pathway.txt" );
	ofs << "graph {" << endl;

	unsigned int index = 0;
	for( unsigned int i = 0; i < _subGraph.size(); i++ ){

        vector< double > rgb;
        unsigned int id = i;

        pickBrewerColor( ( id )%_subGraph.size(), rgb );
        QColor color( rgb[0]*255, rgb[1]*255, rgb[2]*255, 100 );
        // cerr << "nV = " << num_vertices( _subGraph[i] ) << endl;
		BGL_FORALL_VERTICES( vd, _subGraph[i], MetaboliteGraph ) {

            MetaboliteGraph::vertex_descriptor initVD = vertex( _subGraph[i][vd].initID, _graph );
            if( *_graph[initVD].isClonedPtr == true ){
                string str = *_subGraph[i][vd].namePtr;
                //str.replace( str.end()-1, str.end(),1, 'l' );
                //str.replace( str.end()-3, str.end()-2,1, 'l' );
                ofs << " \"" << _subGraph[i][vd].id+index
                    << "\" [cluster=" << i+1
                    << ", clustercolor=\"" << color.name( QColor::HexRgb ).toStdString()
                    << "\", label=\"" << str
//                    << ",label=\"" << *_subGraph[i][vd].namePtr
                    << "\", color=\"" << "#48d1cc"
                        << "\", style=\"filled,rounded"
                        << "\"];"<< endl;
		    }
            else if( _subGraph[i][vd].isAlias == true )
            {
                string str = *_subGraph[i][vd].namePtr;
                //str.replace( str.end()-1, str.end(),1, 'l' );
                //str.replace( str.end()-3, str.end()-2,1, 'l' );
                ofs << " \"" << _subGraph[i][vd].id+index
                    << "\" [cluster=" << i+1
                    << ", clustercolor=\"" << color.name( QColor::HexRgb ).toStdString()
                    << "\", label=\"" << str
//                    << ",label=\"" << *_subGraph[i][vd].namePtr
                    << "\", color=\"" << "#ffc0cb"
                        << "\", style=\"filled,rounded"
                        << "\"];"<< endl;
            }
            else{
                string str = *_subGraph[i][vd].namePtr;
                //str.replace( str.end()-1, str.end(),1, 'l' );
                //str.replace( str.end()-3, str.end()-2,1, 'l' );
                if( _graph[initVD].type == "metabolite" ){
                    ofs << " \"" << _subGraph[i][vd].id+index
                        << "\" [cluster=" << i+1
                        << ", clustercolor=\"" << color.name( QColor::HexRgb ).toStdString()
                        << "\", label=\"" << str
//                    << ",label=\"" << *_subGraph[i][vd].namePtr
                        << "\", color=\"#ffffff"
						<< "\", fontcolor=\"#666666"
                        << "\", style=\"filled,rounded"
                        << "\"];"<< endl;
                }
                else{
                    ofs << " \"" << _subGraph[i][vd].id+index
                        << "\" [cluster=" << i+1
                        << ", clustercolor=\"" << color.name( QColor::HexRgb ).toStdString()
                        << "\", label=\"" << *_subGraph[i][vd].namePtr
                        << "\", color=\"#ffffff"
                        << "\", style=\"filled"
                        << "\"];"<< endl;
                }
            }
		}
		index += num_vertices( _layoutSubGraph[i] );
	}

	index = 0;
	for( unsigned int i = 0; i < _layoutSubGraph.size(); i++ ){
		BGL_FORALL_EDGES( ed, _layoutSubGraph[i], ForceGraph ) {

			ForceGraph::vertex_descriptor vdS = source( ed, _layoutSubGraph[i] );
			ForceGraph::vertex_descriptor vdT = target( ed, _layoutSubGraph[i] );

			ofs << " \"" << _layoutSubGraph[i][vdS].id+index << "\" -- "
				<< "\"" << _layoutSubGraph[i][vdT].id+index
			    << "\" [color=\"#000000\"];" << endl;
//                << "\";" << endl;
		}
		index += num_vertices( _layoutSubGraph[i] );
	}

	ofs << "}" << endl;
	ofs.close();
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
	_graph[ graph_bundle ].widthPtr		= new double( *_widthPtr );
	_graph[ graph_bundle ].heightPtr    = new double( *_heightPtr );

	for( unsigned int i = 0; i < _react.size(); i++ ){

		// add reaction vertex
		MetaboliteGraph::vertex_descriptor reactVD 		= add_vertex( _graph );

		_graph[ reactVD ].id 			= nVertices;
		_graph[ reactVD ].groupID		= nVertices;
		_graph[ reactVD ].componentID	= nVertices;
        _graph[ reactVD ].initID		= nVertices;
        //_graph[ reactVD ].isSelected	= false;
        //_graph[ reactVD ].isNeighbor	= false;
        _graph[ reactVD ].isClonedPtr   = new bool( false );
        _graph[ reactVD ].isSelectedPtr = new bool( false );
        _graph[ reactVD ].selectedIDPtr = new int( -1 );
        _graph[ reactVD ].isAlias       = false;
        _graph[ reactVD ].type 			= "reaction";
		_graph[ reactVD ].coordPtr		= new Coord2;
		_graph[ reactVD ].coordPtr->x() = rand() % (int)*_widthPtr - *_widthPtr/2.0;
		_graph[ reactVD ].coordPtr->y() = rand() % (int)*_heightPtr - *_heightPtr/2.0;
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
			if( _isCloneByThreshold == true ){
				flag = !isExist || ( isExist && ( _graph[ metaVD ].metaPtr->freq > _threshold ) );
			}
			else{
				flag = !isExist || ( isExist && isCloneMetaType( metaVD ) ) || ( stoichiometry > 1 );
			}


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
				_graph[ metaVD ].componentID    = nVertices;
				_graph[ metaVD ].initID			= nVertices;
                _graph[ metaVD ].stoichiometry  = stoichiometry;
				//_graph[ metaVD ].isSelected		= false;
                //_graph[ metaVD ].isNeighbor	    = false;
                _graph[ metaVD ].isClonedPtr    = &_meta[ index ].isCloned;
                _graph[ metaVD ].isSelectedPtr  = new bool( false );
                _graph[ metaVD ].selectedIDPtr  = new int( -1 );
                _graph[ metaVD ].isAlias        = false;
				_graph[ metaVD ].type 			= "metabolite";
				_graph[ metaVD ].coordPtr		= new Coord2;
				_graph[ metaVD ].coordPtr->x() 	= rand() % (int)*_widthPtr - *_widthPtr/2.0;
				_graph[ metaVD ].coordPtr->y() 	= rand() % (int)*_heightPtr - *_heightPtr/2.0;
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
			if( _isCloneByThreshold == true ){
				flag = !isExist || ( isExist && ( _graph[ metaVD ].metaPtr->freq > _threshold ) );
			}
			else{
				flag = !isExist || ( isExist && isCloneMetaType( metaVD ) ) || ( stoichiometry > 1 );
			}
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
				_graph[ metaVD ].componentID    = nVertices;
				_graph[ metaVD ].initID			= nVertices;
                _graph[ metaVD ].stoichiometry  = stoichiometry;
				//_graph[ metaVD ].isSelected		= false;
                //_graph[ metaVD ].isNeighbor		= false;
				_graph[ metaVD ].isClonedPtr    = &_meta[ index ].isCloned;
                _graph[ metaVD ].isSelectedPtr  = new bool( false );
                _graph[ metaVD ].selectedIDPtr  = new int( -1 );
                _graph[ metaVD ].isAlias        = false;
  				_graph[ metaVD ].type 			= "metabolite";
				_graph[ metaVD ].coordPtr 		= new Coord2;
				_graph[ metaVD ].coordPtr->x() 	= rand() % (int)*_widthPtr - *_widthPtr/2.0;
				_graph[ metaVD ].coordPtr->y() 	= rand() % (int)*_heightPtr - *_heightPtr/2.0;
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

#ifdef DEBUG
    //printGraph( _graph );
#endif // DEBUG

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
		_layoutGraph[ vdL ].initID          = _graph[ vd ].initID;
        _layoutGraph[ vdL ].groupID		    = _graph[ vd ].groupID;
		_layoutGraph[ vdL ].componentID   	= _graph[ vd ].componentID;

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
    //printGraph( _layoutGraph );
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

	//cerr << "file name = " << filename << endl;

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

        //cerr << tokens[0] << " = " << tokens[1] << endl;
		_metaFreq.insert( pair< string, unsigned int >( tokens[0], stoi( tokens[1] ) ) );
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
	//cerr << "inputpath = " << _inputpath << endl;
	for( ; itL != namelist.end(); itL++ ) {

        string str = itL->toStdString();
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
//  Output
//  none
//
void Pathway::initSubdomain( void )
{
	double totalArea = *_widthPtr * *_heightPtr;
	double totalNum = 0.0;
    double mag = 0.3;

    cerr << "*_widthPtr = " << *_widthPtr << " *_heightPtr = " << *_heightPtr << endl;
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
//  Pathway::computeIdealAreaOfSubdomain --    compute ideal area of subdomain
//
//  Inputs
//  node
//
//  Output
//  none
//
void Pathway::computeIdealAreaOfSubdomain( void ) {

	// initialization
	for( map< string, Subdomain * >::iterator it = _sub.begin(); it != _sub.end(); ++it ) {
		it->second->idealArea = 0.0;
	}

	// create edge object from the spanning tree and add it to the scene
	for( unsigned int i = 0; i < _layoutSubGraph.size(); i++ ){

		map< string, Subdomain * >::iterator it = _sub.begin();
		advance( it, i );
		BGL_FORALL_VERTICES( vd, _layoutSubGraph[i], ForceGraph )
		{
#ifdef DEBUG
			cerr << "gid = " << _layoutSubGraph[i][vd].groupID
				 << " cid = " << _layoutSubGraph[i][vd].componentID << endl;
			cerr << "textW = " << *_layoutSubGraph[i][vd].namePixelWidthPtr
				 << " textH = " << *_layoutSubGraph[i][vd].namePixelHeightPtr << endl;
#endif // DEBUG
			double textW = *_layoutSubGraph[i][vd].namePixelWidthPtr;
			double textH = *_layoutSubGraph[i][vd].namePixelHeightPtr;
			it->second->idealArea += textW * textH;
		}
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
	randomGraphLayout( (DirectedBaseGraph &) _graph, *_widthPtr, *_heightPtr );
	fruchtermanGraphLayout( (DirectedBaseGraph &) _graph, *_widthPtr, *_heightPtr );
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

	unsigned int totalV = num_vertices( _graph );

	map< string, Subdomain * >::iterator iter;
	for( iter = _sub.begin(); iter != _sub.end(); iter++ ){

		MetaboliteGraph g;
		map< unsigned int, unsigned int > idMap;
		unsigned int nVertices = 0, nEdges = 0, nDummies = 0;

		g[ graph_bundle ].centerPtr	= &iter->second->center;
		g[ graph_bundle ].widthPtr	= &iter->second->width;
		g[ graph_bundle ].heightPtr	= &iter->second->height;

		BGL_FORALL_EDGES( ed, _graph, MetaboliteGraph )
		{
			MetaboliteGraph::vertex_descriptor vdS = source( ed, _graph );
			MetaboliteGraph::vertex_descriptor vdT = target( ed, _graph );
			MetaboliteGraph::vertex_descriptor newSVD = NULL, newTVD = NULL;

			// cerr << "e( " << *_graph[vdS].namePtr << ", " << *_graph[vdT].namePtr << " )" << endl;
			// cerr << "e( " << _graph[vdS].type << ", " << _graph[vdT].type << " )" << endl;
			if( _graph[ vdS ].type == "metabolite" ){
				if( ( _graph[ vdS ].metaPtr != NULL ) && ( _graph[ vdS ].subsystems.size() == 1 ) &&
					( _graph[ vdS ].subsystems.begin()->first == iter->first ) ){

					map< unsigned int, unsigned int >::iterator iter = idMap.find( _graph[ vdS ].id );
					if( iter != idMap.end() ){
						newSVD = findVertex( iter->second, g );
						//newSVD = vertex( iter->second, g );
					}
					else{
						newSVD = add_vertex( g );
						g[ newSVD ].id              = nVertices;
                        g[ newSVD ].initID          = _graph[ vdS ].id;
						g[ newSVD ].namePtr  		= &_graph[ vdS ].metaPtr->name;
                        g[ newSVD ].namePixelWidthPtr  	= &_graph[ vdS ].metaPtr->namePixelWidth;
                        g[ newSVD ].namePixelHeightPtr  = &_graph[ vdS ].metaPtr->namePixelHeight;
						g[ newSVD ].groupID         = _graph[ vdS ].groupID;
                        g[ newSVD ].isClonedPtr    	= _graph[ vdS ].isClonedPtr;
                        g[ newSVD ].isSelectedPtr   = _graph[ vdS ].isSelectedPtr;
                        g[ newSVD ].selectedIDPtr   = _graph[ vdS ].selectedIDPtr;
                        g[ newSVD ].isAlias    	    = false;
						g[ newSVD ].type        	= _graph[ vdS ].type;
                        g[ newSVD ].coordPtr    	= _graph[ vdS ].coordPtr;
                        g[ newSVD ].widthPtr       	= _graph[ vdS ].widthPtr;
                        g[ newSVD ].heightPtr      	= _graph[ vdS ].heightPtr;
                        idMap.insert( pair< unsigned int, unsigned int >( _graph[ vdS ].id, nVertices ) );
						nVertices++;
					}

					//cerr << *g[ newSVD ].namePtr << ", ";
				}
                else if( ( _graph[ vdS ].metaPtr != NULL ) && ( _graph[ vdS ].subsystems.size() > 1 ) ){

					map< string, Subdomain * >::iterator itS;
					for( itS = _graph[ vdS ].subsystems.begin(); itS != _graph[ vdS ].subsystems.end(); itS++ ){
						if( ( itS->first == iter->first ) ){

							map< unsigned int, unsigned int >::iterator iter = idMap.find( _graph[ vdS ].id );
							if( iter != idMap.end() ){
								//cerr << "HERE1 " << iter->second << endl;
								newSVD = findVertex( iter->second, g );
							}
							else{
								newSVD = add_vertex( g );
								g[ newSVD ].id              = totalV + nDummies;
								//g[ newSVD ].id              = nVertices;
								g[ newSVD ].initID          = _graph[ vdS ].id;
								g[ newSVD ].namePtr  		= &_graph[ vdS ].metaPtr->name;
								g[ newSVD ].namePixelWidthPtr  	= &_graph[ vdS ].metaPtr->namePixelWidth;
								g[ newSVD ].namePixelHeightPtr  = &_graph[ vdS ].metaPtr->namePixelHeight;
								g[ newSVD ].groupID         = _graph[ vdS ].groupID;
								g[ newSVD ].isClonedPtr    	= _graph[ vdS ].isClonedPtr;
                                g[ newSVD ].isSelectedPtr   = _graph[ vdS ].isSelectedPtr;
                                g[ newSVD ].selectedIDPtr   = _graph[ vdS ].selectedIDPtr;
								g[ newSVD ].isAlias    	    = true;
								g[ newSVD ].type        	= _graph[ vdS ].type;
								g[ newSVD ].coordPtr    	= new Coord2( _graph[ vdS ].coordPtr->x(), _graph[ vdS ].coordPtr->x() );
								g[ newSVD ].widthPtr       	= _graph[ vdS ].widthPtr;
								g[ newSVD ].heightPtr      	= _graph[ vdS ].heightPtr;
								idMap.insert( pair< unsigned int, unsigned int >( _graph[ vdS ].id, g[ newSVD ].id ) );
								//nVertices++;
								nDummies++;
								//cerr << *g[ newSVD ].namePtr << ", ";
							}
						}
					}
                }
			}
			else if( _graph[ vdS ].type == "reaction" ){

				if( _graph[ vdS ].reactPtr->subsystem->name == iter->first ){

					map< unsigned int, unsigned int >::iterator iter = idMap.find( _graph[ vdS ].id );
					if( iter != idMap.end() ){
						newSVD = findVertex( iter->second, g );
						//newSVD = vertex( iter->second, g );
					}
					else{
						newSVD = add_vertex( g );
                        g[ newSVD ].id              = nVertices;
                        g[ newSVD ].initID          = _graph[ vdS ].id;
						g[ newSVD ].namePtr  		= &_graph[ vdS ].reactPtr->abbr;
                        g[ newSVD ].namePixelWidthPtr  	= &_graph[ vdS ].reactPtr->namePixelWidth;
                        g[ newSVD ].namePixelHeightPtr  = &_graph[ vdS ].reactPtr->namePixelHeight;
						g[ newSVD ].groupID         = _graph[ vdS ].groupID;
                        g[ newSVD ].isClonedPtr    	= _graph[ vdS ].isClonedPtr;
                        g[ newSVD ].isSelectedPtr   = _graph[ vdS ].isSelectedPtr;
                        g[ newSVD ].selectedIDPtr   = _graph[ vdS ].selectedIDPtr;
                        g[ newSVD ].isAlias    	    = false;
                        g[ newSVD ].type        	= _graph[ vdS ].type;
						g[ newSVD ].coordPtr    	= _graph[ vdS ].coordPtr;
                        g[ newSVD ].widthPtr       	= _graph[ vdS ].widthPtr;
                        g[ newSVD ].heightPtr      	= _graph[ vdS ].heightPtr;
                        idMap.insert( pair< unsigned int, unsigned int >( _graph[ vdS ].id, nVertices ) );
						nVertices++;
					}
					//cerr << *g[ newSVD ].namePtr;
				}
			}

			if( _graph[ vdT ].type == "metabolite" ){
				if( ( _graph[ vdT ].metaPtr != NULL ) && ( _graph[ vdT ].subsystems.size() == 1 ) &&
					( _graph[ vdT ].subsystems.begin()->first == iter->first ) ){

  				    map< unsigned int, unsigned int >::iterator iter = idMap.find( _graph[ vdT ].id );
					if( iter != idMap.end() ){
						newTVD = findVertex( iter->second, g );
						//newTVD = vertex( iter->second, g );
					}
					else{
						newTVD = add_vertex( g );
                        g[ newTVD ].id              = nVertices;
                        g[ newTVD ].initID          = _graph[ vdT ].id;
						g[ newTVD ].namePtr  		= &_graph[ vdT ].metaPtr->name;
                        g[ newTVD ].namePixelWidthPtr  	= &_graph[ vdT ].metaPtr->namePixelWidth;
                        g[ newTVD ].namePixelHeightPtr  = &_graph[ vdT ].metaPtr->namePixelHeight;
						g[ newTVD ].groupID         = _graph[ vdT ].groupID;
                        g[ newTVD ].isClonedPtr    	= _graph[ vdT ].isClonedPtr;
                        g[ newTVD ].isSelectedPtr   = _graph[ vdT ].isSelectedPtr;
                        g[ newTVD ].selectedIDPtr   = _graph[ vdT ].selectedIDPtr;
                        g[ newTVD ].isAlias    	    = false;
						g[ newTVD ].type       		= _graph[ vdT ].type;
						g[ newTVD ].coordPtr    	= _graph[ vdT ].coordPtr;
                        g[ newTVD ].widthPtr       	= _graph[ vdT ].widthPtr;
                        g[ newTVD ].heightPtr      	= _graph[ vdT ].heightPtr;
                        idMap.insert( pair< unsigned int, unsigned int >( _graph[ vdT ].id, nVertices ) );
						nVertices++;
					}
				}
                else if( ( _graph[ vdT ].metaPtr != NULL ) && ( _graph[ vdT ].subsystems.size() > 1 ) ){

					map< string, Subdomain * >::iterator itT;
					for( itT = _graph[ vdT ].subsystems.begin(); itT != _graph[ vdT ].subsystems.end(); itT++ ){
						if( ( itT->first == iter->first ) ){
							// cerr << "HERE2" << endl;
							map< unsigned int, unsigned int >::iterator iter = idMap.find( _graph[ vdT ].id );
							if( iter != idMap.end() ){
								newTVD = findVertex( iter->second, g );
							}
							else{
								newTVD = add_vertex( g );
								g[ newTVD ].id              = totalV + nDummies;
								//g[ newTVD ].id              = nVertices;
								g[ newTVD ].initID          = _graph[ vdT ].id;
								g[ newTVD ].namePtr  		= &_graph[ vdT ].metaPtr->name;
								g[ newTVD ].namePixelWidthPtr  	= &_graph[ vdT ].metaPtr->namePixelWidth;
								g[ newTVD ].namePixelHeightPtr  = &_graph[ vdT ].metaPtr->namePixelHeight;
								g[ newTVD ].groupID         = _graph[ vdT ].groupID;
								g[ newTVD ].isClonedPtr    	= _graph[ vdT ].isClonedPtr;
                                g[ newTVD ].isSelectedPtr   = _graph[ vdT ].isSelectedPtr;
                                g[ newTVD ].selectedIDPtr   = _graph[ vdT ].selectedIDPtr;
								g[ newTVD ].isAlias    	    = true;
								g[ newTVD ].type       		= _graph[ vdT ].type;
								g[ newTVD ].coordPtr    	= new Coord2( _graph[ vdT ].coordPtr->x(), _graph[ vdT ].coordPtr->y() );
								g[ newTVD ].widthPtr       	= _graph[ vdT ].widthPtr;
								g[ newTVD ].heightPtr      	= _graph[ vdT ].heightPtr;
								idMap.insert( pair< unsigned int, unsigned int >( _graph[ vdT ].id, g[ newTVD ].id ) );
								//nVertices++;
								nDummies++;
								//cerr << *g[ newTVD ].namePtr << ", ";
							}
						}
					}
                }
			}
			else if( _graph[ vdT ].type == "reaction" ){

				if( _graph[ vdT ].reactPtr->subsystem->name == iter->first ){

					//cerr << "idT = " << _graph[ vdT ].id << endl;
					map< unsigned int, unsigned int >::iterator iter = idMap.find( _graph[ vdT ].id );
					if( iter != idMap.end() ){
						newTVD = findVertex( iter->second, g );
						//newTVD = vertex( iter->second, g );
					}
					else{
						newTVD = add_vertex( g );
                        g[ newTVD ].id              = nVertices;
                        g[ newTVD ].initID          = _graph[ vdT ].id;
						g[ newTVD ].namePtr  		= &_graph[ vdT ].reactPtr->abbr;
                        g[ newTVD ].namePixelWidthPtr  	= &_graph[ vdT ].reactPtr->namePixelWidth;
                        g[ newTVD ].namePixelHeightPtr  = &_graph[ vdT ].reactPtr->namePixelHeight;
						g[ newTVD ].groupID         = _graph[ vdT ].groupID;
                        g[ newTVD ].isClonedPtr    	= _graph[ vdT ].isClonedPtr;
                        g[ newTVD ].isSelectedPtr 	= _graph[ vdT ].isSelectedPtr;;
                        g[ newTVD ].selectedIDPtr 	= _graph[ vdT ].selectedIDPtr;;
                        g[ newTVD ].isAlias    	    = false;
                        g[ newTVD ].type          	= _graph[ vdT ].type;
						g[ newTVD ].coordPtr    	= _graph[ vdT ].coordPtr;
                        g[ newTVD ].widthPtr       	= _graph[ vdT ].widthPtr;
                        g[ newTVD ].heightPtr      	= _graph[ vdT ].heightPtr;
                        idMap.insert( pair< unsigned int, unsigned int >( _graph[ vdT ].id, nVertices ) );
						nVertices++;
					}
					//cerr << *g[ newTVD ].namePtr;
				}
			}

			if( newSVD != NULL && newTVD != NULL ){

				// cerr << "name = " << *g[ newSVD ].namePtr << ", " << *g[ newTVD ].namePtr << endl;

				pair< MetaboliteGraph::edge_descriptor, unsigned int > foreE = add_edge( newSVD, newTVD, g );
				MetaboliteGraph::edge_descriptor foreED = foreE.first;

				g[ foreED ].id = nEdges;
				//g[ foreED ].id = _graph[ ed ].id;
				//put( edge_myid, g, foreED, nEdges );
				nEdges++;
			}
		}

		// reorder the id
		unsigned int vid = 0, eid = 0;
		BGL_FORALL_VERTICES( vd, g, MetaboliteGraph ) {
			g[ vd ].id = vid;
			vid++;
		}
		BGL_FORALL_EDGES( ed, g, MetaboliteGraph ) {
			g[ ed ].id = eid;
			eid++;
		}
		_subGraph.push_back( g );

#ifdef  DEBUG
		//cerr << "_sub: " << iter->first << iter->second->name << endl;
		//printGraph( g );
		cerr << "****" << endl << endl << endl;

		map< unsigned int, unsigned int >::iterator itM = idMap.begin();
		for( itM = idMap.begin(); itM != idMap.end(); itM++ ){
			cerr << "map = " << itM->first << ", " << itM->second << endl;
		}

		cerr << "center = " << *g[ graph_bundle ].centerPtr
			 << " w = " << g[ graph_bundle ].width
			 << " h = " << g[ graph_bundle ].height << endl;
#endif  // DEBUG
	}

	unsigned int index = 0;
	for( iter = _sub.begin(); iter != _sub.end(); iter++ ){

		_subGraph[ index ][ graph_bundle ].centerPtr = &iter->second->center;
		_subGraph[ index ][ graph_bundle ].widthPtr  = &iter->second->width;
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

	//cerr << "subG size = " << _subGraph.size() << endl;
	for( unsigned int i = 0; i < _subGraph.size(); i++ ){

		unsigned int nv = 0;
		//cerr << "node size = " << num_vertices( _subGraph[i] ) << " edge size = " << num_edges( _subGraph[i] ) << endl;
		BGL_FORALL_VERTICES( vd, _subGraph[i], MetaboliteGraph ){

            ForceGraph::vertex_descriptor vdL = add_vertex( _layoutSubGraph[i] );
			_layoutSubGraph[i][ vdL ].id            = _subGraph[i][ vd ].id;
			_layoutSubGraph[i][ vdL ].initID        = _subGraph[i][ vd ].initID;
			_layoutSubGraph[i][ vdL ].groupID       = _subGraph[i][ vd ].groupID;
			_layoutSubGraph[i][ vdL ].componentID   = _subGraph[i][ vd ].componentID;
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
			//cerr << " gid = " << _subGraph[i][ vd ].groupID << " nv = " << nv << ", " << *_layoutSubGraph[i][ vdL ].namePtr << endl;
			nv++;
		}

		BGL_FORALL_EDGES( ed, _subGraph[i], MetaboliteGraph ){

			MetaboliteGraph::vertex_descriptor vdS = source( ed, _subGraph[i] );
			MetaboliteGraph::vertex_descriptor vdT = target( ed, _subGraph[i] );
			unsigned int idS = indexMap.find( _subGraph[i][ vdS ].groupID )->second;
			unsigned int idT = indexMap.find( _subGraph[i][ vdT ].groupID )->second;
            ForceGraph::vertex_descriptor vdSL = vertex( _subGraph[i][ vdS ].id, _layoutSubGraph[i] );
            ForceGraph::vertex_descriptor vdTL = vertex( _subGraph[i][ vdT ].id, _layoutSubGraph[i] );
			//ForceGraph::vertex_descriptor vdSL = vertex( idS, _layoutSubGraph[i] );
			//ForceGraph::vertex_descriptor vdTL = vertex( idT, _layoutSubGraph[i] );

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
		//printGraph( _layoutSubGraph[i] );
#endif // DEBUG
	}
}

//
//  Pathway::computeZone --    compute zone
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::computeZone( void )
{
	unsigned int nEdges = 0;

	BGL_FORALL_EDGES( ed, _skeletonGraph, SkeletonGraph ) {
		_skeletonGraph[ ed ].id = nEdges;
		nEdges++;
	}

	if( nEdges == 0 ){
		BGL_FORALL_VERTICES( vd, _skeletonGraph, SkeletonGraph ) {
			if( _skeletonGraph[vd].id == 0 )
				_skeletonGraph[vd].zone = 0;
			else
				_skeletonGraph[vd].zone = 1;
		}
	}
	else{
		geodesicAssignment( _skeletonGraph );
		_nZoneDiff = zoneAssignment( _skeletonGraph );
	}
}

//
//  Pathway::planar_graph_embedding --    retrieve graph embedding
//
//  Inputs
//  newE: SkeletonGraphVVPair
//
//  Outputs
//  isPlanar: bool
//
void Pathway::planar_graph_embedding( vector< SkeletonGraphVVPair > &addedED )
{
	// create the planar graph
	UndirectedPropertyGraph propG;//( num_vertices( _skeletonGraph ) );
	VertexIndexMap vertexIndex = get( vertex_index, propG );
	EdgeIndexMap edgeIndex = get( edge_index, propG );

#ifdef  DEBUG
	BGL_FORALL_VERTICES( vd, propG, UndirectedPropertyGraph ) {
		cerr << "id = " << vertexIndex[ vd ] << endl;
	}
#endif  // DEBUG
	for( unsigned int i = 0; i < addedED.size(); i++ ){
		pair< UndirectedPropertyGraph::edge_descriptor, int > foreE = add_edge( _skeletonGraph[ addedED[ i ].first ].id, _skeletonGraph[ addedED[ i ].second ].id, propG );
		UndirectedPropertyGraph::edge_descriptor ed = foreE.first;
		edgeIndex[ ed ] = i;
	}

	// create the planar embedding
	embedding_storage_t embedding_storage( num_vertices( propG ) );
	embedding_t embedding( embedding_storage.begin(), get( vertex_index, propG ) );

	// (1)
	// repeat until the format becomes correct
	// find the planar embedding
	boyer_myrvold_planarity_test( boyer_myrvold_params::graph = propG,
								  boyer_myrvold_params::embedding = embedding );
	vector< vector< SkeletonGraph::vertex_descriptor > > embeddingVec; // planar graph embedding of each vertex

	BGL_FORALL_VERTICES( vd, propG, UndirectedPropertyGraph ) {

		//cerr << "id = " << vertexIndex[ vd ] << endl;

		vector< SkeletonGraph::vertex_descriptor > vdVec;
		boost::property_traits< embedding_t >::value_type::const_iterator itE = embedding[ vd ].begin();
		for( ; itE != embedding[ vd ].end(); itE++ ){
			graph_traits< UndirectedPropertyGraph >::edge_descriptor vdE = *itE;
			graph_traits< UndirectedPropertyGraph >::vertex_descriptor vdS = source( vdE, propG );
			graph_traits< UndirectedPropertyGraph >::vertex_descriptor vdT = target( vdE, propG );

			if( vertexIndex[ vd ] == vertexIndex[ vdS ] ) vdVec.push_back( vertex( vertexIndex[ vdT ], _skeletonGraph ) );
			else vdVec.push_back( vertex( vertexIndex[ vdS ], _skeletonGraph ) );
			//cerr << vertexIndex[ vdS ] << " == " << vertexIndex[ vdT ] << ", " << endl;
		}
		//cerr << endl;
		embeddingVec.push_back( vdVec );
	}

#ifdef  SKIP
	for( unsigned int i = 0; i < _embeddingVec.size(); i++ ){
        cerr << i << ": ";
        for( unsigned int j = 0; j < _embeddingVec[i].size(); j++ ){
            SkeletonGraph::vertex_descriptor vd = _embeddingVec[ i ][ j ];
            cerr << _skeletonGraph[ vd ].id << " ";
        }
        cerr << endl;
    }
#endif  //SKIP

#ifdef  STRAIGHT_LINE_DRAWING
	make_biconnected_planar( propG, &embedding[0] ); 		// Make biconnected planar (2/3)

	// (2)
	// repeat until the format becomes correct
	// reorder the edge id
	int index = 0;
	BGL_FORALL_EDGES( ed, propG, UndirectedPropertyGraph ) {
		edgeIndex[ ed ] = index;
		index++;
	}

	// find the planar embedding again
    boyer_myrvold_planarity_test( boyer_myrvold_params::graph = propG,
                                  boyer_myrvold_params::embedding = embedding );

	make_maximal_planar( propG, &embedding[0] ); 			// Make maximal planar (3/3)

	// (3)
	// repeat until the format becomes correct
	// reorder the edge id
	index = 0;
	BGL_FORALL_EDGES( ed, propG, UndirectedPropertyGraph ) {
		edgeIndex[ ed ] = index;
		index++;
	}

	// find the planar embedding again
    boyer_myrvold_planarity_test( boyer_myrvold_params::graph = propG,
                                  boyer_myrvold_params::embedding = embedding );


    // find a canonical ordering
    vector< graph_traits < UndirectedPropertyGraph >::vertex_descriptor > ordering;
	planar_canonical_ordering( propG, embedding, std::back_inserter( ordering ) );

	// set up a property map to hold the mapping from vertices to coord_t's
	typedef vector< coord_t > straight_line_drawing_storage_t;
	typedef boost::iterator_property_map< straight_line_drawing_storage_t::iterator,
										  property_map< UndirectedPropertyGraph, vertex_index_t >::type > straight_line_drawing_t;

	straight_line_drawing_storage_t straight_line_drawing_storage( num_vertices( propG ) );
	straight_line_drawing_t straight_line_drawing( straight_line_drawing_storage.begin(), get( vertex_index, propG ) );

	// compute the straight line drawing
	// cerr << "ordering size = " << ordering.size() << endl;
	chrobak_payne_straight_line_drawing( propG, embedding, ordering.begin(), ordering.end(),
	                                     straight_line_drawing );

	cout << "The straight line drawing is: " << endl;
	graph_traits< UndirectedPropertyGraph >::vertex_iterator vi, vi_end;
	graph_traits< SkeletonGraph >::vertex_iterator di, di_end;
	tie( di, di_end ) = vertices( _skeletonGraph );
	for( tie( vi, vi_end ) = vertices( propG ); vi != vi_end; ++vi ){

		coord_t coord( get( straight_line_drawing, *vi ) );
		// cout << *vi << " -> (" << coord.x << ", " << coord.y << ")" << endl;
		_skeletonGraph[ *di ].coordPtr->x() = coord.x * 50;
		_skeletonGraph[ *di ].coordPtr->y() = coord.y * 50;
		di++;
	}

	// verify that the drawing is actually a plane drawing
  	if( is_straight_line_drawing( propG, straight_line_drawing ) )
    	cout << "Is a plane drawing." << endl;
  	else
    	cout << "Is not a plane drawing." << endl;

	normalization();
#endif  //STRAIGHT_LINE_DRAWING
}


bool Pathway::has_cycle_dfs( const UndirectedPropertyGraph & g, UndirectedPropertyGraph::vertex_descriptor u,
					default_color_type * color )
{
	color[u] = gray_color;
	graph_traits < UndirectedPropertyGraph >::adjacency_iterator vi, vi_end;
	for (boost::tie(vi, vi_end) = adjacent_vertices(u, g); vi != vi_end; ++vi)
		if (color[*vi] == white_color) {
			if (has_cycle_dfs(g, *vi, color))
				return true;            // cycle detected, return immediately
		} else if (color[*vi] == gray_color)        // *vi is an ancestor!
			return true;
	color[u] = black_color;
	return false;
}

bool Pathway::has_cycle( const UndirectedPropertyGraph & g )
{
	vector< default_color_type > color( num_vertices(g), white_color );
	graph_traits< UndirectedPropertyGraph >::vertex_iterator vi, vi_end;
	for( boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi ) {
		if( color[*vi] == white_color) {
			if( has_cycle_dfs(g, *vi, &color[0])) {
				return true;
			}
		}
	}
	return false;
}

//
//  Pathway::loop_in_dual_graph_test --    test if a loop existed in the dual graph
//
//  Inputs
//  g: UndirectedPropertyGraph
//
//  Outputs
//  none
//
bool Pathway::loop_in_dual_graph_test( UndirectedPropertyGraph & propG )
{
	UndirectedPropertyGraph dualG;
	VertexIndexMap 	vertexIndex 	= get( vertex_index, propG );
	EdgeIndexMap 	edgeIndex 		= get( edge_index, propG );
	VertexIndexMap 	dualVertexIndex = get( vertex_index, dualG );
	EdgeIndexMap 	dualEdgeIndex 	= get( edge_index, dualG );

	// create the planar embedding
	embedding_storage_t embedding_storage( num_vertices( propG ) );
	embedding_t embedding( embedding_storage.begin(), get( vertex_index, propG ) );
	// initialize edge index
	unsigned int nEdges = 0;
	BGL_FORALL_EDGES( ed, propG, UndirectedPropertyGraph ){
			edgeIndex[ ed ] = nEdges;
			nEdges++;
			//cerr << " idE = " << edgeIndex[ed] << endl;
		}

	// Compute the planar embedding - we know the input graph is planar,
	boyer_myrvold_planarity_test( boyer_myrvold_params::graph = propG,
								  boyer_myrvold_params::embedding = embedding );

	create_dual_graph( propG, dualG, embedding );

	nEdges = 0;
	map< UndirectedPropertyGraph::edge_descriptor, UndirectedPropertyGraph::edge_descriptor > delEDVec;
	// initialize edge index of the dual graph
	BGL_FORALL_EDGES( ed, dualG, UndirectedPropertyGraph ){

			UndirectedPropertyGraph::vertex_descriptor vdS = source( ed, dualG );
			UndirectedPropertyGraph::vertex_descriptor vdT = target( ed, dualG );

			dualEdgeIndex[ ed ] = nEdges;
			if( dualVertexIndex[ vdS ] == 0 || dualVertexIndex[ vdT ] == 0 ) {
				delEDVec.insert( pair< UndirectedPropertyGraph::edge_descriptor, UndirectedPropertyGraph::edge_descriptor > ( ed, ed ) );
			}
			if( vdS == vdT )
				delEDVec.insert( pair< UndirectedPropertyGraph::edge_descriptor, UndirectedPropertyGraph::edge_descriptor >( ed, ed ) );

			nEdges++;
			//cerr << " didE = " << dualEdgeIndex[ed] << endl;
		}

#ifdef  DEBUG
	cerr << "nV = " << num_vertices( dualG ) << ", nE = " << num_edges( dualG )
         << ", " << ( num_vertices( dualG ) == 0 || num_edges( dualG ) == 0 ) << endl;
    BGL_FORALL_VERTICES( vd, dualG, UndirectedPropertyGraph ){
            cerr << "vertexIndex = " << vertexIndex[ vd ] << endl;
        }
    BGL_FORALL_EDGES( ed, dualG, UndirectedPropertyGraph ){

            UndirectedPropertyGraph::vertex_descriptor vdS = source( ed, dualG );
            UndirectedPropertyGraph::vertex_descriptor vdT = target( ed, dualG );
            cerr << "edgeIndex = " << edgeIndex[ ed ] << ": ( "
                 << vertexIndex[ vdS ] << " ," << vertexIndex[ vdT ] << " ) "<< endl;
        }
#endif  // DEBUG

	// remove the representative vertex of the outer face and edges connected to it
	// self-loop
	map< UndirectedPropertyGraph::edge_descriptor, UndirectedPropertyGraph::edge_descriptor >::iterator itE = delEDVec.begin();
	for( ; itE != delEDVec.end(); itE++ ){
		remove_edge( itE->first, dualG );
	}
	UndirectedPropertyGraph::vertex_descriptor vdO = vertex( 0, dualG );
	remove_vertex( vdO, dualG );

	return has_cycle( dualG );
}

//
//  Pathway::constrained_planarity_test --    test planarity after adding a new edge but vertex degree < k
//
//  Inputs
//  addedED: vector< SkeletonGraphVVPair >
//  newE: SkeletonGraphVVPair
//
//  Outputs
//  isPlanar: bool
//
bool Pathway::constrained_planarity_test( vector< SkeletonGraphVVPair > &addedED,
										  SkeletonGraphVVPair & newE, unsigned int maxDegree )
{
	UndirectedPropertyGraph propG( num_vertices( _skeletonGraph ) );
	for( unsigned int i = 0; i < addedED.size(); i++ ){
		add_edge( _skeletonGraph[ addedED[ i ].first ].id, _skeletonGraph[ addedED[ i ].second ].id, propG );
	}
	pair< UndirectedPropertyGraph::edge_descriptor, unsigned int > foreE = add_edge( _skeletonGraph[ newE.first ].id, _skeletonGraph[ newE.second ].id, propG );
	UndirectedPropertyGraph::edge_descriptor foreED = foreE.first;
	UndirectedPropertyGraph::vertex_descriptor vdS = source( foreE.first, propG );
	UndirectedPropertyGraph::vertex_descriptor vdT = target( foreE.first, propG );

#ifdef SKIP
	// check if generate K3 graphs
	bool isWithK3 = false;
	UndirectedPropertyGraph::out_edge_iterator e3, e3_end;
	for( tie( e3, e3_end ) = out_edges( vdS, propG ); e3 != e3_end; ++e3 ) {
		UndirectedPropertyGraph::edge_descriptor ed = *e3;
		UndirectedPropertyGraph::vertex_descriptor vdTarget = target( ed, propG );

		if( vdT != vdTarget ){

			bool found = false;
			UndirectedPropertyGraph::edge_descriptor edC;
			tie( edC, found ) = edge( vdT, vdTarget, propG );

			if( found == true ) isWithK3 = true;
		}
	}

	// check if generate K4 graphs
	bool isWithK4 = false;
	UndirectedPropertyGraph::out_edge_iterator eo, eo_end;
	vector< UndirectedPropertyGraph::vertex_descriptor > vdVec;
	for( tie( eo, eo_end ) = out_edges( vdS, propG ); eo != eo_end; ++eo ) {
		UndirectedPropertyGraph::edge_descriptor ed = *eo;
		UndirectedPropertyGraph::vertex_descriptor vdTarget = target( ed, propG );

		if( vdT != vdTarget ){

			bool found = false;
			UndirectedPropertyGraph::edge_descriptor edC;
			tie( edC, found ) = edge( vdT, vdTarget, propG );

			if( found == true ) vdVec.push_back( vdTarget );
		}
	}
	for( unsigned int i = 0; i < vdVec.size(); i++ ){
		for( unsigned int j = i+1; j < vdVec.size(); j++ ) {

			bool found = false;
			UndirectedPropertyGraph::edge_descriptor edC;
			tie( edC, found ) = edge( vdVec[ i ], vdVec[ j ], propG );

			if( found == true ) isWithK4 = true;
		}
	}
#endif // SKIP

	// find the planar embedding
	bool isPlanar = true;
	bool isDualLoop = false;
	bool isPlanarity = boyer_myrvold_planarity_test( boyer_myrvold_params::graph = propG );

	if( isPlanarity == true ) {
		isDualLoop = loop_in_dual_graph_test( propG );
	}

	//cerr << "isPlanarity = " << isPlanarity << " isDualLoop = " << isDualLoop << endl;

	if( ( isPlanarity == true ) && ( isDualLoop == false ) &&
		( out_degree( vdS, propG ) <= maxDegree ) &&
		( out_degree( vdT, propG ) <= maxDegree ) ) {
//        ( isWithK4 == false ) ){
//        ( isWithK3 == false && isWithK4 == false ) ){
		isPlanar = true;
		//cout << "The graph is planar." << endl;
	}
	else{
		isPlanar = false;
		//cout << "The graph is not planar." << endl;
	}

	return isPlanar;
}

//
//  Pathway::planarity_test --    test planarity after adding a new edge
//
//  Inputs
//  addedED: vector< SkeletonGraphVVPair >
//  newE: SkeletonGraphVVPair
//
//  Outputs
//  isPlanar: bool
//
bool Pathway::planarity_test( vector< SkeletonGraphVVPair > &addedED, SkeletonGraphVVPair & newE )
{
	bool isPlanar = true;
//    typedef adjacency_list< vecS, vecS, undirectedS, property< vertex_index_t, int > > UndirectedPropertyGraph;

	UndirectedPropertyGraph propG( num_vertices( _skeletonGraph ) );
	for( unsigned int i = 0; i < addedED.size(); i++ ){
		add_edge( _skeletonGraph[ addedED[ i ].first ].id, _skeletonGraph[ addedED[ i ].second ].id, propG );
	}
	add_edge( _skeletonGraph[ newE.first ].id, _skeletonGraph[ newE.second ].id, propG );


	if( boyer_myrvold_planarity_test( propG ) ){
		isPlanar = true;
		//cout << "The graph is planar." << endl;
	}
	else{
		isPlanar = false;
		//cout << "The graph is not planar." << endl;
	}

	return isPlanar;
}


//
//  Pathway::planar_max_filtered_graph --    generate the planar maximally filtered graph
//
//  Inputs
//  node
//
//  Outputs
//  isPlanar: bool
//
void Pathway::planar_max_filtered_graph( void )
{
	vector< pair< double, SkeletonGraphVVPair > > orderedED;
	vector< SkeletonGraphVVPair > addedED;
	//vector< pair< double, SkeletonGraph::edge_descriptor > > orderedED;

	// collect edges with vertex-pairs
	BGL_FORALL_EDGES( ed, _skeletonGraph, SkeletonGraph ) {

			SkeletonGraph::vertex_descriptor vdS = source( ed, _skeletonGraph );
			SkeletonGraph::vertex_descriptor vdT =  target( ed, _skeletonGraph );
			double newW = MAXIMUM_INTEGER - _skeletonGraph[ ed ].weight;
			SkeletonGraphVVPair vvpair( vdS, vdT );
			orderedED.push_back( pair< double, SkeletonGraphVVPair >( newW, vvpair ) );
		}

	// sort edge by weight attributes
	sort( orderedED.begin(), orderedED.end() );

	// clear all edges
	SkeletonGraph::edge_iterator ei, ei_end, e_next;
	tie( ei, ei_end ) = edges( _skeletonGraph );
	for ( e_next = ei; ei != ei_end; ei = e_next ) {
		e_next++;
		remove_edge( *ei, _skeletonGraph );
	}

	// add edge if the graph is still planar
	for( unsigned i = 0; i < orderedED.size(); i++ ){

		//bool isPlanar = planarity_test( addedED, orderedED[ i ].second );
		bool isPlanar = constrained_planarity_test( addedED, orderedED[ i ].second, MAX_DEGREE_ALLOWED );

#ifdef DEBUG
		cerr << endl << "i = " << i << endl;
		cerr << "isPlanar = " << isPlanar << ": " << _skeletonGraph[ orderedED[ i ].second.first ].id
			 << " x " << _skeletonGraph[ orderedED[ i ].second.second ].id << endl;
#endif // DEBUG

		if( isPlanar == true ){
			addedED.push_back( orderedED[ i ].second );
			add_edge( orderedED[ i ].second.first, orderedED[ i ].second.second, _skeletonGraph );
		}
	}

	planar_graph_embedding( addedED );   // must be planar

#ifdef  DEBUG
	for( unsigned i = 0; i < orderedED.size(); i++ ){

		SkeletonGraphVVPair vvpair = orderedED[ i ].second;
		SkeletonGraph::vertex_descriptor vdS = vvpair.first;
		SkeletonGraph::vertex_descriptor vdT = vvpair.second;
		cerr << MAXIMUM_INTEGER - orderedED[ i ].first << ", " << _skeletonGraph[ vdS ].id << " == " << _skeletonGraph[ vdT ].id << endl;
	}
	cerr << endl;
#endif  // DEBUG
}

//
//  Pathway::genDependencyGraph --    generate the spanning tree of sbusystem relationship
//
//  Inputs
//  node
//
//  Outputs
//  none
//
void Pathway::genDependencyGraph( void )
{
	// add vertices
	unsigned int idV = 0, idE = 0;
	_skeletonGraph[ graph_bundle ].centerPtr	= new Coord2( 0.0, 0.0 );
	_skeletonGraph[ graph_bundle ].widthPtr	= new double( *_widthPtr );
	_skeletonGraph[ graph_bundle ].heightPtr	= new double( *_heightPtr );
	//map< pair< double, unsigned int >, SkeletonGraph::vertex_descriptor > vdMap;
	for( map< string, Subdomain * >::iterator it = _sub.begin(); it != _sub.end(); ++it ){

		SkeletonGraph::vertex_descriptor vd 	= add_vertex( _skeletonGraph );
		_skeletonGraph[ vd ].id 			= idV;// + num_vertices( _graph );
		_skeletonGraph[ vd ].name         	= it->second->name;
		_skeletonGraph[ vd ].coordPtr 		= &it->second->center;
		_skeletonGraph[ vd ].domainPtr 		= it->second;
		_skeletonGraph[ vd ].initID 		= -1;	// not exit in the original graph
		_skeletonGraph[ vd ].widthPtr		= &it->second->width;
		_skeletonGraph[ vd ].heightPtr		= &it->second->height;
		//_skeletonGraph[ vd ].areaPtr		= &it->second->area;
		_skeletonGraph[ vd ].areaPtr		= &it->second->idealArea;
		//_skeletonGraph[ vd ].isSelected	= false;

		_skeletonGraph[ vd ].geodesicDist = 0.0;	// geodesic distance
		_skeletonGraph[ vd ].zone			= 0;    // geodesic zone
		_skeletonGraph[ vd ].ezone		= 0;    // euclidean zone
		//_skeletonGraph[ vd ].computeType  = TYPE_FREE;
		//_skeletonGraph[ vd ].childWidth	= 0.0;  	// maximum width among children, including the current one
		//_skeletonGraph[ vd ].childHeight	= 0.0;    	// maximum height among children, including the current one
		_skeletonGraph[ vd ].flag			= false;	// flag for algorithms, needed to be initialized eveytime before computation
		//_skeletonGraph[ vd ].refCoord     = it->second->center; 	// reference coord for relative position in floorplan
		//_skeletonGraph[ vd ].initCoord 	= it->second->center;  	// reference coord for relative position in floorplan
		//_skeletonGraph[ vd ].debugDummy 	= TYPE_RIGHT;     		// mode for debugging

		//_skeletonGraph[ vd ].angle 		= 0.0;          		// angle for initial layout
		//_skeletonGraph[ vd ].force 		= Coord2( 0.0, 0.0 );   // force value for force optimization
		//_skeletonGraph[ vd ].shift 		= Coord2( 0.0, 0.0 );   // shift value for force optimization

		idV++;
		//cerr << " Depend_skeletonGraph[ " << it->first << " ] = " << it->second.width << ", " << it->second.height << endl;
		//vdMap.insert( pair< pair< double, unsigned int >, SkeletonGraph::vertex_descriptor >( pair< double, unsigned int >(it->second.width*it->second.height, idV), vd ) );
	}


	// add edges
	unsigned int nEdges = 0;
	BGL_FORALL_VERTICES( vd, _graph, MetaboliteGraph ) {

		map< unsigned int, unsigned int > domainFreq;
		//if( _graph[ vd ].type == "metabolite" && isCloneMetaType( vd ) == false ) {
		//cerr << "isCloneMetaType( vd ) = " << isCloneMetaType( vd ) << endl;

		bool isProceed = false;
		if( _isCloneByThreshold == true ){
			isProceed = ( _graph[ vd ].type == "metabolite" ) && ( _graph[ vd ].metaPtr->freq <= _threshold );
		}
		else
		{
			isProceed = ( _graph[ vd ].type == "metabolite" ) && isCloneMetaType( vd ) == false;
		}

		if( isProceed ) {

			//assert( _graph[ vd ].metaPtr->metaType != "7_WATER" );

			// out edges
			MetaboliteGraph::out_edge_iterator eo, eo_end;
			for( tie( eo, eo_end ) = out_edges( vd, _graph ); eo != eo_end; ++eo ){

				MetaboliteGraph::edge_descriptor ed = *eo;
				MetaboliteGraph::vertex_descriptor vdT = target( ed, _graph );
				if( _graph[ vdT ].type == "reaction" ) {
					map< unsigned int, unsigned int >::iterator dIter;
					dIter = domainFreq.find( _graph[ vdT ].reactPtr->subsystem->id );
					if( dIter != domainFreq.end() ){
						//cerr << "id = " << _graph[ vdT ].reactPtr->subsystem->id << endl;
						dIter->second += 1;
					}
					else{
						domainFreq.insert( pair< unsigned int, unsigned int >( _graph[ vdT ].reactPtr->subsystem->id, 1 ) );
					}
				}
			}

			// in edges
			MetaboliteGraph::in_edge_iterator ei, ei_end;
			for( tie( ei, ei_end ) = in_edges( vd, _graph ); ei != ei_end; ++ei ){

				MetaboliteGraph::edge_descriptor ed = *ei;
				MetaboliteGraph::vertex_descriptor vdS = source( ed, _graph );
				if( _graph[ vdS ].type == "reaction" ) {
					map< unsigned int, unsigned int >::iterator dIter;
					dIter = domainFreq.find( _graph[ vdS ].reactPtr->subsystem->id );
					if( dIter != domainFreq.end() ){
						//cerr << "id = " << _graph[ vdS ].reactPtr->subsystem->id << endl;
						dIter->second += 1;
					}
					else{
						domainFreq.insert( pair< unsigned int, unsigned int >( _graph[ vdS ].reactPtr->subsystem->id, 1 ) );
					}
				}
			}

			for( map< unsigned int, unsigned int >::iterator oIter = domainFreq.begin();
				 oIter != domainFreq.end(); oIter++ ){

				map< unsigned int, unsigned int >::iterator iIter = oIter;
				iIter++;
				for( ; iIter != domainFreq.end(); iIter++ ){

					unsigned int sysIDS = oIter->first;
					unsigned int sysIDT = iIter->first;
					unsigned int freq = oIter->second * iIter->second;

					SkeletonGraph::vertex_descriptor vdS = vertex( sysIDS, _skeletonGraph );
					SkeletonGraph::vertex_descriptor vdT = vertex( sysIDT, _skeletonGraph );

					bool found = false;
					SkeletonGraph::edge_descriptor oldED;
					tie( oldED, found ) = edge( vdS, vdT, _skeletonGraph );

					if( found == true ){
						_skeletonGraph[ oldED ].weight 	+= freq;
					}
					else{
						pair< SkeletonGraph::edge_descriptor, unsigned int > foreE = add_edge( vdS, vdT, _skeletonGraph );
						SkeletonGraph::edge_descriptor foreED = foreE.first;
						_skeletonGraph[ foreED ].id 			= nEdges;
						_skeletonGraph[ foreED ].weight 		= freq;

						nEdges++;
						//cerr << "nEdges = " << nEdges << endl;
					}
				}
			}
		}
	}

#ifdef  DEBUG
	double avg = 0;
    BGL_FORALL_VERTICES( vd, _skeletonGraph, SkeletonGraph ) {
        cerr << "vid = " << _skeletonGraph[ vd ].id << " degrees = " << out_degree( vd, _skeletonGraph ) << endl;;
        avg += out_degree( vd, _skeletonGraph );
    }
    cerr << "avg = " << avg/num_vertices( _skeletonGraph ) << endl;
#endif  // DEBUG

	//max_spaning_tree();
	planar_max_filtered_graph();

	// reset edge id
	nEdges = 0;
	BGL_FORALL_EDGES( ed, _skeletonGraph, SkeletonGraph ) {
        _skeletonGraph[ ed ].id = nEdges;
        nEdges++;
        //cerr << "id = " << _skeletonGraph[ ed ].id << " w= " << _skeletonGraph[ed].weight << endl;
    }

	computeZone();

#ifdef  DEBUG
	cout << "Print the edges in the MST:" << endl;
	for( unsigned int i = 0; i < mst.size(); i++ ) {
		cout << _skeletonGraph[ source( mst[i], _skeletonGraph ) ].id << " <--> "
			 << _skeletonGraph[ target( mst[i], _skeletonGraph ) ].id
			 << " with weight of " << get( &RelationEdgeProperty::weight, _skeletonGraph )[ mst[i] ] << endl;
	}
	BGL_FORALL_VERTICES( vd, _skeletonGraph, SkeletonGraph ) {
		cerr << "w = " << _skeletonGraph[ vd ].width << " h = " << _skeletonGraph[ vd ].height << endl;;
	}
    BaseGraph test = (BaseGraph &) _skeletonGraph;
    BGL_FORALL_VERTICES( vd, test, BaseGraph ) {
		cerr << "w = " << test[ vd ].width << " h = " << test[ vd ].height << endl;;
	}
#endif  // DEBUG

#ifdef DEBUG
	map< pair< double, unsigned int >, SkeletonGraph::vertex_descriptor >::iterator itM;
	for( itM = vdMap.begin(); itM != vdMap.end(); itM++ ){
		cerr << _skeletonGraph[ itM->second ].id
			 << " w = " << _skeletonGraph[ itM->second ].width << " h = " << _skeletonGraph[ itM->second ].height << endl;

		SkeletonGraph::out_edge_iterator eo, eo_end;
		for( tie( eo, eo_end ) = out_edges( itM->second, _skeletonGraph ); eo != eo_end; ++eo ) {
			cerr << " ( " << _skeletonGraph[ itM->second ].id << " vdT = " << _skeletonGraph[ target( *eo, _skeletonGraph ) ].id << " ), ";
		}
		cerr << endl;
	}
#endif // DEBUG

	//genSubsysWeight();
    radialPlacement( _skeletonGraph, *_widthPtr, *_heightPtr );
	//printGraph( _skeletonGraph );
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
