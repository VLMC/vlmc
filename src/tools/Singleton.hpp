/** \file
  * This file contain the templated singleton.
  * Class/struct to be singletonized just have to
  * iherit from this classe with the class/struct type in template
  * parameter.You have to do few other things, but you know your job,
  * isnt'it :) ?
  */

#ifndef SINGLETON_HPP
#define SINGLETON_HPP

template <typename T>
class       Singleton
{
public:
    static T*      getInstance()
    {
        if ( m_instance == NULL )
            m_instance = new T;
        return m_instance;
    }

    static void    destroyInstance()
    {
        if ( m_instance != NULL )
        {
            delete m_instance;
            m_instance = NULL;
        }
    }
protected:
    Singleton(){}
    virtual ~Singleton(){}
    //Not implemented since these methods should *NEVER* been called. If they do, it probably won't compile :)
    Singleton(const Singleton<T>&);
    Singleton<T>&   operator=(const Singleton<T>&);

private:
    static T*      m_instance;
};

template <typename T>
T*  Singleton<T>::m_instance = NULL;

#endif // SINGLETON_HPP
