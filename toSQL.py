import sqlite3
from datetime import datetime
txt_file = 'datos'
database_file = 'datos.db'
conn = sqlite3.connect(database_file)
cursor = conn.cursor()
cursor.execute('''
    CREATE TABLE IF NOT EXISTS datos (
        timestamp TEXT PRIMARY KEY,
        load REAL,
        solar_generation REAL,
        wind_generation REAL
    )
''')
with open(txt_file, 'r') as file:
    next(file)
    for line in file:

        parts = line.strip().replace(',', '.').split(';')
        timestamp = datetime.strptime(parts[0], '%Y-%m-%dT%H:%M:%SZ')
        load = float(parts[1]) if parts[1] else None
        solar_generation = float(parts[2]) if parts[2] else None
        wind_generation = float(parts[3]) if parts[3] else None
        cursor.execute('''
            INSERT INTO datos (timestamp, load, solar_generation, wind_generation)
            VALUES (?, ?, ?, ?)
        ''', (timestamp, load, solar_generation, wind_generation))


conn.commit()
conn.close()

print("Base de datos creada y datos insertados con éxito.")
