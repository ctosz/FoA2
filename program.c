/* Restaurant recommender:
 *
 * Skeleton code written by Jianzhong Qi, April 2022
 * Edited by: Claire Tosolini, 1271302
 * algorithms are awesome! :D
 *
 */

#include <stdio.h>
#include <string.h>
#include "listops.c"
#include <math.h>

/* stage heading */
#define HEADING "==========================Stage %d==========================\n"
#define MAX_TYPE_LEN 20       /* maximum length of restaurant cuisine type */
#define MAX_NAME_LEN 100      /* maximum length of restaurant name */
#define MAX_RESTRNTS 99       /* maximum number of restaurants */
#define MAX_SIMILAR_CSTMERS 2 /* maximum number of similar customers (of another \
customer) retained */
#define STAGE_ONE 1
#define STAGE_TWO 2
#define STAGE_THREE 3
#define STAGE_FOUR 4
#define NEWLINE '\n'
#define DASH '-'
#define PLUS '+'
#define ASTERISK '*'
#define FIRST_CUSTOMER 1  /* most similar customer */
#define SECOND_CUSTOMER 2 /* second most similar customer */

/* typedefs */
// typedef prototype for customer linked list: inside listops.c

// create the restaurant struct setups
typedef double coordinate_t;
typedef char type_t[MAX_TYPE_LEN]; /* character array to store cuisine type */
typedef char name_t[MAX_NAME_LEN]; /* char array to store restaurant name */

/* all information about one restaurant */
typedef struct
{
    int restaurant_ID, price_per_head;
    double x_coord, y_coord;
    type_t cuisine_type;
    name_t restaurant_name;
} restaurant_t;

/* all parts of a dining transaction time */
typedef struct
{
    int year, month, day, hour, minute, second;
} transaction_time_t;

/* all information of a dining transaction */
typedef struct
{
    transaction_time_t time;
    cust_id_t customer_id;
    int restaurant_ID;
    int spent;
} dining_transaction_t;

/* array of all restaurants (array of structs) */
typedef restaurant_t all_restaurants_t[MAX_RESTRNTS];

/****************************************************************/
/* function prototypes */
void print_stage_header(int stage);

int stage_one(all_restaurants_t rest_list);
int read_restaurant(restaurant_t *one_restaurant, int *num_read);
restaurant_t find_smallest_price(all_restaurants_t restaurant_array,
                                 int num_rstrnts);

void stage_two();
void read_dining_transactions(list_t *customer_list,
                              all_restaurants_t restaurant_array,
                              int num_rstrnts);
void add_to_list(dining_transaction_t current_transaction,
                 list_t *customer_list, all_restaurants_t restaurant_array,
                 int num_rstrnts);
int does_id_exist(list_t *customer_list, cust_id_t id_wanted,
                  all_restaurants_t restaurant_array, int num_rstrnts,
                  int current_r_id);
int search_visiting_record(all_restaurants_t restaurant_array, int num_rstrnts,
                           int id_wanted);
void initialise_to_zero(data_t *data, int num_rstrnts);
void print_array(int visiting_array[], int num_rstrnts);
void print_list(list_t *customer_list, int num_rstrnts);

void stage_three(list_t *customer_list, all_restaurants_t restaurant_array,
                 int num_rstrnts);
void restaurant_similarity(customer_record_t *current, int num_rstrnts,
                           all_restaurants_t restaurant_array);
double calculate_distance(restaurant_t *rest_1, restaurant_t *rest_2);
int has_simimlarity(restaurant_t *rest_1, restaurant_t *rest_2);
int calculate_customer_similarity(visiting_array_t customer_1,
                                  visiting_array_t customer_2, int num_rstrnts);

void stage_four(list_t *customer_list, int num_rstrnts,
                list_t *all_similar_customers);
void find_similar_customers(list_t *similar_customers,
                            customer_record_t *comparison_cust,
                            list_t *customer_list, int num_rstrnts);
