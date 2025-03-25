#include "../include/MPointer.h"
#include <iostream>

int main() {
    MPointer<int>::Init("http://localhost:8080");

    auto ptr = MPointer<int>::New(); 
    ptr = 123;                     
    int valor = *ptr;              

    std::cout << "Valor remoto obtenido: " << valor << std::endl;

    return 0;
}
