#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

//static Q_type queue = {NULL, NULL, 0};
Q_type *Queue(E_type *head, E_type * tail, unsigned int size)
{

	Q_type *queue = malloc(sizeof(Q_type));
	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;
	return queue;
}

void enqueue(void *data, Q_type *queue)
{
    E_type  *elem;
    
    if ((elem = (E_type *)malloc(sizeof(E_type))) == NULL)
    {
        printf("Unable to allocate space!\n");
        exit(1);
    }
    elem->data = data;
    elem->next = NULL;
    
    if (queue->head == NULL)
        queue->head = elem;
    else
        queue->tail->next = elem;
    queue->tail = elem;

    queue->size++;
}

void *dequeue(Q_type *queue)
{
    E_type  *elem;
    void    *data = NULL;
    
    if (queue->size != 0)
    {
        elem = queue->head;
        if (queue->size == 1)
            queue->tail = NULL;
        queue->head = queue->head->next;
        
        queue->size--;
        data = elem->data;
        free(elem);
    }
        
    return data;
}

unsigned int getQsize(Q_type *queue)
{
    return queue->size;
}
