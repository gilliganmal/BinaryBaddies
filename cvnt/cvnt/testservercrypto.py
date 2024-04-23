import nacl.public
import nacl.secret
import nacl.utils
import base64
from nacl.public import PrivateKey, SealedBox

# Generate public private key pair
server_private_key = PrivateKey.generate()
#server_public_key = server_private_key.public_key
#print(server_public_key.encode(encoder=nacl.encoding.HexEncoder).decode('utf-8'))
server_public_key = b'b67aaeb539c0b7dc0d2c68a0a5b34ff4f24758c424875cdfd9d985e9ab1d1d73'


# Load the ciphertext from the file
with open("ciphertext.py", "r") as file:
    exec(file.read())

# Decode the Base64-encoded ciphertext
ciphertext_bytes = base64.b64decode(ciphertext)

# Decrypt the ciphertext
plaintext = nacl.public.SealedBox(server_private_key).decrypt(ciphertext_bytes)

# Print the decrypted plaintext
print("Decrypted plaintext:", plaintext.decode('utf-8'))
