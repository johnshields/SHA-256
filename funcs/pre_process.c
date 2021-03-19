#include <stdio.h>
#include <inttypes.h>

#define W 32
#define WORD uint32_t
#define PF PRIX32
#define BYTE uint8_t

union Block {
    BYTE bytes[64];
    WORD words[16];
    uint64_t sixF[9];
};

enum Status {
    READ, PAD, PAD0, PAD1, END
};

// get next Block.
int next_block(FILE *f, union Block *B, enum Status *S, uint64_t *num_of_bits) {
    // number of bytes read.
    size_t num_of_bytes;

    if (*S == END) {
        return 0;
    } else if (*S == READ) {
        // try to read 64 bytes.
        num_of_bytes = fread(B->bytes, 1, 64, f);
        // calculate the total bits read so far.
        *num_of_bits = *num_of_bits + (8 * num_of_bytes);
        // enough room for padding.
        if (num_of_bytes == 64) {
            return 1;
        } else if (num_of_bytes <= 55) {
            // append a 1 bit (and seven 0 bits to make  full byte)
            B->bytes[num_of_bytes] = 0x80; // In bits: 1000000
            // append enough 0 bits, leaving 64 at the end.
            while (num_of_bytes++ < 56) {
                B->bytes[num_of_bytes] = 0x00; // In bits: 0000000
            }
            // append length of original input.
            B->sixF[7] = *num_of_bits;
            // last block.
            *S = END;
        } else {
            // gotten to the end of the input message.
            // not enough room in the black fo all padding.
            // append a 1 bit (and seven 0 bits to make a full byte)
            B->bytes[num_of_bytes] = 0x80;
            // append 0 bits.
            while (num_of_bytes++ < 64) {
                B->bytes[num_of_bytes] = 0x00; // In bits: 0000000
            }
            // change the status to PAD
            *S = PAD;
        }
    } else if (*S == PAD) {
        num_of_bytes = 0;
        // append 0 bits.
        while (num_of_bytes++ < 56) {
            B->bytes[num_of_bytes] = 0x00; // In bits: 0000000
        }
        // append num of bits as a integer.
        B->sixF[7] = *num_of_bits;
        // change the status to END
        *S = END;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    // Iterator
    int i;
    // current block.
    union Block B;
    // total number of bits read.
    uint64_t num_of_bits = 0;
    // current status of reading input,
    enum Status S = READ;
    // File pointer for reading.
    FILE *f;
    // Open file from cli for reading.
    f = fopen(argv[1], "r");

    // loop through the pre-processed Blocks
    while (next_block(f, &B, &S, &num_of_bits)) {
        // print the 16 32-bit words.
        for (i = 0; i < 16; i++) {
            printf("%08" PF " ", B.words[i]);
        }
        printf("\n");
    }
    // close file
    fclose(f);
    // print total number of bits read.
    printf("Total number of bits read: %lu.\n", num_of_bits);

    return 0;
}