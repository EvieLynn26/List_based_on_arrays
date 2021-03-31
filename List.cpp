#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "D:\Downloads\TX\TXLib.h"


#define NEXT p_lst->next
#define PREV p_lst->prev
#define DATA p_lst->data


typedef int elem_t;


const int Max_size = 10;
const int To_free = 0;
const int Poison = -666;


struct list
{
    int head, tail,  max_size, now_size;

    const int canary_begin = (int) '$';

    elem_t* data;
    elem_t* next;
    elem_t* prev;

    const int canary_end = (int) '$';

    int head_free;
};


void lstConstructor (list* p_lst, size_t list_size)
{
    p_lst->max_size = list_size;

    DATA = (elem_t*) calloc (p_lst->max_size, sizeof (elem_t));
    NEXT = (elem_t*) calloc (p_lst->max_size, sizeof (elem_t));
    PREV = (elem_t*) calloc (p_lst->max_size, sizeof (elem_t));

    for (int i = 0; i < p_lst->max_size; i++)
    {
        DATA[i] = Poison;
        NEXT[i] = i + 1;
        PREV[i] = -1;
    }

    NEXT[0] = 0;

    NEXT[p_lst->max_size - 1] = 0;

    p_lst->head = 1;
    p_lst->tail = 1;
    p_lst->now_size = 0;

    p_lst->head_free = 1;
}

bool Verify (list* p_lst);

int InsertHead (list* p_lst, int val);
int InsertTail (list* p_lst, int val);
int InsertAfter (list* p_lst, int after_which, elem_t val);
int InsertBefore (list* p_lst, int before_which, elem_t val);

void DeleteAfter (list* p_lst, int after_which);
void Delete (list* p_lst, int which);

int LogicToPhys (list* p_lst, const int logic)
{
    Verify (p_lst);

    int phys = PREV[p_lst->head];

    for (int i = 0; i < logic; i++)
    {
        phys = NEXT[phys];
    }

    return phys;
}

int find (list* p_lst, int val)
{
    Verify (p_lst);

    for (elem_t i = p_lst->head; i != p_lst->tail; i = NEXT[i])
    {
        if (DATA[i] == val)
            return i;
    }

    return -1;
}

void Dump (const list* p_lst)
{
    printf ("Head: %d\n", p_lst->head);
    printf ("Tail: %d\n", p_lst->tail);
    printf ("\t" "size of filled list = %d" "\n\n\t{\n", p_lst->now_size);

    for (int i = 0; i < p_lst->max_size; ++i)
    {
        if (DATA[i] != Poison)
            printf ("\t\t*[%d] = %d\n\n", i, DATA[i]);

        else
            printf ("\t\t [%d] = %d [POISON]\n\n", i, DATA[i]);
    }
    printf ("\t}\n}\n\n");
}

void DumpNext (const list* p_lst)
{
     for (int i = 0; i < p_lst->max_size; ++i)
    {
        printf ("\t\t NEXT[%d] = %d\n\n", i, NEXT[i]);
    }
    printf ("\t}\n}\n\n");
}

//рисовать в Graphviz'е в физической последовательности и ставить стрелки от элемента к элементу
void Print (list* p_lst)
{
    Verify (p_lst);

    FILE* f = fopen ("list.txt", "w");

    fprintf (f, "digraph G{\n\tsubgraph list{\n\t\trankdir=LR;\n\t\tModel [shape=record,label=\"");

    for (int i = 0; i < p_lst->max_size; i++)
    {
        fprintf (f, "{{<%d>} | {ElemPointer:\\n%p | {PhysPosition:\\n%d | Data:\\n%d | Next:\\n%d | <prev%d>Prev:\\n%d}}} |\n\t\t\t\t", i, &DATA[i], i, DATA[i], NEXT[i], i, PREV[i]);
    }

    fprintf (f, "\"];\n\t}\n");

    for (int i = p_lst->head; i != NEXT[p_lst->tail]; i = NEXT[i])
    {
        printf ("index of filled: %d\n", i);

        fprintf (f, "Model:<%d>->Model:<%d>[color=\"blue\";style=\"bold\"]\n", i, NEXT[i]);
    }

    for (int i = p_lst->head_free; i != 0; i = NEXT[i])
    {
        printf ("index of free: %d\n", i);
        fprintf (f, "Model:<prev%d>->Model:<prev%d>[color=\"pink\";style=\"bold\"]\n", i, NEXT[i]);
    }

    fprintf (f, "}");

    fclose (f);

    return;
}

void Unittest1 ()
{
    list lst = {};

    lstConstructor (&lst, Max_size);

    InsertBefore (&lst, 1, 1);

    InsertTail (&lst, 3);

    Delete (&lst, 1);

    Print (&lst);

    // Скачай отсюда Graphviz https://www2.graphviz.org/Packages/stable/windows/10/cmake/Release/x64/

    // системный вызов создания по текстовому файлу отсюда(D:\\Code\\List\\list.txt) рисунка, который ляжет сюда(D:\\Code\\List\\image.png)
    system ("\"D:\\Program Files\\Graphviz\\bin\\dot\" -Tpng D:\\Code\\List\\list.txt -o D:\\Code\\List\\image1.png");

    // открыть свежесозданную картинку на весь экран
    system ("start /max D:\\Code\\List\\image1.png");

    return;
}

