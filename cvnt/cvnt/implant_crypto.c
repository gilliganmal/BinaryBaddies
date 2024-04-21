#include <sodium.h>
#include <stdio.h>

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

int main(int argc, char *argv[]) {
    if (initialize_crypto() != 0) {
        return 1;
    }
    if (generate_keypair() != 0) {
        return 1;
    }
    // Additional operations here
    return 0;
}

