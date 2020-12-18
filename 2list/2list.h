#ifndef LIST_H
#define LIST_H

#include "2list_node.h"
#include <cstdio>

int cmp_name(list_node& a, list_node& b);
int cmp_phone(list_node& a, list_node& b);
int cmp_group(list_node& a, list_node& b);

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

    void merge(list_node *a, list_node *b, list_node **head, list_node **tail, int (*cmp)(list_node&, list_node&));
    void sort_neyman(list_node *head, int (*cmp)(list_node&, list_node&));

    void delete_list();
    void delete_element(list_node *a);
    void print_list(int fd);
    int list_init(FILE *fp);
};

#endif

