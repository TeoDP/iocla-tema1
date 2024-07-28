#include "functional.h"
#include "tasks.h"
#include "tests.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/*
void func_swap_elem(void *accumulator, void *listdata) {
	int *acc = (int *)accumulator;
	if (acc[0] < 0)
		return;
	void *dest = listdata;
	void *src = (void *)(((char *)listdata) + acc[0] * (acc[1]));
	void *temp = malloc(acc[1]);
	memcpy(temp, dest, acc[1]);
	memcpy(dest, src, acc[1]);
	memcpy(src, temp, acc[1]);
	free(temp);
	(acc[0]) -= 2;
}

array_t reverse(array_t list) {
	array_t result;
	result.len = list.len;
	result.destructor = list.destructor;
	result.elem_size = list.elem_size;
	result.data = malloc(result.len * result.elem_size);
	memcpy(result.data, list.data, result.len * result.elem_size);
	int acc[3];
	acc[0] = result.len - 1;
	acc[1] = result.elem_size;
	acc[2] = result.len / 2;
	reduce(func_swap_elem, &acc, result);
	// list.destructor(list.data);
	// free(list.data);
	return result;
}*/

void func_insert_swapped(void *accumulator, void *listdata) {
	TArrAcc *acc = (TArrAcc *)accumulator;
	// if (acc->nr < acc->arr.len / 2)
		// return;
	int *input = (int *)listdata;
	*(((int *)acc->arr.data) + acc->nr) = *input;
	acc->nr--;
}

array_t reverse(array_t list) {
	TArrAcc acc;
	acc.nr = list.len - 1;
	acc.arr.len = list.len;
	acc.arr.elem_size = list.elem_size;
	acc.arr.destructor = list.destructor;
	acc.arr.data = calloc(list.len, list.elem_size);
	reduce(func_insert_swapped, &acc, list);
	array_t result;
	result = acc.arr;
	return result;
}

void destruct_num_array(void *result) {
	number_t *num = (number_t *)result;
	free(num->string);
}

void func_number_array(void *result, void **object) {
	number_t *num = (number_t *)result;
	num->integer_part = *(int *)object[0];
	num->fractional_part = *(int *)object[1];
	num->string = malloc(10 * sizeof(*num->string));
	sprintf(num->string, "%d.%d", num->integer_part, num->fractional_part);
}

array_t create_number_array(array_t integer_part, array_t fractional_part) {
	array_t new_list = map_multiple(func_number_array,
					   (int)sizeof(number_t), NULL,
									2, integer_part, fractional_part);
	new_list.destructor = destruct_num_array;
	return new_list;
}

void func_name_destructor(void *result) {
	char **names = (char **)result;
	free(*names);
}

boolean func_filter_passing_students(void *result) {
	student_t *stud = (student_t *)result;
	if (stud->grade >= (float)5)
		return true;
	return false;
}

void func_get_student_name(void *result, void *elem) {
	student_t *stud = (student_t *)elem;
	char **names = (char **)result;
	// free(*names);
	*names = malloc((strlen(stud->name) + 1) * sizeof(char));
	strcpy(*names, stud->name);
}

array_t get_passing_students_names(array_t list) {
	array_t filtered = filter(func_filter_passing_students, list);
	array_t result = map(func_get_student_name,
						 (int)sizeof(char *), func_name_destructor, filtered);
	// filtered.destructor(filtered.data);
	// free(filtered.data);
	// result.destructor =
	return result;
}

void func_sum_the_ints(void *acc, void *value) {
	int *sum = (int *)acc;
	int *add = (int *)value;
	*sum += *add;
}

void func_gen_sum_list(void *result, void *elem) {
	array_t *list_list = (array_t *)elem;
	int *summed = (int *)result;
	int sum = 0;
	reduce(func_sum_the_ints, &sum, *list_list);
	*summed = sum;
}

void func_verify_largest_int(void *result, void **values) {
	boolean *res = (boolean *)result;
	int **val = (int **)values;
	if (*val[0] >= *val[1])
		*res = true;
	else
		*res = false;
}

array_t check_bigger_sum(array_t list_list, array_t int_list) {
	// int sum1 = 0;
	array_t sum_list = map(func_gen_sum_list, (int)sizeof(int),
						   NULL, list_list);
	array_t result = map_multiple(func_verify_largest_int,
								  (int)sizeof(boolean), NULL, 2,
								  sum_list, int_list);
	return result;
}

/*
void func_skip_one_elem (void *result, void *list) {
	char **res = (char **)result;
	char **elem = (char **)list;
	*res = malloc(strlen(*elem) + 2);
	strcpy(*res, *elem);
}*/
/*
void func_destruct_odd(void *accumulator, void *listdata) {
	int *acc = (int *)accumulator;
	// char *val = (char *)listdata;
	(*acc)++;
	if (*acc % 2 == 0)
		free(*(char **)listdata);
}
*/
/*
array_t get_even_indexed_strings(array_t list) {
	int acc = 0;
	reduce(func_destruct_odd, &acc, list);
	int temp = list.len / 2;
	if (list.len % 2 == 1)
		temp++;
	list.len = temp;
	list.elem_size = 2 * list.elem_size;
	array_t result = map(func_skip_one_elem,
						 list.elem_size, list.destructor, list);
	// result.elem_size = 2 * list.elem_size;
	// result.len = list.len / 2;
	// if (list.len % 2 == 1) result.len++;
	// array_t result2 = filter(identical)
	return result;
}*/

