
#include "ui/GraphicsView.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------
void GraphicsView::_item_seeds( void )
{
    if( _is_polygonFlag == true ){

        vector< Coord2 > seeds = _boundary->seeds();

        for( unsigned int i = 0; i < seeds.size(); i++ ){

            GraphicsBallItem *itemptr = new GraphicsBallItem;
            //itemptr->setPen( Qt::NoPen );
            itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
            itemptr->setBrush( QBrush( QColor( 0, 0, 0, 255 ), Qt::SolidPattern ) );
            itemptr->setRect( QRectF( seeds[i].x(), -seeds[i].y(), 10, 10 ) );
            itemptr->id() = i;

            //cerr << vertexCoord[vd];
            _scene->addItem( itemptr );
        }
        cerr << "seeds.size() = " << seeds.size() << endl;
    }
}

void GraphicsView::_item_skeleton( void )
{
    SkeletonGraph &s = _boundary->skeleton();

    // draw edges
    BGL_FORALL_EDGES( ed, s, SkeletonGraph ) {

        BoundaryGraph::vertex_descriptor vdS = source( ed, s );
        BoundaryGraph::vertex_descriptor vdT = target( ed, s );
        QPainterPath path;
        path.moveTo( s[vdS].coordPtr->x(), -s[vdS].coordPtr->y() );
        path.lineTo( s[vdT].coordPtr->x(), -s[vdT].coordPtr->y() );

        GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
        itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setPath( path );

        _scene->addItem( itemptr );
    }

    BGL_FORALL_VERTICES( vd, s, SkeletonGraph ) {

        GraphicsBallItem *itemptr = new GraphicsBallItem;
        itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setRect( QRectF( s[vd].coordPtr->x(), -s[vd].coordPtr->y(), 10, 10 ) );
        itemptr->id() = s[vd].id;

        //cerr << vertexCoord[vd];
        _scene->addItem( itemptr );
    }
}

void GraphicsView::_item_polygons( void )
{
    if( _is_polygonFlag == true ){

        //vector < vector< Coord2 > > p = _boundary->polygons();
        map< unsigned int, Polygon2 >  p = _boundary->polygons();
        map< unsigned int, Polygon2 >::iterator itP = p.begin();
        for( ; itP != p.end(); itP++ ){

            QPolygonF polygon;
            for( unsigned int j = 0; j < itP->second.elements().size(); j++ ){
                polygon.append( QPointF( itP->second.elements()[j].x(), -itP->second.elements()[j].y() ) );
                // cerr << "x = " << p[i][j].x() << " y = " << p[i][j].y() << endl;
            }

            GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
            vector< double > rgb;
            pickBrewerColor( itP->first, rgb );
            QColor color( rgb[0]*255, rgb[1]*255, rgb[2]*255, 100 );
            itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
            itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
            itemptr->setPolygon( polygon );

            //cerr << vertexCoord[vd];
            _scene->addItem( itemptr );
        }
        cerr << "polygon.size() = " << p.size() << endl;
    }
}


void GraphicsView::_item_nodes( void )
{
    BoundaryGraph * g =  NULL;
    if( _is_simplifiedFlag == true )
        g = & _simplifiedBoundary->boundary();
    else
        g = & _boundary->boundary();

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


void GraphicsView::_item_edges( void )
{
    BoundaryGraph * g =  NULL;
    if( _is_simplifiedFlag == true )
        g = & _simplifiedBoundary->boundary();
    else
        g = & _boundary->boundary();

    BGL_FORALL_EDGES( ed, *g, BoundaryGraph ) {

        BoundaryGraph::vertex_descriptor vdS = source( ed, *g );
        BoundaryGraph::vertex_descriptor vdT = target( ed, *g );
        QPainterPath path;
        path.moveTo( (*g)[vdS].coordPtr->x(), -(*g)[vdS].coordPtr->y() );
        path.lineTo( (*g)[vdT].coordPtr->x(), -(*g)[vdT].coordPtr->y() );

        GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
        itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setPath( path );

        _scene->addItem( itemptr );
    }
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
void GraphicsView::initSceneItems ( void )
{
    // initialization
    _scene->clear();

    _item_polygons();
    _item_skeleton();
    //_item_seeds();
    _item_edges();
    _item_nodes();

    // cerr << "_scene.size = " << _scene->items().size() << endl;
}

//------------------------------------------------------------------------------
//	Event handlers
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsView::GraphicsView( QWidget *parent )
    : QGraphicsView( parent )
{
    setAutoFillBackground( true );
    setBackgroundBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );

	_scene = new QGraphicsScene( this );
    _scene->setSceneRect( -DEFAULT_WIDTH/2.0, -DEFAULT_HEIGHT/2.0, DEFAULT_WIDTH, DEFAULT_HEIGHT );  // x, y, w, h

    _is_simplifiedFlag = false;
    _is_polygonFlag = false;
    this->setScene( _scene );
}

GraphicsView::~GraphicsView()
{
}
