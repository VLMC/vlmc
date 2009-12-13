#include <QtDebug>

#include "SettingValue.h"
#include "SettingsManager.h"
#include "KeyboardShortcutHelper.h"

KeyboardShortcutHelper::KeyboardShortcutHelper( const QString& name, QWidget* parent, bool menu ) :
        QShortcut( parent ),
        m_name( name ),
        m_menu( menu )
{
    const SettingValue*   set = SettingsManager::getInstance()->getValue( "keyboard_shortcut", name );
    if ( m_menu == false )
        setKey( QKeySequence( set->get().toString() ) );
    connect( set, SIGNAL( changed( const QVariant& ) ), this, SLOT( shortcutUpdated( const QVariant& ) ) );
}

void    KeyboardShortcutHelper::shortcutUpdated( const QVariant& value )
{
    if ( m_menu == false )
        setKey( QKeySequence( value.toString() ) );
    else
        emit changed( m_name, value.toString() );
}
