#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUF_SIZE            10000
#define FILE_NAME_SIZE      BUF_SIZE
#define FILE_LENGTH_SIZE    BUF_SIZE

void error_handling(char *message);

int  main(int argc, char *argv[]) {
    int  serv_sock, clnt_sock;
    char file_name[FILE_NAME_SIZE];
    char file_length[FILE_LENGTH_SIZE];
    char message[BUF_SIZE];

    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;

    FILE *fp;
    long  nsize = 0;
    long  ssize = 0;

    long fsize = 0;
    long rsize = 0;

    long total_size = 0;

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    if (serv_sock == -1) error_handling("socket() error");
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family      = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port        = htons(atoi(argv[1])); // port

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1) error_handling("bind() error");

    if (listen(serv_sock, 5) == -1) error_handling("listen() error");

    while (1) {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock   = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);

        if (clnt_sock == -1) error_handling("accept() error");

        // title
        nsize      = FILE_NAME_SIZE;
        rsize      = 0;
        total_size = 0;

        do {
            total_size += rsize;
            nsize      -= rsize;
            rsize       = recv(clnt_sock, file_name + total_size, nsize, 0);
        } while (nsize != rsize);
        fp = fopen(file_name, "wb");

        printf("Send Image | name : %s\n", file_name);

        while (rsize = recv(clnt_sock, message, BUF_SIZE, 0)) {
            fwrite(message, sizeof(char), rsize, fp);
        }

        printf("complete!\n");

        fclose(fp);

        close(clnt_sock);
    }

    close(serv_sock);
    return 0;
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void clear_buffer(char *buffer) {
    int i = 0;

    for (i = 0; i < BUF_SIZE; i++) buffer[i] = 0;
}
