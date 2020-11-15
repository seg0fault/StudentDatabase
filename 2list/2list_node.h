#ifndef LIST_NODE
#define LIST_NODE

#include "../Record/record.h"


class list_node: public record
{
private:
    list_node *prev = nullptr;
    list_node *next = nullptr;
    //надстройка для select
    list_node *next_selected = nullptr;
    //надстройка для создания классов эквивалентости в структурах поиска
    list_node *next_eq_name  = nullptr;
    list_node *next_eq_phone = nullptr;
    list_node *next_eq_name_group  = nullptr;
    list_node *next_eq_phone_group = nullptr;

    friend class list;
    friend class btree;
    friend class btree_node;
    friend class database;
    friend class avl_tree;
    friend class avl_node;
    friend class gtree;
    friend class gtree_node;
public:
    list_node() = default;
    ~list_node() = default;
    void insert_eq_phone(list_node *newnext, int type);
};






#endif
