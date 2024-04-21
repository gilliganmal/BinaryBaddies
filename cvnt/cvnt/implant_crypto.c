#include <sodium.h>
#include <stdio.h>
#include <string.h>

#define PUBLIC_KEY_BYTES crypto_kx_PUBLICKEYBYTES
#define SECRET_KEY_BYTES crypto_kx_SECRETKEYBYTES
#define SESSION_KEY_BYTES crypto_kx_SESSIONKEYBYTES
#define PASSWORD "Princess123!"
#define SALT_BYTES crypto_pwhash_SALTBYTES
#define KEY_BYTES crypto_secretstream_xchacha20poly1305_KEYBYTES
#define NONCE_BYTES crypto_aead_aes256gcm_NPUBBYTES
#define TAG_BYTES crypto_aead_aes256gcm_ABYTES

unsigned char key[KEY_BYTES];
unsigned char salt[SALT_BYTES];
unsigned char public_key[PUBLIC_KEY_BYTES];
unsigned char secret_key[SECRET_KEY_BYTES];
unsigned char session_key[SESSION_KEY_BYTES];

int initialize_crypto() {
    if (sodium_init() == -1) {
        fprintf(stderr, "libsodium initialization failed\n");
        return -1;
    }
    return 0;
}

int generate_keypair() {
    if (crypto_kx_keypair(public_key, secret_key) != 0) {
        fprintf(stderr, "Key pair generation failed\n");
        return -1;
    }
    return 0;
}

int generate_session_key(unsigned char *server_public_key) {
    if (crypto_kx_client_session_keys(session_key, NULL, public_key, secret_key, server_public_key) != 0) {
        fprintf(stderr, "Session key generation failed\n");
        return -1;
    }
    return 0;
}

int generate_key() {
    randombytes_buf(salt, sizeof salt);
    if (crypto_pwhash(key, sizeof key, PASSWORD, strlen(PASSWORD), salt,
                      crypto_pwhash_OPSLIMIT_INTERACTIVE,
                      crypto_pwhash_MEMLIMIT_INTERACTIVE, crypto_pwhash_ALG_DEFAULT) != 0) {
        fprintf(stderr, "Key generation from password failed\n");
        return -1;
    }
    return 0;
}

int encrypt_data(unsigned char *data, unsigned long long data_len,
                 unsigned char *nonce, unsigned char *ciphertext, unsigned long long *ciphertext_len) {
    randombytes_buf(nonce, NONCE_BYTES);
    if (crypto_aead_aes256gcm_encrypt(ciphertext, ciphertext_len, data, data_len, NULL, 0, NULL, nonce, key) != 0) {
        fprintf(stderr, "Encryption failed\n");
        return -1;
    }
    return 0;
}

int decrypt_data(unsigned char *ciphertext, unsigned long long ciphertext_len,
                 unsigned char *nonce, unsigned char *data, unsigned long long *decrypted_len) {
    if (crypto_aead_aes256gcm_decrypt(data, decrypted_len, NULL, ciphertext, ciphertext_len, NULL, 0, nonce, key) != 0) {
        fprintf(stderr, "Decryption failed\n");
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (initialize_crypto() != 0) return 1;

    if (generate_keypair() != 0) return 1;

    
    unsigned char server_public_key[PUBLIC_KEY_BYTES];
    if (generate_keypair() != 0) return 1; // simulate server key pair generation
    memcpy(server_public_key, public_key, PUBLIC_KEY_BYTES);

    if (generate_session_key(server_public_key) != 0) return 1;

    if (generate_key() != 0) return 1;

    unsigned char data[] = "Hello, World!";
    unsigned char nonce[NONCE_BYTES];
    unsigned char ciphertext[128];
    unsigned long long ciphertext_len;

    if (encrypt_data(data, strlen((char *)data), nonce, ciphertext, &ciphertext_len) != 0) return 1;

    unsigned char decrypted[128];
    unsigned long long decrypted_len;

    if (decrypt_data(ciphertext, ciphertext_len, nonce, decrypted, &decrypted_len) != 0) return 1;

    printf("Decrypted: %s\n", decrypted);

    return 0;
}

