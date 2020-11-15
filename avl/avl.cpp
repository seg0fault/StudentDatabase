#include "avl.h"

void avl_node::print(FILE *ostream)
{
    fprintf(ostream, "[%s x%d]", key->name, size);
    fprintf(ostream, "[%d]", height);
}

void avl_node::print_detailed(FILE *ostream)
{
    fprintf(ostream, "[AVL NODE]\n");
    for(list_node *it = key; it; it = it->next_eq_name) it->print(ostream);
    fprintf(ostream, "Height = %d; Size = %d; Balance = %d\n", height, size, balance());
    if(left)
    {
        fprintf(ostream, "[LEFT]\n");
        for(list_node *it = left->key; it; it = it->next_eq_name) it->print(ostream);
        fprintf(ostream, "Height = %d; Size = %d; Balance = %d\n", left->height, left->size, left->balance());
    }
    if(right)
    {
        fprintf(ostream, "[RIGHT]\n");
        for(list_node *it = right->key; it; it = it->next_eq_name) it->print(ostream);
        fprintf(ostream, "Height = %d; Size = %d; Balance = %d\n", right->height, right->size, right->balance());
    }
}

void avl_node::add(list_node *next, int type)
{
    size++;
    if(!key){key = next; return;}
    list_node *it = key;
    if(!type)
    {
        for(; it->next_eq_name; it = it->next_eq_name);
        it->next_eq_name = next;
    }
    else //ПРОБЛЕМА ПРОБЛЕМА ПРОБЛЕМА ПРОБЛЕМА ПРОБЛЕМА
    {
        for(; it->next_eq_name_group; it = it->next_eq_name_group);
        it->next_eq_name_group = next;
    }
}

int avl_node::remove(list_node *trash, int type)
{
    list_node *it, *prev;
//    printf("Removing from avl:\n");
//    trash->print(stdout);
//    print_detailed(stdout);
    size--;
    if(size == 0) {key = nullptr; return size;}
    if(!type)
    {
        if(trash == key)
        {
            key = key->next_eq_name;
            if(!key) printf("[!]AVL REMOVE ERROR\n");
            return size;
        }
        for(it = key->next_eq_name, prev = key; it; it = it->next_eq_name)
        {
            if(it == trash)
            {
                prev->next_eq_name = it->next_eq_name;
                return size;
            }
            prev = it;
        }
    }
    else
    {
        if(trash == key)
        {
            key = key->next_eq_name_group;
            if(!key) printf("[!]AVL REMOVE ERROR\n");
            return size;
        }
        for(it = key->next_eq_name_group, prev = key; it; it = it->next_eq_name_group)
        {
            if(it == trash)
            {
                prev->next_eq_name_group = it->next_eq_name_group;
                return size;
            }
            prev = it;
        }
    }
    printf("[!]Avl remove reached end of node!\n");
    return 0;
}

int avl_node::balance()
{
    if(!left && !right) return 0;
    if(!left)  return right->height;
    if(!right) return -(left->height);
    return right->height - left->height;
}

void avl_node::fix_height()
{
    int hleft = get_left_height();
    int hright = get_right_height();
    height = (hleft > hright ? hleft : hright) + 1;
}

void avl_tree::avl_delete_all()
{
    if(!root) return;
    avl_delete_tree(root);
}

int avl_tree::avl_delete_tree(avl_node *start)
{
    if(!start) return 0;
    avl_delete_tree(start->left);
    avl_delete_tree(start->right);
    delete start;
    return 0;
}

int avl_tree::avl_init(list_node *head)
{
    for(; head; head = head->next)
        if(avl_insert_key(head)) return 1;
    return 0;
}


int avl_tree::avl_insert_key(list_node *key)
{
    if(!key) return 1;
    if(!root)
    {
        root = new avl_node;
        if(!root) return 2;
        root->add(key, type);
        return 0;
    }
    rec_count = 1;
    return avl_insert_key_proc(nullptr, root, key);
}

