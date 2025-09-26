#include <stdio.h>
#include <stdlib.h>


        // 1
        //  \
        //   2
        //  / \
        // 3   4
        //    /
        //   5
        //    \
        //     6

struct vertex
{
    int data;
    struct vertex *left;
    struct vertex *right;
};

void Obhod1(struct vertex *p)
{
    if (p != NULL)
    {
        printf("%d ", p->data);
        Obhod1(p->left);
        Obhod1(p->right);
    }
}

void Obhod2(struct vertex *p)
{
    if (p != NULL)
    {
        Obhod2(p->left);
        printf("%d ", p->data);
        Obhod2(p->right);
    }
}

void Obhod3(struct vertex *p)
{
    if (p != NULL)
    {
        Obhod3(p->left);
        Obhod3(p->right);
        printf("%d ", p->data);
    }
}


void freeTree(struct vertex *p)
{
    if (p != NULL)
    {
        freeTree(p->left);
        freeTree(p->right);
        free(p);
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
    int rigthHeight = h(p->right);
    return 1 + (leftHeight > rigthHeight ? leftHeight : rigthHeight);
}

int TotalSum(struct vertex *p, int L)
{
    if (p == NULL)
    {
        return 0;
    }
    return L + TotalSum(p->left, L + 1) + TotalSum(p->right, L + 1);
}

int main()
{
    struct vertex *root = (struct vertex *)malloc(sizeof(struct vertex));
    root->data = 1;

    root->right = (struct vertex *)malloc(sizeof(struct vertex));
    root->right->data = 2;
    root->right->left = NULL;
    root->right->right = NULL;

    root->right->left = (struct vertex *)malloc(sizeof(struct vertex));
    root->right->left->data = 3;
    root->right->left->left = NULL;
    root->right->left->right = NULL;

    root->right->right = (struct vertex *)malloc(sizeof(struct vertex));
    root -> right -> right -> data = 4;
    root -> right -> right -> left = NULL;
    root -> right -> right -> right = NULL;

    root -> right -> right -> left = (struct vertex *)malloc(sizeof(struct vertex));
    root -> right -> right -> left -> data = 5;
    root -> right -> right -> left -> left = NULL;
    root -> right -> right -> left -> right = NULL;

    root -> right -> right -> left -> right = (struct vertex *)malloc(sizeof(struct vertex));
    root -> right -> right -> left -> right -> data = 6;
    root -> right -> right -> left -> right -> left = NULL;
    root -> right -> right -> left -> right -> right = NULL;

    root -> right -> right -> left -> left = (struct vertex *)malloc(sizeof(struct vertex));
    root -> right -> right -> left -> left -> data = 7;
    root -> right -> right -> left -> right -> left = NULL;
    root -> right -> right -> left -> right -> right = NULL;



    printf("Обход дерева (сверху вниз, pre-order): ");
    Obhod1(root);
    printf("\n");

    printf("Обход дерева (слева направо, in-order): ");
    Obhod2(root);
    printf("\n");

    printf("Обход дерева (снизу вверх, post-order): ");
    Obhod3(root);
    printf("\n");

    printf("Размер дерева: %d\n", size(root));
    printf("Контрольная сумма: %d\n", sum(root));
    printf("Высота: %d\n", h(root));
    printf("Средняя высота дерева: %.2f\n", (double)TotalSum(root, 0) / size(root));

    freeTree(root);

    return 0;
}
