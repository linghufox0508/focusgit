#include"stdio.h"
#include"malloc.h"
#include"stdlib.h"
typedef struct Node {
     int data;
     struct Node *pNext;
} NODE,*PNODE;

typedef struct Stack {
    PNODE pTop;
    PNODE pBottom;      
} STACK,*PSTACK;


void initStack(PSTACK S)
{
    S->pBottom = (PNODE)malloc(sizeof(NODE));
    S->pTop = S->pBottom;
    S->pTop->pNext = NULL;
}

void push (PSTACK S,int val)
{
    PNODE p;
    p = S->pTop;
    S->pTop = (PNODE)malloc(sizeof(NODE));
    S->pTop->data = val;
    S->pTop->pNext = p;
}


void travseStack(PSTACK S)
{
    PNODE p;
    
    p = S -> pTop;
    while (p != S->pBottom) {
         
      printf("%d ",p->data);
      p = p->pNext;         
    }
    printf("\n");
}

int pop (PSTACK S)
{
    PNODE p;
    
    if (S->pTop == S->pBottom) {
       printf("stack is empty");
       exit(-1);
    } else {
       p = S->pTop;
       S->pTop = p->pNext;
    }
       
    return p->data;

}
int main(void) {
   STACK S;
   int val;
   int i = 0;
   initStack(&S);
   
   for(;i < 10;i++) {
      push(&S,i);    
   }
   
   travseStack(&S);
   val = pop(&S);
   printf("val = %d\n",val);
   
   val = pop(&S);
   printf("val = %d\n",val);
   
   return 0;
}
