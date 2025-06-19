#include "header.h"
#include <stddef.h>
#include <sys/socket.h>

struct something {
  int socket_file_descriptor;
  struct addrinfo *temp;
};

struct sock_return {
  int version_number;
  int command_code;
  int destiantion_port;
  char destination_ip;
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
  if (argc < 3) {
    fprintf(stderr,
            "Require a minimum of 2 inputs in the command line of the format : "
            "<executable> <address> <port number>\n");
    exit(EXIT_FAILURE);
  }

  int version_number, command_code;
  struct sockaddr destination_ip;
  char *destination_port;

  printf("Enter the version number you would like to use for the socks "
         "connection\nyour options are (only 4 as of now more options will be "
         "provided later) : ");
  scanf("%d", &version_number);

  strcpy(destination_ip.sa_data, argv[1]);
  destination_ip.sa_family = AF_INET;

  strcpy(destination_port, argv[2]);
}

void client_call(int version_number, int command_code, int destination_port,
                 struct sockaddr destination_stuff) {
  int linked_list_values, socket_file_descriptor, no_of_bytes_returned,
      no_of_bytes_sent;
  socklen_t len = sizeof destination_stuff.sa_data;

  char buffer[30];
  struct addrinfo main;
  struct addrinfo *temp, *result;

  memset(&main, 0, sizeof(main));
  main.ai_socktype = AF_INET;
  main.ai_family = AF_INET;
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

  // The socks stuff gets sent from here

  struct sock_return sendtoserver;
  sendtoserver.command_code = command_code;
  sendtoserver.destiantion_port = destination_port;
  strcpy(&sendtoserver.destination_ip, destination_stuff.sa_data);
  sendtoserver.version_number = version_number;

  size_t lenn = sizeof sendtoserver;

  sendto(socket_file_descriptor, (const char)(struct sendtoserver), lenn, 0,
         (struct sockaddr *)&destination_stuff.sa_data, len);

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
