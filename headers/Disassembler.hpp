#include "Utils.hpp"

typedef unsigned char byte;

static const size_t DEFAULT_TEXT_SIZE = 128;

struct Text
{
    char* text;
    size_t size;
    size_t capacity;
};

struct TextResult
{
    Text value;
    ErrorCode error;
};

TextResult Disassemble(const char* binFilePath);

TextResult TextInit();

ErrorCode TextDestructor(Text*);
