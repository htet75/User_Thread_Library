#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

/* Iterator function to test for queue*/
static void iterator_inc(queue_t q, void *data)
{
	int *a = (int*)data;

	if (*a == 42)
		queue_delete(q, data);
	else
		*a += 1;
} 

/* Use iterator function on queue and check results */
void test_iterator(void)
{
	queue_t q;
	int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
	size_t i;

	fprintf(stderr, "*** TEST queue_iterator ***\n");

	q = queue_create();
	for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
		queue_enqueue(q, &data[i]);

	queue_iterate(q, iterator_inc);
	TEST_ASSERT(data[0] == 2);
	TEST_ASSERT(data[1] == 3);
	TEST_ASSERT(queue_length(q) == 9);
}

void test_queue_normal(void)
{
	int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int *return_ptr;

	queue_t q;
	fprintf(stderr, "*** TEST queue_normal ***\n");
	
	q = queue_create();
	for (size_t i = 0; i < sizeof(data)/sizeof(data[0]); i++)
	{
		queue_enqueue(q, &data[i]);
	}
	
	for (size_t i = 0; i < sizeof(data)/sizeof(data[0]); i++)
	{
		queue_dequeue(q, (void**)&return_ptr);
	}

	TEST_ASSERT(queue_length(q) == 0);
}


/* Check NULL/error cases for all functions */
void test_error(void)
{
	fprintf(stderr, "*** TEST queue error checking ***\n");
	queue_t q;
	int data = 3;
	int *return_ptr;
	// int *null_ptr = NULL;

	q = queue_create();
	assert(queue_destroy(q) == 0); // Successfully destroy a queue
	
	q = queue_create();
	
	queue_enqueue(q, &data);
	TEST_ASSERT(queue_destroy(q) == -1); // Unsuccessfully destroy a queue because not empty
	
	TEST_ASSERT(queue_destroy(NULL) == -1); // Unsuccessfully destroy a queue because input is NULL pointer

	// Unsucessfully enqueue/dequeue due to NULL pointers
	TEST_ASSERT(queue_enqueue(NULL, &data) == -1);
	TEST_ASSERT(queue_enqueue(q, NULL) == -1);
	TEST_ASSERT(queue_dequeue(NULL, (void**)&return_ptr) == -1);
	TEST_ASSERT(queue_dequeue(q, NULL) == -1);

	// Unsuccessfully delete node due to NULL pointers and element not in list
	int delete_num = 1;
	TEST_ASSERT(queue_delete(NULL, &data) == -1);
	TEST_ASSERT(queue_delete(q, NULL) == -1);
	TEST_ASSERT(queue_delete(q, &delete_num) == -1);

	// Unsuccessfully iterate through queue due to NULL pointers
	TEST_ASSERT(queue_iterate(q, NULL) == -1);
	TEST_ASSERT(queue_iterate(NULL, iterator_inc) == -1);

	// Unsucessfully return length because it is null
	TEST_ASSERT(queue_length(NULL) == -1);
}

void test_queue_enqueue()
{
	fprintf(stderr, "*** TEST queue enqueue ***\n");
    queue_t queue = queue_create();
    int data1 = 1, data2 = 2, data3 = 3;
    TEST_ASSERT(queue_enqueue(queue, &data1) == 0);
    TEST_ASSERT(queue_enqueue(queue, &data2) == 0);
    TEST_ASSERT(queue_enqueue(queue, &data3) == 0);
    TEST_ASSERT(queue_length(queue) == 3);
}

void test_queue_dequeue()
{
	fprintf(stderr, "*** TEST queue dequeue ***\n");

    queue_t queue = queue_create();
    int data1 = 1, data2 = 2, data3 = 3;

    queue_enqueue(queue, &data1);
    queue_enqueue(queue, &data2);
    queue_enqueue(queue, &data3);

    void *data;
    TEST_ASSERT(queue_dequeue(queue, &data) == 0);
    TEST_ASSERT(*(int*)data == 1);
    TEST_ASSERT(queue_dequeue(queue, &data) == 0);
    TEST_ASSERT(*(int*)data == 2);
    TEST_ASSERT(queue_dequeue(queue, &data) == 0);
    TEST_ASSERT(*(int*)data == 3);
    TEST_ASSERT(queue_dequeue(queue, &data) == -1);
    TEST_ASSERT(queue_length(queue) == 0);
}

void test_queue_delete()
{
	fprintf(stderr, "*** TEST queue delete ***\n");

    queue_t queue = queue_create();
    int data1 = 1, data2 = 2, data3 = 3, data4 = 4;

    queue_enqueue(queue, &data1);
    queue_enqueue(queue, &data2);
    queue_enqueue(queue, &data3);
    queue_enqueue(queue, &data4);

    TEST_ASSERT(queue_delete(queue, &data2) == 0);
    TEST_ASSERT(queue_length(queue) == 3);

    void *data;
    TEST_ASSERT(queue_dequeue(queue, &data) == 0);
    TEST_ASSERT(*(int*)data == 1);
    TEST_ASSERT(queue_dequeue(queue, &data) == 0);
    TEST_ASSERT(*(int*)data == 3);
    TEST_ASSERT(queue_dequeue(queue, &data) == 0);
    TEST_ASSERT(*(int*)data == 4);
    TEST_ASSERT(queue_delete(queue, &data2) == -1);
}

void test_queue_iterate()
{
	fprintf(stderr, "*** TEST queue iterate ***\n");

    queue_t queue = queue_create();
    int data1 = 1, data2 = 2, data3 = 3, data4 = 4;

    queue_enqueue(queue, &data1);
    queue_enqueue(queue, &data2);
    queue_enqueue(queue, &data3);
    queue_enqueue(queue, &data4);

    int sum = 0;
	void sum_func(queue_t q, void *data)
	{
		queue_length(q);
		sum += *(int*)data;
	}

    TEST_ASSERT(queue_iterate(queue, sum_func) == 0);
    TEST_ASSERT(sum == 10);
}

void test_queue_length()
{
	fprintf(stderr, "*** TEST queue length ***\n");

    queue_t queue = queue_create();
    int data1 = 1, data2 = 2, data3 = 3, data4 = 4;

    TEST_ASSERT(queue_length(queue) == 0);
    queue_enqueue(queue, &data1);
    TEST_ASSERT(queue_length(queue) == 1);
    queue_enqueue(queue, &data2);
    TEST_ASSERT(queue_length(queue) == 2);
    queue_enqueue(queue, &data3);
    TEST_ASSERT(queue_length(queue) == 3);
    queue_enqueue(queue, &data4);
    TEST_ASSERT(queue_length(queue) == 4);

    void* data = NULL;
    queue_dequeue(queue, &data);
    TEST_ASSERT(queue_length(queue) == 3);
    queue_dequeue(queue, &data);
    TEST_ASSERT(queue_length(queue) == 2);
    queue_dequeue(queue, &data);
    TEST_ASSERT(queue_length(queue) == 1);
    queue_dequeue(queue, &data);
    TEST_ASSERT(queue_length(queue) == 0);
}



int main(void)
{
	test_create();
	test_queue_simple();
	test_queue_normal();
	test_iterator();
	test_error();
	test_queue_enqueue();
	test_queue_dequeue();
	test_queue_delete();
	test_queue_iterate();
	test_queue_length();

	return 0;
}
