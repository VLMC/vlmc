#include <QKeyEvent>
#include <QKeySequence>
#include <QtDebug>

#include "KeyboardShortcutInput.h"

KeyboardShortcutInput::KeyboardShortcutInput( QWidget* parent ) :
        QPushButton( parent ),
        m_capturing( false ),
        m_current( 0 )
{
    m_timer = new QTimer( this );
    m_timer->setSingleShot( true );
    connect( m_timer, SIGNAL( timeout() ), this, SLOT( timeout() ) );
}

void    KeyboardShortcutInput::mousePressEvent( QMouseEvent* )
{
    m_capturing = true;
    setDown( true );
    memset( m_shortcuts, 0, sizeof( m_shortcuts ) );
    m_current = 0;
    setText( "" );
}

void    KeyboardShortcutInput::mouseReleaseEvent( QMouseEvent* )
{
}

void    KeyboardShortcutInput::release()
{
    m_capturing = false;
    setDown( false );
}

void    KeyboardShortcutInput::keyPressEvent( QKeyEvent* e )
{
    if ( m_capturing == false )
    {
        QPushButton::keyPressEvent( e );
        return ;
    }
    if ( e->modifiers() == Qt::NoModifier && e->key() == Qt::Key_Escape )
    {
        release();
    }
    else
    {
        m_timer->stop();
        int key = e->key();
        if ( key == Qt::Key_Control || key == Qt::Key_Meta ||
             key == Qt::Key_Shift || key == Qt::Key_Alt ||
             key == Qt::Key_AltGr )
            key = 0;
        int res = key | e->modifiers();
        m_shortcuts[m_current] = res;
        QKeySequence    seq( m_shortcuts[0], m_shortcuts[1], m_shortcuts[2], m_shortcuts[3] );
        setText( seq.toString() );
        if ( key != 0 )
        {
            m_timer->start( 500 );
            ++m_current;
        }
    }
}

void    KeyboardShortcutInput::timeout()
{
    release();
}
