#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <cmath>
#include <limits>
#include <algorithm>
#include <queue>

using namespace std;


#define MAX_POINTS_PER_CUBE 1
#define MAX_DEPTH 10000

struct Point {
    double x;
    double y;
    double z;
    Point(double a = 0, double b = 0, double c = 0) : x(a), y(b), z(c) {}
    
    
    bool operator==(const Point& other) const {
        return abs(x - other.x) < 1e-9 && abs(y - other.y) < 1e-9 && abs(z - other.z) < 1e-9;
    }
};

class Octree {

    Octree* children[8];
    vector<Point> points;
    Point bottomLeft;
    double h;
    bool subdivided;
    int depth;
    
   
    int getChildIndex(const Point& p) const {
        int index = 0;
        if (p.x >= bottomLeft.x + h / 2) index |= 4;
        if (p.y >= bottomLeft.y + h / 2) index |= 2;
        if (p.z >= bottomLeft.z + h / 2) index |= 1;
        return index;
    }
    
    
    void subdivide() {
        if (subdivided) return;
        
        subdivided = true;
        double new_h = h / 2;
        
        
        children[0] = new Octree({bottomLeft.x, bottomLeft.y, bottomLeft.z}, new_h, depth + 1);
        children[1] = new Octree({bottomLeft.x, bottomLeft.y, bottomLeft.z + new_h}, new_h, depth + 1);
        children[2] = new Octree({bottomLeft.x, bottomLeft.y + new_h, bottomLeft.z}, new_h, depth + 1);
        children[3] = new Octree({bottomLeft.x, bottomLeft.y + new_h, bottomLeft.z + new_h}, new_h, depth + 1);
        children[4] = new Octree({bottomLeft.x + new_h, bottomLeft.y, bottomLeft.z}, new_h, depth + 1);
        children[5] = new Octree({bottomLeft.x + new_h, bottomLeft.y, bottomLeft.z + new_h}, new_h, depth + 1);
        children[6] = new Octree({bottomLeft.x + new_h, bottomLeft.y + new_h, bottomLeft.z}, new_h, depth + 1);
        children[7] = new Octree({bottomLeft.x + new_h, bottomLeft.y + new_h, bottomLeft.z + new_h}, new_h, depth + 1);
        
        
        for (const Point& p : points) {
            children[getChildIndex(p)]->insert(p);
        }
        
        points.clear();
    }
    
public:
    Octree(Point bl, double size, int d = 0) : bottomLeft(bl), h(size), subdivided(false), depth(d) {
        for (int i = 0; i < 8; i++) {
            children[i] = nullptr;
        }
    }
    
    
    
    void insert(const Point& p) {
        if (subdivided) {
            children[getChildIndex(p)]->insert(p);
        } else {
            
            for (const Point& point : points) {
                if (point == p) return;
            }
            
            points.push_back(p);
            
            
            if (points.size() > MAX_POINTS_PER_CUBE && depth < MAX_DEPTH) {
                subdivide();
            }
        }
    }
    
    bool exist(const Point& p) {
        
        if (p.x < bottomLeft.x || p.x >= bottomLeft.x + h ||
            p.y < bottomLeft.y || p.y >= bottomLeft.y + h ||
            p.z < bottomLeft.z || p.z >= bottomLeft.z + h) {
            return false;
        }
        
        if (!subdivided) {
            for (const Point& point : points) {
                if (point == p) return true;
            }
            return false;
        } else {
            return children[getChildIndex(p)]->exist(p);
        }
    }
    
