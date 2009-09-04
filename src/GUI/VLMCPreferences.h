#ifndef VLMCPREFERENCES_H
#define VLMCPREFERENCES_H

#include <QWidget>
#include <QSettings>
#include "PreferenceWidget.h"
#include "ui_VLMCPreferences.h"

class VLMCPreferences : public QWidget, public PreferenceWidget
{
    //Q_OBJECT
    public:
        VLMCPreferences(QWidget *parent = 0);
        ~VLMCPreferences();
        bool    load();
        void    save( QSettings& settings );
    private:
        Ui::VLMCPreferences m_ui;
};

#endif // VLMCPREFERENCES_H
