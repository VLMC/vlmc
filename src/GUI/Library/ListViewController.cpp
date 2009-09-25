#include "ListViewController.h"


#include "MediaCellView.h"
#include <QPushButton>

ListViewController::ListViewController( StackViewController* nav ) : m_nav( nav )
{
    m_title = new QString( "Media List 42" );

    m_scrollArea = new QScrollArea();
    m_layout = new QVBoxLayout();
    m_container = new QWidget();

    m_layout->setAlignment( Qt::AlignTop );
    m_layout->setSpacing( 0 );
    m_container->setLayout( m_layout );
    m_scrollArea->setWidget( m_container );
    m_scrollArea->setWidgetResizable( true );
}

ListViewController::~ListViewController()
{

}

// ViewController method implementation

const QString&      ListViewController::title() const
{
    return *m_title;
}

QWidget*            ListViewController::view() const
{
    return m_scrollArea;
}

void                ListViewController::addCell( QWidget* cell )
{
    m_layout->addWidget( cell );
}
