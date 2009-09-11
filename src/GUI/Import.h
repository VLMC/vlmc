#ifndef IMPORT_H
#define IMPORT_H

#include <QDialog>

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
    Ui::Import *m_ui;
};

#endif // IMPORT_H
