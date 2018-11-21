
#include "ui/GraphicsView.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------
void GraphicsView::_item_seeds( void )
{
    ForceGraph &c = _boundary->composite();

    BGL_FORALL_VERTICES( vd, c, ForceGraph ) {

        GraphicsBallItem *itemptr = new GraphicsBallItem;
        //itemptr->setPen( Qt::NoPen );
        itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 0, 0, 0, 255 ), Qt::SolidPattern ) );
        itemptr->setRect( QRectF( c[vd].coordPtr->x(), -c[vd].coordPtr->y(), 10, 10 ) );
        itemptr->id() = c[vd].id;

        //cerr << vertexCoord[vd];
        _scene->addItem( itemptr );
    }
    cerr << "seeds.size() = " << num_vertices( c ) << endl;
}

void GraphicsView::_item_skeleton( void )
{
    ForceGraph &s = _boundary->skeleton();

    // draw edges
    BGL_FORALL_EDGES( ed, s, ForceGraph ) {

        ForceGraph::vertex_descriptor vdS = source( ed, s );
        ForceGraph::vertex_descriptor vdT = target( ed, s );
        QPainterPath path;
        path.moveTo( s[vdS].coordPtr->x(), -s[vdS].coordPtr->y() );
        path.lineTo( s[vdT].coordPtr->x(), -s[vdT].coordPtr->y() );

        GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
        itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setPath( path );

        _scene->addItem( itemptr );
    }

    BGL_FORALL_VERTICES( vd, s, ForceGraph ) {

        GraphicsBallItem *itemptr = new GraphicsBallItem;
        itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setRect( QRectF( s[vd].coordPtr->x(), -s[vd].coordPtr->y(), 10, 10 ) );
        itemptr->id() = s[vd].id;

        //cerr << vertexCoord[vd];
        _scene->addItem( itemptr );
    }
}

void GraphicsView::_item_composite( void )
{
    ForceGraph &s = _boundary->composite();

    // draw edges
    BGL_FORALL_EDGES( ed, s, ForceGraph ) {

        ForceGraph::vertex_descriptor vdS = source( ed, s );
        ForceGraph::vertex_descriptor vdT = target( ed, s );
        QPainterPath path;
        path.moveTo( s[vdS].coordPtr->x(), -s[vdS].coordPtr->y() );
        path.lineTo( s[vdT].coordPtr->x(), -s[vdT].coordPtr->y() );

        GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
        itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setPath( path );

        _scene->addItem( itemptr );
    }

    BGL_FORALL_VERTICES( vd, s, ForceGraph ) {

        GraphicsBallItem *itemptr = new GraphicsBallItem;
        itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setRect( QRectF( s[vd].coordPtr->x(), -s[vd].coordPtr->y(), 10, 10 ) );
        itemptr->id() = s[vd].id;

        //cerr << vertexCoord[vd];
        _scene->addItem( itemptr );
    }
}

void GraphicsView::_item_polygonComplex( void )
{
    ForceGraph &s = _boundary->skeleton();
    //vector < vector< Coord2 > > p = _boundary->polygons();
    map< unsigned int, Polygon2 >  p = _boundary->polygonComplex();
    map< unsigned int, Polygon2 >::iterator itP = p.begin();
    for( ; itP != p.end(); itP++ ){

        //cerr << "t area = " << itP->second.area() << endl;
        QPolygonF polygon;
        for( unsigned int j = 0; j < itP->second.elements().size(); j++ ){
            polygon.append( QPointF( itP->second.elements()[j].x(), -itP->second.elements()[j].y() ) );
            // cerr << "x = " << p[i][j].x() << " y = " << p[i][j].y() << endl;
        }

        GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
        vector< double > rgb;
        ForceGraph::vertex_descriptor vd = vertex( itP->first, s );
        unsigned int gid = s[vd].initID;
        pickBrewerColor( gid, rgb );
        QColor color( rgb[0]*255, rgb[1]*255, rgb[2]*255, 100 );
        itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
        itemptr->setPolygon( polygon );

        //cerr << vertexCoord[vd];
        _scene->addItem( itemptr );
    }
    // cerr << "polygon.size() = " << p.size() << endl;
}

void GraphicsView::_item_polygons( void )
{
    ForceGraph &s = _boundary->composite();
    vector< Seed > &seedVec = *_boundary->forceBoundary().voronoi().seedVec();

    for( unsigned int i = 0; i < seedVec.size(); i++ ){

        Polygon2 &p = seedVec[i].cellPolygon;

        QPolygonF polygon;
        for( unsigned int j = 0; j < p.elements().size(); j++ ){
            polygon.append( QPointF( p.elements()[j].x(), -p.elements()[j].y() ) );
            // cerr << "x = " << p[i][j].x() << " y = " << p[i][j].y() << endl;
        }

        GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
        vector< double > rgb;
        ForceGraph::vertex_descriptor vd = vertex( i, s );

        unsigned int gid = s[vd].initID;
        pickBrewerColor( gid, rgb );
        QColor color( rgb[0]*255, rgb[1]*255, rgb[2]*255, 100 );
        itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
        itemptr->setPolygon( polygon );

        //cerr << vertexCoord[vd];
        _scene->addItem( itemptr );
    }
}

