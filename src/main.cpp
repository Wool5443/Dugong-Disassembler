#include "Disassembler.hpp"

int main(int argc, const char* argv[])
{
    argv[1] = "factorial";
    argv[2] = "out.txt";

    const char* binFilePath = argv[1];
    const char* outFilePath = argv[2];

    ErrorCode disAsmError = Disassemble(binFilePath, outFilePath);

    return disAsmError;
}