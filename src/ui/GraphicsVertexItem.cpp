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
void GraphicsVertexItem::init( void ) {
}

QRectF GraphicsVertexItem::boundingRect( void ) const {
	return _fineRect;
}

void GraphicsVertexItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                                QWidget *widget ) {
	_font = QFont( "Arial", _font_size, QFont::Bold, false );
	
	QFontMetrics metrics( _font );
	double sx = metrics.width( _name );
	double sy = 0.5 * metrics.height();
	
	_fineRect = rect();
	_fineRect.setX( rect().x() - 0.5 * sx - MIN_NEIGHBOR_DISTANCE );
	_fineRect.setY( rect().y() - sy - MIN_NEIGHBOR_DISTANCE );
	_fineRect.setWidth( sx + 2.0 * MIN_NEIGHBOR_DISTANCE );
	_fineRect.setHeight( 2.0 * sy + 2.0 * MIN_NEIGHBOR_DISTANCE );
	
	painter->setRenderHints( QPainter::Antialiasing );
	painter->setPen( pen() );
	painter->setBrush( brush() );

	if( _vtype == VERTEX_REACTION ) {
		painter->drawRect( _fineRect );
	}
	else if( _vtype == VERTEX_METABOLITE ) {
		painter->drawRoundedRect( _fineRect, 5, 5, Qt::AbsoluteSize );
	}
	else if( _vtype == VERTEX_DEFAULT ) {
        painter->drawEllipse( QRectF( rect().x()-5, rect().y()-5, 10, 10 ) );
	}
	else {
		cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
		assert( false );
	}

	painter->setFont( _font );
	//cerr << "id = " << _id << endl;
	//painter->drawText( rect().x()+10, rect().y()-10, QString::fromStdString( to_string( _id ) ) );
	if( _textOn == true ) {
		painter->drawText( _fineRect.x() + 0.5 * ( _fineRect.width() - sx ),
		                   _fineRect.y() + 0.5 * ( _fineRect.height() + 0.5 * sy ) + MIN_NEIGHBOR_DISTANCE,
		                   _name ); // (x ,y) must be left-upper corner
	}
	//painter->drawText( fineRect.x()+0.5*( fineRect.width()-sx ) - 0.5*MIN_NEIGHBOR_DISTANCE,
	//                   fineRect.y()+0.5*( fineRect.height()+0.5*sy ) + MIN_NEIGHBOR_DISTANCE, _name );
	
	//cerr << "paint x = " << pos().x() << " y = " << pos().y() << endl;
	
	// Qt function
	//if ( option->state & QStyle::State_Selected )
	//	qt_graphicsItem_highlightSelected( this, painter, option );
	// cerr << "painting ball..." << endl;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsVertexItem::GraphicsVertexItem( QGraphicsItem *parent ) {
	//setFlag( QGraphicsItem::ItemIsSelectable );
	//setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	//setAcceptDrops( true );
	
	//_radius = 10;
	_vtype = VERTEX_DEFAULT;
	_textOn = true;
}

GraphicsVertexItem::GraphicsVertexItem( const QRectF &rect, QGraphicsItem *parent ) {
	setRect( rect );
}

GraphicsVertexItem::GraphicsVertexItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent ) {
	setRect( QRectF( x, y, w, h ) );
}

GraphicsVertexItem::~GraphicsVertexItem() {
}
