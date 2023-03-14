#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
	int rc;

    int stdin2 = dup(1); /// duplica el fd (Se tienen que cerrar los dos.)

	//close(1); 
	rc = write(1, "asdasd\n", 7); /// si hay error (fd cerrado) entonces  rc = 1.
	fprintf(stderr, "rc=%i\n", rc);
	exit(0);
}