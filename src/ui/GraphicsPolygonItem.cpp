#include "ui/GraphicsPolygonItem.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

QRectF GraphicsPolygonItem::boundingRect( void ) const
{
    //return rect();
}

void GraphicsPolygonItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                                 QWidget *widget )
{
    // draw boundary
	painter->setRenderHints( QPainter::Antialiasing );
	painter->setPen( pen() );
	painter->setBrush( brush() );
    painter->drawPolygon( polygon() );

    //cerr << "paint x = " << pos().x() << " y = " << pos().y() << endl;

    // Qt function
    //if ( option->state & QStyle::State_Selected )
    //	qt_graphicsItem_highlightSelected( this, painter, option );
}

int GraphicsPolygonItem::type( void ) const
{
    return QGraphicsItem::UserType;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsPolygonItem::GraphicsPolygonItem( QGraphicsItem *parent )
{
	//setFlag( QGraphicsItem::ItemIsSelectable );
    //setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	//setAcceptDrops( true );

    //pen().setJoinStyle( Qt::MiterJoin );
    pen().setJoinStyle( Qt::RoundJoin );
}


GraphicsPolygonItem::GraphicsPolygonItem( QPolygonF &, QGraphicsItem *parent )
{
}


GraphicsPolygonItem::~GraphicsPolygonItem()
{
}
