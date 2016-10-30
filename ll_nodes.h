#ifndef LL_NODES_H
#define LL_NODES_H

/* Link list node */
struct node
{
    int id;
    float x1, x2, y1, y2, mind, minm;
    struct node* next;
    struct node* prev;
};

/* function prototypes */
struct node *split(struct node *head);
struct node *merge(struct node *first, struct node *second);
struct node *mergeSort(struct node *head);
int listlen(struct node *head);
void insert(struct node **head, int id, float x1, float x2, float y1, float y2, float mind, float minm);
void delete(struct node **head, struct node *curr);
void printList(struct node *head);

#endif
