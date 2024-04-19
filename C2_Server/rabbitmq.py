import pika
from implant_pb2 import TaskRequest  # Import your protobuf module

# Establish connection to RabbitMQ
connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
channel = connection.channel()

# Declare a queue for tasks
channel.queue_declare(queue='task_queue')

def send_command(opcode, args):
    # Create a protobuf TaskRequest message
    task_request = TaskRequest()
    task_request.Opcode = opcode  # Set the opcode according to your defined opcodes
    task_request.Args = args  # Pass the arguments for the task

    # Serialize the message to a string using protobuf
    message = task_request.SerializeToString()

    # Publish the message to the task queue
    channel.basic_publish(exchange='',
                          routing_key='task_queue',
                          body=message)
    print(" [x] Sent command")

send_command(1, 'ls')  # Example command, replace with your actual opcodes and arguments
