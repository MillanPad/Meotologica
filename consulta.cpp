#include <iostream>
#include <sqlite3.h>
#include <string>
#include <limits> 

//Funcion para obtener la energia producida por una tecnologia para un rango temporal
void consultarEnergiaProducida(sqlite3 *db, const std::string &tecnologia, const std::string &inicio, const std::string &fin) {
    sqlite3_stmt *stmt;

    std::string query = "SELECT timestamp, " + tecnologia + " FROM datos WHERE timestamp BETWEEN '" + inicio + "' AND '" + fin + "';";
    
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::cout << "Resultados para " << tecnologia << " entre " << inicio << " y " << fin << ":" << std::endl;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char *tiempo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        double valor = sqlite3_column_double(stmt, 1);

        std::cout << "Tiempo: " << tiempo << ", Valor: " << valor << " MW" << std::endl;
    }

    std::cout << std::endl;

    sqlite3_finalize(stmt);
}

// Funcion para obtener la energia demandada para un rango temporal
void consultarEnergiaDemandada(sqlite3 *db, const std::string &inicio, const std::string &fin) {
   
    consultarEnergiaProducida(db, "load", inicio, fin);
}

// Funcion para calcular el deficit de energia renovable medio para un periodo de tiempo
double calcularDeficitMedio(sqlite3 *db, const std::string &inicio, const std::string &fin) {
    sqlite3_stmt *stmt;

    std::string query = "SELECT timestamp, load, solar_generation, wind_generation FROM datos WHERE timestamp BETWEEN '" + inicio + "' AND '" + fin + "';";
    
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al preparar la consulta: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    double generacionRenovableTotal = 0;
    double demandaTotal = 0;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        double demanda = sqlite3_column_double(stmt, 1);
        double generacionSolar = sqlite3_column_double(stmt, 2);
        double generacionEolica = sqlite3_column_double(stmt, 3);

        generacionRenovableTotal += generacionSolar + generacionEolica;
        demandaTotal += demanda;
    }

    sqlite3_finalize(stmt);

    double deficitTotal = demandaTotal - generacionRenovableTotal;
    int horas = sqlite3_changes(db); // Obtener el numero total de horas (filas)
    double deficitMedio = deficitTotal / horas;

    return deficitMedio;
}

// Funcion para mostrar el menu y procesar la seleccion del usuario
void mostrarMenu() {
    std::cout << "Seleccione la consulta:" << std::endl;
    std::cout << "a. Energia producida por una tecnologia para un rango temporal" << std::endl;
    std::cout << "b. Energia demandada para un rango temporal" << std::endl;
    std::cout << "c. Cálculo de déficit de energía renovable media para un periodo de tiempo" << std::endl;
    std::cout << "d. Cambiar fechas" << std::endl;
    std::cout << "e. Salir del Programa" << std::endl;
    std::cout << "Seleccione (a/b/c/d): ";
}

int main() {
    sqlite3 *db;
    bool menu = true;
    // Abrir la base de datos
    int rc = sqlite3_open("datos.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al abrir la base de datos: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }
    std::string inicio, fin;  

    while (menu) {
        // Solo pedir fechas si no se seleccionó la opción 'd' en la iteración anterior
        if (inicio.empty() && fin.empty()) {
            // Solicitar fechas al usuario
            std::cout << "Introduce la fecha de inicio (formato YYYY-MM-DD HH:MM:SS): " << std::endl;
            std::getline(std::cin, inicio);
            std::cout << "Introduce la fecha de fin (formato YYYY-MM-DD HH:MM:SS): " << std::endl;
            std::getline(std::cin, fin);
        }
    
    
        char opcion;
        mostrarMenu();
        std::cin >> opcion;
        // Limpiar el bufer de entrada despues de leer la opcion
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (opcion) {
            case 'a':
                consultarEnergiaProducida(db, "solar_generation", inicio, fin);
                break;
            case 'b':
                consultarEnergiaDemandada(db, inicio, fin);
                break;
            case 'c':
                    double deficitMedio = calcularDeficitMedio(db, inicio, fin);
                    std::cout << "Déficit medio de energía renovable: " << deficitMedio << " MW por hora" << std::endl;
                break;
            case 'd':
                    inicio.clear();
                    fin.clear();
                break;
            case 'e':
                    menu=false;
                break;
            default:
                std::cout << "Opción no válida" << std::endl;
                break;
        }
    }

    // Cerrar la base de datos
    sqlite3_close(db);

    return 0;
}

