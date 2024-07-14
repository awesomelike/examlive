#ifndef LIST
#define LIST

#include <stdlib.h>
#include <string.h>

typedef struct StudentNode
{
    int fd;
    char id[7];
    char ip[32];
    char name[128];
    struct StudentNode *next;
    struct StudentNode *prev;
    int table_position;
    int score;
} StudentList;

StudentList *newNode(int sock_fd, char *ip)
{
    StudentList *newStudent = (StudentList *)malloc(sizeof(StudentList));
    newStudent->fd = sock_fd;
    strcpy(newStudent->id, "undefined");
    strcpy(newStudent->ip, ip);
    strcpy(newStudent->name, "undefined");
    newStudent->score = 0;
    newStudent->next = NULL;
    newStudent->prev = NULL;
    return newStudent;
}

#endif LIST