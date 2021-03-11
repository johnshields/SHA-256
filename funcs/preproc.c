#include <stdio.h>
#include <inttypes.h>

#define BYTE uint8_t

int main(int argc, char *argv[]) {
    BYTE b;
    FILE *f;
    f = fopen(argv[1], "r");
    size_t numOfBytes;
    numOfBytes = fread(&b, 1, 1, f);

    while (numOfBytes) {
        printf("%c", b);
        numOfBytes = fread(&b, 1, 1, f);
    }
    fclose(f);
    printf("\n");

    return 0;
}