int avl_tree::avl_insert_key_proc(avl_node* parent, avl_node* start, list_node* key)
{
//    printf("Start = "); start->print(stdout); printf(" key = %s\n", key->name);
    rec_count++;
    if(rec_count > MAX_REC){printf("[!]Deep recursion!\n[!]Position: %s\n[!]Tree dumped:\n", start->key->name); avl_print(stdout); return 1;}
    if(!start->key){printf("[!]AVL using previously deleted memory!\n"); return 1;}
    int res = strcmp(key->name, start->key->name);
    if(res < 0)
    {
        if(start->left){if(avl_insert_key_proc(start, start->left, key)) return 1;}
        else
        {
            start->left = new avl_node;
            if(!start->left) return 1;
            start->left->add(key, type);
        }
    }
    else if(res == 0) {start->add(key, type); return 0;}
    else
    {
        if(start->right){if(avl_insert_key_proc(start, start->right, key)) return 1;}
        else
        {
            start->right = new avl_node;
            if(!start->right) return 1;
            start->right->add(key, type);
        }
    }
    if(!rebalance(parent, start)) {printf("[!]Balancing node: %s\n[!]Adding key: %s\n[!]Tree dumped:\n",start->key->name, key->name); avl_print(stdout); return 1;}
//    avl_print(stdout);
//    printf("\n");
    return 0;
}

avl_node* avl_tree::rebalance(avl_node* parent, avl_node* node)
{
    /*
     * Функция получает на вход узел авл-дерева и выполняет его балансировку
     * автоматически определяя необходимые процедуры поворота
     */
    avl_node *new_root = nullptr;
    node->fix_height();
    int balance = node->balance();
    if(balance == -2 )
    {
        if(node->left->balance()  > 0)  //большой поворот получается добавлением малого
        {
            avl_node *new_left = avl_left_turn(node->left);
            if(!new_left){printf("[!]Rebalance exception! No new_left\n"); return nullptr;}
            node->left = new_left;
        }
        new_root = avl_right_turn(node);    //малый поворот
        if(!new_root) {printf("[!]Rebalance exception! No new_root in -2\n"); return nullptr;}
    }
    else if(balance ==  2)
    {
        if(node->right->balance() < 0)  //большой поворот получается добавлением малого
        {
            avl_node *new_right = avl_right_turn(node->right);
            if(!new_right){printf("[!]Rebalance exception! No new_right\n"); return nullptr;}
            node->right = new_right;
        }
        new_root = avl_left_turn(node);     //малый поворот
        if(!new_root){printf("[!]Rebalance exception! No new_root in +2\n"); return nullptr;}
    }
    else if(balance > 2 || balance < -2)
    {
        printf("[!]AVL-Tree structure error! Cannot rebalance\nContext:\n");
        avl_print_proc(node, stdout, 0,0);
//        node->print_detailed(stdout);
        return nullptr;
    }
    if(new_root) //чиним родителя
    {
        if(node == root){root = new_root; return root;}
        if(parent->left == node) parent->left = new_root;
        else parent->right = new_root;
        return new_root;
    }
    return node; // балансировка не нужна
}


avl_node* avl_tree::avl_search_proc(avl_node *start, char *key)
{
    if(!start) return nullptr;
    int res = strcmp(key, start->key->name);
    if(res < 0) return avl_search_proc(start->left, key);
    else if(res == 0) return start;
    else return avl_search_proc(start->right, key);
}

avl_node* avl_tree::avl_search_parent(avl_node* ded, avl_node *parent, avl_node *start, avl_node **out_parent, avl_node **out_ded, char *key)
{
    if(!start) return nullptr;
    int res = strcmp(key, start->key->name);
    if(res < 0) return avl_search_parent(parent, start, start->left, out_parent, out_ded, key);
    else if(res == 0){*out_ded = ded, *out_parent = parent; return start;}
    else return avl_search_parent(parent, start, start->right, out_parent, out_ded, key);
}

list_node* avl_tree::avl_search(char *key)
{
    avl_node *res = avl_search_proc(root, key);
    if(res) return res->key;
    return nullptr;
}

avl_node* avl_tree::avl_right_turn(avl_node *node)
{
    avl_node* left_child = node->left;
    if(!left_child) {printf("[!]No left child!\n"); return nullptr;}
    node->left = left_child->right;
    left_child->right = node;
    node->fix_height();
    left_child->fix_height();
    return left_child;
}

avl_node* avl_tree::avl_left_turn(avl_node *node)
{
    avl_node* right_child = node->right;
    if(!right_child) {printf("[!]No right child!\n"); return nullptr;}
    node->right = right_child->left;
    right_child->left = node;
    node->fix_height();
    right_child->fix_height();
    return right_child;
}


