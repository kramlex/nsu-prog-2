#include <fstream>
#include <cmath>
#include <iomanip>

using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

class Complex {

    double a,b; // Re , Im

public:
    Complex(){
        this->a = 1;
        this->b = 0;
    }
    Complex(const double a , const double b){
        this->a = a;
        this->b = b;
    }
    Complex(const Complex& complex){
        this->a = complex.a;
        this->b = complex.b;
    }

    Complex operator+(const Complex& other){
        return Complex(a+other.a , b+other.b);
    }


    Complex operator+(const double& scalar){
        return *this + Complex(scalar,0);
    }

    Complex operator-(const Complex& other){
        return Complex(a-other.a , b-other.b);
    }

    Complex operator-(const double& scalar){
        return *this - Complex(scalar,0);
    }

    Complex operator*(const Complex& other){
        return Complex(a*other.a - b*other.b , a*other.b + other.a * b);
    }

    Complex operator*(const double& scalar){
        return *this * Complex(scalar,0);
    }

    Complex operator/(const Complex& other){
        return Complex((other.a * a + b * other.b) / (pow(other.a , 2 ) + pow (other.b,2)) ,
                (other.a * b - a * other.b) / (pow(other.a , 2 ) + pow (other.b,2))  );
    }

    Complex operator/(const double& scalar){
        return *this / Complex(scalar, 0);
    }

    bool operator==(const Complex& other){
        return (a==other.a && b == other.b);
    }


    bool operator!=(const Complex& other){
        return (a != other.a && b != other.b);
    }

    Complex& operator=(const Complex& other){
        this->a = a;
        this->b = b;
        return *this;
    }

    double abs(){
        return sqrt(pow(a,2) + pow(b,2));
    }

    void print(){
        if(b >= 0){
            fout << fixed << setprecision(2) << a << "+i*" << b;
        }
        else{
            fout << fixed << setprecision(2) << a << "-i*" << -b;
        }
    }

    void read(){
        fin >> a >> b;
    }
};

int main(){
    Complex a,b,c,d;
    a.read();
    b.read();
    c.read();
    d.read();

    Complex res = ((a*b)-(c*d)) + Complex( a.abs()*a.abs(), d.abs()*d.abs() ) + (c * Complex(b.abs()*b.abs(),1));
    res.print();
    return 0;
}

