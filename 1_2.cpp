#include <iostream>
using namespace std;

int str_len(const char* str) {
    for (const char* c = str;;c++) if (!*c) return c-str;
}

void str_ncpy( char* to, const char* from, int _n ){
    size_t i = 0;
    while(i++ != _n && (*to++ = *from++));
}


class String {
    char *str;
    int n;

public:
    String() : str(nullptr) , n(0) {}
    String(const char *str){

    }
};

int main() {
    cout << str_len("lolkek");

}