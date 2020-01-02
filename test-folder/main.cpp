#include<stdio.h>
#include<string.h>
#include<mysql/mysql.h>

static char *host = "docker01.tharun.me";
static char *user = "docker";
static char *pass = "docker";
static char *dbname = "exam-marker";

unsigned int port = 3306;

static char *unix_socket = NULL;
unsigned int flag = 0;

int main(int argc, char **argv) {
   MYSQL *conn;
   conn = mysql_init(); 
   if (!mysql_real_connect(conn, host, user, pass, dbname, port, unix_socket, flag)) {
       fprintf(stderr, "Error: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
       exit(1);
   }
   printf("Success!!!");
   mysql_query(conn,"insert into  testtable values('"+nId+"','"+szName+"','"+szPassword+"','"+szQuestion+"','"+szPhone+"')");
   return EXIT_SUCCESS;
}
//gcc $(mysql_config --cflags) main.cpp $(mysql_config --libs)