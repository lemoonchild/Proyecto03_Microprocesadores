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

using namespace std;

struct ThreadArgs {
    int autosPorKiosco;
    int autosExtra;
    int thread_id;
    vector<double>* tiempoTotal;
};

void* efectivo(void* args);
void distribucionAutos(int total_compass, int total_efectivo);

int numAutosConCompass, numAutosEfectivo;

mutex mtx;

int main(int argc, char const *argv[]) {
    cout << "\nBienvenido a la simulacion de estacion de pago de la VAS\n";
    cout << "\nPor favor indica cuantos carros desea pagar con Compass: ";
    cin >> numAutosConCompass;
    cout << "\nPor favor indica cuantos carros deseas pagar con efectivo: ";
    cin >> numAutosEfectivo;
    cout << "\nComenzando simulacion...\n";
    
    cout << "\nFinalizando simulacion...\n";
    
    return 0;
}

void* efectivo(void* args) {
    ThreadArgs* arguments = (ThreadArgs*) args;
    int number = arguments->autosPorKiosco + arguments->autosExtra;
    for (int i = 0; i < number; i++) {
        auto start_time = chrono::steady_clock::now();
        sleep(4);
        mtx.lock();
        cout << "\n\tEl auto esta pagando con efectivo.";
        cout << "\n\tGracias por pagar con efectivo, tenga buen viaje\n";
        mtx.unlock();
        auto end_time = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(end_time - start_time).count();
        (*arguments->tiempoTotal)[arguments->thread_id] += duration;
    }
    return NULL;
}


void distribucionAutos(int total_compass, int total_efectivo) {
    pthread_t threads[6];

    ThreadArgs args[6];
    int autosPorKioscoCompass = total_compass / 3;
    int autosExtraCompass[] = {total_compass % 3 >= 1, total_compass % 3 == 2, 0};

    int autosPorKioscoEfectivo = total_efectivo / 3;
    int autosExtraEfectivo[] = {total_efectivo % 3 >= 1, total_efectivo % 3 == 2, 0};

    for (int i = 0; i < 3; i++) {
        args[i+3] = {autosPorKioscoEfectivo, autosExtraEfectivo[i], i}; //&tiempoTotalEfectivo};
        pthread_create(&threads[i+3], NULL, efectivo, (void*)&args[i+3]);
    }

    for (int i = 0; i < 6; i++) {
        pthread_join(threads[i], NULL);
    }
}
