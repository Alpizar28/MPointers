#include <iostream>
#include <exception>
#include "../include/MPointer.h"
#include <cpr/cpr.h>

void forceDump(const MPointer<int>& ptr, const std::string& nombre) {
    int id = &ptr;
    cpr::Post(cpr::Url{"http://localhost:8080/forceDump/" + std::to_string(id)});
    std::cout << "📄 Dump forzado de " << nombre << " (ID: " << id << ")\n";
}

int main() {
    try {
        std::cout << "=== Prueba MPointer ===\n";

        MPointer<int>::Init("http://localhost:8080");

        // Crear varios punteros
        MPointer<int> a = MPointer<int>::New();
        MPointer<int> b = MPointer<int>::New(sizeof(int), 42);
        MPointer<int> c = MPointer<int>::New();

        std::cout << "Asignando valores...\n";
        a = 111;
        b = 222;
        c = 333;

        forceDump(a, "a");
        forceDump(b, "b");
        forceDump(c, "c");

        std::cout << "Reasignando c = b...\n";
        c = b; // b y c ahora apuntan al mismo bloque

        forceDump(c, "c");

        std::cout << "Reasignando b = a...\n";
        b = a; // c queda apuntando a lo que era b, b ahora apunta a a

        forceDump(a, "a");
        forceDump(b, "b");
        forceDump(c, "c");

        std::cout << "📥 Leyendo valores...\n";
        std::cout << "*a: " << *a << " (esperado: 111)\n";
        std::cout << "*b: " << *b << " (esperado: 111)\n";
        std::cout << "*c: " << *c << " (esperado: 222)\n";

        std::cout << "🎯 Validación cruzada terminada.\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << "\n";
        return 1;
    }
}
