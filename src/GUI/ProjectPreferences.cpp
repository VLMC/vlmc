//DEBUG
#include <QtDebug>
//!DEBUG

#include "ui_ProjectPreferences.h"
#include "ProjectPreferences.h"
#include "PreferenceWidget.h"


ProjectPreferences::ProjectPreferences( QWidget* parent )
    : QWidget( parent )
{
    this->setWidget( this );
    m_ui.setupUi( this );
}

ProjectPreferences::~ProjectPreferences() { }


bool    ProjectPreferences::load()
{
    qDebug() << "Loading ProjectPreferences values";
    return true;
}

void    ProjectPreferences::save( QSettings& settings )
{
    settings.beginGroup( "Project" );
    //settings.setValue( "outputFPS", m_ui.outputFPS->text() );
    settings.endGroup();
}
