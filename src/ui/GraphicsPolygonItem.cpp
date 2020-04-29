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

QRectF GraphicsPolygonItem::boundingRect( void ) const {
	QRectF rect;
	return rect;
}

void GraphicsPolygonItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                                 QWidget *widget ) {
	// draw boundary
	painter->setRenderHints( QPainter::Antialiasing );
	painter->setPen( pen() );
	painter->setBrush( brush() );
	painter->drawPolygon( polygon() );
	
	const QPolygonF &p = polygon();
	if( _textOn == true ) {
		
		painter->setPen( QPen( QColor( 0, 0, 0, 255 ), 4 ) );
		for( unsigned int i = 0; i < p.size(); i++ ) {
			painter->drawText( p.at( i ).x() + 5, p.at( i ).y() - 5, QString::fromStdString( to_string( _id ) ) );
		}
		//painter->drawText( rect().x()+10, rect().y()-10, _name );
	}

#ifdef SKIP
	const QPolygonF &p = polygon();
	for( unsigned int i = 0; i < p.size(); i++ ){
		//cerr << p.at(i) << endl;
		int shift = (rand()%6+1)*3;
		painter->drawText( p.at(i).x()+shift, p.at(i).y()-shift, QString::fromStdString( to_string( i ) ) );
	}
#endif // SKIP
	
	//cerr << "paint x = " << pos().x() << " y = " << pos().y() << endl;
	
	// Qt function
	//if ( option->state & QStyle::State_Selected )
	//	qt_graphicsItem_highlightSelected( this, painter, option );
}

int GraphicsPolygonItem::type( void ) const {
	return QGraphicsItem::UserType;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsPolygonItem::GraphicsPolygonItem( QGraphicsItem *parent ) {
	//setFlag( QGraphicsItem::ItemIsSelectable );
	//setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	//setAcceptDrops( true );
	
	//pen().setJoinStyle( Qt::MiterJoin );
	pen().setJoinStyle( Qt::RoundJoin );
	_textOn = false;
}


GraphicsPolygonItem::GraphicsPolygonItem( QPolygonF &, QGraphicsItem *parent ) {
}


GraphicsPolygonItem::~GraphicsPolygonItem() {
}
