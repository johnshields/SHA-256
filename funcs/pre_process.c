#include <stdio.h>
#include <inttypes.h>

#define W 32
#define WORD uint32_t
#define PF PRIX32
#define BYTE uint8_t

union Block {
    BYTE bytes[64];
    WORD words[16];
};

int main(int argc, char *argv[]) {

    // Iterator
    int i;

    // current block.
    union Block B;

    uint64_t num_of_bits = 0;
    // File pointer for reading.
    FILE *f;
    // Open file from cli for reading.
    f = fopen(argv[1], "r");

    // number of bytes read.
    size_t num_of_bytes;
    // try to read 64 bytes.
    num_of_bytes = fread(B.bytes, 1, 64, f);
    // tell the cli how many is read.
    printf("Read %zu bytes.\n", num_of_bytes);
    // update number of bits read.
    num_of_bits = num_of_bits + (8 * num_of_bytes);
    // print the 16 32-bit words.
    for (i = 0; i < 16; i++) {
        printf("%08" PF " ", B.words[i]);
        if ((i + 1) % 8 == 0)
            printf("\n");
    }
    while (!feof(f)) {
        // same as above.
        num_of_bytes = fread(&B.bytes, 1, 64, f);
        printf("Read %zu bytes.\n", num_of_bytes);
        num_of_bits = num_of_bits + (8 * num_of_bytes);

        // print the 16 32-bit words.
        for (i = 0; i < 16; i++) {
            printf("%08" PF " ", B.words[i]);
            if ((i + 1) % 8 == 0)
                printf("\n");
        }
    }
    fclose(f);
    // print total number of bits read.
    printf("Total number of bits read: %lu.\n", num_of_bits);

    return 0;
}