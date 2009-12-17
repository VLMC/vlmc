#ifndef GENERALPAGE_H
#define GENERALPAGE_H

#include "ui_GeneralPage.h"

class GeneralPage : public QWizardPage
{
    Q_OBJECT
public:
    GeneralPage( QWidget *parent = 0 );

protected:
    virtual void changeEvent( QEvent *e );
    virtual int nextId() const;
    virtual void initializePage();
    virtual bool validatePage();
    virtual void cleanupPage();

private slots:
    void openWorkspaceDirectory();
    void updateProjectLocation();
    void setWorkspaceStatus( bool valid );

private:
    Ui::GeneralPage ui;
    QPalette pValid;
    QPalette pInvalid;
};

#endif // GENERALPAGE_H
