#include <stdlib.h>
#include <assert.h>
#include "../include/list.h"

#include "../private/common.h"
#include <time.h>

static uint16_t values[256];

void printAllValues(struct list_head *head)
{
    struct listitem *item, *is = NULL;
    size_t i = 0;
    list_for_each_entry_safe (item, is, head, list) {
        printf("the %ld is %d\n", i, item->i);
        i++;
    }
}

void delay()
{
    unsigned int r = time(0) + 1;
    while (time(0) < r)
        ;
}

static void list_mergesort(struct list_head *head)
{
    if (list_empty(head) || list_is_singular(head)) {
        return;
    }

    struct list_head list_left, list_right;
    INIT_LIST_HEAD(&list_left);
    INIT_LIST_HEAD(&list_right);

    struct listitem *left_item = NULL, *right_item = NULL;
    // split the list to two list
    while (!list_empty(head)) {
        left_item = list_first_entry(head, struct listitem, list);
        right_item = list_last_entry(head, struct listitem, list);
        list_del(&left_item->list);
        list_del(&right_item->list);
        list_add(&right_item->list, &list_right);
        list_add_tail(&left_item->list, &list_left);
        // list_del(&left_item->list);
        // list_del(&right_item->list);
        if (list_is_singular(head)) {
            left_item = list_first_entry(head, struct listitem, list);
            list_del(&left_item->list);
            list_add_tail(&left_item->list, &list_left);
        }
    }
    list_mergesort(&list_left);
    list_mergesort(&list_right);

    while (!list_empty(&list_left) || !list_empty(&list_right)) {
        if (list_empty(&list_left)) {
            list_splice_tail(&list_right, head);
            break;
        } else if (list_empty(&list_right)) {
            list_splice_tail(&list_left, head);
            break;
        }
        // try and error
        struct listitem *l =
                            list_first_entry(&list_left, struct listitem, list),
                        *r = list_first_entry(&list_right, struct listitem,
                                              list);
        if (cmpint(&l->i, &r->i) < 0) {
            list_del(&l->list);
            list_add_tail(&l->list, head);
        } else {
            list_del(&r->list);
            list_add_tail(&r->list, head);
        }
    }
    // free(list_left);
    // free(list_right);
}

int main(void)
{
    struct list_head testlist;
    struct listitem *item, *is = NULL;
    size_t i;

    random_shuffle_array(values, (uint16_t) ARRAY_SIZE(values));

    INIT_LIST_HEAD(&testlist);

    assert(list_empty(&testlist));

    for (i = 0; i < ARRAY_SIZE(values); i++) {
        item = (struct listitem *) malloc(sizeof(*item));
        assert(item);
        item->i = values[i];
        list_add_tail(&item->list, &testlist);
    }
    assert(!list_empty(&testlist));

    qsort(values, ARRAY_SIZE(values), sizeof(values[0]), cmpint);
    printAllValues(&testlist);
    printf("-------------------------------------------------------\n");
    list_mergesort(&testlist);
    printAllValues(&testlist);

    i = 0;
    list_for_each_entry_safe (item, is, &testlist, list) {
        assert(item->i == values[i]);
        list_del(&item->list);
        free(item);
        i++;
    }
    assert(i == ARRAY_SIZE(values));
    assert(list_empty(&testlist));
    return 0;
}
