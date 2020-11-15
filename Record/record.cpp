#include "record.h"

void record::print(FILE *stream)
{
    fprintf(stream, "%12s %10d %4d\n", name, phone, group);
}

void record::destroy()
{
    if(name) delete [] name;
    phone = 0;
    group = 0;
}

int record::record_init_base(const char *new_name, int new_phone, int new_group)
{
    destroy();
    phone = new_phone;
    group = new_group;
    if(new_name)
    {
        name = new char[strlen(new_name) + 1];
        if(!name) return -1;
        strcpy(name, new_name);
    }
    else name = 0;
    return 0;
}

int record::record_init_file(FILE *fp)
{
    char buf[LEN];
    destroy();
    if(fscanf(fp, "%s %d %d", buf, &phone, &group) != 3)
    {
        if(feof(fp)) return 1;
        return -1;
    }
    name = new char[strlen(buf) + 1];
    if(!name) return 3;
    strcpy(name, buf);
    return 0;
}

int record::record_init_string(const char *new_name, const char *new_phone, const char *new_group)
{
    destroy();
    if(new_name)
    {
        name = new char[strlen(new_name) + 1];
        if(!name) return 3;
        strcpy(name, new_name);
    }
    else return 1;
    if(new_phone)
    {
        if(sscanf(new_phone, "%d", &phone) != 1) return 2;
    }
    else return 1;
    if(new_group)
    {
        if(sscanf(new_group, "%d", &group) != 1) return 2;
    }
    else return 1;
    return 0;
}

record& record::operator=(const record& x) //оператор присваивания
{
    destroy(); //удаляем изначальный объект, чтобы не потерять его исходный указатель и не вызвать утечку памяти
    record_init_base(x.name, x.phone, x.group);
    return *this;  // *this - это объект с которым работает данный метод (невидимый аргумент)
}

record& record::operator=(record&& x)
{
    destroy();
    phone = x.phone;
    group = x.group;
    name = x.name;
    x.group = 0;
    x.phone = 0;
    x.name = nullptr;
    return *this;
}

int record::operator==(const record& x) const
{
    int res;
    if(!name && !x.name) return group == x.group && phone == x.phone;
    if(!name || !x.name) return 0;
    res = strcmp(name, x.name);
    if(res == 0) return group == x.group && phone == x.phone;
    return 0;
}



