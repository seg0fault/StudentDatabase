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

void list::print_list(int fd)
{
    list_node *current = head;
    while(current){current->print_client(fd); current = current->next;}
}

void list::merge(list_node *a, list_node *b, list_node **head, list_node **tail, int (*cmp)(slist_node&, slist_node&))
{
        list_node *newhead = nullptr, *newtail = nullptr;
        if(cmp(*a, *b)){newtail = newhead = a; a = a->next_selected;} //определяем новый старт
        else{newtail = newhead = b; b = b->next_selected;}
        while(a && b) //проводим слияние
        {
                if(cmp(*a, *b)){newtail = newtail->next_selected = a; a = a->next_selected;}
                else{newtail = newtail->next_selected = b; b = b->next_selected;}
        }
        if(!a) newtail->next_selected = b;
        else newtail->next_selected = a;
        while(newtail->next_selected) newtail = newtail->next_selected;
        *head = newhead; *tail = newtail;
}

void list::sort_neyman(list_node *head, int (*cmp)(list_node&, list_node&))
{
    list_node *b_head, *newend, *start, *newstart, *startprev, *current;
    int i, step = 1;
    b_head = newend = start = newstart = startprev = nullptr;
    if(!head) return;
    for(step = 1; true; step *= 2)
        {
                current = head;
                for(i = 1; i < step && current; i++) current = current->next_selected;
                if(!current || !current->next_selected) return;
                b_head = current->next_selected;
                current->next_selected = nullptr;
                current = b_head;
                for(i = 1; i < step && current; i++) current = current->next_selected;
                if(current){start = current->next_selected; current->next_selected = nullptr;}
                merge(head, b_head,  &newstart, &newend, cmp);
                head = newstart;
                if(!current) return;
                startprev = newend;
                newend->next_selected = current = start;
                while(true)
                {
                        for(i = 1; i < step && current; i++) current = current->next_selected;
                        if(!current || !current->next_selected) break;
                        b_head = current->next_selected;
                        current->next_selected = nullptr;
                        current = b_head;
                        for(i = 1; i < step && current; i++) current = current->next_selected;
                        if(current) {start = current->next_selected; current->next_selected = nullptr;}
                        merge(startprev->next_selected, b_head,  &newstart, &newend, cmp);
                        startprev->next_selected = newstart;
                        if(!current) break;
                        startprev = newend;
                        newend->next_selected = current = start;
                }
        }
}

int cmp_name(list_node& a, list_node& b)
{
    if(strcmp(a.name, b.name) <= 0) return 1;
    else return 0;
}

int cmp_phone(list_node& a, list_node& b)
{
    return a.phone <= b.phone;
}

int cmp_group(list_node& a, list_node& b)
{
    return a.group <= b.group;
}






