#ifndef gtree_H
#define gtree_H


#include "gtree_node.h"
#include "../2list/2list.h"
#include "../Command/enum.h"
//#include "../Database/database.h"
#include <cstdio>

#define G_INSERT_PATH_SIZE 30

class database;

class gtree
{
private:
    gtree_node *root = nullptr;

    gtree_node *path[G_INSERT_PATH_SIZE] = {nullptr};
    int current_depth = -1;
    friend class database;

    database* parent_database;
    int search_key = 0;

    char *name_key = nullptr;
    int phone_key = 0;
    cond_type second_cond = COND_NONE;
    int second = 0;
public:
    gtree()  = default;
    ~gtree(){gtree_delete_tree(root); root = nullptr;}
    void gtree_delete_tree(gtree_node *start);

    int gtree_init_list(list_node *head);
    int gtree_insert(list_node *key);
    int gtree_insert_proc(gtree_node *start, list_node *key);
    int gtree_node_split();
    void print_proc(FILE *ostream, gtree_node *start, unsigned level, unsigned index);

    list_node*  search_eq(gtree_node *start, int phone);
    gtree_node* search_eq_node(gtree_node *start, int phone, int *index);

    int delete_eq(int phone);
    int delete_key(list_node *key);
    int rebalance_proc(gtree_node *start, int index_for_node);
    gtree_node *rebalance_node_up(gtree_node *start, gtree_node *parent, int type);
    gtree_node* merge_brothers(gtree_node *parent, gtree_node *l_brother, gtree_node *r_brother, int index, int type);
    int rebalance_node_down(gtree_node *start, int index);

    void gtree_select(int key, command &com);
    void gtree_get_database(database* new_database){parent_database = new_database;}
    void gtree_select_key(node *select);
    void gtree_select_all(gtree_node *start);
    void gtree_select_eq(gtree_node *start);
    void gtree_select_ne(gtree_node *start);
    void gtree_select_gt(gtree_node *start);
    void gtree_select_ge(gtree_node *start);
    void gtree_select_lt(gtree_node *start);
    void gtree_select_le(gtree_node *start);
};



#endif