void Unittest2 ()
{
    list lst = {};

    lstConstructor (&lst, Max_size);

    InsertHead (&lst, 1337);
    InsertHead (&lst, 11);

    int phys = InsertAfter (&lst, 1, 12);

    int phys2 = InsertAfter (&lst, phys, 300);

    InsertTail (&lst, 60);
    InsertTail (&lst, 70);

    Delete (&lst, lst.head);

    printf ("Find 30: %d\n", find (&lst, 30));

    Dump (&lst);
    DumpNext (&lst);

    Print (&lst);

    system ("\"D:\\Program Files\\Graphviz\\bin\\dot\" -Tpng D:\\Code\\List\\list.txt -o D:\\Code\\List\\image2.png");
    system ("start /max D:\\Code\\List\\image2.png");

    return;
}


int main ()
{
    Unittest1 ();
    Unittest2 ();

    return 0;
}


int InsertHead (list* p_lst, int val)
{
    if (p_lst->head_free != 0)
    {
        int after_head = p_lst->head;//индекс старой головы

        p_lst->head = p_lst->head_free;

        p_lst->head_free = NEXT[p_lst->head_free];

        PREV[after_head] = p_lst->head;

        DATA[p_lst->head] = val;
        NEXT[p_lst->head] = after_head;
        PREV[p_lst->head] = 0;

        p_lst->now_size++;

        if (p_lst->now_size == 1)
        {
            p_lst->tail = p_lst->head;
            NEXT[p_lst->head] = 0;
            NEXT[p_lst->tail] = 0;
        }

        return p_lst->head;
    }

    return -1;
}

int InsertTail (list* p_lst, int val)
{
    if (p_lst->head_free != 0)
    {
        NEXT[p_lst->tail] = p_lst->head_free;

        PREV[p_lst->head_free] = p_lst->tail;

        p_lst->tail = p_lst->head_free;
        DATA[p_lst->tail] = val;

        p_lst->now_size++;

        p_lst->head_free = NEXT[p_lst->head_free];
        NEXT[p_lst->tail] = 0;

        return p_lst->tail;
    }

    return -1;
}

int InsertAfter (list* p_lst, int after_which, elem_t val)
{
    if ((after_which == p_lst->tail) || (p_lst->now_size == 0) || (p_lst->now_size == 1))
            return InsertTail (p_lst, val);

    if (p_lst->head_free != 0)
    {
        int new_head_free = NEXT[p_lst->head_free];

        NEXT[p_lst->head_free] = NEXT[after_which];
        NEXT[after_which] = p_lst->head_free;

        PREV[p_lst->head_free] = after_which;
        PREV[NEXT[p_lst->head_free]] = p_lst->head_free;

        DATA[p_lst->head_free] = val;

        ++p_lst->now_size;

        int phys = p_lst->head_free;

        p_lst->head_free = new_head_free;

        return phys;
    }

    return -1;//in case of no free place to insert
}

int InsertBefore (list* p_lst, int before_which, elem_t val)
{
    if ((before_which == p_lst->head) || (p_lst->now_size == 0) || (p_lst->now_size == 1))
            return InsertHead (p_lst, val);

    return InsertAfter (p_lst, PREV[before_which], val);
}

bool Verify (list* p_lst)
{
    if ((p_lst->canary_begin != (int) '$') || (p_lst->canary_end != (int) '$'))
        return false;

    if (p_lst->now_size < 0)
        return false;

    int kol  = 0;
    for (int i = p_lst->head; i != NEXT[p_lst->tail]; i = NEXT[i])
        kol++;

    return true;
}

void DeleteAfter (list* p_lst, int after_which)
{
    Verify (p_lst);

    if (after_which == PREV[p_lst->tail]) //удалить хвост
    {
        Delete (p_lst, p_lst->tail);

        return;
    }

    int new_next = NEXT[NEXT[after_which]];

    PREV[new_next] = after_which;

    NEXT[after_which] = new_next;

    int ind = NEXT[after_which];

    NEXT[ind] = p_lst->head_free;
    p_lst->head_free = ind;

    DATA[ind] = Poison;
    PREV[ind] = -1;

    p_lst->now_size--;

    return;
}

void Delete (list* p_lst, int which)
{
    if (PREV[which] != -1)
    {
        Verify (p_lst);

        if (which == p_lst->head)
        {
            DATA[which] = Poison;
            PREV[which] = 0;

            if (NEXT[which] == 0) //в списке один элемент, который удаляем
            {
                p_lst->head = 1;
                p_lst->tail = 1;
            }

            else
                p_lst->head = NEXT[which];

            NEXT[which] = p_lst->head_free;
            p_lst->head_free = which;

            p_lst->now_size--;

            return;
        }

        if (which == p_lst->tail)
        {
            DATA[which] = Poison;
            PREV[which] = 0;

            p_lst->tail = PREV[which];

            NEXT[p_lst->tail] = 0;

            NEXT[which] = p_lst->head_free;
            p_lst->head_free = which;

            p_lst->now_size--;

            return;
        }

        DeleteAfter (p_lst, PREV[which]);
    }

    return;
}


