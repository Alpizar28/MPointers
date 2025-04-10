#!/bin/bash

echo "🔁 Navegando al directorio del backend..."
cd "$(dirname "$0")/backend/build" || { echo "❌ No se encontró el directorio build"; exit 1; }

echo "🧹 Limpiando archivos anteriores..."
rm -rf *

echo "🔧 Ejecutando cmake..."
cmake .. || { echo "❌ Falló cmake"; exit 1; }

echo "🛠️ Compilando con make..."
make || { echo "❌ Falló make"; exit 1; }

echo "🚀 Ejecutando el servidor..."
./server
