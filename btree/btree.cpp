#include "btree.h"

list_node* btree::search_eq(btree_node *start, int phone)
{
    int i = 0;
    if(!start) return nullptr;
    for(; i < NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break;
        if(start->data[i]->phone == phone) return start->data[i];
        if(start->data[i]->phone > phone)
        {
            if(start->child[i]) return search_eq(start->child[i], phone);
            else return nullptr;
        }
    }
    if(start->child[i]) return search_eq(start->child[i], phone);
    else return nullptr;
}

btree_node* btree::search_eq_node(btree_node *start, int phone, int* index)
{
    int i = 0;
    if(!start) return nullptr;
    current_depth++;
    path[current_depth] = start;
    for(; i < NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break;
        if(start->data[i]->phone == phone){*index = i; return start;}
        if(start->data[i]->phone > phone)
            return search_eq_node(start->child[i], phone, index);
    }
    if(start->child[i]) return search_eq_node(start->child[i], phone, index);
    else return nullptr;
}

int btree_node::remove_key(list_node *key, int index, int type)
{
    list_node *head = data[index];
    if(!type)
    {
        if(key == head)
        {
            if(head->next_eq_phone) {data[index] = head->next_eq_phone; return 1;}
            else return 0;
        }
        list_node *prev, *it;
        for(it = head; it; it = it->next_eq_phone)
        {
            if(it == key){prev->next_eq_phone = it->next_eq_phone; return 1;}
            prev = it;
        }
    }
    else
    {
        if(key == head)
        {
            if(head->next_eq_phone_group) {data[index] = head->next_eq_phone_group; return 1;}
            else return 0;
        }
        list_node *prev, *it;
        for(it = head; it; it = it->next_eq_phone_group)
        {
            if(it == key){prev->next_eq_phone_group = it->next_eq_phone_group; return 1;}
            prev = it;
        }
    }
    printf("[!]Btree remove reached end of node!\n");
    return 0;
}

int btree::delete_key(list_node *key)
{
    int index = 0;
    int phone = key->phone;
    current_depth = -1;
    btree_node *start = search_eq_node(root, phone, &index);
    if(!start){printf("[!]Btree(%d) delete error (not found)\n", treetype); return 1;}
    if(start->data[index]->phone != key->phone) printf("Search error\n");
    if(start->remove_key(key, index, treetype)) return 0;
    if(!start->child[0]) //leave
    {
//        start->data[index] = nullptr;
        start->data_shiftleft(index);
//        print_proc(stdout, root, 0, 0);
//        printf("\n");
        return rebalance_proc(start, 0);
    }
    else //not leave
    {
        start->data[index] = nullptr;
//        print_proc(stdout, root, 0, 0);
//        printf("\n");
        return rebalance_proc(start, index);
    }
    return 1;
}

void btree::merge_brothers(btree_node *parent, btree_node *l_brother, btree_node *r_brother, int index, int type)
{
    /*
    функция всегда добавляет правого брата в левый из соображения упорядоченности детей
    так же функция вставляет разделитель между братьями посередине между данными братьев
    это может привести к недостаточному количеству элементов в родительском узле, следовательно
    после выполнения этой функции должна быть запущена ребалансировка родительского узла
    функция получает на вход родителя братьев, братьев и индекс разделителя братьев в родителе
    подразумевается что в любом из братьев количество(=< 4) элементов такое что при слиянии
    образуется не переполненный узел
    */
    int last = 0, i;
    last = l_brother->get_empty();                 //поиск первого свободного места в левом брате
    l_brother->data[last] = parent->data[index];   //перемещаем разделитель
    parent->data[index] = nullptr;                 //зануляем разделитель в родителе
    parent->data_shiftleft(index);
    parent->child_shiftleft(index + 1);
    for( i = 0; i + last + 1 < NODE_SIZE - 1; i++) // перемещаем данные из правого брата в левый
        l_brother->data[i + last + 1] = r_brother->data[i];
    if(type)                                       //если объединяемые узлы не листы необходимо переместить детей
    {
        for( i = 0; i + last + 1 < NODE_SIZE; i++)
            l_brother->child[i + last + 1] = r_brother->child[i];
    }
    if(!parent->data[0] && parent == root)         //остался пустой корень с одним потомком. Он не нужен
    {
        delete root;
        path[0] = nullptr;
//        for(int i = 0; i < INSERT_PATH_SIZE; i++) path[i] = path[i+1];
        root = l_brother;
    }
    delete r_brother; //удаляем правого брата
}

