#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>

using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

class Figure {
public:
    virtual double area() = 0;
};

class Square : public Figure {
    double a;
public:
    Square(double a) : a(a) {}
    double area() override {
        return a*a;
    }
};

class Triangle : public Figure {
    double a;
public:
    Triangle(double a) : a(a) {}
    double area() override {
        return a*a*sqrt(3) / 4;
    }
};

class Circle : public Figure {
    double r;
public:
    Circle(double r) : r(r) {}
    double area() override {
        return M_PI * r * r;
    }
};

int main(){
    int n;
    fin >> n;
    Figure** Figures = new Figure*[n];
    char type;
    double tmp , res = 0;
    for(int i = 0; i < n; ++i){
        fin >> type >> tmp;
        switch(type){
            case 'S' : {
                Figures[i] = new Square(tmp);
                break;
            }
            case 'T' : {
                Figures[i] = new Triangle(tmp);
                break;
            }
            case 'C' : {
                Figures[i] = new Circle(tmp);
                break;
            }
        }
        res += Figures[i]->area();
    }
    fout << fixed << setprecision(2) << res;
    return 0;
}


