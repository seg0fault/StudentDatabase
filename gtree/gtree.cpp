#include "gtree.h"
/*
list_node* gtree::search_eq(gtree_node *start, int group)
{
    int i = 0;
    if(!start) return nullptr;
    for(; i < NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break;
        if(start->data[i]->group == group) return start->data[i];
        if(start->data[i]->group > group)
        {
            if(start->child[i]) return search_eq(start->child[i], group);
            else return nullptr;
        }
    }
    if(start->child[i]) return search_eq(start->child[i], group);
    else return nullptr;
}
*/
gtree_node* gtree::search_eq_node(gtree_node *start, int group, int* index)
{
    int i = 0;
    if(!start) return nullptr;
    current_depth++;
    path[current_depth] = start;
    for(; i < G_NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break;
        if(start->data[i]->group == group){*index = i; return start;}
        if(start->data[i]->group > group)
            return search_eq_node(start->child[i], group, index);
    }
    if(start->child[i]) return search_eq_node(start->child[i], group, index);
    else return nullptr;
}

int gtree_node::insert_key(list_node *key)
{
    int i = 0;
    node *save1, *save2;
    for(; i < G_NODE_SIZE - 1; i++)
    {
        if(!data[i]) break;
        if(key->group == data[i]->group){data[i]->insert(key); return 0;}
        if(key->group < data[i]->group) break;
    }
    if(i >= G_NODE_SIZE - 1){printf("[!]gtree insertion abort\n"); return 1;}
    int out = i;
    if(!data[i]){data[i] = new node; data[i]->insert(key); return out;}
    save2 = new node;
    save2->insert(key);
    for(; i < G_NODE_SIZE - 1; i++)
    {
        save1 = data[i];
        data[i] = save2;
        save2 = save1;
    }
//    printf("(2)key: %d i: %d\n", key->group ,out);
    return out;
}

int gtree_node::insert_node(node *key)
{
    int i = 0;
    node *save1, *save2 = key;
    for(; i < G_NODE_SIZE - 1; i++)
    {
        if(!data[i]) break;
        if(key->group < data[i]->group) break;
    }
    if(i >= G_NODE_SIZE - 1){printf("[!]Gtree insertion abort\n"); return 1;}
    int out = i;
    if(!data[i]){data[i] = key;/* printf("(1)key: %d i: %d\n", key->phone , out); */return out;}
    for(; i < G_NODE_SIZE - 1; i++)
    {
        save1 = data[i];
        data[i] = save2;
        save2 = save1;
    }
//    printf("(2)key: %d i: %d\n", key->phone ,out);
    return out;
}

int gtree::delete_key(list_node *key)
{
    int index = 0;
    int group = key->group;
    current_depth = -1;
    gtree_node *start = search_eq_node(root, group, &index);
    if(!start) return 1;
    if(start->data[index]->remove(key)) return 0;
    delete start->data[index];
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

gtree_node* gtree::merge_brothers(gtree_node *parent, gtree_node *l_brother, gtree_node *r_brother, int index, int type)
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
    last = l_brother->get_empty();                     //поиск первого свободного места в левом брате
    l_brother->data[last] = parent->data[index];       //перемещаем разделитель
    parent->data[index] = nullptr;                     //зануляем разделитель в родителе
    parent->data_shiftleft(index);
    parent->child_shiftleft(index + 1);
    for( i = 0; i + last + 1 < G_NODE_SIZE - 1; i++)   //перемещаем данные из правого брата в левый
        l_brother->data[i + last + 1] = r_brother->data[i];
    if(type)                         //если объединяемые узлы не листы необходимо переместить детей
    {
        for( i = 0; i + last + 1 < G_NODE_SIZE; i++)
            l_brother->child[i + last + 1] = r_brother->child[i];
    }
    if(!parent->data[0] && parent == root)         //остался пустой корень с одним потомком. Он не нужен
    {
        delete root;
        path[0] = nullptr;
//        for(int i = 0; i < G_INSERT_PATH_SIZE; i++) path[i] = path[i+1];
        root = l_brother;
    }
    delete r_brother; //удаляем правого брата
    return l_brother;
}

