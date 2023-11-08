/*--------------------------------------------------------
* UNIVERSIDAD DEL VALLE DE GUATEMALA
* FACULTAD DE INGENIERÍA
* DEPARTAMENTO DE CIENCIA DE LA COMPUTACIÓ
* CC3086 Programación de Microprocesadores
* Programadores: Sofía Velásquez, Fabiola Contreras, Madeline Castro, Maria José Villafuerte 
* Proyecto 03 - Fase 02 
* Fecha: 09/11/2023
* Descripción: Simulacion de la VAS con Pthreads 
* --------------------------------------------------------*/
#include <iostream>
#include <pthread.h>
#include <mutex>
#include <ctime>
#include <vector>
#include <chrono>
#include <unistd.h>
#include <semaphore.h> 

using namespace std;

// Estructura para registro de tiempo
struct ThreadArgs {
    int autosPorKiosco;
    int autosExtra;
    int thread_id;
    vector<double>* tiempoTotal;
};

// Inicialización de funciones
void* efectivo(void* args);
void distribucionAutos(int total_compass, int total_efectivo);
void* compass(void* args);
void estadisticas(double number, vector<double>& tiempoTotalCompass, vector<double>& tiempoTotalEfectivo);

// Variables para datos de ejecucion
int numAutosConCompass, numAutosEfectivo;
vector<double> tiempoTotalCompass(3, 0);
vector<double> tiempoTotalEfectivo(3, 0);
mutex mtx;
sem_t semEfectivo;
sem_t semCompass;

// Función principal, llamada a subrutinas
int main(int argc, char const *argv[]) {
    // Inicializar los semáforos
    sem_init(&semEfectivo, 0, 3); // Suponiendo que hay 3 kioscos para efectivo
    sem_init(&semCompass, 0, 3); // Suponiendo que hay 3 kioscos para Compass

    // Solicitud de datos
    cout << "\nBienvenido a la simulacion de estacion de pago de la VAS\n";
    cout << "\nPor favor indica cuantos carros desea pagar con Compass: ";
    cin >> numAutosConCompass;
    cout << "\nPor favor indica cuantos carros deseas pagar con efectivo: ";
    cin >> numAutosEfectivo;
    cout << "\nComenzando simulacion...\n";

    // Registro de tiempo en ejecución de simulación
    auto start_time = chrono::steady_clock::now();
    distribucionAutos(numAutosConCompass, numAutosEfectivo);
    auto end_time = chrono::steady_clock::now();
    
    cout << "\nFinalizando simulacion...\n";
    auto duration = chrono::duration_cast<chrono::seconds>(end_time - start_time).count();
    estadisticas(duration, tiempoTotalCompass, tiempoTotalEfectivo); // Muestra de resultados

    // Destruir los semáforos
    sem_destroy(&semEfectivo);
    sem_destroy(&semCompass);

    return 0;
}

// Simulación de paso de carros con pago en efectivo
void* efectivo(void* args) {
    ThreadArgs* arguments = (ThreadArgs*) args;
    int number = arguments->autosPorKiosco + arguments->autosExtra;
    for (int i = 0; i < number; i++) {
        sem_wait(&semEfectivo); // Esperar a obtener el semáforo
        auto start_time = chrono::steady_clock::now(); // Registro de tiempo
        sleep(4); //'Tiempo de pago'
        mtx.lock();
        cout << "\n\tEl auto esta pagando con efectivo.";
        cout << "\n\tGracias por pagar con efectivo, tenga buen viaje\n";
        mtx.unlock();
        auto end_time = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(end_time - start_time).count();
        (*arguments->tiempoTotal)[arguments->thread_id] += duration;
        sem_post(&semEfectivo); // Liberar el semáforo
    }
    return NULL;
}

// Simulación de paso de autos
void distribucionAutos(int total_compass, int total_efectivo) {
    pthread_t threads[6];

    // Distribución de autos por hilo
    ThreadArgs args[6];
    int autosPorKioscoCompass = total_compass / 3;
    int autosExtraCompass[] = {total_compass % 3 >= 1, total_compass % 3 == 2, 0};

    int autosPorKioscoEfectivo = total_efectivo / 3;
    int autosExtraEfectivo[] = {total_efectivo % 3 >= 1, total_efectivo % 3 == 2, 0};

    // Asignación de autos por hilo
    for (int i = 0; i < 3; i++) {
        args[i] = {autosPorKioscoCompass, autosExtraCompass[i], i, &tiempoTotalCompass};
        pthread_create(&threads[i], NULL, compass, (void*)&args[i]);
    }
    
    for (int i = 0; i < 3; i++) {
        args[i+3] = {autosPorKioscoEfectivo, autosExtraEfectivo[i], i, &tiempoTotalEfectivo};
        pthread_create(&threads[i+3], NULL, efectivo, (void*)&args[i+3]);
    }

    for (int i = 0; i < 6; i++) {
        pthread_join(threads[i], NULL);
    }
}

// Muestra los datos de tiempo registrados
void estadisticas(double number, vector<double>& tiempoTotalCompass, vector<double>& tiempoTotalEfectivo) {
    cout << "\n--- Estadisticas de la simulacion ---\n";
    for (int i = 0; i < 3; i++) {
        cout << "Tiempo del kiosco Compass " << i + 1 << ": " << tiempoTotalCompass[i] << " segundos\n";
    }
    for (int i = 0; i < 3; i++) {
        cout << "Tiempo del kiosco Efectivo " << i + 1 << ": " << tiempoTotalEfectivo[i] << " segundos\n";
    }
    cout << "\nTiempo total de la estacion: " << number << " segundos\n\n";
}

// Simulación de paso de carros con pago con compass
void* compass(void* args) {
    ThreadArgs* arguments = (ThreadArgs*) args;
    int number = arguments->autosPorKiosco + arguments->autosExtra;
    for (int i = 0; i < number; i++) {
        sem_wait(&semCompass); // Esperar a obtener el semáforo
        auto start_time = chrono::steady_clock::now();
        sleep(3); //'Tiempo de pago'
        mtx.lock();
        cout << "\n\tEl auto esta usando Compass.";
        cout << "\n\tGracias por usar Compass, tenga buen viaje\n";
        mtx.unlock();
        auto end_time = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(end_time - start_time).count();
        (*arguments->tiempoTotal)[arguments->thread_id] += duration;
        sem_post(&semCompass); // Liberar el semáforo
    }
    return NULL;
}
