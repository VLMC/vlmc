#include <QtDebug>

#include "SettingValue.h"
#include "SettingsManager.h"
#include "KeyboardShortcutHelper.h"

KeyboardShortcutHelper::KeyboardShortcutHelper( const QString& name, QWidget* parent, bool menu ) :
        QShortcut( parent ),
        m_name( name ),
        m_menu( menu )
{
    QVariant   set = SettingsManager::getInstance()->value( name );
    if ( m_menu == false )
        setKey( QKeySequence( set.toString() ) );
    SettingsManager::getInstance()->watchValue( "keyboard/" + name, this,
                                                SLOT( shortcutUpdated( const QVariant& ) ),
                                                SettingsManager::QSett );
}

void    KeyboardShortcutHelper::shortcutUpdated( const QVariant& value )
{
    if ( m_menu == false )
        setKey( QKeySequence( value.toString() ) );
    else
        emit changed( m_name, value.toString() );
}
