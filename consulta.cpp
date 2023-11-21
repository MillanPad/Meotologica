#include <iostream>
#include <sqlite3.h>
#include <string>

//Funcion para obtener la energ�a producida por una tecnolog�a para un rango temporal
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

// Funcion para obtener la energ�a demandada para un rango temporal
void consultarEnergiaDemandada(sqlite3 *db, const std::string &inicio, const std::string &fin) {
    // Puedes utilizar la funci�n consultarEnergiaProducida con "load" como tecnolog�a
    consultarEnergiaProducida(db, "load", inicio, fin);
}

// Funcion para calcular el d�ficit de energ�a renovable medio para un periodo de tiempo
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
    int horas = sqlite3_changes(db); // Obtener el n�mero total de horas (filas)
    double deficitMedio = deficitTotal / horas;

    return deficitMedio;
}

// Funci�n para mostrar el men� y procesar la selecci�n del usuario
void mostrarMenu() {
    std::cout << "Seleccione la consulta:" << std::endl;
    std::cout << "a. Energ�a producida por una tecnolog�a para un rango temporal" << std::endl;
    std::cout << "b. Energ�a demandada para un rango temporal" << std::endl;
    std::cout << "c. C�lculo de d�ficit de energ�a renovable media para un periodo de tiempo" << std::endl;
    std::cout << "Seleccione (a/b/c): ";
}

int main() {
    sqlite3 *db;

    // Abrir la base de datos
    int rc = sqlite3_open("datos.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Error al abrir la base de datos: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    // Solicitar fechas al usuario
    std::string inicio, fin;
    std::cout << "Introduce la fecha de inicio (formato YYYY-MM-DD HH:MM:SS): ";
    std::getline(std::cin, inicio);
    std::cout << "Introduce la fecha de fin (formato YYYY-MM-DD HH:MM:SS): ";
    std::getline(std::cin, fin);

    char opcion;
    mostrarMenu();
    std::cin >> opcion;

    switch (opcion) {
        case 'a':
            consultarEnergiaProducida(db, "solar_generation", inicio, fin);
            break;
        case 'b':
            consultarEnergiaDemandada(db, inicio, fin);
            break;
        case 'c':
            {
                double deficitMedio = calcularDeficitMedio(db, inicio, fin);
                std::cout << "D�ficit medio de energ�a renovable: " << deficitMedio << " MW por hora" << std::endl;
            }
            break;
        default:
            std::cout << "Opci�n no v�lida" << std::endl;
            break;
    }

    // Cerrar la base de datos
    sqlite3_close(db);

    return 0;
}

