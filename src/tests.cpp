#include <cstdint>
#include <iostream>
#include <cstring>
/**
* \author Malte Kieﬂling
* \copyright Copyright 2017 Malte Kieﬂling. All rights reserved.
* \licence This project is released under the MIT Licence.
*          See LICENCE in the project root for details.
*/

#include "srle.h"
#define BUFFER_MAX 1024

size_t counterA;
size_t counterB;
uint8_t bufferA[BUFFER_MAX];
uint8_t bufferB[BUFFER_MAX];

bool compareBuffers()
{
    if (counterA != counterB)
    {
        return false;
    }

    return memcmp(bufferA,bufferB,counterA) == 0;
}

bool compareStringA(const char* s)
{
    if (counterA != strlen(s))
    {
        return false;
    }

    return memcmp(bufferA, s, counterA) == 0;
}

bool compareStringB(const char* s)
{
    if (counterB != strlen(s))
    {
        return false;
    }

    return memcmp(bufferB, s, counterB) == 0;
}

void writerA(uint8_t b)
{
    bufferA[counterA] = b;
    counterA++;
}

void writerB(uint8_t b)
{
    bufferB[counterB] = b;
    counterB++;
}


bool noCompression()
{
    SRLE::Encoder<> e(writerA);
    e.puts("ABCDEFG");
    e.writeSequence();
    return compareStringA("ABCDEFG");
}

bool decodeEncodeNoCompression()
{
    SRLE::Encoder<> e(writerA);
    SRLE::Decoder<> d(writerB);
    e.puts("ABCDEFG");
    e.writeSequence();
    d.write(bufferA, counterA);
    return compareBuffers() && compareStringB("ABCDEFG");
}

bool encodeDecode()
{
    SRLE::Encoder<> e(writerA);
    SRLE::Decoder<> d(writerB);
    e.puts("AAAAAAAAAAAAAAAAIASUHRISHDBGFJSHDFSAOSDOASDDDDASDOIJASGGGGGGGGIOSAIIIIIIIIII");
    e.writeSequence();
    d.write(bufferA, counterA);
    return compareStringB("AAAAAAAAAAAAAAAAIASUHRISHDBGFJSHDFSAOSDOASDDDDASDOIJASGGGGGGGGIOSAIIIIIIIIII");
}

bool encodeControl()
{
    SRLE::Encoder<> e(writerA);
    e.puts("AAAA\xfa");
    e.writeSequence();
    // <ctrl>A<4><ctrl><ctrl>1
    return compareStringA("\xfa\x41\x04\xfa\xfa\x01");
}

bool encodeDecodeControl()
{
    SRLE::Encoder<> e(writerA);
    SRLE::Decoder<> d(writerB);
    e.puts("AAAA\xfa AAAAAAAAAAAAAAAAIASUHRISHDBGFJSHDFSAOSDOASDDDDASDOIJASGGG\xfa\xfa\xfa GGGGGIOSAIIIIIIIIII");
    e.writeSequence();
    d.write(bufferA, counterA);
    return compareStringB("AAAA\xfa AAAAAAAAAAAAAAAAIASUHRISHDBGFJSHDFSAOSDOASDDDDASDOIJASGGG\xfa\xfa\xfa GGGGGIOSAIIIIIIIIII");
}

bool methodEqualResult()
{
    SRLE::Encoder<> e(writerA);
    SRLE::Encoder<> e2(writerB);
    const char* data = "AAAA\xfa AAAAAAAAAAAAAAAAIASUHRISHDBGFJSHDFSAOSDOASDDDDASDOIJASGGG\xfa\xfa\xfa GGGGGIOSAIIIIIIIIII";
    e.puts(data);
    e.writeSequence();
    e2.write((void*)data, strlen(data));
    e2.writeSequence();
    if (!compareBuffers()) {
        std::cout << "Write != puts :(" << std::endl;
        return false;
    }
    e.puts("aaaabc");
    e.writeSequence();
    e2.putb('a');
    e2.putb('a');
    e2.putb('a');
    e2.putb('a');
    e2.putb('b');
    e2.putb('c');
    e2.writeSequence();

    return compareBuffers();
}

int main(int argc, char** argv)
{
    int testCounter = 0;
    int successCounter = 0;

#define TEST_FULL(name,func) \
    counterA = counterB = 0; \
    memset(bufferA, 0, BUFFER_MAX); \
    memset(bufferB, 0, BUFFER_MAX); \
    std::cout << "Running Test '" << name << "'..." << "    ";\
    testCounter++; \
    if (func()) { \
        std::cout << "PASS" << std::endl; \
        successCounter++; \
    } else { \
        std::cout << "FAIL!" << std::endl; \
    }

#define TEST(func) TEST_FULL(#func,func)

    // tests run here
    TEST(noCompression);
    TEST(decodeEncodeNoCompression);
    TEST(encodeDecode);
    TEST(encodeControl);
    TEST(encodeDecodeControl);
    TEST(methodEqualResult);
    // this is after the tests
    auto fails = testCounter - successCounter;
    std::cout << "RAN " << testCounter << " TESTS. SUCCESS: " << successCounter << std::endl;
    if (fails > 0)
    {
        std::cout << "HAD " << fails << " FAILS DURING TESTING!" << std::endl;
    }
    return fails;
}