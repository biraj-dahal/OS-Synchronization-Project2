// BENSCHILLIBOWL.c
#include "BENSCHILLIBOWL.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h>

MenuItem BENSCHILLIBOWLMenu[] = { 
    "BensChilli", 
    "BensHalfSmoke", 
    "BensHotDog", 
    "BensChilliCheeseFries", 
    "BensShake",
    "BensHotCakes",
    "BensCake",
    "BensHamburger",
    "BensVeggieBurger", 
    "BensOnionRings",
};
int BENSCHILLIBOWLMenuLength = 10;

/* Helper function to add order to the rear of the queue */
void AddOrderToBack(Order **orders, Order *order) {
    order->next = NULL;  // Ensure the new order points to NULL
    
    if (*orders == NULL) {
        // If orders is empty, make this the first order
        *orders = order;
    } else {
        // Find the last order in the queue
        Order *curr = *orders;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        // Add the new order at the end
        curr->next = order;
    }
}

/* Select a random item from the Menu and return it */
MenuItem PickRandomMenuItem() {
    int index = rand() % BENSCHILLIBOWLMenuLength;
    return BENSCHILLIBOWLMenu[index];
}

/* Allocate memory for the Restaurant, then create the mutex and condition variables needed */
BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
    BENSCHILLIBOWL* bcb = (BENSCHILLIBOWL*) malloc(sizeof(BENSCHILLIBOWL));
    bcb->orders = NULL;
    bcb->current_size = 0;
    bcb->max_size = max_size;
    bcb->next_order_number = 1;
    bcb->orders_handled = 0;
    bcb->expected_num_orders = expected_num_orders;
    
    // Initialize synchronization objects
    pthread_mutex_init(&bcb->mutex, NULL);
    pthread_cond_init(&bcb->can_add_orders, NULL);
    pthread_cond_init(&bcb->can_get_orders, NULL);
    
    return bcb;
}

/* Check that the number of orders received is equal to the number handled */
void CloseRestaurant(BENSCHILLIBOWL* bcb) {
    pthread_mutex_destroy(&bcb->mutex);
    pthread_cond_destroy(&bcb->can_add_orders);
    pthread_cond_destroy(&bcb->can_get_orders);
    
    // Verify all orders were handled
    assert(bcb->orders_handled == bcb->expected_num_orders);
    free(bcb);
}

/* Helper function to check if restaurant is full */
bool IsFull(BENSCHILLIBOWL* bcb) {
    return bcb->current_size >= bcb->max_size;
}

/* Helper function to check if restaurant is empty */
bool IsEmpty(BENSCHILLIBOWL* bcb) {
    return bcb->current_size == 0;
}

/* Add an order to the back of queue */
int AddOrder(BENSCHILLIBOWL* bcb, Order* order) {
    pthread_mutex_lock(&bcb->mutex);
    
    // Wait while restaurant is full
    while (IsFull(bcb)) {
        pthread_cond_wait(&bcb->can_add_orders, &bcb->mutex);
    }
    
    // Add order to the queue
    order->order_number = bcb->next_order_number++;
    AddOrderToBack(&bcb->orders, order);  // Using the helper function
    
    bcb->current_size++;
    pthread_cond_signal(&bcb->can_get_orders);
    pthread_mutex_unlock(&bcb->mutex);
    
    return order->order_number;
}

/* Remove an order from the queue */
Order* GetOrder(BENSCHILLIBOWL* bcb) {
    pthread_mutex_lock(&bcb->mutex);
    
    // Wait while restaurant is empty
    while (IsEmpty(bcb) && bcb->orders_handled < bcb->expected_num_orders) {
        pthread_cond_wait(&bcb->can_get_orders, &bcb->mutex);
    }
    
    // If all orders are handled, return NULL
    if (IsEmpty(bcb) && bcb->orders_handled >= bcb->expected_num_orders) {
        pthread_cond_broadcast(&bcb->can_get_orders);
        pthread_mutex_unlock(&bcb->mutex);
        return NULL;
    }
    
    // Remove and return the first order
    Order* order = bcb->orders;
    bcb->orders = bcb->orders->next;
    bcb->current_size--;
    bcb->orders_handled++;
    
    pthread_cond_signal(&bcb->can_add_orders);
    pthread_mutex_unlock(&bcb->mutex);
    
    return order;
}