void GraphicsView::_item_boundary( void )
{
    BoundaryGraph * g =  NULL;
    if( _is_simplifiedFlag == true )
        g = & _simplifiedBoundary->boundary();
    else
        g = & _boundary->boundary();

    // draw edges
    BGL_FORALL_EDGES( ed, *g, BoundaryGraph ) {

        BoundaryGraph::vertex_descriptor vdS = source( ed, *g );
        BoundaryGraph::vertex_descriptor vdT = target( ed, *g );
        QPainterPath path;
        path.moveTo( (*g)[vdS].coordPtr->x(), -(*g)[vdS].coordPtr->y() );
        path.lineTo( (*g)[vdT].coordPtr->x(), -(*g)[vdT].coordPtr->y() );

        GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setPath( path );

        _scene->addItem( itemptr );
    }

    // draw vertices
    BGL_FORALL_VERTICES( vd, *g, BoundaryGraph ) {

        GraphicsBallItem *itemptr = new GraphicsBallItem;
        itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setRect( QRectF( (*g)[vd].coordPtr->x(), -(*g)[vd].coordPtr->y(), 10, 10 ) );
        itemptr->id() = (*g)[vd].id;

        //cerr << vertexCoord[vd];
        _scene->addItem( itemptr );
    }
}

void GraphicsView::_item_pathways( void )
{
    vector< ForceGraph > &lsubg    = _pathway->lsubG();

    // create edge object from the spanning tree and add it to the scene
    for( unsigned int i = 0; i < lsubg.size(); i++ ){

        // draw edges
        BGL_FORALL_EDGES( ed, lsubg[i], ForceGraph ) {

            ForceGraph::vertex_descriptor vdS = source( ed, lsubg[i] );
            ForceGraph::vertex_descriptor vdT = target( ed, lsubg[i] );

            QPainterPath path;
            path.moveTo( lsubg[i][vdS].coordPtr->x(), -lsubg[i][vdS].coordPtr->y() );
            path.lineTo( lsubg[i][vdT].coordPtr->x(), -lsubg[i][vdT].coordPtr->y() );

            // add path
            GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

            itemptr->setPen( QPen( QColor( 0, 0, 0, 125 ), 2 ) );
            //itemptr->setPen( QPen( QColor( 0, 0, 0, 200 ), 2 ) );
            itemptr->setPath( path );
            _scene->addItem( itemptr );
        }

        // draw vertices
        BGL_FORALL_VERTICES( vd, lsubg[i], ForceGraph ) {

            GraphicsBallItem *itemptr = new GraphicsBallItem;
            itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
            itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
            itemptr->setRect( QRectF( lsubg[i][vd].coordPtr->x(), -lsubg[i][vd].coordPtr->y(), 10, 10 ) );
            itemptr->id() = lsubg[i][vd].id;

            //cerr << vertexCoord[vd];
            _scene->addItem( itemptr );
        }
    }
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
void GraphicsView::initSceneItems ( void )
{
    // initialization
    _scene->clear();

    if( _is_polygonFlag == true ) _item_polygons();
    if( _is_polygonComplexFlag == true ) _item_polygonComplex();
    if( _is_compositeFlag == true ) _item_composite();
    if( _is_skeletonFlag == true ) _item_skeleton();
    // if( _is_polygonFlag == true ) _item_seeds();
    if( _is_boundaryFlag == true ) _item_boundary();
    if( _is_boundaryFlag == true ) _item_pathways();

    // cerr << "_scene.size = " << _scene->items().size() << endl;
}

//------------------------------------------------------------------------------
//	Event handlers
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
void GraphicsView::exportPNG ( double x, double y, double w, double h )
{
    // Take file path and name that will create
    //QString newPath = QFileDialog::getSaveFileName(this, trUtf8("Save SVG"),
    //                                               path, tr("SVG files (*.svg)"));
    QString newPath = QString( QLatin1String( "../svg/pathway.png" ) );
    _scene->setSceneRect( x, y, w, h );  // x, y, w, h

    if ( newPath.isEmpty() ) return;

    QImage  screenshot( w, h, QImage::Format_RGB32 ); // maximum 32767x32767

    QPainter painter( &screenshot );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.fillRect( 0, 0, w, h, Qt::white );
    _scene->render( &painter );
    screenshot.save( newPath );
}


GraphicsView::GraphicsView( QWidget *parent )
    : QGraphicsView( parent )
{
    setAutoFillBackground( true );
    setBackgroundBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );

	_scene = new QGraphicsScene( this );
    _scene->setSceneRect( -DEFAULT_WIDTH/2.0, -DEFAULT_HEIGHT/2.0, DEFAULT_WIDTH, DEFAULT_HEIGHT );  // x, y, w, h

    _is_simplifiedFlag = false;
    _is_polygonFlag = false;
    _is_polygonComplexFlag = false;
    _is_skeletonFlag = false;
    _is_compositeFlag = false;
    _is_boundaryFlag = false;
    _is_pathwayFlag = false;
    this->setScene( _scene );
}

GraphicsView::~GraphicsView()
{
}
