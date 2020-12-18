#include "database.h"

int cmp_eq(int a, int b){return a == b;}
int cmp_ne(int a, int b){return a != b;}
int cmp_gt(int a, int b){return a > b;}
int cmp_ge(int a, int b){return a >= b;}
int cmp_lt(int a, int b){return a < b;}
int cmp_le(int a, int b){return a <= b;}

int strcmp_eq(char* a, char* b){return !strcmp(a, b);}
int strcmp_ne(char* a, char* b){return strcmp(a,b);}
int strcmp_gt(char* a, char* b){return (strcmp(a,b) > 0);}
int strcmp_ge(char* a, char* b){return (strcmp(a,b) >= 0);}
int strcmp_lt(char* a, char* b){return (strcmp(a,b) < 0);}
int strcmp_le(char* a, char* b){return (strcmp(a,b) <= 0);}

void avl_tree::avl_select_node(avl_node *node, int treetype)
{
    if(!node) return;
    if(!treetype)
    {
        for(list_node *it = node->key; it; it = it->next_eq_name)
            parent_database->add_to_selection(it);
    }
    else
    {
        for(list_node *it = node->key; it; it = it->next_eq_name_group)
            parent_database->add_to_selection(it);
    }
}


void btree::btree_select_key(list_node *node, int typetree)
{
    if(!node) return;
    if(!typetree)
    {
        for(list_node *it = node; it; it = it->next_eq_phone)
            parent_database->add_to_selection(it);
    }
    else
    {
        for(list_node *it = node; it; it = it->next_eq_phone_group)
            parent_database->add_to_selection(it);
    }
}

void gtree::gtree_select_key(node *select)
{
    if(!select) return;
    if(second == 1)
    {
        select->tname.avl_get_parent_database(parent_database);
        select->tname.avl_select(name_key, second_cond);
    }
    else if(second == 2)
    {
        select->tphone.btree_get_database(parent_database);
        select->tphone.btree_select(phone_key, second_cond);
    }
    else if(second == 0)
    {
        select->tphone.btree_get_database(parent_database);
        select->tphone.btree_select_all(select->tphone.root);
    }
}

int database::database_init(FILE *fp)
{
    int res, out = 0;
    double t;
    printf("-----------------\n[i]Reading data and preparing search\n");
    t = clock();
    if(! (res = data.list_init(fp)) ) printf("[+]List build OK          (time: %lf)\n", (clock() - t)/ CLOCKS_PER_SEC);
    else {printf("[!]Error creating List. Error code: %d\n", res);}
    out += res;
    t = clock();
    if(! (res = name_tree.avl_init(data.get_head()))) printf("[+]Name avl-tree build OK (time: %lf)\n", (clock() - t) / CLOCKS_PER_SEC);
    else {printf("[!]Error creating name avl-tree. Error code: %d\n", res);}
    out += res;
    t = clock();
    if(! (res = phone_tree.btree_init_list(data.get_head()))) printf("[+]Phone btree build OK   (time: %lf)\n", (clock() - t) / CLOCKS_PER_SEC);
    else {printf("[!]Error creating phone btree. Error code: %d\n", res);}
    out += res;
    t = clock();
    if(! (res = group_tree.gtree_init_list(data.get_head()))) printf("[+]Group btree build OK   (time: %lf)\n", (clock() - t) / CLOCKS_PER_SEC);
    else {printf("[!]Error creating group btree. Error code: %d\n", res);}
    out += res;
    name_tree.avl_get_parent_database(this);
    phone_tree.btree_get_database(this);
    group_tree.gtree_get_database(this);
    return out;
}

