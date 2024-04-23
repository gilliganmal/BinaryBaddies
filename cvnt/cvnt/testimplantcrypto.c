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
    unsigned char server_public_key[] = {
    0x16, 0xdd, 0x11, 0xe4, 0x36, 0x59, 0xb7, 0x09,
    0x8d, 0x43, 0xd0, 0x2f, 0xe9, 0x61, 0x47, 0xa2,
    0x3e, 0xf9, 0x00, 0x06, 0x89, 0xd9, 0x8b, 0xbe,
    0xc9, 0x87, 0x49, 0xe8, 0x31, 0x50, 0x64, 0x39
};

    // send this message
    unsigned char plaintext[] = "Hello, Baddies!";
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
