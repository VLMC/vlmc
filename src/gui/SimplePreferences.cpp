#include <QPushButton>
#include <QLabel>


#include "SimplePreferences.h"

/********************************************************************************
                            PreferencesPanel
********************************************************************************/

const int   PreferencesPanel::ICON_HEIGHT = 64;

PreferencesPanel::PreferencesPanel( QWidget* parent )
    : QWidget( parent ),
    m_nbPanel(0)
{
    m_layout = new QVBoxLayout( this );
    m_buttonGroup = new QButtonGroup( this );

    QObject::connect( m_buttonGroup, SIGNAL( buttonClicked( int ) ),
                      this, SLOT( switchPanel( int ) ) );
}

PreferencesPanel::~PreferencesPanel()
{
    delete m_layout;
    delete m_buttonGroup;
}

void    PreferencesPanel::addPanel( const QString& icon,
                                    const QString& label,
                                    const QString& name)
{
    QPushButton* button = new QPushButton( this );

    ++m_nbPanel;
    //button->setIcon(IconPath + icon);
    button->setText( label );
    button->resize( ICON_HEIGHT + 6, ICON_HEIGHT + 6 );
    m_panels.insert( m_nbPanel, name );
    m_buttonGroup->addButton( button, m_nbPanel );
    m_layout->addWidget( button );
}

void    PreferencesPanel::switchPanel( int panel )
{
    if ( !m_panels.contains( panel ) )
        return ;
    emit panelSwitched( m_panels.value( panel ) );
}

/********************************************************************************
                             SimplePreferences
********************************************************************************/

SimplePreferences*  SimplePreferences::m_instance = 0;

SimplePreferences*  SimplePreferences::getInstance()
{
    if ( 0 == SimplePreferences::m_instance )
        SimplePreferences::m_instance = new SimplePreferences();
    return ( SimplePreferences::m_instance );
}

void                SimplePreferences::releaseInstance()
{
    if ( 0 != SimplePreferences::m_instance )
        delete SimplePreferences::m_instance;
    return ;
}

QWidget*    SimplePreferences::getWidget( const QString& name ) const
{
    if ( m_widgets.contains( name ) )
        return ( m_widgets.value( name ) );
    return ( NULL );
}

//TODO : see if the widget MUST have a fixed size, or if the window can dynamicaly
//adjust to the size of the biggest Widget.
void        SimplePreferences::addWidget( const QString& name,
                                          QWidget* widget,
                                          const QString& icon,
                                          const QString& label)
{
    //If there's already a widget with the same name, remove it
    if ( m_widgets.contains( name ) )
       m_widgets.remove( name );
    m_widgets.insert( name, widget );
    m_panel->addPanel( icon, label, name );
}

SimplePreferences::SimplePreferences()
{
    m_panel = new PreferencesPanel( this );
    //Build Ui here
}

SimplePreferences::~SimplePreferences()
{
    QHash<QString, QWidget*>::iterator     end = m_widgets.end();
    QHash<QString, QWidget*>::iterator     it = m_widgets.begin();
    for ( ; it != end; ++it )
        delete it.value();
}

void    SimplePreferences::switchWidget( const QString& name )
{
    //Hide the current widget and show the new one.
    
    if ( !m_widgets.contains( name ) )
        return ;
    m_currentWidget->hide();

    QWidget*    wid = m_widgets.value( name );

    m_currentWidget = wid;
    m_currentWidget->show();
}
