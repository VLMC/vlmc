#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

class ClickableLabel : public QLabel
{
    Q_OBJECT

    public:
        ClickableLabel( QWidget* parent = NULL );
        void    mousePressEvent( QMouseEvent* ev );

    signals:
        void    clicked( QWidget* sender, QMouseEvent* ev );
};

#endif // CLICKABLELABEL_H
