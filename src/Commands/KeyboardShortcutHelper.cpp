#include <QtDebug>

#include "SettingValue.h"
#include "SettingsManager.h"
#include "KeyboardShortcutHelper.h"

KeyboardShortcutHelper::KeyboardShortcutHelper( const QString& name, QWidget* parent, bool menu ) :
        QShortcut( parent ),
        m_name( name ),
        m_menu( menu )
{
    if ( m_menu == false )
    {
        QString set = VLMC_GET_STRING( name );
        setKey( QKeySequence( set ) );
    }
    SettingsManager::getInstance()->watchValue( name, this,
                                                SLOT( shortcutUpdated( const QVariant& ) ),
                                                SettingsManager::Vlmc );
}

void    KeyboardShortcutHelper::shortcutUpdated( const QVariant& value )
{
    if ( m_menu == false )
        setKey( QKeySequence( value.toString() ) );
    else
        emit changed( m_name, value.toString() );
}
