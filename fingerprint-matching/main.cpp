#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sgfplib.h"

#include <mysql/mysql.h>

static char *host = "docker01.tharun.me";
static char *user = "docker";
static char *pass = "docker";
static char *dbname = "exam-marker";

unsigned int port = 3306;
static char *unix_socket = NULL;
unsigned int flag = 0;


int main(int argc, char* argv[]) {
    MYSQL *conn;
    char* studentRegistrationNumber = argv[1];

    conn = mysql_init(NULL);
    if(!(mysql_real_connect(conn, host, user, pass, dbname, port, unix_socket, flag))) {
        fprintf(stderr, "\nError: %s [%d]\n",mysql_error(conn), mysql_errno(conn));
        exit(1);
    }
    printf("success");
    return EXIT_SUCCESS;
    // printf("%s",studentRegistrationNumber);
}