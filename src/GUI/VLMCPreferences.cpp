#include "VLMCPreferences.h"

#include "QDebug"

VLMCPreferences::VLMCPreferences(QWidget *parent) :
    QWidget(parent)
{
    this->setWidget( this );
    m_ui.setupUi(this);
}

VLMCPreferences::~VLMCPreferences() { }

bool    VLMCPreferences::load()
{
    qDebug() << "Loading VLMCPreferences values";
    return true;
}

void    VLMCPreferences::save( QSettings& settings )
{
    settings.beginGroup( "Project" );
    //settings.setValue( "outputFPS", m_ui.outputFPS->text() );
    settings.endGroup();
}
