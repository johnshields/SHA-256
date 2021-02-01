// John Shields
// Bit Operations
// Lab Tutorial - https://web.microsoftstream.com/video/9e7f870c-309e-43eb-805e-ea4e7ee719e1

#include <stdio.h>

void bin_print(int i) {
    // number of bits in an integer
    int j = sizeof(int) * 8;

    // temporary variable
    int k;

    // -- = unary operator
    // >= = binary operator
    for (j--; j >= 0; j--) {
        // ternary operator
        k = ((1 << j) & i) ? 1 : 0;
        printf("%d", k);
    }
}

int main(int argc, char *argv[]) {
    int i = 0xf1; //11110001

//    printf("Dec %\n", i);
//    printf("Size of i: %d", sizeof(i));
//    printf("Size of int: %d", sizeof(int));
//    printf("Size of char: %d", sizeof(char));
//
//    char c = 41;
//    printf("c in char is %c\n", c);
//    printf("c in int is %d\n", c);
//
//    int j = 1000000000;
//    printf("j int is %d\n", c);
//    printf("j char is %c\n", (char) j);
//    printf("j int from char is %d\n", (int) (char) j);

//    for (unsigned  int i = 0: i <100000000000000000; i++);
//    printf("1");
//
//    printf("\n");

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