btree_node* btree::rebalance_node_up(btree_node *start, btree_node *parent, int type)
{
    /*
    функция выполняет ребалансировку узла вверх откуда УЖЕ был удален элемент и произошел сдвиг
    type = 0 балансируем лист, type = 1 балансируем узел
    подразумевается что в листе находится строго t - 2 (3) вершины
    менььше быть не может по св-ву б-дерева, больше - не нужна балансировка
    функция либо перемещает элемент из брата в балансируемый узел
    либо сливает балансируемый узел с некоторым братом
    функия возвращает указатель на отбалансированный узел в случае успеха
    и нулевой указатель в случае ошибки
    */
    int index = 0;
    for(int i = 0; i < NODE_SIZE; i++) {if(parent->child[i] == start){index = i; break;}} //ищем расположение узла в родителе
    btree_node *l_brother = nullptr,
               *r_brother = nullptr;
    if(index > 0) l_brother = parent->child[index - 1]; //выбираем брата
    else
    {
        r_brother = parent->child[1];
        if(!r_brother){printf("[!]Btree technical error! Only one child!!\n"); return nullptr;}
    }
    if(r_brother) //рассматриваем правого (старшего) брата (только если нет младшего)
    {
        if(r_brother->data[NODE_POWER - 1]) //правый брат содержит достаточно много элементов (5)
        {
            start->data[NODE_POWER - 2] = parent->data[index]; //перемещаем разделитель из родителя в конец баланс.
            parent->data[index] = r_brother->data[0]; //перемещаем первый элемент брата в родитель
            r_brother->data_shiftleft(0);             //сдвиг данных в брате + автоматическое зануление
            if(type)
            {
                start->child[NODE_POWER - 1] = r_brother->child[0]; //трансфер первой ссылки из брата в балансируемый
                r_brother->child_shiftleft(0);                      //на картинке работает :)
            }
            return nullptr;
        }
        else //правый брат содержит (<= 4) элементов и необходимо слияние
        {
            merge_brothers(parent, start, r_brother, index, type);
            return start;
        }
    }
    else if(l_brother) //рассматриваем левого (младшего) брата
    {
        if(l_brother->data[NODE_POWER - 1]) //левый брат содержит достаточно много элементов (5)
        {
            int i = 0;
            start->data_shiftright(); //сдвигаем эл. балансируемого вправо
            start->data[0] = parent->data[index - 1]; //перемещаем разделитель из родителя в начало баланс.
            for(i = 0; i < NODE_SIZE - 2; i++) //ищем последний элемент в брате
            {
                if(!l_brother->data[i+1])
                    break;
            }
            parent->data[index - 1] = l_brother->data[i]; //перемещаем его на место разделителя
            l_brother->data[i] = nullptr; //зануляем перемещенный элемент в брате
            if(type)
            {
                start->child_shiftright(); //трансфер последней ссылки из левого брата в балансируемый узел
                start->child[0] = l_brother->child[i+1];
                l_brother->child[i+1] = nullptr;
            }
            return start;
        }
        else //левый брат содержит (<= 4) элементов и необходимо слияние
        {
            merge_brothers(parent, l_brother, start, index - 1, type);
            return l_brother;
        }
    }
    else {printf("[!] Btree could not get brother!\n"); return nullptr;}
    return nullptr;
}

int btree::rebalance_node_down(btree_node *start, int index)
{
    /* функция выполняет ребалансировку узла б-дерева не являющегося листом откуда УЖЕ был удален элемент но сдвига не было
     * функция специфически используется при движении ВНИЗ, к листу
     * для балансировки при движении ВВЕРХ, к корню, см функцию rebalance_node_up
     * при ребалансировке на место удаленного узла ставится последний элемент соответствующего потомка
     * после этого в потомке возможна ситуация при которой в нем будет недостаточное число элементов
     * поэтому возможна необходимость ребалансировки этого потомка
     * функция возвращает индекс перемещенного элемента потомка в случае успеха и -1 в случае ошибки
     */
    btree_node *child = start->child[index];
    if(!child) {printf("[!]Btree node rebalance error! No such child!"); return -1;}
    for(int i = 0; i < NODE_SIZE - 2; i++)
    {
        if(!child->data[i + 1]) //нашли последний
        {
            start->data[index] = child->data[i]; //трансфер данных и зануление
            child->data[i] = nullptr;
            return i;
        }
    }
    start->data[index] = child->data[NODE_SIZE - 2]; //случай полного потомка
    child->data[NODE_SIZE - 2] = nullptr;
    return NODE_SIZE - 2;
}

