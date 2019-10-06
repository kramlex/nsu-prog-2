//
// Created by markd on 17.09.2019.
//
#include <iostream>
#include <fstream>
#include <math.h>
#include <iomanip>

using namespace std;

class vector2{
private:
    double x,y;
public:
    vector2() : x(0) , y(0) {}
    vector2(double x1, double y1 , double x2 , double y2) : x(x2-x1) , y(y2-y1) {}
    vector2(double x2 , double y2) : x(x2) , y(y2) {}
    vector2(const vector2 &v) : x(v.x) , y(v.y) {}

    double length() const {
        return sqrt( pow(this->x,2) + pow(this->y,2));
    }
    double dot(const vector2 &v) const {
        return this->x * v.x + this->y * v.y;
    }

    vector2 operator +(vector2 &v){
        return vector2(this->x + v.x , this->y + v.y);
    }
    vector2 operator - (vector2 &v){
        return vector2(this->x - v.x , this->y - v.y);
    }
    vector2 & operator += (const vector2 &v){
        this->x += v.x;
        this->y += v.y;
        return *this;
    }

    vector2 &operator -= (const vector2 &v){
        this->x -= v.x;
        this->y -= v.y;
        return *this;
    }

    vector2 & operator= (const vector2 &v){
        this->x = v.x;
        this->y = v.y;
        return *this;
    }
    void print(){
        cout << "{ " << this->x << " , " << this->y << " }" << endl;
    }

};

int main(){
    double x1,y1,x2,y2;
    ifstream fin("input.txt");
    ofstream fout("output.txt");

    // a
    fin >> x1 >> y1 >> x2 >> y2;
    vector2 a(x1,y1,x2,y2);
    // b
    fin >> x1 >> y1 >> x2 >> y2;
    vector2 b(x1,y1,x2,y2);
    // c
    fin >> x1 >> y1 >> x2 >> y2;
    vector2 c(x1,y1,x2,y2);
    //d
    fin >> x1 >> y1 >> x2 >> y2;
    vector2 d(x1,y1,x2,y2);

    fout << fixed << setprecision(2) << (a+b).dot(c-d) + (a.length() * a.dot(b)) - 3 * b.length() * c.dot(d);
}
