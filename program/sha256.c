/*
 * The Ch function in C - One of the inline functions in the Secure Hash Standard.
 * SHA: Constants and functions - The functions and constants of the Secure Hash Algorithm.
 *
 * SHA-224 and SHA-256 both use six logical functions, where each function operates on 32-bit
 * words, which are represented as x, y, and z. The result of each function is a new 32-bit word.
 *
 * https://web.microsoftstream.com/video/7fed3236-f072-433f-a512-a3007da35953
 * [1] https://www.nist.gov/publications/secure-hash-standard
 * [2] https://developer.ibm.com/technologies/systems/articles/au-endianc/
 * https://www.geeksforgeeks.org/bitwise-operators-in-c-cpp/
 * https://www.guru99.com/c-bitwise-operators.html
*/

#include <stdio.h>
#include <inttypes.h>
#include <byteswap.h>

// [2] Endianness
const int _i = 1;
#define is_little_endian() ((*(char*)&_i) != 0) // char = 8 bits

// Words and bytes.
#define WORD uint32_t
#define PF PRIx32
#define BYTE uint8_t

// Page 5 of the secure hash standard. [1]
#define ROTL(_x, _n) ((_x << _n) | (_x >> ((sizeof(_x)*8) - _n)))
#define ROTR(_x, _n) ((_x >> _n) | (_x << ((sizeof(_x)*8) - _n)))
#define SHR(_x, _n) (_x >> _n)

// Page 10 of the secure hash standard. [1]
#define CH(_x, _y, _z) ((_x & _y) ^ (~_x & _z))
#define MAJ(_x, _y, _z) ((_x & _y) ^ (_x & _z) ^ (_y & _z))

#define SIG0(_x) (ROTR(_x,2)  ^ ROTR(_x,13) ^ ROTR(_x,22))
#define SIG1(_x) (ROTR(_x,6)  ^ ROTR(_x,11) ^ ROTR(_x,25))
#define Sig0(_x) (ROTR(_x,7)  ^ ROTR(_x,18) ^ SHR(_x,3))
#define Sig1(_x) (ROTR(_x,17) ^ ROTR(_x,19) ^ SHR(_x,10))

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

// Page 11 - section 4.2.2 of secure hash standard. [1]
const WORD K[] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// Returns 1 if it created a new block from original message or padding.
// Returns 0 if all padded message has already been consumed.
// get next Block.
int next_block(FILE *f, union Block *M, enum Status *S, uint64_t *num_of_bits) {
    // number of bytes read.
    size_t num_of_bytes;

    if (*S == END) {
        return 0;
    } else if (*S == READ) {
        // Try to read 64 bytes from the input file.
        num_of_bytes = fread(M->bytes, 1, 64, f);
        // Calculate the total bits read so far.
        *num_of_bits = *num_of_bits + (8 * num_of_bytes);
        // Enough room for padding.
        if (num_of_bytes == 64) {
            // This happens when we can read 64 bytes from f.
            return 1;
        } else if (num_of_bytes < 56) {
            // This happens when we have enough roof for all the padding.
            // Append a 1 bit (and seven 0 bits to make a full byte).
            M->bytes[num_of_bytes] = 0x80; // In bits: 10000000.
            // Append enough 0 bits, leaving 64 at the end.
            for (num_of_bytes++; num_of_bytes < 56; num_of_bytes++) {
                M->bytes[num_of_bytes] = 0x00; // In bits: 00000000
            }
            // Append length of original input - Check endianness.
            M->sixF[7] = (is_little_endian() ? bswap_64(*num_of_bits) : *num_of_bits);
            // Say this is the last block.
            *S = END;
        } else {
            // Got to the end of the input message and not enough room
            // in this block for all padding.
            // Append a 1 bit (and seven 0 bits to make a full byte.)
            M->bytes[num_of_bytes] = 0x80;
            // Append 0 bits.
            for (num_of_bytes++; num_of_bytes < 64; num_of_bytes++) {
                // Error: trying to write to
                M->bytes[num_of_bytes] = 0x00; // In bits: 00000000
            }
            // Change the status to PAD.
            *S = PAD;
        }
    } else if (*S == PAD) {
        // Append 0 bits.
        for (num_of_bytes = 0; num_of_bytes < 56; num_of_bytes++) {
            M->bytes[num_of_bytes] = 0x00; // In bits: 00000000
        }
        // Append num_of_bits as an integer - Check endian
        //M->sixF[7] = *num_of_bits;
        M->sixF[7] = (is_little_endian() ? bswap_64(*num_of_bits) : *num_of_bits);
        // Change the status to END.
        *S = END;
    }

    // swap the byte order of the words if is_little_endian
    if (is_little_endian()) {
        for (int i = 0; i < 16; i++) {
            M->words[i] = bswap_32(M->words[i]);
        }
    }
    return 1;
}

// designed to make it difficult to reverse the process
int next_hash(union Block *M, WORD H[]) {
    // Message schedule, [1] Section 6.2.2
    WORD W[64];
    // Iterator.
    int t;
    // Temporary variables.
    WORD a, b, c, d, e, f, g, h, T1, T2;

    // [1] Section 6.2.2, part 1.
    for (t = 0; t < 16; t++)
        W[t] = M->words[t];
    for (t = 16; t < 64; t++)
        W[t] = Sig1(W[t - 2]) + W[t - 7] + Sig0(W[t - 15]) + W[t - 16];

    // [1] Section 6.2.2, part 2.
    a = H[0];
    b = H[1];
    c = H[2];
    d = H[3];
    e = H[4];
    f = H[5];
    g = H[6];
    h = H[7];

    // [1] Section 6.2.2, part 3.
    for (t = 0; t < 64; t++) {
        T1 = h + SIG1(e) + CH(e, f, g) + K[t] + W[t];
        T2 = SIG0(a) + MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;
    }

    // [1] Section 6.2.2, part 4.
    // next hash from current message block and previous hash value
    H[0] = a + H[0];
    H[1] = b + H[1];
    H[2] = c + H[2];
    H[3] = d + H[3];
    H[4] = e + H[4];
    H[5] = f + H[5];
    H[6] = g + H[6];
    H[7] = h + H[7];

    return 0;
}

// The function that performs/orchestrates the SHA256 algorithm on message f.
int sha256(FILE *f, WORD H[]) {
    // The current block.
    union Block M;

    // Total number of bits read.
    uint64_t num_of_bits = 0;

    // Current status of reading input.
    enum Status S = READ;

    // Loop through the (preprocessed) blocks.
    while (next_block(f, &M, &S, &num_of_bits)) {
        next_hash(&M, H);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    // [1] Section 5.3.4
    WORD H[] = {
            0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
            0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    // File pointer for reading.
    FILE *f;

    if (argc != 2) {
        printf("[ALERT] expected filename in argument \n");
        return 1;
    }

    // Open file from command line for reading.
    if (!(f = fopen(argv[1], "r"))) {
//    if (!(f = fopen("input.txt", "w+"))) {
        printf("[ALERT] Not able to read file %s. \n", argv[1]);
        return 1;
    }

    // Calculate the SHA256 of f.
    sha256(f, H);

    // Print the final SHA256 hash.
    for (int i = 0; i < 8; i++)
        printf("%08" PF, H[i]);
    printf("\n");

    // Close the file.
    fclose(f);

    return 0;
}