int btree::rebalance_proc(btree_node *start, int index_for_node)
{
    /* Функция выполняет рекурсивную ребалансировку узлов дерева
     * Функция распознает тип узла с которым она работает и выполняет соответствующую операцию ребалансировки
     * Функция возвращает 0 если она закончила балансировку и 1 если требуется дальнейшая балансировка узла
     */
    if(!start->child[0])  //лист
    {
        if(start == root) //корень + лист, балансировка не требуется
            return 0;
        else if(start->size() >= NODE_POWER - 1)
            return 0;     //лист достаточного размера, балансировка не требуется
        else              //лист недостаточного размера, требуется балансировка
        {                 //выполняем балансировку листа
            rebalance_node_up(start, path[current_depth - 1], 0);
//            print_proc(stdout, root, 0, 0);
//            printf("\n");
            if(path[current_depth - 1] == root) //корню в этой ситуации балансировка не требуется
                return 0;
            current_depth--;
            if(!path[current_depth]) return 0;
            if(path[current_depth]->size() < NODE_POWER - 1)
                return rebalance_proc(path[current_depth], -1);
            else return 0;
                          //посылаем флаг -1 чтобы обозначить балансировку родителя вверх
                          //балансировка родителя в такой конфигурации может быть сделана
                          //только путем слияния или обмена соседних узлов; корню ее делать не надо
                          //все остальное кажется +- работает
        }
    }
    else if(index_for_node == -1) //не лист (балансировка вверх)
    {
        rebalance_node_up(start, path[current_depth - 1], 1);
//        print_proc(stdout, root, 0, 0);
//        printf("Balanced node up\n");
        current_depth--;
        if(path[current_depth] == root || !path[current_depth]) //корню в этой ситуации балансировка не требуется
            return 0;
        if(path[current_depth]->size() < NODE_POWER - 1)
            return rebalance_proc(path[current_depth], -1);
        else return 0;
    }
    else                  //не лист (балансировка вниз)
    {
        btree_node *it = start->child[index_for_node + 1];
        if(!it){printf("[i]BTREE RIGHT CHILD ERROR!\n"); return 1;}
        for(; it; it = it->child[0])
        {
            current_depth++;
            path[current_depth] = it;
        }
        start->data[index_for_node] = path[current_depth]->data[0];
        path[current_depth]->data_shiftleft(0);
//        print_proc(stdout, root, 0, 0);
//        printf("\n");
        return rebalance_proc(path[current_depth], 0);
        /*
        int res = rebalance_node_down(start, index_for_node);
                          //выполняем балансировку соответствующего ребенка
        current_depth++;
        path[current_depth] = start->child[index_for_node];
        print_proc(stdout, root, 0, 0);
        printf("\n");
        return rebalance_proc(start->child[index_for_node], res);
        */
    }
}

void btree::btree_delete_tree(btree_node *start)
{
    if(!start) return;
    for(int i = 0; i < NODE_SIZE; i++){btree_delete_tree(start->child[i]);}
//    printf("Deleted! %d\n", start->data[0]->phone);
    delete start;
//    printf("hooray!\n");
}

void list_node::insert_eq_phone(list_node *newnext, int type)
{
    if(!type)
    {
        list_node *it = next_eq_phone;
        if(!it){next_eq_phone = newnext; return;}
        for(; it->next_eq_phone; it = it->next_eq_phone){}
        it->next_eq_phone = newnext;
    }
    else
    {
        list_node *it = next_eq_phone_group;
        if(!it){next_eq_phone_group = newnext; return;}
        for(; it->next_eq_phone_group; it = it->next_eq_phone_group){}
        it->next_eq_phone_group = newnext;
    }
}

