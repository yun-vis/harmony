
#include "ui/GraphicsView.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------
void GraphicsView::_item_nodes( void )
{
    BoundaryGraph * g =  NULL;
    if( _is_simplifiedFlag == true )
        g = & _simplifiedBoundary->g();
    else
        g = & _boundary->g();

    BGL_FORALL_VERTICES( vd, *g, BoundaryGraph ) {

        GraphicsBallItem *itemptr = new GraphicsBallItem;
        itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setRect( QRectF( (*g)[vd].coordPtr->x(), -(*g)[vd].coordPtr->y(), 10, 10 ) );

        //cerr << vertexCoord[vd];
        _scene->addItem( itemptr );
    }
}


void GraphicsView::_item_edges( void )
{
    BoundaryGraph * g =  NULL;
    if( _is_simplifiedFlag == true )
        g = & _simplifiedBoundary->g();
    else
        g = & _boundary->g();

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
    this->setScene( _scene );
}

GraphicsView::~GraphicsView()
{
}