int avl_tree::avl_delete_key(avl_node *parent, avl_node *start, list_node *key)
{
    if(!start){printf("[!]AVL node not found!\n"); return 1;}
    int res = strcmp(start->key->name, key->name);
    if(res > 0) avl_delete_key(start, start->left, key);
    else if(res < 0) avl_delete_key(start, start->right, key);
    else
    {
        if(start->remove(key, type)) return 0;
        avl_node *new_start;
        if((new_start = avl_delete_node(parent, start))) rebalance(parent, new_start);
        return 0;
    }
    if(!rebalance(parent, start)){printf("[!]Rebalance error 1: end rebalance incorrect\n"); return 1;}
    return 0;
}

avl_node* avl_tree::avl_delete_node(avl_node *parent, avl_node *node)
{
    avl_node **dest;

    if(node == root) dest = &root;
    else if(parent->left == node) dest = &parent->left;
    else dest = &parent->right;

    if(node->right)
    {
        *dest = avl_extract_min(node, node->right);
        (*dest)->left = node->left;
        if(*dest == node->right) (*dest)->right = nullptr;
        else (*dest)->right = node->right;
    }
    else *dest = node->left; //нет правого потомка => левый потомок если есть то лист, поэтому можно завершать

    delete node;
    return *dest;
}

avl_node* avl_tree::avl_extract_min(avl_node *parent, avl_node *start)
{
    //Может вернуть вершину, которую в нее дали изначально если у той нет левого потомка!
    if(start->left){avl_node *res = avl_extract_min(start, start->left); if(!rebalance(parent, start)) printf("[!]Rebalance error 3: extractor rebalance error\n"); return res;}
    if(parent->left == start)
    {
        parent->left = start->right;
        if(parent->left) rebalance(parent, parent->left);
        if(!start) printf("[!]AVL extract min tried to return 0\n");
        return start;
    }
    else
    {
        parent->right = start->right;
        if(parent->right) rebalance(parent, parent->right);
        if(!start) printf("[!]AVL extract min tried to return 0\n");
        return start;
    }
    return start;
}

void avl_tree::avl_print(FILE *ostream)
{
    rec_count = 0;
    if(root) avl_print_proc(root, ostream, 0, 0);
    else fprintf(ostream, "avl-tree empty\n");
}

int avl_tree::avl_print_proc(avl_node *start, FILE *ostream, int level, int pos)
{
    rec_count++;
    if(rec_count > MAX_REC){printf("[!]AVL: Too deep recursion in print\n"); return 1;}
    if(level > AVL_MAX_PRINT) return 0;
    switch(pos)
    {
    case 0: fprintf(ostream, "[P]"); break;
    case 1: fprintf(ostream, "(L)"); break;
    case 2: fprintf(ostream, "(R)"); break;
    default: fprintf(ostream, "(=))");
    }
    for(int i = 0; i < level; i++) fprintf(ostream, "--");
//    fprintf(ostream, "(%d)", level);
    start->print(ostream); fprintf(ostream, "\n");
    if(start->left) if(avl_print_proc(start->left, ostream, level + 1, 1)) return 1;
    rec_count--;
    if(start->right) if(avl_print_proc(start->right, ostream, level + 1, 2)) return 1;
    rec_count--;
    return 0;
}

int avl_tree::avl_sanitizer(avl_node *start)
{
    if(!start) return 0;
    if(rec_count > MAX_REC){printf("AVL_SANITIZER: Deep rec detected"); return 1;}
    if(start->balance() > 1 || start->balance() < -1)
    {
        printf("AVL_SANITIZER: Disbalance detected\nContext:\n");
        avl_print_proc(start, stdout, 0, 0);
        start->print_detailed(stdout);
        if(start->left)
        {
            printf("Left child:\n");
            start->left->print_detailed(stdout);
        }
        if(start->right)
        {
            printf("Right child:\n");
            start->right->print_detailed(stdout);
        }
        printf("\n");
        return 1;
    }
    if(start->left  && (start->height < start->left->height))
    {
        printf("AVL_SANITIZER: Cycle 1(l) detected\nContext:\n");
        avl_print_proc(start, stdout, 0, 0);
        start->print_detailed(stdout);
        printf("Child:\n");
        start->left->print_detailed(stdout);
        printf("\n");
        return 1;
    }

    if(start->right && (start->height < start->right->height))
    {
        printf("AVL_SANITIZER: Cycle 1(r) detected\nContext:\n");
        avl_print_proc(start, stdout, 0, 0);
        start->print_detailed(stdout);
        printf("Child:\n");
        start->right->print_detailed(stdout);
        printf("\n");
        return 1;
    }
/*
    if(start->height < start->left->left->height
            || start->height < start->left->right->height)
    {printf("AVL_SANITIZER: Cycle 2 detected\n"); return 1;}
    if(start->height < start->right->left->height
            || start->height < start->right->right->height)
    {printf("AVL_SANITIZER: Cycle 2 detected\n"); return 1;}
*/
    if(avl_sanitizer(start->right)) return 1;
    if(avl_sanitizer(start->left)) return 1;
    return 0;
}

