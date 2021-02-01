// John Shields
// Bit Operations
// Lab Tutorial - https://web.microsoftstream.com/video/9e7f870c-309e-43eb-805e-ea4e7ee719e1

#include <stdio.h>

void bin_print(int i) {
    // number of bits in an integer
    int j = sizeof(int) * 8;

    // temporary variable
    int k;

    for (j--; j >= 0; j--) {
        k = ((1 << j) & i) ? 1 : 0;
        printf("%d", k);
    }

}

int main(int argc, char *argv[]) {
    int i = 0xf1;

    printf("Original:  ");
    bin_print(i);
    printf("\n");

    for (int j = 0; j < 40; j++) {
        printf("%3d << %2d: ", i, j);
        bin_print(i << j);
        printf("\n");
    }

    return 0;
}