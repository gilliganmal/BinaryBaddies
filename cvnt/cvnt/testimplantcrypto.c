#include <stdio.h>
#include <sodium.h>
#include <stdlib.h>
#include <string.h>

int main() {
    if (sodium_init() == -1) {
        printf("Error initializing libsodium\n");
        return 1;
    }

    // Hardcoded server public key
    unsigned char server_public_key[crypto_box_PUBLICKEYBYTES] = {
        0x6b, 0xd9, 0x34, 0x7c, 0xbc, 0x91, 0x5d, 0xb3,
        0x55, 0xb4, 0x0b, 0x7b, 0x5b, 0x0e, 0x97, 0x69,
        0x4e, 0x6f, 0xc3, 0x57, 0xfe, 0xa0, 0x8b, 0xaf,
        0xf1, 0xdf, 0xcf, 0xe6, 0xea, 0x17, 0x5b, 0x22
    };

    // send this message
    unsigned char plaintext[] = "Hello, world!";
    unsigned long long plaintext_len = sizeof(plaintext);

    // Allocate memory for the ciphertext + overhead 
    unsigned char ciphertext[crypto_box_SEALBYTES + plaintext_len];

    // Seal the plaintext using the server's public key
    if (crypto_box_seal(ciphertext, plaintext, plaintext_len, server_public_key) != 0) {
        printf("Error sealing the plaintext\n");
        return 1;
    }

    // Calculate the maximum possible size of the Base64 encoded ciphertext
    size_t base64_maxlen = sodium_base64_ENCODED_LEN(sizeof(ciphertext), sodium_base64_VARIANT_ORIGINAL);

    // Allocate memory for the Base64 encoded ciphertext
    char *base64_ciphertext = malloc(base64_maxlen);
    if (base64_ciphertext == NULL) {
        printf("Error allocating memory for Base64\n");
        return 1;
    }

    // Encode the ciphertext using Base64 encoding
    if (sodium_bin2base64(base64_ciphertext, base64_maxlen, ciphertext, sizeof(ciphertext), sodium_base64_VARIANT_ORIGINAL) == NULL) {
        printf("Error encoding ciphertext to Base64\n");
        free(base64_ciphertext);
        return 1;
    }

    // Write  to  python file
    FILE *file = fopen("ciphertext.py", "w");
    if (file == NULL) {
        printf("Error writing\n");
        free(base64_ciphertext);
        return 1;
    }

    fprintf(file, "ciphertext = b'%s'\n", base64_ciphertext);

    fclose(file);

    // Free dynamically allocated memory
    free(base64_ciphertext);

    return 0;
}
