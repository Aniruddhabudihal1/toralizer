#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFF_SIZE 200

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr,
            "Require a minimum of 3 inputs in the command line of the format : "
            "<executable> <address> <port number> <messages>\n");
    exit(EXIT_FAILURE);
  }

  int linked_list_values, socket_file_descriptor, connection_status,
      no_of_bytes_returned, no_of_bytes_sent;
  ssize_t len;
  char *buf;
  char buffer[30];
  struct addrinfo main;
  struct addrinfo *temp, *result;

  memset(&main, 0, sizeof(main));
  main.ai_socktype = AF_UNSPEC;
  main.ai_family = AF_UNSPEC;
  main.ai_protocol = 0;
  main.ai_flags = 0;

  if ((linked_list_values = getaddrinfo(argv[1], argv[2], &main, &result)) !=
      0) {
    fprintf(stderr, "Error for getaddrinfo : %s\n",
            gai_strerror(linked_list_values));
  }

  for (temp = result; temp != NULL; temp = temp->ai_next) {
    socket_file_descriptor =
        socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);

    if (socket_file_descriptor == -1) {
      fprintf(stderr,
              "Something went wrong while assigning the socket as it \n");
      break;
    }

    if ((connection_status = connect(socket_file_descriptor, temp->ai_addr,
                                     temp->ai_addrlen)) != 0) {
      fprintf(stderr, "Something went wrong in connecting to the server! \n");
      close(socket_file_descriptor);
    } else {
      break;
    }
  }

  if (temp == NULL) {
    fprintf(stderr, "The information being returned is empty !\n");
  }
  freeaddrinfo(temp);

  for (int i = 3; i < argc; i++) {
    len = strlen(argv[i]) + 1;
    if (len > BUFF_SIZE) {
      printf("Length of the message exceeds the buffer length , so discarding "
             "this message :\n%s\n",
             argv[i]);
      continue;
    }
    buf = argv[i];
    if ((no_of_bytes_sent = send(socket_file_descriptor, buf, len, 0) == 0)) {
      fprintf(stderr,
              "Something went wrong while sending the data to the server\n");
    }

    if ((no_of_bytes_returned =
             recv(socket_file_descriptor, buffer, sizeof(buffer), 0) == 0)) {
      fprintf(
          stderr,
          "Something went wrong while recieving the data from the server\n");
    }
    printf("Message sent by the server : %s\n", buffer);
  }
  close(socket_file_descriptor);
  return 0;
}
