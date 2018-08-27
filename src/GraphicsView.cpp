
#include "GraphicsView.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------
void GraphicsView::_item_nodes( void )
{
    //Graph * g = & _simmetro->g();
    Graph * g = & _metro->g();

    VertexCoordMap          vertexCoord         = get( vertex_mycoord, *g );

    BGL_FORALL_VERTICES( vd, *g, Graph ) {

        GraphicsBallItem *itemptr = new GraphicsBallItem;
        itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
        itemptr->setRect( QRectF( vertexCoord[vd].x(), -vertexCoord[vd].y(), 10, 10 ) );

        //cerr << vertexCoord[vd];
        _scene->addItem( itemptr );
    }
}


void GraphicsView::_item_edges( void )
{
    //Graph * g = & _simmetro->g();
    Graph * g = & _metro->g();

    VertexCoordMap          vertexCoord         = get( vertex_mycoord, *g );

    BGL_FORALL_EDGES( ed, *g, Graph ) {

        VertexDescriptor vdS = source( ed, *g );
        VertexDescriptor vdT = target( ed, *g );
        QPainterPath path;
        path.moveTo( vertexCoord[vdS].x(), -vertexCoord[vdS].y() );
        path.lineTo( vertexCoord[vdT].x(), -vertexCoord[vdT].y() );

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
    this->setScene( _scene );
}

GraphicsView::~GraphicsView()
{
}
