#include "functional.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

void for_each(void (*func)(void *), array_t list) {
	for (int i = 0; i < list.len; i++)
		func((void *)(((char *)list.data) + i * list.elem_size));
}

array_t map(void (*func)(void *, void *), int new_list_elem_size,
			void (*new_list_destructor)(void *), array_t list) {
	// array_t * result = malloc(sizeof(array_t));
	array_t result;
	result.data = malloc(new_list_elem_size * list.len);
	result.elem_size = new_list_elem_size;
	result.destructor = new_list_destructor;
	result.len = list.len;
	for (int i = 0; i < list.len; i++) {
// if ((void *)(((char *)result.data) + i * result.elem_size) == NULL) break;
		func((void *)(((char *)result.data) + i * result.elem_size),
			 (void *)(((char *)list.data) + i * list.elem_size));
		list.destructor((void *)(((char *)list.data) + i * list.elem_size));
	}
	free(list.data);
	return result;
}

array_t filter(boolean(*func)(void *), array_t list) {
	int j = 0;
	array_t result;
	result.data = malloc(list.elem_size * list.len);
	result.elem_size = list.elem_size;
	for (int i = 0; i < list.len; i++) {
		if (func((void *)(((char *)list.data) + i * list.elem_size))) {
			memcpy((void *)(((char *)result.data) +
			j * result.elem_size), (void *)(((char *)list.data) +
			i * list.elem_size), list.elem_size);
			j++;
		}
	}
	result.len = j;
	// result.data = realloc(result.data, result.elem_size * result.len);
	result.destructor = list.destructor;
	free(list.data);
	// void (*list_destruction)(void*) = list.destructor;
	// list_destruction((list.data));
	// result.destructor((void*)list);
	// void (*list_destruction)(void *elem) = list.destructor;
	// list_destruction((void*)(&list));
	// (void)func;
	// (void)list;
	// return (array_t){0};
	return result;
}

void *reduce(void (*func)(void *, void *), void *acc, array_t list) {
	for (int i = 0; i < list.len; i++)
		func(acc, (void *)(((char *)list.data) + i * list.elem_size));
	return acc;
}

void for_each_multiple(void(*func)(void **), int varg_c, ...) {
	int bec = 0;
	va_list args;
	for (int i = 0; bec == 0 && i < 100; i++) {
		va_start(args, varg_c);
		void **operation = malloc(sizeof(void *) * varg_c);
		for (int j = 0; j < varg_c; j++) {
			array_t temp = va_arg(args, array_t);
			if (temp.len == i) {
				bec = 1;
				break;
			}
			temp.data = (void *)((char *)temp.data + i * temp.elem_size);
			operation[j] = temp.data;
		}
		va_end(args);
		if (bec == 1) {
			free(operation);
			break;
		}
		func(operation);
		free(operation);
	}
}

array_t map_multiple(void (*func)(void *, void **),
					 int new_list_elem_size,
					 void (*new_list_destructor)(void *),
					 int varg_c, ...) {
	int bec = 0;
	int length = 0;
	va_list args;
	array_t result;
	result.data = malloc(new_list_elem_size * 100);
	void *aux = result.data;
	result.destructor = new_list_destructor;
	result.elem_size = new_list_elem_size;
	for (int i = 0; i < 100 && bec == 0; i++) {
		va_start(args, varg_c);
		void **operation = malloc(sizeof(void *) * varg_c);
		result.data = (char *)aux + i * result.elem_size;
		for (int j = 0; j < varg_c; j++) {
			array_t temp = va_arg(args, array_t);
			if (temp.len == i) {
				bec = 1;
				length = i;
				break;
			}
			temp.data = (void *)((char *)temp.data + i * temp.elem_size);
			operation[j] = temp.data;
			// temp.destructor(temp.data);
		}
		va_end(args);
		if (bec == 1) {
			free(operation);
			break;
		}
		func(result.data, operation);
		free(operation);
		// va_end(args);
	}
	result.data = aux;
	result.len = length;
	va_start(args, varg_c);
	for (int i = 0; i < varg_c; i++) {
		array_t temp = va_arg(args, array_t);
		if (temp.destructor)
			for (int j = 0; j < temp.len; j++)
				temp.destructor((void *)(((char *)temp.data) +
				j * temp.elem_size));
		free(temp.data);
	}
	va_end(args);
	return result;
}

void *reduce_multiple(void(*func)(void *, void **),
					  void *acc, int varg_c, ...) {
	int bec = 0;
	va_list args;
	for (int i = 0; bec == 0 && i < 100; i++) {
		va_start(args, varg_c);
		void **operation = malloc(sizeof(void *) * varg_c);
		for (int j = 0; j < varg_c; j++) {
			array_t temp = va_arg(args, array_t);
			if (temp.len == i) {
				bec = 1;
				break;
			}
			temp.data = (void *)((char *)temp.data + i * temp.elem_size);
			operation[j] = temp.data;
		}
		va_end(args);
		if (bec == 1) {
			free(operation);
			break;
		}
		func(acc, operation);
		free(operation);
	}
	return acc;
}
