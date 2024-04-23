import nacl.public
import nacl.secret
import nacl.utils
import base64
from nacl.public import PrivateKey, SealedBox


# Generate public private key pair
server_private_key = PrivateKey.generate()
server_public_key = server_private_key.public_key
print(server_public_key.encode(encoder=nacl.encoding.HexEncoder).decode('utf-8'))

def generate_or_load_private_key(filename):
    try:
        # Try to load the private key from file
        with open(filename, "rb") as file:
            private_key = PrivateKey(file.read())
        print("Private key loaded successfully")
    except FileNotFoundError:
        # If file doesn't exist, generate a new private key and save it
        private_key = PrivateKey.generate()
        with open(filename, "wb") as file:
            file.write(private_key.encode())
        print("New private key generated and saved")
    return private_key

# Generate or load the private key
server_private_key = generate_or_load_private_key("private_key.bin")

# Load the ciphertext from the file
with open("ciphertext.py", "r") as file:
    exec(file.read())

# Decode the Base64-encoded ciphertext
ciphertext_bytes = base64.b64decode(ciphertext)

# Decrypt the ciphertext
plaintext = nacl.public.SealedBox(server_private_key).decrypt(ciphertext_bytes)

# Print the decrypted plaintext
print("Decrypted plaintext:", plaintext.decode('utf-8'))





