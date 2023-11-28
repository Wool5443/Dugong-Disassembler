#include <string.h>
#include <sys/stat.h>
#include "Disassembler.hpp"
#include "SPUsettings.ini"

static size_t _getFileSize(const char* path);

ErrorCode Disassemble(const char* binFilePath, const char* outFilePath)
{
    FILE* binFile = fopen(binFilePath, "rb");

    size_t binSize = _getFileSize(binFilePath);
    const byte* codeArray = (byte*)calloc(binSize, 1);
    MyAssertSoft(codeArray, ERROR_NO_MEMORY);

    fread((void*)codeArray, 1, binSize, binFile);

    FILE* outFile = fopen(outFilePath, "w");

    size_t codePosition = 0;

    byte command = codeArray[codePosition++];
    byte commandType = command & ~((byte)~0 << BITS_FOR_COMMAND);

    while (codePosition < binSize)
    {
        switch (commandType)
        {
            #define DEF_COMMAND(name, num, ...)                                                         \
            case num:                                                                                   \
            {                                                                                           \
                fprintf(outFile, "[%08lu]%4s%-4s%4s", codePosition - 1, "", #name, "");                 \
                double arg = 0;                                                                         \
                if (command & (ImmediateNumberArg << BITS_FOR_COMMAND))                                 \
                {                                                                                       \
                    memcpy(&arg, codeArray + codePosition, sizeof(double));                             \
                    codePosition += sizeof(double);                                                     \
                    fprintf(outFile, "%-8lg%4s", arg, "");                                              \
                }                                                                                       \
                if (command & (RegisterArg << BITS_FOR_COMMAND))                                        \
                {                                                                                       \
                    byte reg = codeArray[codePosition];                                                 \
                    codePosition++;                                                                     \
                    fprintf(outFile, "%-4s%4s", REG_NAMES[reg], "");                                    \
                }                                                                                       \
                fprintf(outFile, "\n");                                                                 \
                break;                                                                                  \
            }

            #include "Commands.gen"

            #undef DEF_COMMAND

            default:
                free((void*)codeArray);
                return ERROR_SYNTAX;
        }
        command = codeArray[codePosition];
        codePosition++;
        commandType = command & ~((byte)~0 << BITS_FOR_COMMAND);
    }
    free((void*)codeArray);
    fclose(outFile);
    return EVERYTHING_FINE;
}

static size_t _getFileSize(const char* path)
{
    MyAssertHard(path, ERROR_NULLPTR);

    struct stat result = {};

    stat(path, &result);

    return (size_t)result.st_size;
}
