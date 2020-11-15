#ifndef gtree_NODE_H
#define gtree_NODE_H

#define G_NODE_SIZE 10
#define G_NODE_POWER 5

#include "../avl/avl.h"
#include "../btree/btree.h"

struct node
{
    int group = 0;
    int size = 0;
    avl_tree tname;
    btree    tphone;
    node(){tname.type = 1; tphone.treetype = 1;}
    ~node() = default;
    void insert(list_node *key)
    {
        size++;
        if(!group) group = key->group;
        if(group != key->group) printf("[!]Gtree wrong insert dest\n");
        tname.avl_insert_key(key);
        tphone.btree_insert(key);
    }
    int remove(list_node *key)
    {
        if(!key){printf("[!]Gtree: removing null\n"); return 0;}
        if(group != key->group) printf("[!]Gtree wrong delete dest\n");
        size--;
        tname.avl_delete_key(nullptr, tname.root, key);
        if(tphone.delete_key(key)) printf("[!]Tphone delete error\n");
        return size;
    }
    void print()
    {
        tphone.print_proc(stdout, tphone.root, 0, 0);
    }
    friend class gtree;
    friend class gtree_node;
};

class gtree_node
{
private:
    node       *data     [NODE_SIZE - 1] = {nullptr};
    gtree_node *child    [NODE_SIZE]     = {nullptr};

    friend class gtree;
public:
    ~gtree_node(){delete_node();}
    gtree_node() = default;
private:
    void delete_node()
    {
        for(int i = 0; i < NODE_SIZE - 1; i++)
        {
            if(data[i]) delete data[i];
            else break;
        }
    }
    int insert_key(list_node *key);
    int insert_node(node *key);
    void insert_child(gtree_node *newchild, int i);
    void print_node(FILE *ostream);
    int size()
    {
        int count = 0;
        for(int i = 0; i < NODE_SIZE - 1; i++) if(data[i]) count++;
        return count;
    }
    int get_empty()
    {
        for(int i = 0; i < NODE_SIZE - 1; i++) if(!data[i]) return i;
        return -1;
    }
    void data_shiftright()
    {
        for(int i = NODE_SIZE - 2; i > 0; i--)
            data[i] = data[i - 1];
        data[0] = nullptr;
    }
    void child_shiftright()
    {
        for(int i = NODE_SIZE - 1; i > 0; i--)
            child[i] = child[i - 1];
        child[0] = nullptr;
    }
    void data_shiftleft(int start_index)
    {
        for(int i = start_index; i < NODE_SIZE - 2; i++)
            data[i] = data[i + 1];
        data[NODE_SIZE - 2] = nullptr;
    }
    void child_shiftleft(int start_index)
    {
        for(int i = start_index; i < NODE_SIZE - 1; i++)
            child[i] = child[i + 1];
        child[NODE_SIZE - 1] = nullptr;
    }
};


#endif