void database::get_sel_parameters(int position, int type, command &com)
{
    oper = com.op;
    other_statement_type = type;
    if(type == 1)
    {
        if(position == 2) other_name_key = com.statement2;
        else other_name_key = com.statement1;
        switch(com.c_name)
        {
        case COND_NONE: printf("[!]Parcer error!\n"); break;
        case COND_EQ: cmp_name = &strcmp_eq; break;
        case COND_NE: cmp_name = &strcmp_ne; break;
        case COND_GT: cmp_name = &strcmp_gt; break;
        case COND_GE: cmp_name = &strcmp_ge; break;
        case COND_LT: cmp_name = &strcmp_lt; break;
        case COND_LE: cmp_name = &strcmp_le; break;
        case COND_LIKE: cmp_name = &like; break;
        }
        return;
    }
    if(type == 2)
    {

        if(position == 2) sscanf(com.statement2, "%d", &other_phone_key);
        else sscanf(com.statement1, "%d", &other_phone_key);
        switch(com.c_phone)
        {
        case COND_NONE: printf("[!]Parcer error!\n"); break;
        case COND_EQ: cmp_phone = &cmp_eq; break;
        case COND_NE: cmp_phone = &cmp_ne; break;
        case COND_GT: cmp_phone = &cmp_gt; break;
        case COND_GE: cmp_phone = &cmp_ge; break;
        case COND_LT: cmp_phone = &cmp_lt; break;
        case COND_LE: cmp_phone = &cmp_le; break;
        case COND_LIKE: printf("[!]Like in phone\n"); break;
        }
        return;
    }
    if(type == 3)
    {

        if(position == 2) sscanf(com.statement2, "%d", &other_group_key);
        else sscanf(com.statement1, "%d", &other_group_key);
        switch(com.c_group)
        {
        case COND_NONE: printf("[!]Parcer error!\n"); break;
        case COND_EQ: cmp_group = &cmp_eq; break;
        case COND_NE: cmp_group = &cmp_ne; break;
        case COND_GT: cmp_group = &cmp_gt; break;
        case COND_GE: cmp_group = &cmp_ge; break;
        case COND_LT: cmp_group = &cmp_lt; break;
        case COND_LE: cmp_group = &cmp_le; break;
        case COND_LIKE: printf("[!]Like in group\n"); break;
        }
        return;
    }
}

void database::add_to_selection(list_node *sel)
{
    //Реализация функции ТРЕБУЕТ аккуратного задания необходимых (не всех) параметров для успешной работы
    if(oper == OP_AND) //супер-крутая техника позволяет проходить AND за один проход по дереву
    {
        switch(other_statement_type)
        {
        case 1: if(!cmp_name(sel->name,  other_name_key))  return; break;
        case 2: if(!cmp_phone(sel->phone, other_phone_key)) return; break;
        case 3: if(!cmp_group(sel->group, other_group_key)) return; break;
        default: break;
        }
    }
    else if(oper == OP_OR) //тут по факту работает xor чтобы не было пересечений
    {
        switch(other_statement_type)
        {
        case 1: if(cmp_name(sel->name,  other_name_key))  return; break;
        case 2: if(cmp_phone(sel->phone, other_phone_key)) return; break;
        case 3: if(cmp_group(sel->group, other_group_key)) return; break;
        default: break;
        }
    }

    if(!sel_start) sel_start = sel;
    else sel_last->next_selected = sel;
    sel_last = sel;
}

void database::print_all(int fd, int type)
{
    if(!data.head){send_signal(fd, SIG_EMPTY); return;}
    switch(type)
    {
    case 0: data.print_list(fd); break;
/*
    case 1: name_tree.avl_print(ostream); fprintf(ostream ,"\n"); break;
    case 2: phone_tree.print_proc(ostream, phone_tree.root, 0, 0); fprintf(ostream ,"\n"); break;
    case 3: group_tree.print_proc(ostream, group_tree.root, 0, 0); fprintf(ostream ,"\n"); break;
*/
    default: printf("Unknown type!\n");
    }
}

int database::get_command(command& com, int fd)
{
    switch(com.cmd)
    {
        case CMD_NONE: return 1;
        case CMD_INSERT: return com_insert(com, fd);
        case CMD_SELECT: com_select(com); return com_print_selected(fd);
        case CMD_DELETE: com_select(com); return com_delete_selected(fd);
        case CMD_STOP: send_signal(fd, SIG_STOP); return 10;
        case CMD_QUIT: send_signal(fd, SIG_STOP); return 5;
    }
    return 10;
}

