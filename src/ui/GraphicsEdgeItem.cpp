#include "ui/GraphicsEdgeItem.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Private functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------

QRectF GraphicsEdgeItem::boundingRect( void ) const
{
    return path().controlPointRect();
}

void GraphicsEdgeItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                              QWidget *widget )
{
    // draw boundary
	painter->setRenderHints( QPainter::Antialiasing );
	painter->setPen( pen() );
	painter->setBrush( brush() );
    painter->drawPath( path() );

    // draw text
    if( _textOn == true ){
        painter->setPen( pen() );
        painter->setFont( QFont( "Arial", 12, QFont::Bold, false ) );
        painter->drawText( path().boundingRect().x()+0.5*( path().boundingRect().width() ),
                           path().boundingRect().y()+0.5*( path().boundingRect().height() ),
                           QString::fromStdString( to_string( _id ) ) );
    }

    //cerr << "paint x = " << pos().x() << " y = " << pos().y() << endl;

    // Qt function
    //if ( option->state & QStyle::State_Selected )
    //	qt_graphicsItem_highlightSelected( this, painter, option );
}

int GraphicsEdgeItem::type( void ) const
{
    return QGraphicsItem::UserType;
}

//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
GraphicsEdgeItem::GraphicsEdgeItem( QGraphicsItem *parent )
{
	//setFlag( QGraphicsItem::ItemIsSelectable );
    //setFlag( QGraphicsItem::ItemIsMovable );
	//setFlag( QGraphicsItem::ItemSendsGeometryChanges );
	//setAcceptDrops( true );

    //pen().setJoinStyle( Qt::MiterJoin );
    pen().setJoinStyle( Qt::RoundJoin );

    _id = 0;
    _name = "";
    _textOn = false;
}

GraphicsEdgeItem::GraphicsEdgeItem( const QPainterPath &path, QGraphicsItem *parent )
{
    if ( !path.isEmpty() ) setPath( path );
}


GraphicsEdgeItem::~GraphicsEdgeItem()
{
}
