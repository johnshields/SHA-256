/*
 * John Shields
 * Bit Operations
 *
 * Lab Tutorials - https://web.microsoftstream.com/video/9e7f870c-309e-43eb-805e-ea4e7ee719e1
 * https://web.microsoftstream.com/video/78e6725b-35ff-4818-b411-8eec4dea15c2?referrer=https:%2F%2Flearnonline.gmit.ie%2F
 * https://web.microsoftstream.com/video/350c8cb1-d005-4c21-a534-c5947c7f9ef1
*/

#include <stdio.h>

void bin_print(unsigned int i) {
    // number of bits in an integer
    int j = sizeof(int) * 8;

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

// standard main signature
int main(int argc, char *argv[]) {
    // i to a literal value
    unsigned int i = 0x0f0f0f0f; // 1; // 0xf1; // 241 // 4294967295

    printf("Original:\t");
    // print i in binary
    bin_print(i);
    printf("\t%x\t%u\n\n", i, i);

    // 32
    int j = sizeof(unsigned int) * 8;

    for (j--; j >= 0 ; j--) {
        // 1 shifted left j times
        bin_print(1 << j); printf("\n");
        // i
        bin_print(i); printf("\n");
        printf("-------------------------------- &\n");
        // (1 shifted left j times) bitwise logical and i
        bin_print((1 << j) & i); printf("\n\n");
    }

    // everything is good
    return 0;
}