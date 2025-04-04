#include <iostream>
#include "../include/MPointer.h"

int main() {
    MPointer<int>::Init("http://localhost:8080");

    MPointer<int> ptr1 = MPointer<int>::New(42);  // ✅ Ahora se crea con 42
    MPointer<int> ptr2 = MPointer<int>::New();

    ptr2 = ptr1;  // ✅ Copia el ID y hace incref

    std::cout << "ptr1: " << *ptr1 << " | ptr2: " << *ptr2 << std::endl;

    return 0;
}
