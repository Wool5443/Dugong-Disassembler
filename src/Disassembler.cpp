#include <string.h>
#include <sys/stat.h>
#include "Disassembler.hpp"
#include "SPUsettings.ini"

static size_t _getFileSize(const char* path);

static ErrorCode _reallocText(Text* text);

TextResult Disassemble(const char* binFilePath)
{
    FILE* binFile = fopen(binFilePath, "rb");

    size_t binSize = _getFileSize(binFilePath);
    const byte* codeArray = (byte*)calloc(binSize, 1);
    MyAssertSoftResult(codeArray, NULL, ERROR_NO_MEMORY);

    fread((void*)codeArray, 1, binSize, binFile);

    size_t codePosition = 0;

    TextResult textResult = TextInit();
    MyAssertSoftResult(textResult.error == EVERYTHING_FINE, {}, textResult.error);
    Text text = textResult.value;

    byte command = codeArray[codePosition++];
    byte commandType = command & ~((byte)~0 << BITS_FOR_COMMAND);

    while (codePosition < binSize)
    {
        switch (commandType)
        {
            #define DEF_COMMAND(name, num, ...)                                                         \
            case num:                                                                                   \
            {                                                                                           \
                text.size += sprintf(text.text + text.size, "%s%4s", #name, "");                        \
                printf("%s%4s", #name, "");                                                             \
                _reallocText(&text);                                                                    \
                double arg = 0;                                                                         \
                byte   reg = 0;                                                                         \
                if (command & (ImmediateNumberArg << BITS_FOR_COMMAND))                                 \
                {                                                                                       \
                    memcpy(&arg, codeArray + codePosition, sizeof(double));                             \
                    codePosition += sizeof(double);                                                     \
                    text.size += sprintf(text.text + text.size, "%lg%4s", arg, "");                     \
                    printf("%lg%4s", arg, "");                                                          \
                    _reallocText(&text);                                                                \
                }                                                                                       \
                if (command & (RegisterArg << BITS_FOR_COMMAND))                                        \
                {                                                                                       \
                    reg = codeArray[codePosition];                                                      \
                    codePosition++;                                                                     \
                    text.size += sprintf(text.text + text.size, "%s %u%4s", REG_NAMES[regNum], "");     \
                    printf("%s%4s", REG_NAMES[regNum], "");                                             \
                    _reallocText(&text);                                                                \
                }                                                                                       \
                text.size += sprintf(text.text + text.size, "\n");                                      \
                printf("\n");                                                                           \
                _reallocText(&text);                                                                    \
                break;                                                                                  \
            }

            #include "Commands.gen"

            #undef DEF_COMMAND

            default:
                free((void*)codeArray);
                return {{}, ERROR_SYNTAX};
        }
        command = codeArray[codePosition];
        codePosition++;
        commandType = command & ~((byte)~0 << BITS_FOR_COMMAND);
    }
    free((void*)codeArray);
    return {text, EVERYTHING_FINE};
}

TextResult TextInit()
{
    char* text = (char*)calloc(DEFAULT_TEXT_SIZE, 1);
    if (!text)
        return {{}, ERROR_NO_MEMORY};
    
    return 
    {
        {
        .text = text,
        .size = 0,
        .capacity = DEFAULT_TEXT_SIZE,
        },
        EVERYTHING_FINE
    };
}

ErrorCode TextDestructor(Text* text)
{
    MyAssertSoft(text, ERROR_NULLPTR);
    free((void*)text->text);
    return EVERYTHING_FINE;
}

static ErrorCode _reallocText(Text* text)
{
    if (text->capacity - text->size < 32)
    {
        char* newText = (char*)realloc(text->text, text->capacity * 2);
        if (!newText)
            return ERROR_NO_MEMORY;
        text->text = newText;
        text->capacity *= 2;
    }
    return EVERYTHING_FINE;
}

static size_t _getFileSize(const char* path)
{
    MyAssertHard(path, ERROR_NULLPTR);

    struct stat result = {};

    stat(path, &result);

    return (size_t)result.st_size;
}
