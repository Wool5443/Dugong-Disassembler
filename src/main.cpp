#include "Disassembler.hpp"

int main(int argc, const char* argv[])
{
    argv[1] = "factorial";
    argv[2] = "out.txt";

    const char* binFilePath = argv[1];
    const char* outFilePath = argv[2];

    TextResult textResult = Disassemble(binFilePath);

    if (!textResult.error)
    {
        Text text = textResult.value;
        FILE* outFile = fopen(outFilePath, "w");
        fwrite(text.text, 1, text.size, outFile);
        fclose(outFile);
    }

    TextDestructor(&textResult.value);

    return textResult.error;
}