    Point find_closest(const Point& target, double radius) {
        double min_dist_sq = radius * radius;
        Point closest_point = {numeric_limits<double>::infinity(), 0, 0};
        
        
        queue<Octree*> q;
        q.push(this);

        while (!q.empty()) {
            Octree* current = q.front();
            q.pop();

            if (!current->subdivided) {
                for (const auto& existing_p : current->points) {
                    double dist_sq = pow(existing_p.x - target.x, 2) + pow(existing_p.y - target.y, 2) + pow(existing_p.z - target.z, 2);
                    if (dist_sq < min_dist_sq) {
                        min_dist_sq = dist_sq;
                        closest_point = existing_p;
                    }
                }
            } else {
                for (int i = 0; i < 8; ++i) {
                    if (current->children[i]) {
                    
                        double dx = max(0.0, abs(target.x - (current->children[i]->bottomLeft.x + current->children[i]->h / 2.0)) - current->children[i]->h / 2.0);
                        double dy = max(0.0, abs(target.y - (current->children[i]->bottomLeft.y + current->children[i]->h / 2.0)) - current->children[i]->h / 2.0);
                        double dz = max(0.0, abs(target.z - (current->children[i]->bottomLeft.z + current->children[i]->h / 2.0)) - current->children[i]->h / 2.0);
                        if ((dx * dx + dy * dy + dz * dz) <= min_dist_sq) {
                            q.push(current->children[i]);
                        }
                    }
                }
            }
        }

        return closest_point;
    }
    
    void printTree(int current_depth = 0) const {
        string indent(current_depth * 2, ' ');
        cout << indent << "Nodo - BottomLeft: (" << bottomLeft.x << ", " << bottomLeft.y << ", " << bottomLeft.z
             << "), Tamaño: " << h;
        
        if (!subdivided) {
            cout << " [HOJA con " << points.size() << " puntos]" << endl;
        } else {
            cout << " [INTERNO]" << endl;
            for (int i = 0; i < 8; i++) {
                if (children[i]) {
                    cout << indent << "  Hijo " << i << ":" << endl;
                    children[i]->printTree(current_depth + 2);
                }
            }
        }
    }
    
    
    void getRootInfo() const {
        
        cout << "Coordenada bottomLeft: (" << bottomLeft.x << ", " << bottomLeft.y << ", " << bottomLeft.z << ")" << endl;
        cout << "Tamaño del lado: " << h << endl;
       
    }
    
    void exportToOBJ(const string& filename) {
        ofstream file(filename);
       
        
        file << "# Octree Visualization\n";
        file << "# Generated with MAX_POINTS_PER_CUBE = " << MAX_POINTS_PER_CUBE << "\n";
        
        int vertexCount = 1;
        exportLeafCubes(file, vertexCount);
        
        file.close();
        cout << "Octree exportado a: " << filename << endl;
    }
    
    
    void exportLeafCubes(ofstream& file, int& vertexCount) {
        if (!subdivided && !points.empty()) {
            
            //file << "# Cubo hoja con " << points.size() << " puntos\n";
            
            
            Point vertices[8] = {
                Point(bottomLeft.x, bottomLeft.y, bottomLeft.z),
                Point(bottomLeft.x + h, bottomLeft.y, bottomLeft.z),
                Point(bottomLeft.x + h, bottomLeft.y + h, bottomLeft.z),
                Point(bottomLeft.x, bottomLeft.y + h, bottomLeft.z),
                Point(bottomLeft.x, bottomLeft.y, bottomLeft.z + h),
                Point(bottomLeft.x + h, bottomLeft.y, bottomLeft.z + h),
                Point(bottomLeft.x + h, bottomLeft.y + h, bottomLeft.z + h),
                Point(bottomLeft.x, bottomLeft.y + h, bottomLeft.z + h)
            };
            
            for (int i = 0; i < 8; i++) {
                file << "v " << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << "\n";
            }
            
            
            int base = vertexCount;
            file << "f " << base << " " << base + 1 << " " << base + 2 << " " << base + 3 << "\n";
            file << "f " << base + 4 << " " << base + 5 << " " << base + 6 << " " << base + 7 << "\n";
            file << "f " << base << " " << base + 1 << " " << base + 5 << " " << base + 4 << "\n";
            file << "f " << base + 2 << " " << base + 3 << " " << base + 7 << " " << base + 6 << "\n";
            file << "f " << base + 1 << " " << base + 2 << " " << base + 6 << " " << base + 5 << "\n";
            file << "f " << base + 3 << " " << base << " " << base + 4 << " " << base + 7 << "\n";
            
            vertexCount += 8;
        } else if (subdivided) {
            for (int i = 0; i < 8; i++) {
                if (children[i]) {
                    children[i]->exportLeafCubes(file, vertexCount);
                }
            }
        }
    }
};


