#include "StackViewController.h"

StackViewController::StackViewController( QWidget* parent ) :
        QWidget( parent ), m_current( 0 )
{
    m_nav     = new StackViewNavController( this );
    m_footer  = new QLabel( "Footer" );
    m_layout  = new QVBoxLayout;
    m_controllerStack = new QStack<ViewController*>();

    m_footer->setAlignment( Qt::AlignCenter );
    QObject::connect( m_nav->previousButton(), SIGNAL( clicked() ),
                     this, SLOT( previous() ) );

    m_layout->addWidget( m_nav );
    m_layout->addWidget( m_footer );

    parent->setLayout( m_layout );
}

StackViewController::~StackViewController()
{
    delete m_nav;
    delete m_footer;
    delete m_layout;
    delete m_controllerStack;
}

void    StackViewController::pushViewController( ViewController* viewController,
                                                 bool animated )
{
    animated = false;

    if ( m_current )
    {
        m_layout->removeWidget( m_current->view() );
        m_current->view()->hide();
        m_controllerStack->push( m_current );

        m_nav->previousButton()->setHidden( false );
        m_nav->previousButton()->setText( "< " + m_current->title() );
    }

    m_current = viewController;
    m_nav->setTitle( m_current->title() );
    m_layout->insertWidget( 1, m_current->view() );
}

void        StackViewController::popViewController( bool animated )
{
    animated = false;

    if ( !m_controllerStack->size() )
        return ;

    m_layout->removeWidget( m_current->view() );
    m_current->view()->hide();
    m_current = m_controllerStack->pop();

    m_nav->setTitle( m_current->title() );
    m_layout->insertWidget( 1, m_current->view() );
    m_current->view()->setHidden( false );

    if ( !m_controllerStack->size() )
        m_nav->previousButton()->setHidden( true );
    else
    {
       m_nav->previousButton()->setText( "< " +
           m_controllerStack->value( m_controllerStack->size() - 1 )->title() );
    }
}

void        StackViewController::previous()
{
    popViewController();
}
