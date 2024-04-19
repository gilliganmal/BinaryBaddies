import socket
from client_pb2 import Command

# Replace later after deployment??
HOST, PORT = 'localhost', 5000

#send the command from operator to the server
def send_command(command_str):
    # Split the command string into command and arguments.
    parts = command_str.split(' ', 1)
    cmd = Command()
    if len(parts) == 2:
        cmd.cmd, cmd.args = parts
    else:
        cmd.cmd = parts[0]
        cmd.args = ""

    # Connect to the server and send the command.
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((HOST, PORT))
        sock.sendall(cmd.SerializeToString())
        response = sock.recv(1024)
        print("Received response:", response)


def main():
    while True:
        try:
            # Display a simple shell prompt.
            command_str = input("Enter command: ")
            if command_str.lower() == 'exit':
                break
            send_command(command_str)
        except KeyboardInterrupt:
            break
        except Exception as e:
            print(f"An error occurred: {e}")

if __name__ == "__main__":
    main()
