#ifndef NODES_H
#define NODES_H

/* Link list node */
typedef struct
{
    int id;
    float x1, x2, y1, y2, mind, minm;
    struct node* next;
    struct node* prev;
} Node;

typedef struct
{
  double x,y;
} Point;

typedef struct
{
  float dist;
  Node rect;
} Nearest;

#endif
