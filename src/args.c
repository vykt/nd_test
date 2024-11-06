#include <stdlib.h>

#include <unistd.h>
#include <getopt.h>


int process_args(int argc, char ** argv) {

    //defined cmdline options
    struct option long_opts[] = {
        {"output-file", required_argument, NULL, 'a'},
        {"ui-term", no_argument, NULL, 'b'},
        {0,0,0,0}
    };

    int opt, opt_index;

    //iterate over supplied flags
    while(((opt = getopt_long(argc, argv,"a:b", 
           long_opts, &opt_index)) != -1) && (opt != 0)) {

        //determine parsed argument
        switch (opt) {

            case 'a': //output file
                break;

            case 'b': //input file
                break;

        } //end switch

    } //end opt while loop

    return 0;
}
