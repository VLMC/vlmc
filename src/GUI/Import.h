#ifndef IMPORT_H
#define IMPORT_H

#include <QDialog>
#include "PreviewWidget.h"
#include "ImportBrowser.h"

namespace Ui
{
    class Import;
}

class Import : public QDialog
{
    Q_OBJECT
public:
    Import( QWidget *parent = 0 );
    ~Import();

protected:
    void changeEvent( QEvent *e );

private:
    Ui::Import*     m_ui;
    PreviewWidget*  m_previewWidget;
    ImportBrowser*  m_importBrowser;

private slots:
    void    setUIMetaData( QFileInfo fileInfos );
};

#endif // IMPORT_H
