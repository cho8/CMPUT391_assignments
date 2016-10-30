#include<stdio.h>
#include<stdlib.h>
#include "ll_nodes.h"

/* Doubly Linked list implementation referenced from:
  http://www.geeksforgeeks.org/merge-sort-for-doubly-linked-list/
  October 29, 2016
  */


struct node *split(struct node *head);

// Function to merge two linked lists
struct node *merge(struct node *first, struct node *second)
{
    // If first linked list is empty
    if (!first)
        return second;

    // If second linked list is empty
    if (!second)
        return first;

    // Pick the smaller value
    if (first->mind < second->minm)
    {
        first->next = merge(first->next,second);
        first->next->prev = first;
        first->prev = NULL;
        return first;
    }
    else
    {
        second->next = merge(first,second->next);
        second->next->prev = second;
        second->prev = NULL;
        return second;
    }
}

// Function to do merge sort
struct node *mergeSort(struct node *head)
{
    if (!head || !head->next)
        return head;
    struct node *second = split(head);

    // Recur for left and right halves
    head = mergeSort(head);
    second = mergeSort(second);

    // Merge the two sorted halves
    return merge(head,second);
}

// A utility function to insert a new node at the
// beginning of doubly linked list
void insert(struct node **head, int id, float x1, float x2, float y1, float y2, float mind, float minm)
{
    struct node *temp =
        (struct node *)malloc(sizeof(struct node));
    temp->id = id;
    temp->x1= x1;
    temp->x2 = x2;
    temp->y1 = y1;
    temp->y2 = y2;
    temp->mind = mind;
    temp->minm = minm;
    temp->next = temp->prev = NULL;
    if (!(*head))
        (*head) = temp;
    else
    {
        temp->next = *head;
        (*head)->prev = temp;
        (*head) = temp;
    }
}

void delete(struct node **head, struct node *curr) {

  curr->prev->next = curr->next;
  curr->next->prev = curr->prev;

  curr->next = NULL;
  curr->prev = NULL;


}

int listlen(struct node *head) {
  int count=0;
  struct node *temp = head;
  while (head)
  {
      count++;
      temp = head;
      head = head->next;
  }
  return count;
}

// A utility function to print a doubly linked list in
// both forward and backward directions
void printList(struct node *head)
{
    struct node *temp = head;
    printf("Forward Traversal using next poitner\n");
    while (head)
    {
        printf("%d ",head->id);
        temp = head;
        head = head->next;
    }
}

// Utility function to swap two integers
void swap(int *A, int *B)
{
    int temp = *A;
    *A = *B;
    *B = temp;
}

// Split a doubly linked list (DLL) into 2 DLLs of
// half sizes
struct node *split(struct node *head)
{
    struct node *fast = head,*slow = head;
    while (fast->next && fast->next->next)
    {
        fast = fast->next->next;
        slow = slow->next;
    }
    struct node *temp = slow->next;
    slow->next = NULL;
    return temp;
}
