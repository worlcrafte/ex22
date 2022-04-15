#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX 256
// structure for message queue
struct mesg_buffer
{
    long mesg_type;
    char mesg_text[256];
} message;

void send(int msgid)
{
    while (1)
    {

        if (fgets(message.mesg_text, MAX, stdin) == NULL)
        {
            perror("fgets");
            exit(EXIT_FAILURE);
        }
        if (message.mesg_text[0] == '\n')
        {
            break;
        }
        else
        {
            // msgsnd to send message
            if (msgsnd(msgid, &message, sizeof(message), 0) == -1)
            {
                perror("mgsnd");
                exit(EXIT_FAILURE);
            }
        }
    }
}
void receive(int msgid)
{
    // msgrcv to receive message
    if (msgrcv(msgid, &message, sizeof(message), 0, IPC_NOWAIT) == -1)
    {
        perror("mgsnd");
        exit(EXIT_FAILURE);
    }

    // display the message
    printf("%s", message.mesg_text);
}

void statistique(int msgid)
{

    struct msqid_ds d;
    if (msgctl(msgid, IPC_STAT, &d) == -1)
    {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }
    printf("file: %lu \ntaille: %lu\n", d.msg_qnum, d.__msg_cbytes);
}

void rmve(int msgid)
{
    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }
}
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("usage: %s CMD\n ", argv[0]);
        exit(EXIT_FAILURE);
    }
    key_t key;
    int msgid;

    // ftok to generate unique key
    if ((key = ftok("indexe.c", 65)) == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    // msgget creates a message queue
    // and returns identifier
    if ((msgid = msgget(key, 0666 | IPC_CREAT)) == -1)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    message.mesg_type = 1;
    if (strncmp(argv[1], "snd", 3) == 0)
    {
        send(msgid);
    }
    else if (strncmp(argv[1], "rcv", 3) == 0)
    {
        receive(msgid);
    }
    else if (strncmp(argv[1], "stat", 4) == 0)
    {
        statistique(msgid);
    }
    else if (strncmp(argv[1], "rm", 2) == 0)
    {
        rmve(msgid);
    }
    else
    {
        perror("invalide argument");
        exit(EXIT_FAILURE);
    }

    return 0;
}