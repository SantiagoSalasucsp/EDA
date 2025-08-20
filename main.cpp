//
//  main.cpp
//  EDA
//
//  Created by Santiago Salas on 19/08/25.
//

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <cmath>
#include <fstream>
#include <iomanip>

using namespace std;

double distancia_euclidiana(const vector<double>& p1, const vector<double>& p2) {
    double sum_of_squares = 0.0;
    for (size_t i = 0; i < p1.size(); i++) {
        double diff = p1[i] - p2[i];
        sum_of_squares += diff * diff;
    }
    return sqrt(sum_of_squares);
}

int main()
{
    vector<int> dimensionalidad = {10, 50, 100, 500, 1000, 2000, 5000};
    
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    
    vector<vector<double>> todas_las_distancias;

    for (int k = 0; k < dimensionalidad.size(); k++) {
        int dim = dimensionalidad[k];
        vector<vector<double>> d_dimension;

        
        for (int i = 0; i < 100; i++) {
            vector<double> punto;
            for (int j = 0; j < dim; j++) {
                punto.push_back(dis(gen));
            }
            d_dimension.push_back(punto);
        }
        vector<double> distancias;
        for (size_t i = 0; i < d_dimension.size(); i++) {
            for (size_t j = i + 1; j < d_dimension.size(); j++) {
                distancias.push_back(distancia_euclidiana(d_dimension[i], d_dimension[j]));
            }
        }

        todas_las_distancias.push_back(distancias);

        cout << "Distancias para dimensión " << dim << " calculadas ("
             << distancias.size() << " valores)." << endl;
    }


    string nombre_archivo = "/Users/santiagosalas/Desktop/distancia/distancias.csv";
    ofstream archivo_salida(nombre_archivo);
    if (archivo_salida.is_open()) {
      
        for (int k = 0; k < dimensionalidad.size(); k++) {
            archivo_salida << "dim_" << dimensionalidad[k];
            if (k != dimensionalidad.size() - 1) archivo_salida << ",";
        }
        archivo_salida << endl;

  
        for (int fila = 0; fila < todas_las_distancias[0].size(); fila++) {
            for (int col = 0; col < todas_las_distancias.size(); col++) {
                archivo_salida << fixed << setprecision(10) << todas_las_distancias[col][fila];
                if (col != todas_las_distancias.size() - 1) archivo_salida << ",";
            }
            archivo_salida << endl;
        }

        archivo_salida.close();
        cout << "Todas las distancias guardadas en " << nombre_archivo << endl;
    } else {
        cerr << "Error: no se pudo abrir el archivo " << nombre_archivo << endl;
    }

    return 0;
}

