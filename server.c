#include "server.h"
#include <netdb.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Require 2 command line inputs in the format : %s <port>\n",
            argv[0]);
    exit(EXIT_FAILURE);
  }

  int binding_return;
  struct addrinfo hints, *result, *temp;
  size_t len_input;
  int socket_file_descriptor, socket_file_descriptor_2, linked_lists;
  int listening_value;
  struct sockaddr_storage client_content;
  socklen_t client_address_size;

  // specifacation for the type of socket you want made
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  linked_lists = getaddrinfo(NULL, argv[1], &hints, &result);

  if (linked_lists != 0) {
    fprintf(stderr, "Something went wrong while using getaddrinfo : %s",
            gai_strerror(linked_lists));
  }

  for (temp = result; temp != NULL; temp = temp->ai_next) {
    socket_file_descriptor =
        socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);

    if (socket_file_descriptor == -1) {
      fprintf(stderr, "socket not created properly\n");
      continue;
    }

    binding_return =
        bind(socket_file_descriptor, temp->ai_addr, temp->ai_addrlen);

    if (binding_return == 0) {
      continue;
    } else {
      break;
    }
    close(socket_file_descriptor);
  }

  freeaddrinfo(result);

  if (temp == NULL) {
    fprintf(stderr,
            "Something went wrong cause it is returning an empty list!\n");
    // exit(EXIT_FAILURE);
  }

  listening_value = listen(socket_file_descriptor, BUFF_LEN);

  if (listening_value != 0) {
    fprintf(
        stderr,
        "Something went wrong while listening to the client hence exiting\n");
    exit(EXIT_FAILURE);
  }

  client_address_size = sizeof client_content;
  socket_file_descriptor_2 =
      accept(socket_file_descriptor, (struct sockaddr *)&client_content,
             &client_address_size);

  if (socket_file_descriptor_2 != 0) {
    fprintf(stderr,
            "Something went wrong while accepting the client on the socket \n");
  } else {
    printf("TCP server socket established\n");
  }
}
