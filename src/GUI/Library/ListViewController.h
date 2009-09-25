#ifndef LISTVIEWCONTROLLER_H
#define LISTVIEWCONTROLLER_H

#include <QWidget>
#include <QListView>
#include <QAbstractItemModel>
#include <QScrollArea>

#include "ViewController.h"
#include "StackViewController.h"


class ListViewController : public ViewController
{

public:
    ListViewController( StackViewController* nav );
    ~ListViewController();

    QWidget*        view() const;
    const QString&  title() const;

    void            addCell( QWidget* cell );




private:
    QString*                    m_title;
    QScrollArea*                m_scrollArea;
    QWidget*                    m_container;
    QVBoxLayout*                m_layout;
    StackViewController*        m_nav;
};

#endif // LISTVIEWCONTROLLER_H
