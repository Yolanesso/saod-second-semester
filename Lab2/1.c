#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct vertex{
    int data;
    struct vertex *left;
    struct vertex *right;
};

struct vertex* ISDP(int L, int R, int A[]){
    if (L > R){
        return NULL;
    }
    int m = (L + R + 1)/2;
    struct vertex* p = (struct vertex*)malloc(sizeof(struct vertex));
    p -> data = A[m];
    p -> left = ISDP (L, m - 1, A);
    p -> right = ISDP (m + 1, R, A);
    return p;
}

void FillInc(int size, int arr[])
{
    for (int i = 0; i < size; i++)
    {
        arr[i] = i;
    }
}


void Obhod(struct vertex *p){
    if (p != NULL)
    {
        Obhod(p->left);
        printf("%d ", p->data);
        Obhod(p->right);
    }
}

int size(struct vertex *p)
{
    if (p == NULL)
    {
        return 0;
    }
    return 1 + size(p->left) + size(p->right);
}

int sum(struct vertex *p)
{
    if (p == NULL)
    {
        return 0;
    }
    return p->data + sum(p->left) + sum(p->right);
}

int h(struct vertex *p)
{
    if (p == NULL)
    {
        return 0;
    }
    int leftHeight = h(p->left);
    int rightHeight = h(p->right);
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

int TotalSum(struct vertex *p, int L)
{
    if (p == NULL)
    {
        return 0;
    }
    return L + TotalSum(p->left, L + 1) + TotalSum(p->right, L + 1);
}

void freeTree(struct vertex* p) {
    if (p == NULL) return;
    freeTree(p->left);
    freeTree(p->right);
    free(p);
}

int main(){
    srand(time(NULL));

    int n = 100;
    int A[100];

    FillInc(n, A);

    struct vertex* root = ISDP(0, n-1, A);

    printf("Обход слева направа: ");
    Obhod(root);
    printf("\n");

    printf("Размер: %d\n", size(root));
    printf("Контрольная сумма: %d\n", sum(root));
    printf("Высота: %d\n", h(root));
    printf("Средняя высота дерева: %.2f\n", (double)TotalSum(root, 0) / size(root));

    freeTree(root);

}
