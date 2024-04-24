import nacl.secret
import nacl.utils
import base64
from nacl.public import PrivateKey, SealedBox

# Generate public-private key pair
server_private_key = PrivateKey.generate()
server_public_key = server_private_key.public_key
print(server_public_key.encode(encoder=nacl.encoding.HexEncoder).decode('utf-8'))
#7544890c9f96afde9e103ad955afacd4e1638d15ba3981b77a2726c779b03375
# Save the keys to files
with open("server_private_key.txt", "wb") as f:
    f.write(server_private_key.encode(encoder=nacl.encoding.RawEncoder))

with open("server_public_key.txt", "wb") as f:
    f.write(server_public_key.encode(encoder=nacl.encoding.RawEncoder))