void find_all_sim_customers(list_t *customer_list, int num_rstrnts,
                            list_t *all_similar_customers);
int check_if_equal(cust_id_t comparison_id, cust_id_t current_id);
int count_customers(list_t *customer_list);
void copy_array(int num_rstrnts, visiting_array_t source,
                visiting_array_t dest);
void edit_visiting_array(customer_record_t *comparison_cust,
                         list_t *similar_customers, int num_rstrnts,
                         list_t *customer_list);
void add_to_list_2(int num_rstrnts, list_t *similar_customers,
                   customer_record_t *sim_customer, int cust_value);
void create_list_array(int num_cust, list_t *all_similar_customers);
void free_list_array(int num_cust, list_t *all_similar_customers);
void set_list_nodes(customer_record_t *sim_cust_1,
                    customer_record_t *sim_cust_2, int sim_val_1, int sim_val_2,
                    list_t *similar_customers, int num_rstrnts);

/****************************************************************/

/* main program controls all the action */
int main(int argc, char *argv[])
{
    /* TODO: Write your solution! */

    all_restaurants_t restaurant_array;
    list_t *customer_list = make_empty_list();

    /* stage 1: read all restaurant records, output number of records */
    int num_rstrnts = stage_one(restaurant_array);

    /* stage 2: read all dining transactions, store all customer information in
    a linked list. output list as a matrix */
    stage_two(customer_list, restaurant_array, num_rstrnts);

    /* stage 3: edit the linked list using content-based recommendation.
    output updated list as a matrix */
    stage_three(customer_list, restaurant_array, num_rstrnts);

    /* number of customers (varies with input) */
    int num_cust = count_customers(customer_list);

    /* create an array of (pointers to) lists, where each list corresponds to a
    customer and holds its two most similar customers */
    list_t *all_similar_customers = malloc(num_cust * sizeof(list_t));
    /* the following site helped with the above line:
    https://www.codegrepper.com/code-examples/c/how
    +to+use+malloc+to+create+array+in+c */
    assert(all_similar_customers);
    create_list_array(num_cust, all_similar_customers);

    /* stage 4: edit linked list using user-based collaborative filtering.
    output updated list as a matrix */
    stage_four(customer_list, num_rstrnts, all_similar_customers);

    /* free all memory set using malloc */
    free_list(customer_list);
    free_list_array(num_cust, all_similar_customers);
    free(all_similar_customers);

    return 0;
}

/* print stage header given stage number */
void print_stage_header(int stage_num)
{
    printf(HEADING, stage_num);
}

/* TODO: Define more functions! */

/* stage one: read in inputs into all restaurants array */
int stage_one(all_restaurants_t all_restaurants)
{
    print_stage_header(STAGE_ONE);

    int i = 0, num_rstrnts = 0;
    while (read_restaurant(&all_restaurants[i], &num_rstrnts) == 1)
    {
        i += 1;
    }

    printf("Number of restaurants: %d\n", num_rstrnts);
    printf("Restaurant with the smallest average price: %s\n",
           find_smallest_price(all_restaurants, num_rstrnts).restaurant_name);
    printf("%c", NEWLINE);

    return num_rstrnts;
}

/* takes in a pointer to: a restaurant struct and an integer (number of reads).
    returns 1 while current input contains valid restaurant information */
int read_restaurant(restaurant_t *one_restaurant, int *num_read)
{
    while (scanf("%d %lf %lf %d %s %s", &one_restaurant->restaurant_ID,
                 &one_restaurant->x_coord, &one_restaurant->y_coord,
                 &one_restaurant->price_per_head, one_restaurant->cuisine_type,
                 one_restaurant->restaurant_name) == 6)
    {
        *num_read += 1;
        return 1; // success
    }

    return 0;
}

