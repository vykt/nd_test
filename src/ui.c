#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "ui.h"
#include "init.h"
#include "book.h"
#include "tree.h"
#include "list.h"
#include "error.h"


/*
 *
 *  No time for input validation; writing UI in C is questionable anyway.
 *
 */


/*
 *  --- INTERNAL ---
 */

static void _clear_input() {

    char c;

    while ((c = getchar()) != '\n' && c != EOF) {
        //discard
    }

    return;
}



static void _get_string(char * buf, const char * prompt, const size_t len) {

    printf("[size: %lu] ", len);
    printf(prompt); //not being a literal is ok here
    fflush(stdout);
    fgets(buf, len, stdin);
    _clear_input();

    return;
}



static void _get_number(uint64_t * num, const char * prompt) {

    char buf[sizeof(*num)];

    printf("[size: %lu] ", sizeof(*num));
    printf(prompt); //not being a literal is ok here
    fflush(stdout);
    fgets(buf, sizeof(*num), stdin);
    *num = atoll(buf);

    return;
}


static void _get_book_entry(book_entry * be) {

    _get_string(be->name, "name > ", NAME_LEN);
    _get_string(be->other_name, "other_name > ", NAME_LEN);
    _get_string(be->email, "email > ", EMAIL_LEN);
    _get_number(&be->phone, "phone (no extensions) > ");
    _get_string(be->addr.street, "street > ", ADDR_LEN);
    _get_string(be->addr.town, "town > ", ADDR_LEN);
    _get_string(be->addr.country, "country > ", ADDR_LEN);

    return;
}


static void _get_view_rm(query * q) {

    uint64_t type;

    puts("types:\n\n");
    
    puts("name:       > 0\n");
    puts("other_name: > 1\n");
    puts("email:      > 2\n");
    puts("phone:      > 3\n");
    puts("town:       > 4\n");
    puts("country:    > 5\n");

/*
 *  I'm aware Djikstra published the anti-goto paper in the 60s, which is why languages today, except for C,
 *  dont support goto statements. I know it results in unmanageable control flow. However I am too tired to 
 *  think about what this looks like as a loop. Just ASM jump. :>
 */

_get_view_rm_again:
    _get_number(&type, "number from menu > ");
    if (type > 5) goto _get_view_rm_again;

    switch (type) {

        case 0:
            _get_string(q->keys.name, "name (key) > ", NAME_LEN);
            break;
        
        case 1:
            _get_string(q->keys.other_name, "other name (key) > ", NAME_LEN);
            break;

        case 2:
            _get_string(q->keys.email, "email (key) > ", EMAIL_LEN);
            break;

        case 3:
            _get_number(&q->keys.phone, "phone (key) > ");
            break;
        
        case 4:
            _get_string(q->keys.addr.town, "town (key) > ", ADDR_LEN);
            break;

        case 5:
            _get_string(q->keys.addr.country, "country (key) > ", ADDR_LEN);
            break;
    }

    return;
}



//TODO add book entry to prog_data



#define ADD  0
#define VIEW 1
#define RM   2
#define EXIT 3

static int _get_menu_opt() {

    char buf[16];

_get_menu_opt_again:
    fgets(buf, 16, stdin);
    _clear_input();

    if (!strncmp(buf, "add", 16))  return ADD;
    if (!strncmp(buf, "view", 16)) return VIEW;
    if (!strncmp(buf, "rm", 16))   return RM;
    if (!strncmp(buf, "exit", 16)) return EXIT;

    goto _get_menu_opt_again;
}



static void _print_menu() {

    puts("use: \n\n");
    
    puts("> add\n");
    puts("> view\n");
    puts("> rm\n");
    puts("> exit\n\n");

    return;
}



/*
 *  --- EXTERNAL ---
 */

void start_ui(prog_data * pd) {

    int ret;
    int action;

    book_entry be, * be_ptr;
    query q;

    _print_menu();

    while (1) {

        action = _get_menu_opt();

        switch(action) {

            case ADD:
                _get_book_entry(&be);
                ret = add_book_entry(pd, &be);
                if (ret == -1) {
                    get_error();
                    return;
                }

                break;

            case VIEW:
                _get_view_rm(&q);
                be_ptr = get_book_entry(pd, &q);
                if (ret == -1) {
                    get_error();
                    return;
                }

                //TODO print contents of book entry be_ptr

                break;

            case RM:
                _get_view_rm(&q);
                ret = del_book_entry(pd, &q);
                if (ret == -1) {
                    get_error();
                    return;
                }
                break;

            case EXIT:
                return;

        } //end switch

    } //end while
}
