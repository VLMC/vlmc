#ifndef PROJECTPREFERENCES_H
#define PROJECTPREFERENCES_H

#include <QSettings>
#include <QWidget>

#include "ui_ProjectPreferences.h"
#include "PreferenceWidget.h"

class   ProjectPreferences : public QWidget, public PreferenceWidget
{
    //Q_OBJECT
    public:
        ProjectPreferences( QWidget* parent = 0 );
        ~ProjectPreferences();
        bool    load();
        void    save( QSettings& settings );
    private:
        Ui::ProjectPreferences  m_ui;
};

#endif