void func_move_even_to_acc(void *accumulator, void *listdata) {
	TIndexAcc *acc = (TIndexAcc *)accumulator;
	//acc->nr++;
	if (acc->nr % 2 == 0) {
		*(((char **)acc->data) + acc->nr / 2) = malloc(100 * sizeof(char));
		memcpy(*(((char **)acc->data) + acc->nr / 2),
			   *(char **)listdata, acc->size);
	}
	acc->nr++;
}

array_t get_even_indexed_strings(array_t list) {
	TIndexAcc acc;
	acc.nr = 0;
	acc.size = list.elem_size;
	int temp = list.len / 2;
	if (list.len % 2 == 1)
		temp++;
	acc.data = malloc(list.elem_size * temp);
	reduce(func_move_even_to_acc, &acc, list);
	array_t result;
	result.data = acc.data;
	result.len = temp;
	result.elem_size = list.elem_size;
	result.destructor = list.destructor;
	if (list.destructor)
		for_each(list.destructor, list);
	free(list.data);
	return result;
}

/*
void func_gen_items(void *accumulator, void *listdata) {
	int *acc = (int *)accumulator;
	int *val = (int *)listdata;
	(*acc)++;
	*val = *acc;
}

void func_generate_lines(void *accumulator, void *list) {
	int *acc = (int *)accumulator;
	array_t *line = (array_t *)list;
	// int temp = acc[0];
	// (*acc)++;
	array_t *aux = (array_t *)line;
	aux->data = malloc(sizeof(int) * acc[1]);
	aux->len = acc[1];
	aux->elem_size = (int)sizeof(int);
	aux->destructor = NULL;
	reduce(func_gen_items, acc, *aux);
	acc[0] -= line->len;
	acc[0]++;
}
*/
void func_destruct_list_list(void *data) {
	array_t *elem = (array_t *)data;
	free(elem->data);
}

/*
array_t generate_square_matrix(int n) {
	int counter[] = {0, 8};
	array_t list;
	list.elem_size = (int)sizeof(array_t);
	list.len = n;
	list.data = malloc(list.len * list.elem_size);
	list.destructor = func_destruct_list_list;
	reduce(func_generate_lines, counter, list);
	return list;
	// array_t result = map_multiple(func_generate_lines,
				// (int)sizeof(array_t), func_destruct_array, n, )
}*/
/*
void func_gen_items(void *accumulator, void *listdata) {
	TIndexAcc *acc = (TIndexAcc *)accumulator;
	acc->nr++;

	(void)listdata;
}

void func_generate_lines(void *accumulator, void *listdata) {
	TIndexAcc *acc = (TIndexAcc *)accumulator;
	//(((array_t *)acc->data) + acc->nr)->data = malloc(sizeof(array_t));
	TIndexAcc aux;
	aux.nr = acc->nr;
	aux.size = sizeof(array_t);
	aux.data = ((array_t *)acc->data + acc->nr)->data;
	aux.data = malloc(sizeof(array_t));
	((array_t *)aux.data)->len = acc->len;
	((array_t *)aux.data)->destructor = NULL;
	((array_t *)aux.data)->elem_size = sizeof(int);
	((array_t *)aux.data)->data = malloc(sizeof(int)
	* ((array_t *)aux.data)->len);
	reduce(func_gen_items, &aux, *((array_t *)aux.data));
	acc->nr = aux.nr - acc->len;
	(void)listdata;
}

array_t generate_square_matrix(int n) {
	TIndexAcc acc;
	acc.nr = 0;
	acc.len = n;
	acc.size = sizeof(array_t *);
	acc.data = malloc(n * acc.size);
	array_t list;
	list.len = n;
	list.destructor = func_destruct_list_list;
	list.data = acc.data;
	list.elem_size = acc.size;
	reduce(func_generate_lines, &acc, list);
	list.data = acc.data;
	return list;
}*/

void func_gen_line(void *listdata) {
	int *number = (int *)listdata;
	if (*(number) != 0)
		return;
	*(number) = *(number - 1) + 1;
}

void func_gen_first_col(void *listdata) {
	int *number = (int *)listdata;
	if (*number != 0)
		return;
	*(number) = *(number - 1) + 1;
}

void func_gen_lines(void *destination, void *source) {
	array_t *dest = (array_t *)destination;
	// dest = malloc(sizeof(array_t));
	// int temp = *(int *)source;
	if (*(int *)source == 0) {
		dest->len = *((int *)source + 1) - 2;
		*(int *)source = 1 + dest->len;
	} else
		dest->len = *(int *)source - *((int *)source - 1) - 1;
	*(int *)source = *(int *)source - dest->len;
	dest->elem_size = sizeof(int);
	dest->data = calloc(dest->len, dest->elem_size);
	*(int *)dest->data = *(int *)source;
	for_each(func_gen_line, *dest);
}

array_t generate_square_matrix(int n) {
	// int acc = 0;// this is to manipulate memory i think
	// (void)acc;
	array_t aux;
	aux.data = calloc(n, sizeof(int));
	aux.len = n;
	aux.elem_size = sizeof(int);
	aux.destructor = NULL;
	*(int *)aux.data = 1 + n;
	// *((int *)aux.data + 1) = n;
	for_each(func_gen_first_col, aux);
	*(int *)aux.data = 0;
	array_t result = map(func_gen_lines, sizeof(array_t),
						 func_destruct_list_list, aux);
	return result;
}
