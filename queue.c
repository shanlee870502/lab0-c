#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

#ifndef strlcpy
#define strlcpy(dst, src, sz) snprintf((dst), (sz), "%s", (src))
#endif

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q) {
        return NULL;
    }
    q->size = 0;
    q->head = NULL;
    q->tail = NULL;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q) {
        return;
    }
    for (int i = 0; i < q->size; i++) {
        list_ele_t *tmp = q->head;
        q->head = q->head->next;
        free(tmp->value);
        free(tmp);
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q) {
        return false;
    }

    list_ele_t *newh = malloc(sizeof(list_ele_t));

    if (!newh) {
        return false;
    }
    int s_len = strlen(s) + 1;
    newh->value = (char *) malloc(s_len);
    if (!newh->value) {
        free(newh);
        return false;
    }
    strlcpy(newh->value, s, s_len);
    newh->next = q->head;
    q->head = newh;
    if (q->size == 0) {
        q->tail = newh;
    }
    q->size += 1;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q) {
        return false;
    }

    list_ele_t *newt = malloc(sizeof(list_ele_t));

    if (!newt) {
        return false;
    }

    int t_len = strlen(s) + 1;  // Including '\x00'
    newt->value = (char *) malloc(t_len);
    if (!newt->value) {
        free(newt);
        return false;
    }
    strncpy(newt->value, s, t_len);
    newt->next = NULL;
    if (q->size == 0) {
        q->head = newt;
    } else {
        q->tail->next = newt;
    }
    q->tail = newt;
    q->size += 1;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    list_ele_t *tmp;
    if (!q || !q->head || q->size == 0) {
        return false;
    }
    if (sp) {
        strlcpy(sp, q->head->value, bufsize);
    }
    tmp = q->head;
    q->head = tmp->next;
    if (tmp == q->tail) {
        q->tail = NULL;
    }
    q->size--;
    free(tmp->value);
    free(tmp);
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (!q || !q->head) {
        return 0;
    }
    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || q->size < 2) {
        return;
    }
    list_ele_t *prev, *curr, *next;
    prev = NULL;
    curr = q->head;

    while (curr) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    q->tail = q->head;
    q->head = prev;
}
list_ele_t *merge(list_ele_t *left, list_ele_t *right)
{
    list_ele_t *tmp, *head;

    if (strcmp(left->value, right->value) < 0) {
        head = left;
        left = left->next;
    } else {
        head = right;
        right = right->next;
    }
    tmp = head;
    while (left && right) {
        if (strcmp(left->value, right->value) < 0) {
            tmp->next = left;
            tmp = tmp->next;
            left = left->next;
        } else {
            tmp->next = right;
            tmp = tmp->next;
            right = right->next;
        }
    }
    if (left) {
        tmp->next = left;
    }
    if (right) {
        tmp->next = right;
    }
    return head;
}
list_ele_t *mergeSortList(list_ele_t *head)
{
    // Divide until only one list_element in splited list
    if (!head || !head->next) {
        return head;
    }
    list_ele_t *fast = head->next;
    list_ele_t *slow = head;

    // Split list (using Floyd Cycle Detection Algorithm)
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;

    list_ele_t *left = mergeSortList(head);
    list_ele_t *right = mergeSortList(fast);
    // Conquer the list
    return merge(left, right);
}
/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q || q->size < 2) {
        return;
    }
    q->head = mergeSortList(q->head);
    while (q->tail->next) {
        q->tail = q->tail->next;
    }
}
