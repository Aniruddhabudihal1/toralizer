/*
                   /_  __/___  _________ _/ (_)___  ___  _____
                    / / / __ \/ ___/ __ `/ / /_  / / _ \/ ___/
                   / / / /_/ / /  / /_/ / / / / /_/  __/ /
                  /_/  \____/_/   \__,_/_/_/ /___/\___/_/
*/

#include "header.h"
#include <stdio.h>
#include <stdlib.h>

void serve(const char *port);

int main(int argc, char *argv[]) {
  serve(argv[1]);
  return 0;
}

void serve(const char *port) {
  int yes = 1;
  char cli_add[BUFF_LEN];
  int binding_return;
  struct addrinfo hints, *result, *temp, hints2, *result2, *temp2;

  int socket_file_descriptor, socket_file_descriptor_2, linked_lists;
  int socket_file_descriptor_3, socket_file_descriptor_4, linked_lists_2;
  int listening_value;
  ssize_t sending_value;
  struct sockaddr_in client_address;
  ssize_t length_of_content_to_be_sent = 80;
  socklen_t client_address_size;
  client_address_size = sizeof client_address.sin_addr;
  int connection_status;

  int fifo_descriptor;
  int version_number;
  int command_code;
  int destination_port;
  char URL[30];
  struct sockaddr dest_addr;
  char destination_port_buf[20];

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
  printf("now listening for client\n");

  while (1) { // main accept() loop from the client to get the responses
    socket_file_descriptor_2 = accept(
        socket_file_descriptor, (struct sockaddr *)&client_address.sin_addr,
        &client_address_size);

    if (socket_file_descriptor_2 == -1) {
      fprintf(stderr,
              "An errror occured while making the second file descriptor \n");
      exit(EXIT_FAILURE);
    }

    printf("Established a connection with the client\n\n");

    fifo_descriptor = open("FIFO1", O_RDONLY);

    if (read(fifo_descriptor, &version_number, sizeof(int)) < 0) {
      fprintf(stderr,
              "something went wrong while reading the version number from "
              "the client\n");
    } else {
      printf("Version numebr gotten from the client : %d\n", version_number);
    }

    if (version_number != 4) {
      printf("Version of the socks protocol other than 4 not supported!\n");
      exit(EXIT_FAILURE);
    }

    if (read(fifo_descriptor, &command_code, sizeof(int)) < 0) {
      fprintf(stderr,
              "something went wrong while reading the command code from "
              "the client\n");
    } else {
      printf("command code gotten from the client : %d\n", command_code);
    }

    if (command_code != 1) {
      printf("Only the connect command allowed for now\nfuture versions will "
             "allow other commands\n");
      exit(EXIT_FAILURE);
    }

    if (read(fifo_descriptor, &destination_port, sizeof(int)) < 0) {
      fprintf(stderr,
              "something went wrong while reading the destination port from "
              "the client\n");
    } else {
      printf("destination port gotten from the client :");
      sprintf(destination_port_buf, "%d", destination_port);
      printf(" %s\n", destination_port_buf);
    }

    if (read(fifo_descriptor, &dest_addr.sa_data, sizeof(dest_addr.sa_data)) <
        0) {
      fprintf(stderr, "something went wrong while reading the destination ip "
                      "address from "
                      "the client\n");
    } else {
      printf("destination ip address gotten from the client : %s\n",
             dest_addr.sa_data);
    }

    if (read(fifo_descriptor, URL, sizeof(URL)) < 0) {
      fprintf(stderr,
              "something went wrong while reading the destination URL from "
              "the client\n");
    } else {
      printf("destination URL gotten from the client : %s\n", URL);
    }
    printf("\n\n");
    // conneccting with the actual server to get the data from the site the
    // client requested

    close(fifo_descriptor);
    close(socket_file_descriptor_2);

    // second part where you talk to the main server

    memset(&hints2, 0, sizeof hints2);
    hints2.ai_family = AF_UNSPEC;
    hints2.ai_socktype = AF_UNSPEC;
    hints2.ai_flags = AI_PASSIVE;

    linked_lists_2 =
        getaddrinfo(dest_addr.sa_data, destination_port_buf, &hints2, &result2);

    if (linked_lists_2 != 0) {
      fprintf(stderr, "Something went wrong while using getaddrinfo : %s",
              gai_strerror(linked_lists_2));
      exit(EXIT_FAILURE);
    }

    for (temp2 = result2; temp2 != NULL; temp2 = temp2->ai_next) {
      socket_file_descriptor_3 =
          socket(temp2->ai_family, temp2->ai_socktype, temp2->ai_protocol);
      if (socket_file_descriptor_3 < 0) {
        fprintf(stderr,
                "Something went wrong while creating a socket to connect "
                "to the server\n");
        continue;
      }
      connection_status =
          connect(socket_file_descriptor_3, temp->ai_addr, temp->ai_addrlen);
      if (connection_status < 0) {
        fprintf(
            stderr,
            "Something went wrong while connecting to the external server\n");
        close(socket_file_descriptor_3);
      } else {
        printf("Connected succesfully to the external server\n");
        break;
      }
    }
    char http_request_buffer[100];

    sprintf(http_request_buffer,
            "GET / HTTP/1.1\r\n"
            "Host: %s\r\n"
            "User-Agent: C-HTTP-Client/1.0\r\n"
            "Acccept: */*\r\n"
            "\r\n",
            URL);
    ssize_t main_server_http_request_size = sizeof http_request_buffer;

    ssize_t sending_status;

    sending_status = send(socket_file_descriptor_3, http_request_buffer,
                          main_server_http_request_size, MSG_OOB);

    if (sending_status < 0) {
      fprintf(stderr, "Some local error is there\n");
    } else {
      printf("Get Request sent succesfully to the main server\n");
    }

    char main_server_http_response[900];
    ssize_t nread;
    nread = read(socket_file_descriptor_3, main_server_http_response, 900);

    if (nread == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }

    printf("Received %zd bytes: %s\n", nread, main_server_http_response);
  }
}