/* takes in the array of all restaurants, finds the smallest average price
per head and returns the information of the corresponding restaurant */
restaurant_t find_smallest_price(all_restaurants_t restaurant_array,
                                 int num_rstrnts)
{
    int smallest_price = restaurant_array[0].price_per_head;
    restaurant_t rstrnt_w_smallest = restaurant_array[0];

    for (int i = 1; i < num_rstrnts; i++)
    {
        if (restaurant_array[i].price_per_head < smallest_price)
        {
            smallest_price = restaurant_array[i].price_per_head;
            rstrnt_w_smallest = restaurant_array[i];
        }
    }

    return rstrnt_w_smallest;
}

/* process input dining transactions and translate customer information into a
linked list */
void stage_two(list_t *customer_list, all_restaurants_t restaurant_array,
               int num_rstrnts)
{
    read_dining_transactions(customer_list, restaurant_array, num_rstrnts);

    print_stage_header(STAGE_TWO);

    print_list(customer_list, num_rstrnts);
    printf("%c", NEWLINE);
}

/* while the scanned input is a valid dining transaction, use the relevant
information from that transaction to update the customer linked list with
another customer's information */
void read_dining_transactions(list_t *customer_list,
                              all_restaurants_t restaurant_array,
                              int num_rstrnts)
{

    dining_transaction_t current_transaction;

    // skip #####
    getchar();
    getchar();
    getchar();
    getchar();
    getchar();

    while (scanf("%d:%d:%d:%d:%d:%d %s %d %d", &current_transaction.time.year,
                 &current_transaction.time.month, &current_transaction.time.day,
                 &current_transaction.time.hour,
                 &current_transaction.time.minute,
                 &current_transaction.time.second,
                 current_transaction.customer_id,
                 &current_transaction.restaurant_ID,
                 &current_transaction.spent) == 9)
    {
        add_to_list(current_transaction, customer_list, restaurant_array,
                    num_rstrnts);
    }
}

/* add a new customer (customer ID and visiting array) to the linked list of
all customers */
void add_to_list(dining_transaction_t current_transaction,
                 list_t *customer_list, all_restaurants_t restaurant_array,
                 int num_rstrnts)
{
    // create a new struct to hold customer information
    data_t current_data;
    strcpy(current_data.customer_id, current_transaction.customer_id);
    initialise_to_zero(&current_data, num_rstrnts);

    // if the list is empty, this is the first customer
    if (is_empty_list(customer_list))
    {
        insert_at_head(customer_list, current_data);

        // find the correct index of the visiting array record to add 1 to
        int index_to_insert = search_visiting_record(restaurant_array,
                                                     num_rstrnts, current_transaction.restaurant_ID);
        customer_list->head->data.visiting_array[index_to_insert] = 0;
        customer_list->head->data.visiting_array[index_to_insert] += 1;
    }
    else if (!is_empty_list(customer_list))
    {
        /* if the customer is already in the list, add 1 to the visiting array
        at the correct index */
        int id_existence = does_id_exist(customer_list,
                                         current_transaction.customer_id,
                                         restaurant_array, num_rstrnts,
                                         current_transaction.restaurant_ID);

        // if the customer is not in the linked list, add it to the end
        if (id_existence == 0)
        {
            insert_at_foot(customer_list, current_data);

            int index_to_insert = search_visiting_record(restaurant_array,
                                                         num_rstrnts, current_transaction.restaurant_ID);
            customer_list->foot->data.visiting_array[index_to_insert] = 0;
            customer_list->foot->data.visiting_array[index_to_insert] += 1;
        }
    }
}

/* search for the specific customer id in the list. if found, add 1 to the
correct index in the visiting array and return 1. return -1 if the customer id
was not found. */
int does_id_exist(list_t *customer_list, cust_id_t id_wanted,
                  all_restaurants_t restaurant_array, int num_rstrnts,
                  int current_r_id)
{
    customer_record_t *current = customer_list->head;
    assert(current->data.customer_id != NULL); // shouldn't be NULL here

    while (current)
    {
        if (strncmp(current->data.customer_id, id_wanted, MAX_ID_LEN) == 0)
        {
            //  add 1 to correct place in array
            int index_to_insert = search_visiting_record(restaurant_array,
                                                         num_rstrnts, current_r_id);
            current->data.visiting_array[index_to_insert] += 1;
            return 1;
        }
        else
        {
            current = current->next;
        }
    }
    return 0;
}

