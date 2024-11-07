#include "init.h"
#include "ui.h"
#include "error.h"


int main() {

    int ret;
    prog_data pd;

    new_prog_data(&pd);
    ret = init_prog(&pd);
    if (ret == -1) {
        get_error();
        return -1;
    }

    start_ui(&pd);

    ret = fini_prog(&pd);
    if (ret == -1) {
        get_error();
        return -1;
    }

    return 0;
}
