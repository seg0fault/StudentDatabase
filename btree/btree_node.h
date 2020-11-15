#ifndef BTREE_NODE_H
#define BTREE_NODE_H

#define NODE_SIZE 10
#define NODE_POWER 5

#include "../2list/2list_node.h"

class btree_node
{
private:
    list_node  *data     [NODE_SIZE - 1] = {nullptr};
    btree_node *child    [NODE_SIZE]     = {nullptr};

    friend class btree;
public:
    ~btree_node(){}
    btree_node() = default;
private:
    int insert_key(list_node *key, int type);
    int remove_key(list_node *key, int index, int type);
    void insert_child(btree_node *newchild, int i);
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
