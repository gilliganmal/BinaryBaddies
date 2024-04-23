from nacl.public import PrivateKey, PublicKey
from nacl.encoding import HexEncoder

# Generate public private key pair
server_private_key = PrivateKey.generate()
server_public_key = server_private_key.public_key

# Encode the public key in hexadecimal format for easy embedding or display
encoded_public_key = server_public_key.encode(encoder=HexEncoder).decode('utf-8')
print(encoded_public_key)