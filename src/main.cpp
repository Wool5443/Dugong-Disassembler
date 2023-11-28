#include "Disassembler.hpp"

int main(int argc, const char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Please, give input and output files.\n");

        return ERROR_BAD_FILE;
    }

    const char* binFilePath = argv[1];
    const char* outFilePath = argv[2];

    ErrorCode disAsmError = Disassemble(binFilePath, outFilePath);

    return disAsmError;
}