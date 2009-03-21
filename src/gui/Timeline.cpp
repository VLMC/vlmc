#include "Timeline.h"

Timeline::Timeline(QWidget *parent) :
    QWidget(parent){
    m_ui.setupUi(this);
}

void Timeline::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui.retranslateUi(this);
        break;
    default:
        break;
    }
}
