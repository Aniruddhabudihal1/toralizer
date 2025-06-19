#include "header.h"

void serve(const char *port);

int main(int argc, char *argv[]) {
  serve(argv[1]);
  return 0;
}

void serve(const char *port) {
  int yes = 1;
  char cli_add[BUFF_LEN];
  int binding_return;
  struct addrinfo hints, *result, *temp;

  int socket_file_descriptor, socket_file_descriptor_2, linked_lists;
  int listening_value;
  ssize_t sending_value;
  struct sockaddr_in client_address;
  ssize_t length_of_content_to_be_sent = 80;
  socklen_t client_address_size;
  client_address_size = sizeof client_address.sin_addr;

  char client_input[80];
  size_t client_input_length = 80;
  char client_output[40];
  ssize_t nread;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  linked_lists = getaddrinfo(NULL, port, &hints, &result);

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

    if (setsockopt(socket_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &yes,
                   sizeof(int)) == -1) {
      fprintf(stderr, "Something went wrong while performing setsockopt\n");
    }

    binding_return =
        bind(socket_file_descriptor, temp->ai_addr, temp->ai_addrlen);

    if (binding_return == 0) {
      break;
    } else {
      continue;
    }
    close(socket_file_descriptor);
  }

  freeaddrinfo(result);

  if (temp == NULL) {
    fprintf(stderr,
            "Something went wrong cause it is returning an empty list!\n");
    exit(EXIT_FAILURE);
  }

  listening_value = listen(socket_file_descriptor, BUFF_LEN);

  if (listening_value != 0) {
    fprintf(
        stderr,
        "Something went wrong while listening to the client hence exiting\n");
    exit(1);
  }
  printf("now listening\n");

  while (1) { // main accept() loop
    socket_file_descriptor_2 = accept(
        socket_file_descriptor, (struct sockaddr *)&client_address.sin_addr,
        &client_address_size);

    if (socket_file_descriptor_2 == -1) {
      fprintf(stderr,
              "An errror occured while making the second file descriptor \n");
      exit(EXIT_FAILURE);
    }

    printf("Establishing connection with the client\n");

    nread =
        recvfrom(socket_file_descriptor_2, client_input, client_input_length, 0,
                 (struct sockaddr *)&client_address, (&client_address_size));

    client_input[nread] = '\0';
    client_input[strcspn(client_input, "\r\n")] = '\0';

    printf("Received the clients input \n");

    if (strcmp(client_input, "Max Verstappen") == 0) {
      strcpy(client_output, "Red Bull Racing");
      sendto(socket_file_descriptor_2, client_output, sizeof(client_output),
             MSG_EOR, (struct sockaddr *)&client_address, client_address_size);
    } else if (strcmp(client_input, "Sebastien Vettel") == 0) {
      strcpy(client_output, "Aston Martin");
      sendto(socket_file_descriptor_2, client_output, sizeof(client_output),
             MSG_EOR, (struct sockaddr *)&client_address, client_address_size);
    } else if (strcmp(client_input, "Fernando Alonso") == 0) {
      strcpy(client_output, "Renault");
      sendto(socket_file_descriptor_2, client_output, sizeof client_output,
             MSG_EOR, (struct sockaddr *)&client_address, client_address_size);
    } else {
      strcpy(client_output, "Driver not recognized\n");
      sendto(socket_file_descriptor_2, client_output, sizeof client_output,
             MSG_EOR, (struct sockaddr *)&client_address, client_address_size);
    }
    close(socket_file_descriptor_2);
    /*
        // so upto this point we have gotten the client address and now we need
       to
        // cater to it this is done by the server by using fork to create new
        // children that handle these requests

        if (!fork()) {
          close(socket_file_descriptor);
          sending_value =
              send(socket_file_descriptor_2, "Namaskara from the server\n",
                   length_of_content_to_be_sent, 0);
          inet_ntop(AF_INET, (struct sockaddr *)&(client_address).sin_addr,
       cli_add, BUFF_LEN); printf("The connection is coming from address : %s
       \n", cli_add); if (sending_value == -1) { fprintf(stderr, "An error
       occured while sending data to the client\n");
            close(socket_file_descriptor_2);
            exit(EXIT_FAILURE);
          }
          close(socket_file_descriptor_2);
        }
    */
  }
  close(socket_file_descriptor_2);
}
