#ifndef REACTOR_H
#define REACTOR_H

typedef struct Reactor Reactor;
typedef void (*handler_t)(int, Reactor *);
typedef struct FdNode FdNode;

void removeFd(int fd, Reactor *reactor);
Reactor *createReactor();
void stopReactor(Reactor *reactor);
void startReactor(Reactor *reactor);
void addFd(Reactor *reactor, int fd, handler_t handler);
void waitFor(Reactor *reactor);

#endif /* REACTOR_H */
