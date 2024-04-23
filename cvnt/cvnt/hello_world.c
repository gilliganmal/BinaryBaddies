#include <sodium.h>
#include <stdio.h>

int main(void) {
    if (sodium_init() == -1) {
        return 1;
    }

    unsigned char key[crypto_aead_aes256gcm_KEYBYTES];
    unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES];
    unsigned char ciphertext[128];
    unsigned char decrypted[128];
    const unsigned char message[] = "Hello, World!";
    unsigned long long ciphertext_len;

    // Randomly generate a key and a nonce
    randombytes_buf(key, sizeof key);
    randombytes_buf(nonce, sizeof nonce);

    // Encrypt
    crypto_aead_aes256gcm_encrypt(ciphertext, &ciphertext_len,
                                  message, sizeof message,
                                  NULL, 0,
                                  NULL, nonce, key);

    // Decrypt
    if (crypto_aead_aes256gcm_decrypt(decrypted, NULL,
                                      NULL,
                                      ciphertext, ciphertext_len,
                                      NULL, 0,
                                      nonce, key) != 0) {
        // message forged!
        return 1;
    }

    printf("Original: %s\\n", message);
    printf("Decrypted: %s\\n", decrypted);

    return 0;
}