void avl_tree::avl_select_all(avl_node *start)
{
    if(!start) return;
    avl_select_node(start);
    avl_select_all(start->right);
    avl_select_all(start->left);
}

void avl_tree::avl_select_eq(avl_node *start)
{
    if(!start) return;
    int res = strcmp(start->key->name, search_key);
    if(res < 0) avl_select_eq(start->right);
    else if(res > 0) avl_select_eq(start->left);
    else avl_select_node(start);
}

void avl_tree::avl_select_ne(avl_node *start)
{
    if(!start) return;
    int res = strcmp(start->key->name, search_key);
    if(res < 0)
    {
        avl_select_node(start);
        avl_select_all(start->left);
        avl_select_ne(start->right);
    }
    else if(res > 0)
    {
        avl_select_node(start);
        avl_select_all(start->right);
        avl_select_ne(start->left);
    }
    else
    {
        avl_select_all(start->left);
        avl_select_all(start->right);
    }
}

void avl_tree::avl_select_gt(avl_node *start)
{
    if(!start) return;
    int res = strcmp(start->key->name, search_key);
    if(res < 0) avl_select_gt(start->right);
    else if(res > 0)
    {
        avl_select_node(start);
        avl_select_all(start->right);
        avl_select_gt(start->left);
    }
    else avl_select_all(start->right);
}

void avl_tree::avl_select_ge(avl_node *start)
{
    if(!start) return;
    int res = strcmp(start->key->name, search_key);
    if(res < 0) avl_select_ge(start->right);
    else if(res > 0)
    {
        avl_select_node(start);
        avl_select_all(start->right);
        avl_select_ge(start->left);
    }
    else
    {
        avl_select_node(start);
        avl_select_all(start->right);
    }
}

void avl_tree::avl_select_lt(avl_node *start)
{
    if(!start) return;
    int res = strcmp(start->key->name, search_key);
    if(res < 0)
    {
        avl_select_node(start);
        avl_select_all(start->left);
        avl_select_lt(start->right);
    }
    else if(res > 0)
    {
        avl_select_lt(start->left);
    }
    else
    {
        avl_select_all(start->left);
    }
}

void avl_tree::avl_select_le(avl_node *start)
{
    if(!start) return;
    int res = strcmp(start->key->name, search_key);
    if(res < 0)
    {
        avl_select_node(start);
        avl_select_all(start->left);
        avl_select_le(start->right);
    }
    else if(res > 0)
    {
        avl_select_le(start->left);
    }
    else
    {
        avl_select_node(start);
        avl_select_all(start->left);
    }
}

void avl_tree::avl_select_like(avl_node *start)
{
    if(!start) return;
    int res = like(start->key->name, search_key);
    if(res) avl_select_node(start);
    avl_select_like(start->left);
    avl_select_like(start->right);
}

void avl_tree::avl_select(char *key, cond_type cnd)
{
    search_key = key;
    switch(cnd)
    {
    case COND_NONE: return;
    case COND_EQ: avl_select_eq(root); break;
    case COND_NE: avl_select_ne(root); break;
    case COND_GE: avl_select_ge(root); break;
    case COND_GT: avl_select_gt(root); break;
    case COND_LE: avl_select_le(root); break;
    case COND_LT: avl_select_lt(root); break;
    case COND_LIKE: avl_select_like(root); break;
    }
}

int like(char *left, char *right)
{
    int i;
    for(i = 0; left[i] && right[i]; i++)
    {
        if(left[i] != right[i])
        {
            if(right[i] != '%') return 0;
            else return 1;
        }
    }
    if(left[i] || right[i]) return 0;
    else return 1;
}