int btree_node::insert_key(list_node *key, int type)
{
    int i = 0;
    list_node *save1, *save2 = key;
    for(; i < NODE_SIZE - 1; i++)
    {
        if(!data[i]) break;
        if(key->phone == data[i]->phone){data[i]->insert_eq_phone(key, type); return 0;}
        if(key->phone < data[i]->phone) break;
    }
    if(i >= NODE_SIZE - 1){printf("[!]Btree insertion abort\n"); return 1;}
    int out = i;
    if(!data[i]){data[i] = key;/* printf("(1)key: %d i: %d\n", key->phone , out); */return out;}
    for(; i < NODE_SIZE - 1; i++)
    {
        save1 = data[i];
        data[i] = save2;
        save2 = save1;
    }
//    printf("(2)key: %d i: %d\n", key->phone ,out);
    return out;
}

void btree_node::insert_child(btree_node *newchild, int i)
{
    btree_node *save1 , *save2 = newchild;
    if(!child[i]){child[i] = newchild; return;}
    for(; i < NODE_SIZE; i++)
    {
        save1 = child[i];
        child[i] = save2;
        save2 = save1;
    }
}

int btree::btree_init_list(list_node *head)
{
    for(list_node *it = head; it; it = it->next)
    {
        if(btree_insert(it) > 0) return 1;
    }
    return 0;
}

int btree::btree_insert(list_node *key)
{
    current_depth = -1;
    if(!root)
    {
        root = new btree_node;
        root->data[0] = key;
        return -1;
    }
    if(btree_insert_proc(root, key) > 0) return 1;
    return 0;
}

int btree::btree_insert_proc(btree_node *start, list_node *key)
{
    current_depth++; path[current_depth] = start;
    if(start->data[NODE_SIZE - 2])
    {
        if(btree_node_split()){current_depth = -1; if(!btree_insert_proc(root, key)) return 0; else return 1;}
        else      {current_depth -= 2; if(!btree_insert_proc(path[current_depth + 1], key)) return 0; else return 1;}
    }
    if(!start->child[0]){start->insert_key(key, treetype); return 0;}
    int i = 0;
    for(; i < NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break; //maximum
        if(start->data[i]->phone == key->phone){start->data[i]->insert_eq_phone(key, treetype); return 0;}
        if(start->data[i]->phone > key->phone) break; //middle
    }
    if(start->child[i]){if(!btree_insert_proc(start->child[i], key)) return 0;}
    else printf("[!]Btree child error!\n");
    return 1;
}

int btree::btree_node_split()
{
    if(current_depth <= 0)
    {
        btree_node *newroot = new btree_node;
        btree_node *newright = new btree_node;

        newroot->data[0] = root->data[NODE_POWER - 1];
        root->data[NODE_POWER - 1] = nullptr;

        newroot->child[0] = root;
        newroot->child[1] = newright;

        for(int i = NODE_POWER; i < NODE_SIZE - 1; i++)
        {
            newright->data[i - NODE_POWER] = root->data[i];
            root->data[i] = nullptr;
        }

        for(int i = NODE_POWER; i < NODE_SIZE; i++)
        {
            newright->child[i - NODE_POWER] = root->child[i];
            root->child[i] = nullptr;
        }

        root = newroot;
        return 1;
    }
    btree_node *current = path[current_depth];
    btree_node *parent = path[current_depth - 1];
    btree_node *newright = new btree_node;

    parent->insert_child(newright, parent->insert_key(current->data[NODE_POWER - 1], treetype) + 1);
    current->data[NODE_POWER - 1] = nullptr;
    for(int i = NODE_POWER; i < NODE_SIZE - 1; i++)
    {
        newright->data [i - NODE_POWER] = current->data[i];
        current->data[i] = nullptr;
    }

    for(int i = NODE_POWER; i < NODE_SIZE; i++)
    {
        newright->child[i - NODE_POWER] = current->child[i];
        current->child[i] = nullptr;
    }

    return 0;
}

void btree::print_proc(FILE *ostream, btree_node *start, unsigned level, unsigned index)
{
    if(!start) return;
    if(level > MAX_PRINT) return;
    btree_node **down = start->child;
    fprintf(ostream, "(%d, %d)", level, index);
    for(unsigned i = 0; i < level; i++) fprintf(ostream, "--");
    start->print_node(ostream);
    for(unsigned i = 0; /*down[i]*/ i < NODE_SIZE; i++){print_proc(ostream, down[i], level + 1, i);}
}

