#ifndef STACKVIEWCONTROLLER_H
#define STACKVIEWCONTROLLER_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QStack>

#include "StackViewNavController.h"
#include "ViewController.h"

class StackViewController : public QWidget
{
    Q_OBJECT

public:
    StackViewController( QWidget* parent = 0 );
    ~StackViewController();

    void        pushViewController( ViewController* viewController,
                                    bool animated = false );

    void        popViewController( bool animated = false );

private:
    StackViewNavController*     m_nav;
    QLabel*                     m_footer;
    QVBoxLayout*                m_layout;
    ViewController*             m_current;
    QStack<ViewController*>*    m_controllerStack;

public slots:
    void                        previous();

};

#endif // STACKVIEWCONTROLLER_H
