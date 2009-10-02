#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include <QObject>

class ViewController : public QObject
{
    Q_OBJECT

public:
    //ViewController() {}
    virtual ~ViewController() {}


    virtual QWidget*            view() const = 0;
    virtual const QString       &title() const = 0;
};

#endif // VIEWCONTROLLER_H
