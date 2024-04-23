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
        0xb6, 0x7a, 0xae, 0xb5, 0x39, 0xc0, 0xb7, 0xdc, 
        0x0d, 0x2c, 0x68, 0xa0, 0xa5, 0xb3, 0x4f, 0xf4, 
        0xf2, 0x47, 0x58, 0xc4, 0x24, 0x87, 0x5c, 0xdf, 
        0xd9, 0xd9, 0x85, 0xe9, 0xab, 0x1d, 0x1d, 0x73
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
