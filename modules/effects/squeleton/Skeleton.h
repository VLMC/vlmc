#ifndef SKELETON_H
#define SKELETON_H

#include "GenericEffect.h"

class   Skeleton : public GenericEffect
{
    Q_CLASSINFO("authors", "Clement CHAVANCE")
    Q_CLASSINFO("mail", "kinder@vlmc.org")
    Q_CLASSINFO("name", "Skeleton")
    Q_CLASSINFO("www", "www.vlmc.org")
    Q_CLASSINFO("version", "0.1")
    Q_CLASSINFO("description", "Exemple of a skeleton plugin")
    Q_CLASSINFO("category", "Exemples")
    public:
        Skeleton();
        ~Skeleton();

        void    render( void );
};

extern "C"
{
    Skeleton*   
}

#endif
