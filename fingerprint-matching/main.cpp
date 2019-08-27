#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sgfplib.h"

#include <mysql/mysql.h>

static char *host = "docker01.tharun.me";
static char *user = "docker";
static char *pass = "docker";
static char *dbname = "exam-marker";


int main(int argc, char* argv[]) {
    char* studentRegistrationNumber = argv[1];
    // printf("%s",studentRegistrationNumber);
}