/* searches through all restaurants. returns the index of the id wanted if
found, or -1 if not. this function is used when a customer has visited a
restaurant, to determine at what index in the visiting record (array) should
the value be increased by 1 */
int search_visiting_record(all_restaurants_t restaurant_array, int num_rstrnts,
                           int id_wanted)
{
    for (int i = 0; i < num_rstrnts; i++)
    {
        if (restaurant_array[i].restaurant_ID == id_wanted)
        {
            return i;
        }
    }
    return -1;
}

/* input pointer to struct, set all elements of array in this struct to zero */
void initialise_to_zero(data_t *data, int num_rstrnts)
{
    for (int i = 0; i < num_rstrnts; i++)
    {
        data->visiting_array[i] = 0;
    }
}

/* print out a linked list in matrix format */
void print_list(list_t *customer_list, int num_rstrnts)
{
    customer_record_t *current = customer_list->head;
    while (current)
    {
        printf("%s:", current->data.customer_id);
        print_array(current->data.visiting_array, num_rstrnts);
        current = current->next;
    }
}

/* prints a customer visiting array. Special outputs for negative numbers are
included: (-1 --> '-', -2 --> '+', -3 --> '*') */
void print_array(int visiting_array[], int num_rstrnts)
{
    for (int i = 0; i < num_rstrnts; i++)
    {
        if (visiting_array[i] == -1)
        {
            printf("%3c", DASH);
        }
        else if (visiting_array[i] == -2)
        {
            printf("%3c", PLUS);
        }
        else if (visiting_array[i] == -3)
        {
            printf("%3c", ASTERISK);
        }
        else
        {
            printf("%3d", visiting_array[i]);
        }
    }
    printf("%c", NEWLINE);
}

/* for every customer, edit their visiting arrays by determining if any
unvisited restaurants share characteristics with visited ones */
void stage_three(list_t *customer_list, all_restaurants_t restaurant_array,
                 int num_rstrnts)
{
    print_stage_header(STAGE_THREE);

    customer_record_t *current = customer_list->head;
    while (current)
    {
        restaurant_similarity(current, num_rstrnts, restaurant_array);
        current = current->next;
    }

    print_list(customer_list, num_rstrnts);
    printf("%c", NEWLINE);
}

/* changes visiting array values to -1 if the corresponding restaurants share
similarity */
void restaurant_similarity(customer_record_t *current, int num_rstrnts,
                           all_restaurants_t restaurant_array)
{
    int i = 0, j = 0;

    for (i = 0; i < num_rstrnts; i++)
    {
        if (current->data.visiting_array[i] > 0)
        {
            // compare to all other restaurants in array which have value 0
            for (j = 0; j < num_rstrnts; j++)
            {
                if (current->data.visiting_array[j] == 0)
                {
                    if (has_simimlarity(&restaurant_array[i],
                                        &restaurant_array[j]) == 1)
                    {
                        current->data.visiting_array[j] = -1;
                    }
                }
            }
        }
    }
}

/* returns the distance between two points using euclidean distance formula */
double calculate_distance(restaurant_t *rest_1, restaurant_t *rest_2)
{
    double x_component = (rest_1->x_coord - rest_2->x_coord) *
                         (rest_1->x_coord - rest_2->x_coord);
    double y_component = (rest_1->y_coord - rest_2->y_coord) *
                         (rest_1->y_coord - rest_2->y_coord);

    double distance = sqrt(x_component + y_component);
    return distance;
}

