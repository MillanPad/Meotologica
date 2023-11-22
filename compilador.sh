#!/bin/bash

# Nombre del archivo de origen
SOURCE_FILE="consulta.cpp"

# Nombre del ejecutable de salida
OUTPUT_EXECUTABLE="ejecutable"

# Comando de compilación
g++ -std=c++11 -o $OUTPUT_EXECUTABLE $SOURCE_FILE -lsqlite3

# Verificar si la compilación fue exitosa
if [ $? -eq 0 ]; then
    echo "Compilación exitosa. Puedes ejecutar el programa con ./$OUTPUT_EXECUTABLE"
