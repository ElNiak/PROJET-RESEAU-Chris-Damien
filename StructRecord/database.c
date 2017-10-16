#include <stdlib.h>
#include <stdio.h>


#include "macros.h"
#include "record.h"

int main(int argc, const char **argv)
{
	
	unsigned char *step=NULL;
	struct record *r=record_init();
	set_Payload(r,step,5);
	printf("%i\n",get_Length(r));
	record_free(r);
	free(step);	
	printf("%i,%s\n", argc, argv[0]);

	return EXIT_SUCCESS;
}