gtree_node* gtree::rebalance_node_up(gtree_node *start, gtree_node *parent, int type)
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
    for(int i = 0; i < G_NODE_SIZE; i++) {if(parent->child[i] == start){index = i; break;}} //ищем расположение узла в родителе
    gtree_node *l_brother = nullptr,
               *r_brother = nullptr;
    if(index > 0) l_brother = parent->child[index - 1]; //выбираем брата
    else
    {
        r_brother = parent->child[1];
        if(!r_brother){printf("[!]gtree technical error! Only one child!!\n"); return nullptr;}
    }
    if(r_brother) //рассматриваем правого (старшего) брата (только если нет младшего)
    {
        if(r_brother->data[G_NODE_POWER - 1]) //правый брат содержит достаточно много элементов (5)
        {
            start->data[G_NODE_POWER - 2] = parent->data[index]; //перемещаем разделитель из родителя в конец баланс.
            parent->data[index] = r_brother->data[0]; //перемещаем первый элемент брата в родитель
            r_brother->data_shiftleft(0);             //сдвиг данных в брате + автоматическое зануление
            if(type)
            {
                start->child[G_NODE_POWER - 1] = r_brother->child[0]; //трансфер первой ссылки из брата в балансируемый
                r_brother->child_shiftleft(0);                      //на картинке работает :)
            }
            return nullptr;
        }
        else //правый брат содержит (<= 4) элементов и необходимо слияние
        {
            return merge_brothers(parent, start, r_brother, index, type);
        }
    }
    else if(l_brother) //рассматриваем левого (младшего) брата
    {
        if(l_brother->data[G_NODE_POWER - 1]) //левый брат содержит достаточно много элементов (5)
        {
            int i = 0;
            start->data_shiftright(); //сдвигаем эл. балансируемого вправо
            start->data[0] = parent->data[index - 1]; //перемещаем разделитель из родителя в начало баланс.
            for(i = 0; i < G_NODE_SIZE - 2; i++) //ищем последний элемент в брате
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
            return merge_brothers(parent, l_brother, start, index - 1, type);
        }
    }
    else {printf("[!] gtree could not get brother!\n"); return nullptr;}
    return nullptr;
}

int gtree::rebalance_node_down(gtree_node *start, int index)
{
    /* функция выполняет ребалансировку узла б-дерева не являющегося листом откуда УЖЕ был удален элемент но сдвига не было
     * функция специфически используется при движении ВНИЗ, к листу
     * для балансировки при движении ВВЕРХ, к корню, см функцию rebalance_node_up
     * при ребалансировке на место удаленного узла ставится последний элемент соответствующего потомка
     * после этого в потомке возможна ситуация при которой в нем будет недостаточное число элементов
     * поэтому возможна необходимость ребалансировки этого потомка
     * функция возвращает индекс перемещенного элемента потомка в случае успеха и -1 в случае ошибки
     */
    gtree_node *child = start->child[index];
    if(!child) {printf("[!]gtree node rebalance error! No such child!"); return -1;}
    for(int i = 0; i < G_NODE_SIZE - 2; i++)
    {
        if(!child->data[i + 1]) //нашли последний
        {
            start->data[index] = child->data[i]; //трансфер данных и зануление
            child->data[i] = nullptr;
            return i;
        }
    }
    start->data[index] = child->data[G_NODE_SIZE - 2]; //случай полного потомка
    child->data[G_NODE_SIZE - 2] = nullptr;
    return G_NODE_SIZE - 2;
}

