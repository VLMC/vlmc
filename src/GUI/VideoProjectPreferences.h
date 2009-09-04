#ifndef VIDEOPROJECTPREFERENCES_H
#define VIDEOPROJECTPREFERENCES_H

#include <QWidget>
#include <QSettings>
#include "PreferenceWidget.h"
#include "ui_VideoProjectPreferences.h"

class VideoProjectPreferences : public QWidget, public PreferenceWidget
{
    //Q_OBJECT
    public:
        VideoProjectPreferences(QWidget *parent = 0);
        ~VideoProjectPreferences();
        bool    load();
        void    save( QSettings& settings );
    private:
        Ui::VideoProjectPreferences m_ui;
};

#endif // VIDEOPROJECTPREFERENCES_H
