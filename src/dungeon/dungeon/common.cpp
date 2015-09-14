#include "common.h"
#include <stdlib.h>

void common_merge_sort(void *what, int number_elements, int size_of_element, int (*compare)(const void *a, const void *b), void *buffer){
	int offset = (number_elements >> 1) + (number_elements & 1), i = 0, j = 0, k = 0;
	void *c = what, *d = (char*)what + offset * size_of_element, *e = buffer;
	if(number_elements <= 1)
		return;
	common_merge_sort(c, offset, size_of_element, compare, buffer);
	common_merge_sort(d, number_elements - offset, size_of_element, compare, buffer);
	while(k != number_elements){
		if(i != offset && (j == number_elements - offset || compare(c, d) < 0)){
			memcpy(e, c, size_of_element);
			c = (char*)c + size_of_element;
			++i;
		}else{
			memcpy(e, d, size_of_element);
			d = (char*)d + size_of_element;
			++j;
		}
		e = (char*)e + size_of_element;
		++k;
	}
	memcpy(what, buffer, size_of_element * number_elements);
}