#include <stdio.h>
#include <iostream>

 void* operator new(size_t size)
 {
//     std::cout << "Allocating " << size << "bytes\n";

     return malloc(size);
 }

#include "App.h"

int main()
{

    App app;
    app.Run();

    return 0;
}