/* returns 1 if two restaurants fulfil any of the similarity criteria */
int has_simimlarity(restaurant_t *rest_1, restaurant_t *rest_2)
{
    double restaurant_distance = calculate_distance(rest_1, rest_2);
    int price_diff = abs(rest_1->price_per_head - rest_2->price_per_head);

    /* similar if: cuisine types are the same OR distance between them is
    <= 30 units OR average prices per head are within $20 */
    if ((strcmp(rest_1->cuisine_type, rest_2->cuisine_type) == 0) ||
        (restaurant_distance <= 30.0) || (price_diff <= 20))
    {
        return 1;
    }
    else
    {
        // false, keep array value as it is
        return 0;
    }
}

/* for every customer, edit their visiting arrays by first finding their most
similar customers, then determining which restaurants these similar customers
have visitied that the comparison customer has not */
void stage_four(list_t *customer_list, int num_rstrnts,
                list_t *all_similar_customers)
{
    print_stage_header(STAGE_FOUR);

    find_all_sim_customers(customer_list, num_rstrnts, all_similar_customers);

    print_list(customer_list, num_rstrnts);
}

/* for each customer, find the two most similar customers */
void find_all_sim_customers(list_t *customer_list, int num_rstrnts,
                            list_t *all_similar_customers)
{
    customer_record_t *comparison_cust = customer_list->head;
    int num_customer = 0;

    // list of 2 most similar to current customer:
    list_t *similar_customers = &all_similar_customers[num_customer];

    while (comparison_cust)
    {
        find_similar_customers(similar_customers, comparison_cust,
                               customer_list, num_rstrnts);
        edit_visiting_array(comparison_cust,
                            &all_similar_customers[num_customer], num_rstrnts,
                            customer_list);

        num_customer += 1;
        comparison_cust = comparison_cust->next;
        similar_customers = &all_similar_customers[num_customer];
    }
}

/* finds the two most similar customers to a given customer comparison_cust,
add to that customer's list of similar ONLY IF the similarity value > 0 */
void find_similar_customers(list_t *similar_customers,
                            customer_record_t *comparison_cust,
                            list_t *customer_list, int num_rstrnts)
{
    customer_record_t *current = customer_list->head;
    customer_record_t *sim_cust_1, *sim_cust_2; // two most similar customers
    int sim_val_1, sim_val_2;                   // largest similarity values

    while (current)
    {
        if (check_if_equal(comparison_cust->data.customer_id,
                           current->data.customer_id))
        {
            current = current->next;
            continue;
        }

        // similar value 1 = comparing to the first customer
        sim_val_1 = calculate_customer_similarity(
            comparison_cust->data.visiting_array, current->data.visiting_array,
            num_rstrnts);
        sim_cust_1 = current;
        current = current->next;

        if (check_if_equal(comparison_cust->data.customer_id,
                           current->data.customer_id))
        {
            current = current->next;
        }

        // similar value 2: compare to second customer.
        int current_similarity = calculate_customer_similarity(
            comparison_cust->data.visiting_array, current->data.visiting_array,
            num_rstrnts);
        // if new largest value
        if (current_similarity > sim_val_1)
        {
            sim_val_2 = sim_val_1;
            sim_val_1 = current_similarity;
            sim_cust_2 = sim_cust_1;
            sim_cust_1 = current;
        }
        else // value is equal or smaller
        {
            sim_val_2 = current_similarity;
            sim_cust_2 = current;
        }

        // we know there are at least 3 customers (from assignment sheet)
        current = current->next;

        while (current)
        {
            if (check_if_equal(comparison_cust->data.customer_id,
                               current->data.customer_id))
            {
                current = current->next;
                continue;
            }

            current_similarity = calculate_customer_similarity(
                comparison_cust->data.visiting_array,
                current->data.visiting_array, num_rstrnts);
            if ((current_similarity > sim_val_2) &&
                (current_similarity <= sim_val_1))
            {
                sim_val_2 = current_similarity;
                sim_cust_2 = current;
            }
            else if (current_similarity > sim_val_1)
            {
                sim_val_2 = sim_val_1;
                sim_val_1 = current_similarity;
                sim_cust_2 = sim_cust_1;
                sim_cust_1 = current;
            }
            // if the new value is smaller than or equal to sim_val_2, ignore
            current = current->next;
        }

        assert(sim_val_1 >= sim_val_2); // should always be true

        // set the head and foot nodes of the list to the two similar customers
        set_list_nodes(sim_cust_1, sim_cust_2, sim_val_1, sim_val_2,
                       similar_customers, num_rstrnts);

        return;
    }
}

