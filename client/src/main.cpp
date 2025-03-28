#include <iostream>
#include <exception>
#include "../include/MPointer.h"

int main() {
    try {
        setenv("CURL_VERBOSE", "1", 1);

        std::cout << "=== Iniciando prueba MPointer ===" << std::endl;
        
        // 1. Inicialización
        std::cout << "\n[Paso 1] Inicializando conexión..." << std::endl;
        MPointer<int>::Init("http://localhost:8080");
        std::cout << "✓ Conexión configurada" << std::endl;

        // 2. Creación de instancias
        std::cout << "\n[Paso 2] Creando ptr1..." << std::endl;
        auto ptr1 = MPointer<int>::New();
        std::cout << "✓ ptr1 creado" << std::endl;

        std::cout << "\n[Paso 3] Creando ptr2 con valor 42..." << std::endl;
        auto ptr2 = MPointer<int>::New(sizeof(int), 42);
        std::cout << "✓ ptr2 creado" << std::endl;

        // 3. Operaciones básicas
        std::cout << "\n[Paso 4] Asignando *ptr1 = 100..." << std::endl;
        *ptr1 = 100;
        std::cout << "✓ Valor asignado" << std::endl;

        std::cout << "\n[Paso 5] Leyendo *ptr1..." << std::endl;
        int value = *ptr1;
        std::cout << "✓ Valor leído: " << value << std::endl;

        std::cout << "\n[Paso 6] Asignando ptr2 = ptr1..." << std::endl;
        ptr2 = ptr1;
        std::cout << "✓ Asignación completada" << std::endl;

        std::cout << "\n=== Todas las pruebas pasaron ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "✗ Error: " << e.what() << std::endl;
        return 1;
    }
}