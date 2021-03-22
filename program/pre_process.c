#include <stdio.h>
#include <inttypes.h>

//#define WORDLen 32
#define WORD uint32_t
#define PF PRIX32
#define BYTE uint8_t

// SHA256 works on blocks of 512 bits.
union Block {
    // 8 x 64 = 512 - dealing with block as bytes.
    BYTE bytes[64];
    // 32 x 16 = 512 - dealing with block as words.
    WORD words[16];
    // 64 x 8 = 512 - dealing with the last 64 bits of last block.
    uint64_t sixF[8];
};

// For keeping track of where we are with the input message/padding.
enum Status {
    READ, PAD, END
};

// get next Block.
int next_block(FILE *f, union Block *B, enum Status *S, uint64_t *num_of_bits) {
    // number of bytes read.
    size_t num_of_bytes;

    if (*S == END) {
        return 0;
    } else if (*S == READ) {
        // Try to read 64 bytes from the input file.
        num_of_bytes = fread(B->bytes, 1, 64, f);
        // Calculate the total bits read so far.
        *num_of_bits = *num_of_bits + (8 * num_of_bytes);
        // Enough room for padding.
        if (num_of_bits == 64) {
            // This happens when we can read 64 bytes from f.
            return 1;
        } else if (num_of_bytes < 56) {
            // This happens when we have enough roof for all the padding.
            // Append a 1 bit (and seven 0 bits to make a full byte).
            B->bytes[num_of_bytes] = 0x80; // In bits: 10000000.
            // Append enough 0 bits, leaving 64 at the end.
            for (num_of_bytes++; num_of_bytes < 56; num_of_bytes++) {
                B->bytes[num_of_bytes] = 0x00; // In bits: 00000000
            }
            // Append length of original input (CHECK ENDIANNESS).
            B->sixF[7] = *num_of_bits;
            // Say this is the last block.
            *S = END;
        } else {
            // Got to the end of the input message and not enough room
            // in this block for all padding.
            // Append a 1 bit (and seven 0 bits to make a full byte.)
            B->bytes[num_of_bytes] = 0x80;
            // Append 0 bits.
            for (num_of_bytes++; num_of_bytes < 64; num_of_bytes++) {
                // Error: trying to write to
                B->bytes[num_of_bytes] = 0x00; // In bits: 00000000
            }
            // Change the status to PAD.
            *S = PAD;
        }
    } else if (*S == PAD) {
        // Append 0 bits.
        for (num_of_bytes = 0; num_of_bytes < 56; num_of_bytes++) {
            B->bytes[num_of_bytes] = 0x00; // In bits: 00000000
        }
        // Append num_of_bits as an integer. CHECK ENDIAN!
        B->sixF[7] = *num_of_bits;
        // Change the status to END.
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
    if (!(f = fopen(argv[1], "r"))) {
        printf("Not able to read file", argv[1]);
        return 1;
    }

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