#include "ui/GraphicsVertexItem.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
void GraphicsVertexItem::init ( void )
{
}

QRectF GraphicsVertexItem::boundingRect( void ) const
{
	return rect();
}

void GraphicsVertexItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                              QWidget *widget )
{
    QFontMetrics metrics( _font );
    double sx = metrics.width( _name );
    double sy = 0.5*metrics.height();

    QRectF fineRect( rect() );
    fineRect.setX( rect().x() - 0.5*sx - MIN_NEIGHBOR_DISTANCE );
    fineRect.setY( rect().y() - sy - MIN_NEIGHBOR_DISTANCE );
    fineRect.setWidth( sx + 2.0*MIN_NEIGHBOR_DISTANCE );
    fineRect.setHeight( 2.0*sy + 2.0*MIN_NEIGHBOR_DISTANCE );

	painter->setRenderHints( QPainter::Antialiasing );
	painter->setPen( pen() );
	painter->setBrush( brush() );
    painter->drawRect( fineRect );

	//cerr << "id = " << _id << endl;
	//painter->drawText( rect().x()+10, rect().y()-10, QString::fromStdString( to_string( _id ) ) );
    painter->drawText( fineRect.x()+0.5*( fineRect.width()-sx ) - 0.5*MIN_NEIGHBOR_DISTANCE,
                       fineRect.y()+0.5*( fineRect.height()+0.5*sy ) + MIN_NEIGHBOR_DISTANCE, _name ); // (x ,y) must be left-upper corner

    //cerr << "paint x = " << pos().x() << " y = " << pos().y() << endl;

    // Qt function
    //if ( option->state & QStyle::State_Selected )
    //	qt_graphicsItem_highlightSelected( this, painter, option );
    // cerr << "painting ball..." << endl;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsVertexItem::GraphicsVertexItem( QGraphicsItem *parent )
{
	//setFlag( QGraphicsItem::ItemIsSelectable );
    //setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	//setAcceptDrops( true );

    _radius = 10;
    _font = QFont( "Arial", 12, QFont::Normal, false );
}

GraphicsVertexItem::GraphicsVertexItem( const QRectF &rect, QGraphicsItem *parent )
{
	setRect( rect );
}

GraphicsVertexItem::GraphicsVertexItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent )
{
	setRect( QRectF( x, y, w, h ) );
}

GraphicsVertexItem::~GraphicsVertexItem()
{
}
