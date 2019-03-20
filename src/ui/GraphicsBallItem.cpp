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
	_font = QFont( "Arial", _font_size, QFont::Bold, false );

    // draw boundary
    //rect().setX( rect().x() - sx );
    //rect().setY( rect().y() - sy );
    QRectF fineRect( rect() );
    fineRect.setX( fineRect.x()-_radius );
    fineRect.setY( fineRect.y()-_radius );
    fineRect.setWidth( 2.0*_radius );
    fineRect.setHeight( 2.0*_radius );
	painter->setRenderHints( QPainter::Antialiasing );
	painter->setPen( pen() );
	painter->setBrush( brush() );
	painter->setFont( _font );

	painter->drawEllipse( fineRect );

	//cerr << "id = " << _id << endl;
	if( _textOn == true ){
		painter->drawText( rect().x()+5, rect().y()-5, QString::fromStdString( to_string( _id ) ) );
		//painter->drawText( rect().x()+10, rect().y()-10, _name );
	}

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

	_textOn = false;
	_radius = 5;
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
