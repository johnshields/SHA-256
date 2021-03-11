#include <stdio.h>
#include <inttypes.h>

#define BYTE uint8_t

int main(int argc, char *argv[]) {
    BYTE b;

    FILE *f;

    f = fopen(argv[1], "r");
    size_t numBytes;

    numBytes = fread(&b, 1, 1, f);

    while (numBytes) {
        numBytes = fread(&b, 1, 1, f);
    }
    fclose(f);

    return 0;
}