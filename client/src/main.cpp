#include <iostream>
#include <cpr/cpr.h>

int main() {
    // Haciendo una solicitud GET a la API de la cola de mensajes
    auto response = cpr::Get(cpr::Url{"http://localhost:5288/weatherforecast"});
    
    // Verificando la respuesta
    if (response.status_code == 200) {
        std::cout << "Respuesta de la API: " << response.text << std::endl;
    } else {
        std::cerr << "Error: " << response.status_code << " - " << response.error.message << std::endl;
    }
    
    return 0;
}
