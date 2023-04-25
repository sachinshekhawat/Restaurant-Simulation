RESTAURANT = restaurant.o
DOORMAN = doorman.o
WAITER = waiter.o
CUSTOMER = customer.o
OUTRESTAURANT = restaurant
OUTDOORMAN = doorman
OUTWAITER = waiter
OUTCUSTOMER = customer

CC = gcc
FLAGS  = -c -g
all: $(OUTRESTAURANT) $(OUTDOORMAN) $(OUTWAITER) $(OUTCUSTOMER)

$(OUTRESTAURANT): $(RESTAURANT)
	$(CC) -g -pthread $(RESTAURANT) -o $(OUTRESTAURANT)

$(OUTDOORMAN): $(DOORMAN)
	$(CC) -g -pthread $(DOORMAN) -o $(OUTDOORMAN)

$(OUTWAITER): $(WAITER)
	$(CC) -g -pthread $(WAITER) -o $(OUTWAITER)

$(OUTCUSTOMER): $(CUSTOMER)
	$(CC) -g -pthread $(CUSTOMER) -o $(OUTCUSTOMER)

restaurant.o: restaurant.c
	$(CC) $(FLAGS) restaurant.c

doorman.o: doorman.c
	$(CC) $(FLAGS) doorman.c

waiter.o: waiter.c
	$(CC) $(FLAGS) waiter.c

customer.o: customer.c
	$(CC) $(FLAGS) customer.c

clean:
	rm -f $(OUTRESTAURANT) $(RESTAURANT) $(OUTDOORMAN) $(DOORMAN) $(OUTWAITER) $(WAITER) $(OUTCUSTOMER) $(CUSTOMER)
