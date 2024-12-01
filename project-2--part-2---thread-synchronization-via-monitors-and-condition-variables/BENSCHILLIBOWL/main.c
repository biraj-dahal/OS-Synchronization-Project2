// main.c
#include "BENSCHILLIBOWL.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Global variable for the restaurant
BENSCHILLIBOWL* bcb;

void* BENSCHILLIBOWLCustomer(void* tid) {
    int customer_id = *((int*) tid);
    
    // Allocate space for order
    Order* order = (Order*) malloc(sizeof(Order));
    
    // Select random menu item and populate order
    order->menu_item = PickRandomMenuItem();
    order->customer_id = customer_id;
    order->next = NULL;
    
    // Add order to restaurant
    int order_number = AddOrder(bcb, order);
    printf("Customer #%d added order #%d\n", customer_id, order_number);
    
    return NULL;
}

void* BENSCHILLIBOWLCook(void* tid) {
    int cook_id = *((int*) tid);
    
    while (1) {
        Order* order = GetOrder(bcb);
        if (order == NULL) break;
        
        // Process order
        printf("Cook #%d fulfilled order #%d for customer #%d\n", 
               cook_id, order->order_number, order->customer_id);
        free(order);
    }
    
    return NULL;
}

int main() {
    // Set random seed
    srand(time(NULL));
    
    // Restaurant parameters
    int num_customers = 50;
    int num_cooks = 5;
    int max_size = 10;
    
    // Open restaurant
    bcb = OpenRestaurant(max_size, num_customers);
    
    // Create thread IDs
    pthread_t customers[num_customers];
    pthread_t cooks[num_cooks];
    int customer_ids[num_customers];
    int cook_ids[num_cooks];
    
    // Create customer threads
    for (int i = 0; i < num_customers; i++) {
        customer_ids[i] = i + 1;
        pthread_create(&customers[i], NULL, BENSCHILLIBOWLCustomer, &customer_ids[i]);
    }
    
    // Create cook threads
    for (int i = 0; i < num_cooks; i++) {
        cook_ids[i] = i + 1;
        pthread_create(&cooks[i], NULL, BENSCHILLIBOWLCook, &cook_ids[i]);
    }
    
    // Wait for all customers to finish
    for (int i = 0; i < num_customers; i++) {
        pthread_join(customers[i], NULL);
    }
    
    // Wait for all cooks to finish
    for (int i = 0; i < num_cooks; i++) {
        pthread_join(cooks[i], NULL);
    }
    
    // Close restaurant
    CloseRestaurant(bcb);
    printf("Restaurant closed.\n");
    
    return 0;
}