int database::com_insert(command& com, int fd)
{
    list_node *new_record = new list_node;
    list_node *eq_phone = nullptr;
    if(!new_record) return 2;
    if(new_record->record_init_string(com.statement1, com.statement2, com.statement3)) {delete new_record; return 3;}
    if(!data.head)
    {
        data.head = data.tail = new_record;
        name_tree.avl_insert_key(new_record);
        phone_tree.btree_insert(new_record);
        group_tree.gtree_insert(new_record);
        return 0;
    }
    if((eq_phone = phone_tree.search_eq(phone_tree.root, new_record->phone)))
    {
        for(; eq_phone; eq_phone = eq_phone->next_eq_phone) if(*eq_phone == *new_record) {delete new_record; send_signal(fd, SIG_EXIST); return -1;}
    }
    name_tree.avl_insert_key(new_record);
    phone_tree.btree_insert(new_record);
    group_tree.gtree_insert(new_record);
    data.tail->next = new_record;
    new_record->prev = data.tail;
    data.tail = new_record;
    return 0;
}

int database::com_select(command &com)
{
    list_node *it = data.head;
    sel_all = 0;
    sel_start = sel_last = nullptr;
    if(!it) return 2;
    if(!com.first){sel_all = 1; return 1;}
    if(com.op != OP_OR) //в поиске по группе с AND выгоднее искать в дереве по группе!
    {
        int key = 0;
        if(com.second == 3){com.swap_select();}
        switch(com.first)
        {
        case 1: get_sel_parameters(2, com.second, com); name_tree.avl_select(com.statement1, com.c_name); break;
        case 2: get_sel_parameters(2, com.second, com); sscanf(com.statement1, "%d", &key); phone_tree.btree_select(key, com.c_phone); break;
        case 3: clear_sel_parameters(); sscanf(com.statement1, "%d", &key); group_tree.gtree_select(key, com, 0); break;
        default: printf("[!]Select type error; parcer sent wrong data\n");
        }
        clear_sel_parameters();
    }
    else //в поиске по группе с OR ничего хоршего придумать не получается
    {
        int key = 0;
        if(com.second == 3){com.swap_select();}
        clear_sel_parameters();
        switch(com.first) //просто поиск без доп условий
        {
        case 1: name_tree.avl_select(com.statement1, com.c_name); break;
        case 2: sscanf(com.statement1, "%d", &key); phone_tree.btree_select(key, com.c_phone); break;
        case 3: sscanf(com.statement1, "%d", &key); group_tree.gtree_select(key, com, 1); break;
        default: printf("[!]Select type error; parcer sent wrong data\n");
        }
        get_sel_parameters(1, com.first, com);
        switch(com.second) //xor на второе условие
        {
        case 1: name_tree.avl_select(com.statement1, com.c_name); break;
        case 2: sscanf(com.statement2, "%d", &key); phone_tree.btree_select(key, com.c_phone); break;
        case 3: printf("[!]Program reached not possible switch\n"); break;
        default: printf("[!]Select type error; parcer sent wrong data\n");
        }
    }
    if(sel_last) sel_last->next_selected = nullptr;
    return 0;
}


int database::com_delete_selected(int fd)
{
    if(sel_all)
    {
        phone_tree.btree_delete_tree(phone_tree.root);
        name_tree.avl_delete_all();
        group_tree.gtree_delete_tree(group_tree.root);
        data.delete_list();
        return 0;
    }
    if(!sel_start) {send_signal(fd, SIG_EMPTY); return 1;}
    for(list_node *it = sel_start, *trash = sel_start; it;)
    {
        trash = it;
        it = it->next_selected;
        if(phone_tree.delete_key(trash))
            fprintf(stdout,"[!]Btree element delete error\n");
        if(name_tree.avl_delete_key(nullptr, name_tree.root, trash))
            fprintf(stdout,"[!]AVL element delete error\n");
        if(group_tree.delete_key(trash))
            fprintf(stdout,"[!]Gtree element delete error\n");
        data.delete_element(trash);
    }
    return 0;
}

int database::com_print_selected(int fd)
{
    if(sel_all){print_all(fd, 0); return 0;}
    if(!sel_start) {send_signal(fd, SIG_EMPTY); return 1;}
    for(list_node *it = sel_start; it; it = it->next_selected)
    {
#ifdef LESS
        it->print_enh_client(fd);
#else
        it->print_client(fd);
#endif
    }
    return 0;
}












