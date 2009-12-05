#ifndef LOADPAGE_H
#define LOADPAGE_H

#include <QWizardPage>
#include "ProjectWizard.h"
#include "ui_WelcomePage.h"

class   QWizard;

class WelcomePage : public QWizardPage
{
    Q_OBJECT
    public:
        WelcomePage( QWidget* parent = 0 );
        ~WelcomePage() { }

        int nextId() const;

    private:
        Ui::WelcomePage m_ui;
};

#endif
