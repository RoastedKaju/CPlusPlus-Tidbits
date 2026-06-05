#ifndef GRAPHS_H
#define GRAPHS_H

#include <iostream>

namespace graphs
{
    class A
    {
    public:
        A() { std::cout << "ctor" << std::endl; }
        ~A() { std::cout << "dtor" << std::endl; }

        A(const A &rhs) { std::cout << "copy ctor" << std::endl; }
        A &operator=(const A &rhs)
        {
            std::cout << "copy assign" << std::endl;
            return *this;
        }

        A(A &&rhs) { std::cout << "move ctor" << std::endl; }
        A &operator=(A &&rhs)
        {
            std::cout << "move assign" << std::endl;
            return *this;
        }
    };

    void graphsExampleDriver()
    {
        
    }
}

#endif