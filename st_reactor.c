#include "st_reactor.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/select.h>

typedef struct FdNode
{
    int fd;
    handler_t handler;
    struct FdNode *next;
} FdNode;

typedef struct Reactor
{
    pthread_t thread;
    FdNode *fds;
    int stop;
} Reactor;

static void *reactorThread(void *arg)
{
    Reactor *reactor = (Reactor *)arg;

    while (!reactor->stop)
    {
        fd_set read_fds;
        int max_fd = 0;
        FD_ZERO(&read_fds);

        FdNode *current = reactor->fds;
        while (current != NULL)
        {
            FD_SET(current->fd, &read_fds);
            if (current->fd > max_fd)
                max_fd = current->fd;
            current = current->next;
        }

        int ready = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (ready > 0)
        {
            current = reactor->fds;
            while (current != NULL)
            {
                if (FD_ISSET(current->fd, &read_fds))
                    current->handler(current->fd, reactor);
                current = current->next;
            }
        }
    }

    return NULL;
}

void removeFd(int fd, Reactor *reactor)
{
    FdNode *current = reactor->fds;
    while (current->next != NULL)
    {
        if (current->next->fd == fd)
        {
            current->next = current->next->next;
            break;
        }
        current = current->next;
    }
    printf("SUCCESS TO REMOVE %d\n", fd);

}

Reactor *createReactor()
{
    Reactor *reactor = (Reactor *)malloc(sizeof(Reactor));
    if (reactor == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for reactor\n");
        return NULL;
    }

    reactor->fds = NULL;
    reactor->stop = 0;

    return reactor;
}

void stopReactor(Reactor *reactor)
{
    reactor->stop = 1;
}

void startReactor(Reactor *reactor)
{
    pthread_create(&reactor->thread, NULL, reactorThread, reactor);
}

void addFd(Reactor *reactor, int fd, handler_t handler)
{
    FdNode *current = reactor->fds;
    FdNode *newFd = (FdNode *)malloc(sizeof(FdNode));
    if (newFd == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for FdNode\n");
        return;
    }

    newFd->next = NULL;
    if (current == NULL)
    {
        current = newFd;
        reactor->fds = newFd;
    }
    else
    {
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = newFd;
    }

    newFd->fd = fd;
    newFd->handler = handler;
}

void waitFor(Reactor *reactor)
{
    pthread_join(reactor->thread, NULL);
}
