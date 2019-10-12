//
// Created by markd on 12.10.2019.
//


#include <cstdint>
#include <iostream>

struct Color{
    struct Decoded {
        uint8_t red; // 1 byte
        uint8_t green;  // 1 byte
        uint8_t blue; // 1 byte
        bool enabled  : 1; // 1 bit |
        uint8_t alpha   : 3; // 3 bit | => 1 byte
        uint8_t reserved : 4; // 4 bit |
    };
    // sizeof(Decoded) = 4 byte
    union {
        Decoded decoded;
        uint32_t encoded;
    };
};

uint8_t get_n_byte(uint32_t data, size_t byte_number){
    union Byte4{
        uint32_t data;
        uint8_t data_array[4];
    };
    Byte4 byte4;
    byte4.data = data;
    return byte4.data_array[byte_number-1];
}

int main(){
    Color color;
    color.decoded = {255,254,253,true, (uint8_t) 100,(uint8_t) 100};
    std::cout << color.encoded << " " << (int) get_n_byte(color.encoded , 3);
}


