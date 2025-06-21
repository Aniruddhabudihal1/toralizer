#include "header.h"
#include <stdio.h>
#include <string.h>

struct something {
  int socket_file_descriptor;
  struct addrinfo *temp;
};

void client_call(int version_number, int command_code, int destination_port,
                 struct sockaddr stuff);
struct something socket_maker(struct addrinfo *x, struct addrinfo *y);

/*
 *  Format for the socks message to be sent to the proxy server
 *
 *                +----+----+----+----+----+----+----+----+----+----+....+----+
 *                | VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
 *                +----+----+----+----+----+----+----+----+----+----+....+----+
 *   # of bytes:     1    1      2              4           variable        1
 *
 *
 *
 * */

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr,
            "Require a minimum of 2 inputs in the command line of the format : "
            "<executable> <address>\n");
    exit(EXIT_FAILURE);
  }

  int version_number, command_code;
  struct sockaddr destination_ip;
  int destination_port;

  printf("Enter the version number you would like to use for the socks "
         "connection\nyour options are (only 4 as of now more options will be "
         "provided later) : ");
  scanf("%d", &version_number);

  strcpy(destination_ip.sa_data, argv[1]);
  printf("works here\n");

  printf("Enter the destination port number :\n");
  scanf("%d", &destination_port);

  client_call(version_number, command_code, destination_port, destination_ip);

  return 0;
}

void client_call(int version_number, int command_code, int destination_port,
                 struct sockaddr destination_stuff) {
  int linked_list_values, socket_file_descriptor;
  int fifo_descriptor;
  struct sockaddr dest;
  strcpy(dest.sa_data, proxy_address);
  socklen_t len = sizeof dest.sa_data;

  char *buf[20];

  strcpy(*buf, destination_stuff.sa_data);

  int sending_status;

  struct addrinfo main;
  struct addrinfo *temp, *result;

  memset(&main, 0, sizeof(main));
  main.ai_socktype = AF_UNSPEC;
  main.ai_family = AF_UNSPEC;
  main.ai_protocol = 0;
  main.ai_flags = 0;

  if ((linked_list_values = getaddrinfo((const char *)proxy_address, port_tcp,
                                        &main, &result)) != 0) {
    fprintf(stderr, "Error for getaddrinfo : %s\n",
            gai_strerror(linked_list_values));
  }

  struct something x = socket_maker(temp, result);

  socket_file_descriptor = x.socket_file_descriptor;
  temp = x.temp;

  if (temp == NULL) {
    fprintf(stderr, "The information being returned is empty !\n");
  }
  freeaddrinfo(temp);
  /*

  This part of the code tried to send the data directly through the socket
  instead of the FIFO file

    sending_status =
        sendto(socket_file_descriptor, (const void *)&command_code,
               sizeof(command_code), 0, temp->ai_addr, temp->ai_addrlen);

    if (sending_status == -1) {
      fprintf(stderr, "Something went wrong while sending the command"
                      "code\nplease try again\n");
      exit(EXIT_FAILURE);
    } else {
      printf("command code sent successfully to the server\n");
    }

    sending_status =
        sendto(socket_file_descriptor, (const void *)&version_number,
               sizeof(version_number), 0, temp->ai_addr, temp->ai_addrlen);

    if (sending_status == -1) {
      fprintf(stderr, "Something went wrong while sending the version "
                      "number\nplease try again\n");
      exit(EXIT_FAILURE);
    } else {
      printf("version number sent successfully to the server\n");
    }

    sending_status =
        sendto(socket_file_descriptor, (const void *)&destination_port,
               sizeof(destination_port), 0, temp->ai_addr, temp->ai_addrlen);

    if (sending_status == -1) {
      fprintf(stderr, "Something went wrong while sending the destination "
                      "port\nplease try again\n");
      exit(EXIT_FAILURE);
    } else {
      printf("destination port sent successfully to the server\n");
    }

    sending_status = sendto(
        socket_file_descriptor, (const void *)&destination_stuff.sa_data,
        sizeof(destination_stuff.sa_data), 0, temp->ai_addr, temp->ai_addrlen);

    if (sending_status == -1) {
      fprintf(stderr, "Something went wrong while sending the destination "
                      "server\nplease try again\n");
      exit(EXIT_FAILURE);
    } else {
      printf("destination server sent successfully to the server\n");
    }
  */

  // making the fifo file to send the client inputs into it so that server can
  // read it

  // pathname for the first FIFO1 which sends the content to the server
  const char *pathname = "FIFO1";
  mode_t mode_for_fifo = 0666;

  if (mkfifo(pathname, mode_for_fifo) == -1) {
    fprintf(stderr,
            "Something went wrong while making the FIFO file or the file has "
            "alredy been created\nrun ls in the terminal and check\n");

  } else {
    printf("FIFO file has been created\n");
  }

  fifo_descriptor = open(pathname, O_RDWR);

  char buffer[89];

  sprintf(buffer, "%d %d %d %s", version_number, command_code, destination_port,
          *buf);
  ssize_t size_of_buffer = sizeof buffer;
  if (write(fifo_descriptor, buffer, size_of_buffer) == -1) {
    fprintf(stderr, "something went wrong while writing to the FIFO1 fil\n");
  } else {
    printf("Written successfully to FIFO1\n");
  }
  close(socket_file_descriptor);
}

struct something socket_maker(struct addrinfo *temp, struct addrinfo *result) {
  int socket_file_descriptor;
  int connection_status;
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
  struct something s;
  s.socket_file_descriptor = socket_file_descriptor;
  s.temp = temp;
  return s;
}
