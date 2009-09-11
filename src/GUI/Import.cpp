#include "Import.h"
#include "ui_Import.h"
#include "FileBrowser.h"
#include "PreviewWidget.h"
#include "ClipRenderer.h"

Import::Import(QWidget *parent) :
    QDialog(parent), ui(new Ui::Import)
{
    ui->setupUi(this);
    ui->FileBrowserWidget = new FileBrowser(this);
    ui->PreviewWidget = new PreviewWidget( new ClipRenderer, this );
}

Import::~Import()
{
    delete ui;
}

void Import::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