int gtree::rebalance_proc(gtree_node *start, int index_for_node)
{
    /* Функция выполняет рекурсивную ребалансировку узлов дерева
     * Функция распознает тип узла с которым она работает и выполняет соответствующую операцию ребалансировки
     * Функция возвращает 0 если она закончила балансировку и 1 если требуется дальнейшая балансировка узла
     */
    if(!start->child[0])  //лист
    {
        if(start == root) //корень + лист, балансировка не требуется
            return 0;
        else if(start->size() >= G_NODE_POWER - 1)
            return 0;     //лист достаточного размера, балансировка не требуется
        else              //лист недостаточного размера, требуется балансировка
        {                 //выполняем балансировку листа
            rebalance_node_up(start, path[current_depth - 1], 0);
//            print_proc(stdout, root, 0, 0);
//            printf("\n");
            if(path[current_depth - 1] == root) //корню в этой ситуации балансировка не требуется
                return 0;
            current_depth--;
            if(!path[current_depth]) return 0; //для ситуации когда мы убили старый корень
            if(path[current_depth]->size() < G_NODE_POWER - 1)
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
        if(path[current_depth]->size() < G_NODE_POWER - 1)
            return rebalance_proc(path[current_depth], -1);
        else return 0;
    }
    else                  //не лист (балансировка вниз)
    {
        gtree_node *it = start->child[index_for_node + 1];
        if(!it){printf("[i]gtree RIGHT CHILD ERROR!\n"); return 1;}
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

void gtree::gtree_delete_tree(gtree_node *start)
{
    if(!start) return;
    for(int i = 0; i < G_NODE_SIZE; i++){gtree_delete_tree(start->child[i]);}
//    printf("Deleted! %d\n", start->data[0]->group);
    delete start;
//    printf("hooray!\n");
}

void gtree_node::insert_child(gtree_node *newchild, int i)
{
    gtree_node *save1 , *save2 = newchild;
    if(!child[i]){child[i] = newchild; return;}
    for(; i < G_NODE_SIZE; i++)
    {
        save1 = child[i];
        child[i] = save2;
        save2 = save1;
    }
}

int gtree::gtree_init_list(list_node *head)
{
    for(list_node *it = head; it; it = it->next)
    {
        if(gtree_insert(it) > 0) return 1;
    }
    return 0;
}

int gtree::gtree_insert(list_node *key)
{
    current_depth = -1;
    if(!root)
    {
        root = new gtree_node;
        root->data[0] = new node;
        root->data[0]->insert(key);
        return -1;
    }
    if(gtree_insert_proc(root, key) > 0) return 1;
    return 0;
}

int gtree::gtree_insert_proc(gtree_node *start, list_node *key)
{
    current_depth++; path[current_depth] = start;
    if(start->data[G_NODE_SIZE - 2])
    {
        if(gtree_node_split()){current_depth = -1; if(!gtree_insert_proc(root, key)) return 0; else return 1;}
        else      {current_depth -= 2; if(!gtree_insert_proc(path[current_depth + 1], key)) return 0; else return 1;}
    }
    if(!start->child[0]){start->insert_key(key); return 0;}
    int i = 0;
    for(; i < G_NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break; //maximum
        if(start->data[i]->group == key->group){start->data[i]->insert(key); return 0;}
        if(start->data[i]->group > key->group) break; //middle
    }
    if(start->child[i]){return gtree_insert_proc(start->child[i], key);}
    else printf("[!]gtree child error!\n");
    return 1;
}

int gtree::gtree_node_split()
{
    if(current_depth <= 0)
    {
        gtree_node *newroot = new gtree_node;
        gtree_node *newright = new gtree_node;

        newroot->data[0] = root->data[G_NODE_POWER - 1];
        root->data[G_NODE_POWER - 1] = nullptr;

        newroot->child[0] = root;
        newroot->child[1] = newright;

        for(int i = G_NODE_POWER; i < G_NODE_SIZE - 1; i++)
        {
            newright->data[i - G_NODE_POWER] = root->data[i];
            root->data[i] = nullptr;
        }

        for(int i = G_NODE_POWER; i < G_NODE_SIZE; i++)
        {
            newright->child[i - G_NODE_POWER] = root->child[i];
            root->child[i] = nullptr;
        }

        root = newroot;
        return 1;
    }
    gtree_node *current = path[current_depth];
    gtree_node *parent = path[current_depth - 1];
    gtree_node *newright = new gtree_node;

    parent->insert_child(newright, parent->insert_node(current->data[G_NODE_POWER - 1]) + 1);
    current->data[G_NODE_POWER - 1] = nullptr;
    for(int i = G_NODE_POWER; i < G_NODE_SIZE - 1; i++)
    {
        newright->data [i - G_NODE_POWER] = current->data[i];
        current->data[i] = nullptr;
    }

    for(int i = G_NODE_POWER; i < G_NODE_SIZE; i++)
    {
        newright->child[i - G_NODE_POWER] = current->child[i];
        current->child[i] = nullptr;
    }

    return 0;
}

void gtree::print_proc(FILE *ostream, gtree_node *start, unsigned level, unsigned index)
{
    if(!start) return;
    gtree_node **down = start->child;
    fprintf(ostream, "(%d, %d)", level, index);
    for(unsigned i = 0; i < level; i++) fprintf(ostream, "--");
    start->print_node(ostream);
    for(unsigned i = 0; /*down[i]*/ i < G_NODE_SIZE; i++){print_proc(ostream, down[i], level + 1, i);}
}

void gtree_node::print_node(FILE *ostream)
{
    if(!data[0]) {fprintf(ostream, "NODE EMPTY\n"); return;}
    fprintf(ostream, "[ ");
    for(int i = 0; data[i]; i++)
        fprintf(ostream, "(%d x%d)", data[i]->group, data[i]->size);
    fprintf(ostream, "]\n");
}

void gtree::gtree_select_all(gtree_node *start)
{
    int i;
    if(!start) return;
    for(i = 0; i < G_NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break;
        gtree_select_key(start->data[i]);
        gtree_select_all(start->child[i]);
    }
    if(start->child[i]) gtree_select_all(start->child[i]);
}

void gtree::gtree_select_eq(gtree_node *start)
{
    int i;
    if(!start) return;
    for(i = 0; i < G_NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break;
        if(start->data[i]->group > search_key)
        {
            gtree_select_eq(start->child[i]); break;
        }
        else if(start->data[i]->group == search_key)
        {
            gtree_select_key(start->data[i]); break;
        }
    }
    if(start->child[i]) gtree_select_eq(start->child[i]);
}

void gtree::gtree_select_ne(gtree_node *start)
{
    int i;
    if(!start) return;
    for(i = 0; i < G_NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break;
        if(start->data[i]->group > search_key)
        {
            gtree_select_key(start->data[i]);
            gtree_select_ne(start->child[i]);
        }
        else if(start->data[i]->group < search_key)
        {
            gtree_select_key(start->data[i]);
            gtree_select_all(start->child[i]);
        }
        else
        {
            gtree_select_all(start->child[i]);
        }
    }
    if(start->child[i]) gtree_select_ne(start->child[i]);
}

void gtree::gtree_select_lt(gtree_node *start)
{
    int i;
    if(!start) return;
    for(i = 0; i < G_NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break;
        if(start->data[i]->group > search_key)
        {
            gtree_select_lt(start->child[i]);
        }
        else if(start->data[i]->group < search_key)
        {
            gtree_select_key(start->data[i]);
            gtree_select_all(start->child[i]);
        }
        else
        {
            gtree_select_all(start->child[i]);
        }
    }
    if(start->child[i]) gtree_select_lt(start->child[i]);
}


void gtree::gtree_select_le(gtree_node *start)
{
    int i;
    if(!start) return;
    for(i = 0; i < G_NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break;
        if(start->data[i]->group > search_key)
        {
            gtree_select_lt(start->child[i]);
        }
        else if(start->data[i]->group < search_key)
        {
            gtree_select_key(start->data[i]);
            gtree_select_all(start->child[i]);
        }
        else
        {
            gtree_select_key(start->data[i]);
            gtree_select_all(start->child[i]);
        }
    }
    if(start->child[i]) gtree_select_lt(start->child[i]);
}

void gtree::gtree_select_gt(gtree_node *start)
{
    int i; int flag = 0;
    if(!start) return;
    for(i = 0; i < G_NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break;
        if(flag){gtree_select_all(start->child[i]); continue;}
        if(start->data[i]->group > search_key)
        {
            gtree_select_key(start->data[i]);
            gtree_select_gt(start->child[i]);
        }
        else if(start->data[i]->group == search_key)
        {
            flag = 1;
        }
    }
    if(start->child[i])
    {
        if(flag) gtree_select_all(start->child[i]);
        else gtree_select_gt(start->child[i]);
    }
}

void gtree::gtree_select_ge(gtree_node *start)
{
    int i;
    int flag = 0;
    if(!start) return;
    for(i = 0; i < G_NODE_SIZE - 1; i++)
    {
        if(!start->data[i]) break;
        if(flag){gtree_select_all(start->child[i]); continue;}
        if(start->data[i]->group > search_key)
        {
            gtree_select_key(start->data[i]);
            gtree_select_gt(start->child[i]);
        }
        else if(start->data[i]->group == search_key)
        {
            gtree_select_key(start->data[i]);
            flag = 1;
        }
    }
    if(start->child[i])
    {
        if(flag) gtree_select_all(start->child[i]);
        else gtree_select_gt(start->child[i]);
    }
}

void gtree::gtree_select(int key, command& com)
{
    search_key = key;
    second = com.second;
    if(second == 1) name_key = com.statement2;
    else if(second == 2) sscanf(com.statement2, "%d", &phone_key);
    switch(com.c_group)
    {
    case COND_NONE: return;
    case COND_EQ: gtree_select_eq(root); break;
    case COND_NE: gtree_select_ne(root); break;
    case COND_GE: gtree_select_ge(root); break;
    case COND_GT: gtree_select_gt(root); break;
    case COND_LE: gtree_select_le(root); break;
    case COND_LT: gtree_select_lt(root); break;
    case COND_LIKE: printf("[!]Like in group search!\n");
    }
}



