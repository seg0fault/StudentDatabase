#include "2list.h"

int list::list_init(FILE *fp)
{
    int ret;
    delete_list();
    list_node *next, *current;
    next = new list_node;
    if(!next) return -1;
    ret = next->record_init_file(fp);
    if(ret < 0){delete next; return -2;}
    if(ret > 0){delete next; return -1;} //file end
    head = current = next;
    for (;;)
    {
        next = new list_node;
        if(!next) return 2;
        ret = next -> record_init_file(fp);
        if(ret < 0){delete_list(); return -1;} //read err
        if(ret > 0){delete next; tail = current; return 0;} //file end
        current -> next = next;
        next -> prev = current;
        current = next;
        next = nullptr;
    }
    return 0;
}

void list::delete_list()
{
    list_node *current;
    while(head)
    {
        current = head->next;
        delete head;
        head = current;
    }
    head = tail = nullptr;
}

void list::delete_element(list_node *a)
{
    if(a->prev) a->prev->next = a->next;
    else{head = a->next; if(head) head->prev = nullptr;}
    if(a->next) a->next->prev = a->prev;
    else{tail = a->prev; if(tail) tail->next = nullptr;}
    delete a;
}

void list::print_list(FILE *stream)
{
    list_node *current = head;
//    fprintf(stream, "|        NAME |     PHONE |GROUP|\n");
    while(current){current->print(stream); current = current->next;}
}
