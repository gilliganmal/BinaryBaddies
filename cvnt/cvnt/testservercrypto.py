import nacl.secret
import nacl.utils
import base64
from nacl.public import PrivateKey, SealedBox, PublicKey

# Load the public-private key pair from files
with open("private_key.txt", "rb") as f:
    private_key_bytes = f.read()
    server_private_key = PrivateKey(private_key_bytes)

with open("public_key.txt", "rb") as f:
    public_key_bytes = f.read()
    server_public_key = PublicKey(public_key_bytes)

# Load the ciphertext from the file
with open("ciphertext.py", "r") as file:
    exec(file.read())

# Decode the Base64-encoded ciphertext
ciphertext_bytes = base64.b64decode(ciphertext)

# Decrypt the ciphertext
plaintext = SealedBox(server_private_key).decrypt(ciphertext_bytes)

# Print the decrypted plaintext
print("Decrypted plaintext:", plaintext.decode('utf-8'))

