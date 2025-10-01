#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "RTree.h"

using namespace std;

void print_pair(const vector<pair<int, int>>& v) {
    for (const auto& p : v) {
        cout << "(" << p.first << ", " << p.second << ") ";
    }
}

void console_print_mbrs(const vector<vector<vector<pair<int, int>>>>& objects_n) {
    if (objects_n.empty()) return;
    
    cout << " number of levels :" << objects_n.size() << endl;

    for (size_t level_idx = 0; level_idx < objects_n.size(); ++level_idx) {
        const auto& level_mbrs = objects_n[objects_n.size() - 1 - level_idx];

        cout << "\n--------------------" << endl;
        cout << "Nivel " << objects_n.size() - 1 - level_idx << " (" << level_mbrs.size() << " MBRs):" << endl;
        
        for (const auto& mbr : level_mbrs) {
            if (mbr.size() == 2) {
                cout << "   MBR: (" << mbr[0].first << ", " << mbr[0].second << ") to ("
                     << mbr[1].first << ", " << mbr[1].second << ")" << endl;
            } else {
                cout << "   MBR: Formato de puntos incorrecto (" << mbr.size() << " puntos)" << endl;
            }
        }
    }
    cout << "--------------------" << endl;
}

string generate_python_data_string(
    const vector<vector<pair<int, int>>>& vpoints,
    const vector<vector<vector<pair<int, int>>>>& objects_n)
{
    string output = "";
    
    output += "P";
    output += "|" + to_string(vpoints.size());

    for (const auto& polygon : vpoints) {
        output += "|" + to_string(polygon.size());
        for (const auto& point : polygon) {
            output += "|" + to_string(point.first) + "|" + to_string(point.second);
        }
    }

    output += "|M";
    output += "|" + to_string(objects_n.size());

    for (const auto& level_mbrs : objects_n) {
        output += "|" + to_string(level_mbrs.size());
        for (const auto& mbr : level_mbrs) {
            if (mbr.size() == 2) {
                output += "|" + to_string(mbr[0].first) + "|" + to_string(mbr[0].second);
                output += "|" + to_string(mbr[1].first) + "|" + to_string(mbr[1].second);
            }
        }
    }
    
    output += "|END";
    return output;
}

int main()
{
    vector<vector<pair<int, int>>> vpoints;
    
    pair<int, int> points[8] = {
        {-10, 40}, {-5, 45},
        {-20, 40}, {-15, 45},
        {-30, 40}, {-25, 45},
        {-40, 40}, {-35, 45} 
    };
    
    for (unsigned int i = 0; i < 8; i += 2) {
        vector<pair<int, int>> sub1(&points[i], &points[i+2]);
        vpoints.push_back(sub1);
    }

    pair<int, int> points2[6] = {
        {-10, 30}, {-5, 30}, {-10, 35},
        {-20, 30}, {-15, 30}, {-20, 35} 
    };
    
    for (unsigned int i = 0; i < 6; i += 3) {
        vector<pair<int, int>> sub1(&points2[i], &points2[i+3]);
        vpoints.push_back(sub1);
    }

    RTree rtree;

    vector<vector<vector<pair<int, int>>>> objects_n;

    cout << "--- INSERCIÓN DE OBJETOS ---" << endl;
    for(auto &x : vpoints)
    {
        cout << "inserting " << x.size() << ": ";
        print_pair(x);
        Rect rect = rtree.MBR(x);
        rtree.Insert(rect.m_min, rect.m_max, x);
        cout << endl;
    }
    
    rtree.getMBRs(objects_n);
    console_print_mbrs(objects_n);


    cout << "\n--- INSERTING ONE MORE ---" << endl;
    vector<pair<int, int>> ad;
    pair<int, int> ad1;
    ad1.first = 54;
    ad1.second = 12;
    pair<int, int> ad2;
    ad2.first = 53;
    ad2.second = 4;
    ad.push_back(ad2);
    ad.push_back(ad1);

    cout << "inserting " << ad.size() << ": ";
    print_pair(ad);
    Rect rect_ad = rtree.MBR(ad);
    rtree.Insert(rect_ad.m_min, rect_ad.m_max, ad);
    cout << endl;
    
    vpoints.push_back(ad);

    rtree.getMBRs(objects_n);
    console_print_mbrs(objects_n);

    vector<vector<pair<int, int>>> search_results;
    
    Rect search_rect(-50, 25, 0, 50);
    
    cout << "\n\n--- SEARCH TEST ---" << endl;
    cout << "Searching for objects in MBR: (min: "
           << search_rect.m_min[0] << "," << search_rect.m_min[1]
           << ") (max: " << search_rect.m_max[0] << "," << search_rect.m_max[1] << ")\n";

    rtree.Search(search_rect, search_results);

    cout << "Search Results (" << search_results.size() << " objects found):" << endl;
    for (const auto& obj : search_results) {
        print_pair(obj);
        cout << endl;
    }
    
    string python_data = generate_python_data_string(vpoints, objects_n);

    const string FILE_PATH = "/Users/santiagosalas/Desktop/resultados/rtree_data.txt";
    
    ofstream outfile(FILE_PATH);
    if (outfile.is_open()) {
        outfile << python_data;
        outfile.close();
        cout << "\n=========================================================" << endl;
        cout << "DATOS GUARDADOS EN EL ESCRITORIO PARA PYTHON:" << endl;
        cout << FILE_PATH << endl;
        cout << "=========================================================\n" << endl;
    } else {
        cerr << "\nERROR: No se pudo abrir o escribir en el archivo: " << FILE_PATH << endl;
        cerr << "Asegúrate de que la carpeta 'resultados' exista en tu Escritorio y tengas permisos." << endl;
    }

    return 0;
}


