//
// Created by markd on 13.10.2019.
//

#include <iostream>
#include <cmath>
#include <fstream>
#include <iomanip>

using namespace std;

class Equation{
    virtual double solve() = 0;
};

class Linear : public Equation {
    double a,b;
public:
    Linear(double a , double b) : a(a) , b(b) {}
    double solve() override {
        return a == 0 ? 0 : -this->b / this->a;
    }
};

class Trigonometric : public Equation {
    double a;
public:
    explicit Trigonometric(double a) : a(a) {}

    double solve() override {
        return asin(a);
    }
};

int main(){
    ifstream fin("input.txt");
    ofstream fout("output.txt");

    int n;
    double a = 0, b = 0, res = 0;
    char type;
    fin >> n;
    for(int i = 0; i < n; i++){
        fin >> type;
        switch(type){
            case 'L': {
                fin >> a >> b;
                res += Linear(a,b).solve();
                break;
            }
            case 'T': {
                fin >> a;
                res += Trigonometric(a).solve();
                break;
            }
            default: {
                break;
            }
        }
    }
    fout << fixed << setprecision(2) << res;
    return 0;
}