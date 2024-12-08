// main.c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include "BENSCHILLIBOWL.h"


#define BENSCHILLIBOWL_SIZE 100
#define NUM_CUSTOMERS 50
#define NUM_COOKS 10
#define ORDERS_PER_CUSTOMER 5
// #define NUM_CUSTOMERS 5
// #define NUM_COOKS 2
// #define ORDERS_PER_CUSTOMER 2
#define EXPECTED_NUM_ORDERS (NUM_CUSTOMERS * ORDERS_PER_CUSTOMER)


BENSCHILLIBOWL *bcb;

void* BENSCHILLIBOWLCustomer(void* tid) {
    int customer_id = (int)(long)tid;

    for (int i = 0; i < ORDERS_PER_CUSTOMER; i++) {
        Order *order = (Order *)malloc(sizeof(Order));
        order->menu_item = PickRandomMenuItem();
        order->customer_id = customer_id;
        order->next = NULL;

        int order_number = AddOrder(bcb, order);
        printf("Customer #%d placed order #%d: %s\n", customer_id, order_number, order->menu_item);
    }

    return NULL;
}

void* BENSCHILLIBOWLCook(void* tid) {
    int cook_id = (int)(long)tid;
    int orders_fulfilled = 0;

    while (1) {
        Order *order = GetOrder(bcb);
        if (order == NULL) break;  

        printf("Cook #%d fulfilled order #%d: %s for customer #%d\n",
               cook_id, order->order_number, order->menu_item, order->customer_id);
        free(order);
        orders_fulfilled++;
    }

    printf("Cook #%d fulfilled %d orders\n", cook_id, orders_fulfilled);
    return NULL;
}

int main() {
    pthread_t customers[NUM_CUSTOMERS];
    pthread_t cooks[NUM_COOKS];

    bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS);

for (int i = 0; i < NUM_CUSTOMERS; i++) {
    if (pthread_create(&customers[i], NULL, BENSCHILLIBOWLCustomer, (void *)(long)i) != 0) {
        perror("Failed to create customer thread");
        exit(EXIT_FAILURE);
    }
}

for (int i = 0; i < NUM_COOKS; i++) {
    if (pthread_create(&cooks[i], NULL, BENSCHILLIBOWLCook, (void *)(long)i) != 0) {
        perror("Failed to create cook thread");
        exit(EXIT_FAILURE);
    }
}


    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_join(customers[i], NULL);
    }

    for (int i = 0; i < NUM_COOKS; i++) {
        pthread_join(cooks[i], NULL);
    }

    CloseRestaurant(bcb);

    return 0;
}
