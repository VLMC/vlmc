#ifndef TIMELINE_H
#define TIMELINE_H

#include "ui_Timeline.h"

class Timeline : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY( Timeline )
public:
    explicit Timeline( QWidget *parent = 0 );

protected:
    virtual void changeEvent( QEvent *e );

private:
    Ui::Timeline m_ui;

};

#endif // TIMELINE_H
