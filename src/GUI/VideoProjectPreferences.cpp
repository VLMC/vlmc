#include "VideoProjectPreferences.h"

#include "QDebug"

VideoProjectPreferences::VideoProjectPreferences(QWidget *parent) :
    QWidget(parent)
{
    this->setWidget( this );
    m_ui.setupUi( this );
}

VideoProjectPreferences::~VideoProjectPreferences() { }

bool    VideoProjectPreferences::load()
{
    qDebug() << "Loading VideoProjectPreferences values";
    return true;
}

void    VideoProjectPreferences::save( QSettings& settings )
{
    settings.beginGroup( "Project" );
    //settings.setValue( "outputFPS", m_ui.outputFPS->text() );
    settings.endGroup();
}
