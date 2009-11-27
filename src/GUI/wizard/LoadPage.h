#ifndef LOADPAGE_H
#define LOADPAGE_H

#include <QWizardPage>

class   QWizard;

class LoadPage : public QWizardPage
{
    Q_OBJECT
    public:
        LoadPage( QWidget* parent = 0 );
        ~LoadPage();
    private:
        void            build();

    private slots:
        void    loadProjectClicked();

    signals:
        void    projectButtonClicked();
};

#endif
