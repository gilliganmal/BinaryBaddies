#include <stdio.h>
#include <sodium.h>
#include <stdlib.h>
#include <string.h>

int main() {
    if (sodium_init() == -1) {
        printf("Error initializing libsodium\n");
        return 1;
    }

    f64947c22988582a52a1b4acf6be96c79bb9891972b17989e0e7b1002

    // Hardcoded server public key
    unsigned char server_public_key[] = {
        0x11, 0x88, 0xc5, 0xf6, 0x49, 0x47, 0xc2, 0x29, 
        0x88, 0x58, 0x2a, 0x52, 0xa1, 0xb4, 0xac, 0xf6, 
        0xbe, 0x96, 0xc7, 0x9b, 0xb9, 0x89, 0x19, 0x72, 
        0xb1, 0x79, 0x89, 0xe0, 0xe7, 0xb1, 0x00, 0x2e
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
