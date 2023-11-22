import sqlite3
from datetime import datetime

# Nombre del archivo de texto con los datos
txt_file = 'datos'

# Nombre de la base de datos SQLite
database_file = 'datos.db'

# Crear una conexion a la base de datos SQLite
conn = sqlite3.connect(database_file)

# Crear un cursor para ejecutar consultas
cursor = conn.cursor()

# Crear la tabla en la base de datos
cursor.execute('''
    CREATE TABLE IF NOT EXISTS datos (
        timestamp TEXT PRIMARY KEY,
        load REAL,
        solar_generation REAL,
        wind_generation REAL
    )
''')

# Abrir el archivo de texto y leer los datos
with open(txt_file, 'r') as file:
    # Ignorar la primera linea que contiene encabezados
    next(file)
    
    # Iterar sobre las lineas del archivo e insertar en la base de datos
    for line in file:
        # Reemplazar las comas por puntos y dividir la linea en partes
        parts = line.strip().replace(',', '.').split(';')
        
        # Convertir el primer elemento a un objeto de fecha y hora
        timestamp = datetime.strptime(parts[0], '%Y-%m-%dT%H:%M:%SZ')
        
        # Convertir las otras partes a numeros en coma flotante (si existen)
        load = float(parts[1]) if parts[1] else None
        solar_generation = float(parts[2]) if parts[2] else None
        wind_generation = float(parts[3]) if parts[3] else None

        # Insertar datos en la base de datos
        cursor.execute('''
            INSERT INTO datos (timestamp, load, solar_generation, wind_generation)
            VALUES (?, ?, ?, ?)
        ''', (timestamp, load, solar_generation, wind_generation))

# Confirmar los cambios y cerrar la conexion
conn.commit()
conn.close()

print("Base de datos creada y datos insertados con éxito.")
