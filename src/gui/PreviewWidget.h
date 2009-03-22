#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QtGui/QDialog>

namespace Ui {
    class PreviewWidget;
}

class PreviewWidget : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(PreviewWidget)
public:
    explicit PreviewWidget(QWidget *parent = 0);
    virtual ~PreviewWidget();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::PreviewWidget *m_ui;
};

#endif // PREVIEWWIDGET_H
