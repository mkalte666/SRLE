#include <iostream>
#include <fstream>
#include "srle.h"

/**
* \author Malte Kieﬂling
* \copyright Copyright 2017 Malte Kieﬂling. All rights reserved.
* \licence This project is released under the MIT Licence.
*          See LICENCE in the project root for details.
*/

void writer(uint8_t b)
{
    std::cout.write((char*)&b, 1);
}

int main(int argc, char** argv)
{
    SRLE::Decoder<> decoder(writer);
    if (argc != 2) {
        std::cerr << "Syntax: srledecoder <file>" << std::endl;
        return 1;
    }
    std::fstream in(argv[1], std::ios::in | std::ios::binary);
    uint8_t b;
    while (in.read((char*)&b, 1)) {
        decoder.putb(b);
    }
    return 0;
}