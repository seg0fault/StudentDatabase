#ifndef LIST_H
#define LIST_H

#include "2list_node.h"
#include <cstdio>


class list
{
private:
    list_node *head = nullptr;
    list_node *tail = nullptr;

    friend class database;
public:
    list() = default;
    ~list(){delete_list(); head = tail = nullptr;}

    list_node* get_head(){return head;}

    void delete_list();
    void delete_element(list_node *a);
    void print_list(FILE *stream);
    int list_init(FILE *fp);
};

#endif

