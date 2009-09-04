#ifndef AUDIOPROJECTPREFERENCES_H
#define AUDIOPROJECTPREFERENCES_H

#include <QWidget>
#include <QSettings>
#include "PreferenceWidget.h"
#include "ui_AudioProjectPreferences.h"

class AudioProjectPreferences : public QWidget, public PreferenceWidget
{
    //Q_OBJECT
    public:
        AudioProjectPreferences(QWidget *parent = 0);
        ~AudioProjectPreferences();
        bool    load();
        void    save( QSettings& settings );
    private:
        Ui::AudioProjectPreferences m_ui;
};

#endif // AUDIOPROJECTPREFERENCES_H
