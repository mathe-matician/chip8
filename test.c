#include <stdio.h>
#include "SDLRect.h"
#include <stdlib.h>

void scancode_test() {
    const unsigned int keys[16] = {
        SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2,
        SDL_SCANCODE_3, SDL_SCANCODE_4, SDL_SCANCODE_5,
        SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8,
        SDL_SCANCODE_9, SDL_SCANCODE_A, SDL_SCANCODE_B,
        SDL_SCANCODE_C, SDL_SCANCODE_D, SDL_SCANCODE_E,
        SDL_SCANCODE_F
    };

    const unsigned int* test = keys;

    for (int i = 0; i < 16; i++) {
        printf("%d\n", *test);
        test++;
    }
}

union unionJob
{
   //defining a union
   float salary;
   int workerNo;
   char name[32];
} uJob;

struct structJob
{
   char name[32];
   float salary;
   int workerNo;
} sJob;

struct node {
    int val;
    struct node* next;
};

void push(struct node** a_stackhead, uint16_t a_address) {
    struct node* node = (struct node*)malloc(sizeof(struct node));
    node->val = a_address;
    node->next = *a_stackhead;
    *a_stackhead = node;
}

void node_print(struct node** head) {
    struct node* temp = *head;
    while (temp) {
        printf("Node = %d\n", temp->val);
        temp = temp->next;
    }
}

void tester(struct node* test) {
    int a = test->val;
    printf("a = %d\n", test->val);
    test->val = 653;
}

int main()
{
   //printf("size of union = %d bytes", sizeof(uJob));
   //printf("\nsize of structure = %d bytes", sizeof(sJob));
   struct node* head = (struct node*)malloc(sizeof(struct node));
   head->val = 999;
   //push(&head, 325);
   //node_print(&head);
   tester(head);
   printf("a = %d\n", head->val);
   
   free(head);
   return 0;
}