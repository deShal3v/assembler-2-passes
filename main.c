/*Mmn 14 , course 20465 , The Open University of Israel
 *author Omer Shalev , ID 325******
 * 2019B
*/

#include "moduleanalyzing.h"
#include <stdio.h>
#include <stdlib.h>



/*Main function of the program */
int main(int argc, char* argv[]) {

    if (argc < 2) { /*Havn't got command line arguments */

        fprintf(stderr, "%s", "Aborting! No source files were specified\nHelp: main <source_1.as> <source2.as> .... <source_n.as>\n");
		exit(1);
	}
    /*starting the assemblying process */
    processInputFIles(argv , argc);
    return 1;
}