void btree_node::print_node(FILE *ostream)
{
    if(!data[0]) {fprintf(ostream, "NODE EMPTY\n"); return;}
    fprintf(ostream, "[ ");
    list_node *it = nullptr;
    for(int i = 0; data[i]; i++)
    {
        int count = 0;
        for(it = data[i]; it; it = it->next_eq_phone) count++;
        fprintf(ostream, "(%d x%d)", data[i]->phone, count);
    }
    fprintf(ostream, "]\n");
}

void btree::btree_select_all(btree_node *start)
{
    int i;
    if(!start) return;
    for(i = 0; i < NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break;
        btree_select_key(start->data[i]);
        btree_select_all(start->child[i]);
    }
    if(start->child[i]) btree_select_all(start->child[i]);
}

void btree::btree_select_eq(btree_node *start)
{
    int i;
    if(!start) return;
    for(i = 0; i < NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break;
        if(start->data[i]->phone > search_key)
        {
            btree_select_eq(start->child[i]); break;
        }
        else if(start->data[i]->phone == search_key)
        {
            btree_select_key(start->data[i]); break;
        }
    }
    if(start->child[i]) btree_select_eq(start->child[i]);
}

void btree::btree_select_ne(btree_node *start)
{
    int i;
    if(!start) return;
    for(i = 0; i < NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break;
        if(start->data[i]->phone > search_key)
        {
            btree_select_key(start->data[i]);
            btree_select_ne(start->child[i]);
        }
        else if(start->data[i]->phone < search_key)
        {
            btree_select_key(start->data[i]);
            btree_select_all(start->child[i]);
        }
        else
        {
            btree_select_all(start->child[i]);
        }
    }
    if(start->child[i]) btree_select_ne(start->child[i]);
}

void btree::btree_select_lt(btree_node *start)
{
    int i;
    if(!start) return;
    for(i = 0; i < NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break;
        if(start->data[i]->phone > search_key)
        {
            btree_select_lt(start->child[i]);
        }
        else if(start->data[i]->phone < search_key)
        {
            btree_select_key(start->data[i]);
            btree_select_all(start->child[i]);
        }
        else
        {
            btree_select_all(start->child[i]);
        }
    }
    if(start->child[i]) btree_select_lt(start->child[i]);
}


void btree::btree_select_le(btree_node *start)
{
    int i;
    if(!start) return;
    for(i = 0; i < NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break;
        if(start->data[i]->phone > search_key)
        {
            btree_select_lt(start->child[i]);
        }
        else if(start->data[i]->phone < search_key)
        {
            btree_select_key(start->data[i]);
            btree_select_all(start->child[i]);
        }
        else
        {
            btree_select_key(start->data[i]);
            btree_select_all(start->child[i]);
        }
    }
    if(start->child[i]) btree_select_lt(start->child[i]);
}

void btree::btree_select_gt(btree_node *start)
{
    int i; int flag = 0;
    if(!start) return;
    for(i = 0; i < NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break;
        if(flag){btree_select_all(start->child[i]); continue;}
        if(start->data[i]->phone > search_key)
        {
            btree_select_key(start->data[i]);
            btree_select_gt(start->child[i]);
        }
        else if(start->data[i]->phone == search_key)
        {
            flag = 1;
        }
    }
    if(start->child[i])
    {
        if(flag) btree_select_all(start->child[i]);
        else btree_select_gt(start->child[i]);
    }
}

void btree::btree_select_ge(btree_node *start)
{
    int i;
    int flag = 0;
    if(!start) return;
    for(i = 0; i < NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break;
        if(flag){btree_select_all(start->child[i]); continue;}
        if(start->data[i]->phone > search_key)
        {
            btree_select_key(start->data[i]);
            btree_select_gt(start->child[i]);
        }
        else if(start->data[i]->phone == search_key)
        {
            btree_select_key(start->data[i]);
            flag = 1;
        }
    }
    if(start->child[i])
    {
        if(flag) btree_select_all(start->child[i]);
        else btree_select_gt(start->child[i]);
    }
}

void btree::btree_select(int key, cond_type cnd)
{
    search_key = key;
    switch(cnd)
    {
    case COND_NONE: return;
    case COND_EQ: btree_select_eq(root); break;
    case COND_NE: btree_select_ne(root); break;
    case COND_GE: btree_select_ge(root); break;
    case COND_GT: btree_select_gt(root); break;
    case COND_LE: btree_select_le(root); break;
    case COND_LT: btree_select_lt(root); break;
    case COND_LIKE: printf("[!]Like in phone search!\n");
    }
}



