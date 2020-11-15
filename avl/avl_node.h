#ifndef AVL_NODE_H
#define AVL_NODE_H

#include "../2list/2list_node.h"

class avl_node
{
private:
    list_node *key    = nullptr;
    avl_node  *left   = nullptr;
    avl_node  *right  = nullptr;
    int        height       = 1;
    int        size         = 0;
    friend class avl_tree;
public:
    avl_node() = default;
    ~avl_node() = default;
    void print(FILE *ostream);
    void print_detailed(FILE *ostream);
    int balance();
    void add(list_node *next, int type);
    int remove(list_node *trash, int type);
    int get_left_height() {if(left)  return left->height;  else return 0;}
    int get_right_height(){if(right) return right->height; else return 0;}
    void fix_height();
};


#endif
