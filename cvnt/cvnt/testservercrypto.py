import nacl.secret
import nacl.utils
import base64
from nacl.public import PrivateKey

# Generate public private key pair
server_private_key = PrivateKey.generate()
server_public_key = server_private_key.public_key
# Encode the public key in hexadecimal format for easy embedding or display
encoded_public_key = server_public_key.encode(encoder=HexEncoder).decode('utf-8')
print(encoded_public_key)

# Load the ciphertext from the file
with open("ciphertext.py", "r") as file:
    exec(file.read())

# Decode the Base64-encoded ciphertext
ciphertext_bytes = base64.b64decode(ciphertext)

# Decrypt the ciphertext
plaintext = nacl.secret.SealedBox(server_private_key).decrypt(ciphertext_bytes)

# Print the decrypted plaintext
print("Decrypted plaintext:", plaintext.decode('utf-8'))