vector<Point> loadPointsFromFile(const string& filename) {
    vector<Point> points;
    ifstream file(filename);
    
    double x;
    double y;
    double z;
    while (file >> x >> y >> z) {
        points.push_back(Point(x, y, z));
    }
    
    file.close();
    
    return points;
}


void calculateBounds(const vector<Point>& points, Point& minPoint, Point& maxPoint) {
    if (points.empty()) return;
    
    minPoint = maxPoint = points[0];
    
    for (const Point& p : points) {
        minPoint.x = min(minPoint.x, p.x);
        minPoint.y = min(minPoint.y, p.y);
        minPoint.z = min(minPoint.z, p.z);
        
        maxPoint.x = max(maxPoint.x, p.x);
        maxPoint.y = max(maxPoint.y, p.y);
        maxPoint.z = max(maxPoint.z, p.z);
    }
}

int main() {
    
    
    
    const string input_file = "/Users/santiagosalas/Desktop/puntos/aguila.xyz";
    const string output_dir = "/Users/santiagosalas/Desktop/resultados";
    const string obj_filepath = output_dir + "/octree.obj";

    
    vector<Point> points = loadPointsFromFile(input_file);
    
    if (points.empty()) {
        cerr << "No se pudieron cargar puntos del archivo." << endl;
        return 1;
    }
    
    
    Point minPoint, maxPoint; //limtes xd
    calculateBounds(points, minPoint, maxPoint);
    

    double maxRange = max({maxPoint.x - minPoint.x,
                         maxPoint.y - minPoint.y,
                         maxPoint.z - minPoint.z});
    
    
    double margin = maxRange * 0.1; // para el margen
    Point bottomLeft(minPoint.x - margin, minPoint.y - margin, minPoint.z - margin);
    double cubeSize = maxRange + 2 * margin;
    
    
    Octree octree(bottomLeft, cubeSize);
    
    
    for (const Point& p : points) {
        octree.insert(p);
    }
    //octree.printTree();

    
    //cout << "xdxdxdxd" << endl;
    octree.getRootInfo();
    
    
    
    filesystem::create_directories(output_dir);
    octree.exportToOBJ(obj_filepath);
    
    
    cout <<endl<< "=== Preguntas" << endl;
    /*if (!points.empty()) {
        Point p = points[0];
        cout << "existe (" << p.x << ", " << p.y << ", " << p.z << ")? ";
        if((octree.exist(p))){
            cout<<"si"<<endl;
        }
        else{
            cout<<"no"<<endl;
        }
             
        
        
    }*/
    
    
    if (!points.empty()) {
        int mitad=points.size()/2;
        Point x = points[mitad];
        Point y(3.618588 ,0.745581, -3.839039);
        octree.insert(y);
        Point respuesta = octree.find_closest(y, 2);
        if (respuesta.x != numeric_limits<double>::infinity()) {
            cout <<" este de aqui ("<< respuesta.x << ", " << respuesta.y << ", " << respuesta.z << ") xd <--" << endl;
        } else {
            cout << "no hay xd" << endl;
        }
    }
    
    
    Point xd(1,2,1);
    if(octree.exist(xd)){
        cout<<"si"<<endl;
    }
    else{
        cout<<"no"<<endl;
    }
    
    
   

    cout<<"("<<"-31.1358,-13.2306,-12.1719"<<")"<< endl;
    cout<<"h: "<< "62.37"<< endl;
    
    
    return 0;
}





