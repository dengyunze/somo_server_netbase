#pragma once

template<typename T>
class Singleton
{
public:
    static T* Ins()
    {
        static T _ins;
        return &_ins;
    }
};

