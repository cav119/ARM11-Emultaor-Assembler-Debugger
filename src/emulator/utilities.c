#include <stdio.h>
#include <stdlib.h>

#include "utilities.h"


void print_error_exit(char error_msg[]) {
	printf("%s\n", error_msg);
	exit(EXIT_FAILURE);
}

void check_ptr_not_null(void *ptr, char error_msg[]) {
	if (!ptr) {
		print_error_exit(error_msg);
	}
}
