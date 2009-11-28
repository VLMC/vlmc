#ifndef STACKVIEWNAVCONTROLLER_H
#define STACKVIEWNAVCONTROLLER_H

#include <QWidget>
#include <QPushButton>

namespace Ui
{
    class StackViewNavController;
}

class StackViewNavController : public QWidget
{
    Q_OBJECT
public:
    StackViewNavController( QWidget *parent = 0 );
    ~StackViewNavController();

    void            setTitle( const QString& title );
    QPushButton*    previousButton() const;

protected:
    void changeEvent( QEvent *e );

private:
    Ui::StackViewNavController *m_ui;
};

#endif // STACKVIEWNAVCONTROLLER_H