/* returns the integer similarity value between two customers
calculated from their visiting arrays */
int calculate_customer_similarity(visiting_array_t customer_1,
                                  visiting_array_t customer_2, int num_rstrnts)
{
    int similarity = 0;
    int i = 0;

    while (i < num_rstrnts)
    {
        if ((customer_1[i] > 0) && (customer_2[i] > 0))
        {
            similarity += (customer_1[i] * customer_2[i]);
        }

        i++;
    }
    return similarity;
}

/* add -1 to array values if the comparison customer has not visited the
restaurant correcponding to that array index */
void edit_visiting_array(customer_record_t *comparison_cust,
                         list_t *similar_customers, int num_rstrnts,
                         list_t *customer_list)
{
    if (similar_customers == NULL)
    {
        return;
    }

    for (int i = 0; i < num_rstrnts; i++)
    {
        if (!similar_customers->head)
        {
            break;
        }
        else if ((similar_customers->head->data.visiting_array[i] > 0) &&
                 (comparison_cust->data.visiting_array[i] <= 0))
        {
            comparison_cust->data.visiting_array[i] -= 1;
        }
        else
        {
            // do nothing
        }
    }

    // repeat for cust 2
    for (int i = 0; i < num_rstrnts; i++)
    {

        if (!similar_customers->foot)
        {
            break;
        }
        else if ((similar_customers->foot->data.visiting_array[i] > 0) &&
                 (comparison_cust->data.visiting_array[i] <= 0))
        {
            comparison_cust->data.visiting_array[i] -= 1;
        }
        else
        {
            // do nothing
        }
    }
}

/* returns 1 if two customer id character arrays are equal, 0 if they are not */
int check_if_equal(cust_id_t comparison_id, cust_id_t current_id)
{
    if (strncmp(comparison_id, current_id, MAX_ID_LEN) == 0)
    {
        return 1;
    }

    return 0;
}

/* counts and returns the number of (unique) customers */
int count_customers(list_t *customer_list)
{
    customer_record_t *current = customer_list->head;
    int num_customers = 0;
    while (current)
    {
        num_customers += 1;
        current = current->next;
    }

    return num_customers;
}

/* copy the value at every (valid) index of the source array into the
destination array */
void copy_array(int num_rstrnts, visiting_array_t source, visiting_array_t dest)
{
    for (int i = 0; i < num_rstrnts; i++)
    {
        dest[i] = source[i];
    }
}

/* a modified version of the previous function add_to_list(), specifically for
lists of similar customers */
void add_to_list_2(int num_rstrnts, list_t *similar_customers,
                   customer_record_t *sim_customer, int cust_value)
{
    data_t current_data;
    // set up data
    strcpy(current_data.customer_id, sim_customer->data.customer_id);
    initialise_to_zero(&current_data, num_rstrnts);
    copy_array(num_rstrnts, sim_customer->data.visiting_array,
               current_data.visiting_array);

    /* add this data to the list. if customer value == 1, it is the first
    similar customer, so add to head. if customer value == 2, it is the second
    similar customer, so add to foot*/
    if (cust_value == FIRST_CUSTOMER)
    {
        insert_at_head(similar_customers, current_data);
    }
    else if (cust_value == SECOND_CUSTOMER)
    {
        insert_at_foot(similar_customers, current_data);
    }
}

