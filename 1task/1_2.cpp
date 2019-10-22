#include <iostream>
#include <fstream>
using namespace std;

ifstream fin("input.txt");
ofstream fout("output.txt");

int str_len(const char* str) {
    for (const char* c = str;;c++) if (!*c) return c-str;
}

void str_ncpy( char* to, const char* from, int _n ){
    size_t i = 0;
    while(i++ != _n && (*to++ = *from++));
}

class String {
    char *str;
    int len;

public:
    String() : str(NULL) , len(0) {}
    String(const char* s){
        len = str_len(s);
        str = new char[len+1];
        str_ncpy(str,s,len);
        str[len] = '\0';
    }
    String(const String& s){
        len = str_len(s.str);
        str = new char[len+1];
        str_ncpy(str , s.str , len);
        str[len] = '\0';
    }

    ~String(){
        delete[] str;
        len = 0;
    }

    String& operator= (const String& s){
        delete[] str;
        len = str_len(s);
        str = new char[len + 1];
        str_ncpy(str, s , len);
        return *this;
    }

    operator const char* () const {
        return str ? str : "";
    }

    int length(){
        return len;
    }

    char& operator[] (int index){
        return str[index];
    }

    String operator+ (String& s){
        char* newStr = new char[len + s.len + 1];
        str_ncpy(newStr, str , len);
        str_ncpy(newStr + len  , s.str , s.len );
        newStr[len + s.len] = '\0';
        return String(newStr);
    }

    String& operator+= (String& s){
        char* newStr = new char[len + s.len + 1];
        str_ncpy(newStr, str , len);
        str_ncpy(newStr + len , s.str , s.len);
        newStr[len + s.len] = '\0';
        len = len + s.len;
        delete[] str;
        str = newStr;
        return *this;
    }

    friend ofstream&operator<< (ofstream& ofstream1 , const String& s){
        ofstream1 << s.str;
        return ofstream1;
    }

    friend ifstream&operator>> (ifstream& ifstream1 , String& s){
        if(!s.str){
            s.str = new char[1];
            s[0] = '\0';
        }
        char c;
        char* buff = new char[2];
        buff[1] = '\0';
        while(true){
            c = ifstream1.get();
            if(c == '\n' || fin.eof() ) break;
            buff[0] = c;
            String tmp(buff);
            s += tmp;
        }
        return ifstream1;
    }


    int find(String substr){
        String razdelitel = "#";
        String s = substr + razdelitel + *this;

        int pi[s.len];
        pi[0] = 0;
        for (int i=1; i < s.len; ++i) {
            int j = pi[i-1];
            while (j > 0 && s[i] != s[j])
                j = pi[j-1];
            if (s[i] == s[j])  ++j;
            pi[i] = j;
            if(pi[i] == substr.len)
                return i - 2 * substr.len;
        }

        return -1;
    }

    String str_sub(int start , int length){
        char* newStr = new char[length+1];
        str_ncpy(newStr , str + start , length);
        newStr[length] = '\0';
        return String(newStr);
    }

};

int main() {
    String s , findStr , replaceStr ;
    fin >> s;
    fin >> findStr;
    fin >> replaceStr;
    while(true){
        int k = s.find(findStr);
        if(k < 0) break;
        String untilStr = s.str_sub(0, k);
        String behindStr = s.str_sub(k + findStr.length() , s.length() - k - findStr.length());
        s = untilStr + replaceStr + behindStr;
    }
    fout << s;


}