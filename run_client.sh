#!/bin/bash

echo "🧹 Limpiando compilación anterior..."
cd "$(dirname "$0")/client/build" || exit 1
rm -rf *

echo "🔧 Configurando proyecto con CMake..."
cmake .. || { echo "❌ Error en CMake"; exit 1; }

echo "⚙️ Compilando..."
make || { echo "❌ Error al compilar"; exit 1; }

echo "🚀 Ejecutando cliente..."
./client
