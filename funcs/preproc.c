#include <stdio.h>
#include <inttypes.h>

#define BYTE uint8_t

void bin_print(BYTE i) {
    // number of bits in an integer
    int j = sizeof(BYTE) * 8;

    // temporary variable
    int k;

    // -- = unary operator
    // >= = binary operator
    // loop over the number of bits in i
    for (j--; j >= 0; j--) {
        // ternary operator - '?' - checks true and false
        // pick out the j^th bit of i.
        k = ((1 << j) & i) ? 1 : 0;
        // print k
        printf("%d", k);
    }
}

int main(int argc, char *argv[]) {
    BYTE b;
    FILE *f;
    f = fopen(argv[1], "r");
    size_t num_of_bytes;
    num_of_bytes = fread(&b, 1, 1, f);

    while (num_of_bytes) {
        bin_print(b);
        num_of_bytes = fread(&b, 1, 1, f);
    }
    fclose(f);
    printf("\n");

    return 0;
}