#ifndef LOADPAGE_H
#define LOADPAGE_H

#include <QWizardPage>
#include "ProjectWizard.h"
#include "ui_LoadPage.h"

class   QWizard;

class LoadPage : public QWizardPage
{
    Q_OBJECT
    public:
        LoadPage( QWidget* parent = 0 );
        ~LoadPage() { }

        int nextId() const;

    private:
        Ui::LoadPage    m_ui;
};

#endif
