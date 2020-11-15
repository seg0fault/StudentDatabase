#ifndef BTREE_H
#define BTREE_H


#include "btree_node.h"
#include "../2list/2list.h"
#include "../Command/enum.h"
//#include "../Database/database.h"
#include <cstdio>

#define MAX_PRINT 5
#define INSERT_PATH_SIZE 30

class database;

class btree
{
private:
    btree_node *root = nullptr;

    btree_node *path[INSERT_PATH_SIZE] = {nullptr};
    int current_depth = -1;
    friend class database;
    friend struct node;
    friend class gtree;
    database* parent_database;
    int search_key = 0;

    int treetype = 0; //лежит ли оно в дереве по группе
public:
    btree()  = default;
    ~btree(){btree_delete_tree(root); root = nullptr;}
    void btree_delete_tree(btree_node *start);

    int btree_init_list(list_node *head);
    int btree_insert(list_node *key);
    int btree_insert_proc(btree_node *start, list_node *key);
    int btree_node_split();
    void print_proc(FILE *ostream, btree_node *start, unsigned level, unsigned index);

    list_node*  search_eq(btree_node *start, int phone);
    btree_node* search_eq_node(btree_node *start, int phone, int *index);

    int delete_eq(int phone);
    int delete_key(list_node *key);
    int rebalance_proc(btree_node *start, int index_for_node);
    btree_node *rebalance_node_up(btree_node *start, btree_node *parent, int type);
    void merge_brothers(btree_node *parent, btree_node *l_brother, btree_node *r_brother, int index, int type);
    int rebalance_node_down(btree_node *start, int index);

    void btree_select(int key, cond_type cnd);
    void btree_get_database(database* new_database){parent_database = new_database;}
    void btree_select_key(list_node *node);
    void btree_select_all(btree_node *start);
    void btree_select_eq(btree_node *start);
    void btree_select_ne(btree_node *start);
    void btree_select_gt(btree_node *start);
    void btree_select_ge(btree_node *start);
    void btree_select_lt(btree_node *start);
    void btree_select_le(btree_node *start);
};



#endif
