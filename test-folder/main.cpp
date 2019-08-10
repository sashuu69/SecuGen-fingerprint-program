#include<stdio.h>
#include<string.h>

int main(int argc, char **argv) {
    char location[50];
    strcat(location, "/tmp/");
    if(argv[1]) {
        strcat(location, argv[1]);
        printf("%s",location);
    }
    else {
        return 0;
    }
}