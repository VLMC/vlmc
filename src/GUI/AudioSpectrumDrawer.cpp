#include "AudioSpectrumDrawer.h"

AudioSpectrumDrawer::AudioSpectrumDrawer( QPainter* painter, int height, int width, QList<int>* audioValueList )
    : m_painter( painter ), m_height( height ), m_width( width ), m_audioValueList( audioValueList )
{
    m_painter->setRenderHint( QPainter::Antialiasing, true );
    m_painter->setPen( QPen( QColor( 79, 106, 25 ), 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin ) );
}

void    AudioSpectrumDrawer::run()
{
    qreal max = 0;
    for( int i = 0; i < m_audioValueList->count(); i++ )
        if ( m_audioValueList->at(i) > max )
            max = m_audioValueList->at(i);

    for( int x = 0; x < m_audioValueList->count(); x++ )
    {
        if ( x <= m_height )
        {
            qreal y = ( (qreal)m_audioValueList->at(x) / max ) * 500;
            y -= 365;
            m_path.lineTo( x, y );
        }
    }
    m_painter->drawPath( m_path );
}