/* create an array of empty lists such that each unique customer from the input
has their own */
void create_list_array(int num_cust, list_t *all_similar_customers)
{
    for (int i = 0; i < num_cust; i++)
    {
        all_similar_customers[i] = *make_empty_list();
    }
}

/* free each list in the array of lists. modelled similar to free_list() from
listops.c, written by Alistair Moffat from "Programming, Problem Solving, and
Abstraction with C", Pearson Custom Books, Sydney, Australia, 2002; revised
edition 2012, ISBN 9781486010974. */
void free_list_array(int num_cust, list_t *all_similar_customers)
{
    for (int i = 0; i < num_cust; i++)
    {
        customer_record_t *head, *foot;
        head = all_similar_customers[i].head;
        foot = all_similar_customers[i].foot;
        // for each index <= number of customers, free head and foot
        free(head);
        free(foot);
    }
}

/* takes arguments containing two customer records and one element of the array
of lists (all_similar_customers), setting the nodes to the corresponding
similar customers */
void set_list_nodes(customer_record_t *sim_cust_1,
                    customer_record_t *sim_cust_2, int sim_val_1, int sim_val_2,
                    list_t *similar_customers, int num_rstrnts)
{
    // add both to list:
    if ((sim_val_1 > 0) && (sim_val_2 > 0))
    {
        add_to_list_2(num_rstrnts, similar_customers, sim_cust_1, 1);
        assert(similar_customers->head);
        add_to_list_2(num_rstrnts, similar_customers, sim_cust_2, 2);
        assert(similar_customers->foot);
    }
    // only one is above zero: add to head and set tail to null
    else if ((sim_val_1 > 0) || (sim_val_2 > 0))
    {
        if (sim_val_1 > 0)
        {
            add_to_list_2(num_rstrnts, similar_customers, sim_cust_1, 1);
            similar_customers->foot = NULL;
            assert(similar_customers->foot == NULL);
        }

        else if (sim_val_2 > 0)
        {
            add_to_list_2(num_rstrnts, similar_customers, sim_cust_2, 1);
            similar_customers->foot = NULL;
            assert(similar_customers->foot == NULL);
        }
    }
    // both are zero or lower: set head and tail to null i.e. all to null
    else
    {
        similar_customers = NULL;
    }
}

/* TIME COMPLEXITY: worst case
Where r = number of restaurants, k = number of similar customers kept,
c = number of customers.
There are two parts of stage four:
1. Finding the top k most similar customers for c number of customers
2. Editing the restaurant arrays of length r.
- There are c iterations of (part 1 + part 2).

Time Complexity for part 1:
- For c iterations:
    - Compare strings of set length 6 characters: O(6)
    - Calculate customer similarity at each index of restaurant array: 0(r)
    - For c iterations:
        - Compare strings: O(6)
        - Calculate similarity: O(r)
    - For k iterations:
        - Copy over string of length 6: O(6)
        - Set all array elements to 0: O(r)
        - Copy array: O(r)
        - Add element to head or foot of list: O(1)
HENCE PART 1 COMPLEXITY = c * (6 + r + c * (6 + r) + k * (6 + r + r + 1))
                        = c * (6 + r + 6c + 6r + 6k + kr + kr + k)
                        = c * (6 + 7r + 6c + 7k + 2kr)
                        = 6c + 7cr + 6c^2 + 7kc + 2krc
                        = c + cr + c^2 + kc + krc

Time Complexity for part 2:
- For k iterations:
    - Edit elements of visiting array by iterating through all elements: O(r)
HENCE PART 2 COMPLEXITY = kr

HENCE TOTAL COMPLEXITY FOR PART 4 = c * (PART 1 + PART 2)
                                  = c * (c + cr + c^2 + kc + krc + kr)
                                  = c^2 + rc^2 + c^3 + kc^2 + krc^2 + krc
                                  = c^2(1 + r + c + k + kr) + krc
If c > k,r: O(c^3).
If c < k,r: O(krc^2).

 */