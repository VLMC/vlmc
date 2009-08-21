#ifndef PROJECTPREFERENCES_H
#define PROJECTPREFERENCES_H

#include "ui_ProjectPreferences.h"

class   ProjectPreferences : public PreferenceWidget
{
    //Q_OBJECT
    public:
        ProjectPreferences( QWidget* parent = 0 );
        ~ProjectPreferences();
        bool    load();
        bool    save();
    private:
        Ui::ProjectPreferences  m_ui;
};

#endif
