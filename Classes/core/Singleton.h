
#ifndef _SINGLETON_H_
#define _SINGLETON_H_

template < typename T>
class Singleton
{
private:
    static T* ms_Singleton;
protected:
    Singleton(void){}
    ~Singleton(void){}
    inline static void FreeSingleton(void)
    {
        if( ms_Singleton )
            delete ms_Singleton;
    }
public:
    inline static T* sharedSingleton()
    {
        if( 0 == ms_Singleton )
        {
            ms_Singleton = new T;
        }
        return ms_Singleton;
    }
    inline static T* getInstance()
    {
        if( 0 == ms_Singleton )
        {
            ms_Singleton = new T;
        }
        return ms_Singleton;
    }
    inline static T& getSingleton()
    {
        if( 0 == ms_Singleton )
        {
            ms_Singleton = new T;
        }
        return *ms_Singleton;
    }
};

template <typename T> T* Singleton<T>::ms_Singleton = 0;

template <class T>
class singleton : private T
{
private:
    singleton();
    ~singleton();
public:
    static T &instance();
};

template <class T>
inline singleton<T>::singleton()
{
    /* no-op */
}
template <class T>
inline singleton<T>::~singleton()
{
    /* no-op */
}

template <class T>
/*static*/ T &singleton<T>::instance()
{
    //function-local static to force this to work correctly at static
    //initialization time
    static singleton<T> s_oT;
    return (s_oT);
}


#endif
