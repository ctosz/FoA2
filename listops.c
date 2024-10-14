
/* Linked list defs:
 *
 * Adapted from implementation by Alistair Moffat: Jianzhong Qi, April 2022
 * Edited by: [Add your name and student ID here]
 *
 */

#include <assert.h>
#include <stdlib.h>

#define MAX_RESTRNTS 99 /* max number of restaurants */
#define MAX_ID_LEN 6	/* maximum length of a customer's id */

typedef char cust_id_t[MAX_ID_LEN + 1];		/* character array for customer ids, +1 for null byte */
typedef int visiting_array_t[MAX_RESTRNTS]; /* number at each index represents the number of times the corresponding restaurant has been visited */

/* struct (node) containing all customer information */
typedef struct
{
	cust_id_t customer_id;
	visiting_array_t visiting_array;
} data_t;

typedef struct customer_record customer_record_t; /* prototype */

/* one customer record, with all information and a pointer to the next record */
struct customer_record
{
	data_t data;
	customer_record_t *next;
};

/* linked list of customer records */
typedef struct
{
	customer_record_t *head;
	customer_record_t *foot;
} list_t;

list_t
	*
	make_empty_list(void)
{
	list_t *list;
	list = (list_t *)malloc(sizeof(*list));
	assert(list != NULL);
	list->head = list->foot = NULL;
	return list;
}

int is_empty_list(list_t *list)
{
	assert(list != NULL);
	return list->head == NULL;
}

void free_list(list_t *list)
{
	customer_record_t *curr, *prev;
	assert(list != NULL);
	curr = list->head;
	while (curr)
	{
		prev = curr;
		curr = curr->next;
		free(prev);
	}
	free(list);
}

list_t
	*
	insert_at_head(list_t *list, data_t value)
{
	customer_record_t *new;
	new = (customer_record_t *)malloc(sizeof(*new));
	assert(list != NULL && new != NULL);
	new->data = value;
	new->next = list->head;
	list->head = new;
	if (list->foot == NULL)
	{
		/* this is the first insertion into the list */
		list->foot = new;
	}
	return list;
}

list_t
	*
	insert_at_foot(list_t *list, data_t value)
{
	customer_record_t *new;
	new = (customer_record_t *)malloc(sizeof(*new));
	assert(list != NULL && new != NULL);
	new->data = value;
	new->next = NULL;
	if (list->foot == NULL)
	{
		/* this is the first insertion into the list */
		list->head = list->foot = new;
	}
	else
	{
		list->foot->next = new;
		list->foot = new;
	}
	return list;
}

data_t
get_head(list_t *list)
{
	assert(list != NULL && list->head != NULL);
	return list->head->data;
}

list_t
	*
	get_tail(list_t *list)
{
	customer_record_t *oldhead;
	assert(list != NULL && list->head != NULL);
	oldhead = list->head;
	list->head = list->head->next;
	if (list->head == NULL)
	{
		/* the only list node just got deleted */
		list->foot = NULL;
	}
	free(oldhead);
	return list;
}

/* =====================================================================
   Program written by Alistair Moffat, as an example for the book
   "Programming, Problem Solving, and Abstraction with C", Pearson
   Custom Books, Sydney, Australia, 2002; revised edition 2012,
   ISBN 9781486010974.

   See http://people.eng.unimelb.edu.au/ammoffat/ppsaa/ for further
   information.

   Prepared December 2012 for the Revised Edition.
   ================================================================== */
