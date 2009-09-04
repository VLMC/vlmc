#include "AudioProjectPreferences.h"

#include "QDebug"

AudioProjectPreferences::AudioProjectPreferences( QWidget *parent ) :
    QWidget( parent )
{
    this->setWidget( this );
    m_ui.setupUi( this );
}

AudioProjectPreferences::~AudioProjectPreferences() { }

bool    AudioProjectPreferences::load()
{
    qDebug() << "Loading AudioProjectPreferences values";
    return true;
}

void    AudioProjectPreferences::save( QSettings& settings )
{
    settings.beginGroup( "Project" );
    //settings.setValue( "outputFPS", m_ui.outputFPS->text() );
    settings.endGroup();
}
