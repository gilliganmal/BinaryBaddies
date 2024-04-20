#include <sodium.h>

int main() {
    // Initialize libsodium
    if (sodium_init() < 0) {
        return 1;
    }

    // Password-based key derivation
    char password[] = "your-secure-password";
    unsigned char salt[crypto_pwhash_SALTBYTES];
    unsigned char key[crypto_aead_aes256gcm_KEYBYTES];
    randombytes_buf(salt, sizeof salt);
    if (crypto_pwhash
        (key, sizeof key, password, strlen(password), salt,
         crypto_pwhash_OPSLIMIT_MODERATE, crypto_pwhash_MEMLIMIT_MODERATE,
         crypto_pwhash_ALG_DEFAULT) != 0) {
        /* out of memory */
    }

    // Encryption
    unsigned char nonce[crypto_aead_aes256gcm_NPUBBYTES];
    unsigned char ciphertext[128];
    unsigned long long ciphertext_len;
    unsigned char additional_data[1] = {0};
    unsigned char message[] = "Sensitive data";
    randombytes_buf(nonce, sizeof nonce);
    crypto_aead_aes256gcm_encrypt(ciphertext, &ciphertext_len,
                                  message, sizeof message,
                                  additional_data, sizeof additional_data,
                                  NULL, nonce, key);

    // Decryption
    unsigned char decrypted[128];
    if (crypto_aead_aes256gcm_decrypt(decrypted, NULL,
                                      NULL,
                                      ciphertext, ciphertext_len,
                                      additional_data, sizeof additional_data,
                                      nonce, key) != 0) {
        /* message forged or wrong key used */
    }

    return 0;
}

