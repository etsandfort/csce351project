/* 
    Name:   queue.h
    Author: Dongyuan Zhan
    Date:   11/20/2010

Description:
    Functions used to implment a queue and its operations
*/

typedef struct element_type
{
    void                *data;
    struct element_type *next;
} E_type;

typedef struct queue_type
{
    E_type              *head;
    E_type              *tail;
    unsigned int        size;
} Q_type;


Q_type *Queue(E_type *head, E_type * tail, unsigned int size);

void enqueue(void *data, Q_type *queue);

void *dequeue(Q_type *queue);

unsigned int getQsize(Q_type *queue);
