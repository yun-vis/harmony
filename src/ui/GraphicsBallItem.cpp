#include "ui/GraphicsBallItem.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
void GraphicsBallItem::init ( void )
{

}

QRectF GraphicsBallItem::boundingRect( void ) const
{
	return rect();
}

void GraphicsBallItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                              QWidget *widget )
{
    int _radius = 10;

    // draw boundary
    //rect().setX( rect().x() - sx );
    //rect().setY( rect().y() - sy );
    QRectF fineRect( rect() );
    fineRect.setX( fineRect.x()-_radius/2.0 );
    fineRect.setY( fineRect.y()-_radius/2.0 );
    fineRect.setWidth( _radius );
    fineRect.setHeight( _radius );
	painter->setRenderHints( QPainter::Antialiasing );
	painter->setPen( pen() );
	painter->setBrush( brush() );
	painter->drawEllipse( fineRect );

    //cerr << "paint x = " << pos().x() << " y = " << pos().y() << endl;

    // Qt function
    //if ( option->state & QStyle::State_Selected )
    //	qt_graphicsItem_highlightSelected( this, painter, option );
    // cerr << "painting ball..." << endl;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsBallItem::GraphicsBallItem( QGraphicsItem *parent )
{
	//setFlag( QGraphicsItem::ItemIsSelectable );
    //setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	//setAcceptDrops( true );
}

GraphicsBallItem::GraphicsBallItem( const QRectF &rect, QGraphicsItem *parent )
{
	setRect( rect );
}

GraphicsBallItem::GraphicsBallItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent )
{
	setRect( QRectF( x, y, w, h ) );
}

GraphicsBallItem::~GraphicsBallItem()
{
}
