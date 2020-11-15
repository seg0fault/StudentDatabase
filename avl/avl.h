#ifndef AVL_H
#define AVL_H

//#include "../Database/database.h"
#include "avl_node.h"
#include "../Command/enum.h"
#include "../Command/command.h"

#include <cstdio>
#define AVL_MAX_PRINT 4
#define MAX_REC 100

class database;

int like(char *left, char *right);

class avl_tree
{
private:
    avl_node *root = nullptr;
    int rec_count = 0;
    friend class database;
    friend struct node;
    database* parent_database;
    char *search_key = nullptr;

    int type = 0; //лежит ли оно в дереве по группе
public:
    avl_tree() = default;
    ~avl_tree(){avl_delete_all(); root = nullptr;}
    int avl_delete_tree(avl_node *start);
    void avl_delete_all();

    void avl_print(FILE *ostream);
    int avl_print_proc(avl_node *start, FILE *ostream, int level, int pos);

    int avl_init(list_node *head);
    int avl_insert_key(list_node *key);
    int avl_insert_key_proc(avl_node *parent, avl_node *start, list_node *key);
    avl_node *avl_delete_node(avl_node *parent, avl_node *node);
    int avl_delete_key(avl_node *parent, avl_node *start, list_node *key);
    avl_node* avl_extract_min(avl_node *parent, avl_node *start);

    list_node* avl_search(char *key);
    avl_node*  avl_search_proc(avl_node *start, char *key);
    avl_node*  avl_search_parent(avl_node *ded, avl_node *parent, avl_node *start, avl_node **out_parent, avl_node **out_ded, char *key);
    avl_node*  rebalance(avl_node *parent, avl_node *start);
    avl_node*  avl_left_turn(avl_node *start);
    avl_node*  avl_right_turn(avl_node *start);

    int avl_sanitizer(avl_node *start);

    void avl_select(char *key, cond_type cnd);
    void avl_get_parent_database(database* parent){parent_database = parent;}
    void avl_select_node(avl_node *node);
    void avl_select_all(avl_node *start);
    void avl_select_eq(avl_node *start);
    void avl_select_ne(avl_node *start);
    void avl_select_gt(avl_node *start);
    void avl_select_ge(avl_node *start);
    void avl_select_lt(avl_node *start);
    void avl_select_le(avl_node *start);
    void avl_select_like(avl_node *start);
};



#endif
