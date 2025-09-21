#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

struct vertex
{
    int data;
    struct vertex *left;
    struct vertex *right;
};

struct vertex *ISDP(int L, int R, int A[])
{
    if (L > R)
    {
        return NULL;
    }
    int m = (L + R + 1) / 2;
    struct vertex *p = (struct vertex *)malloc(sizeof(struct vertex));
    p->data = A[m];
    p->left = ISDP(L, m - 1, A);
    p->right = ISDP(m + 1, R, A);
    return p;
}

void FillInc(int size, int arr[])
{
    for (int i = 0; i < size; i++)
    {
        arr[i] = i;
    }
}

struct vertex *addRecursive(struct vertex *root, int data)
{
    if (root == NULL)
    {
        struct vertex *newNode = (struct vertex *)malloc(sizeof(struct vertex));
        newNode->data = data;
        newNode->left = NULL;
        newNode->right = NULL;
        return newNode;
    }

    if (data < root->data)
    {
        root->left = addRecursive(root->left, data);
    }
    else if (data > root->data)
    {
        root->right = addRecursive(root->right, data);
    }
    return root;
}

void addDoubleIndirect(struct vertex **root, int data)
{
    struct vertex **p = root;

    while (*p != NULL)
    {
        if (data < (*p)->data)
        {
            p = &((*p)->left);
        }
        else if (data > (*p)->data)
        {
            p = &((*p)->right);
        }
        else
        {
            return;
        }
    }
    *p = (struct vertex *)malloc(sizeof(struct vertex));
    (*p)->data = data;
    (*p)->left = NULL;
    (*p)->right = NULL;
}

void FillRand(int size, int arr[])
{
    for (int i = 0; i < size; i++)
    {
        arr[i] = rand() % 100 + 1;
    }
}

void Obhod(struct vertex *p)
{
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

void freeTree(struct vertex *p)
{
    if (p == NULL)
        return;
    freeTree(p->left);
    freeTree(p->right);
    free(p);
}

int main()
{
    srand(time(NULL));

    int n = 100;
    int A[100];
    int B[100];

    FillInc(n, B);
    FillRand(n, A);

    struct vertex *idealtree = ISDP(0, n - 1, B);
    struct vertex *tree1 = NULL; // рекурсия
    struct vertex *tree2 = NULL; // двойная косвенность

    for (int i = 0; i < n; i++)
    {
        tree1 = addRecursive(tree1, A[i]);
        addDoubleIndirect(&tree2, A[i]);
    }

    // Красивые заголовки для каждого дерева
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                      ИДЕАЛЬНОЕ СДП                          ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("Обход: ");
    Obhod(idealtree);
    printf("\n");
    printf("┌────────────┬────────────────┬──────────┬───────────────────┐\n");
    printf("│   Размер   │ Контр. сумма   │  Высота  │ Средняя высота    │\n");
    printf("├────────────┼────────────────┼──────────┼───────────────────┤\n");
    printf("│ %10d │ %14d │ %8d │ %17.2f │\n", 
           size(idealtree), sum(idealtree), h(idealtree), 
           (double)TotalSum(idealtree, 0) / size(idealtree));
    printf("└────────────┴────────────────┴──────────┴───────────────────┘\n\n");

    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                 РЕКУРСИВНОЕ ДОБАВЛЕНИЕ                       ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("Обход: ");
    Obhod(tree1);
    printf("\n");
    printf("┌────────────┬────────────────┬──────────┬───────────────────┐\n");
    printf("│   Размер   │ Контр. сумма   │  Высота  │ Средняя высота    │\n");
    printf("├────────────┼────────────────┼──────────┼───────────────────┤\n");
    printf("│ %10d │ %14d │ %8d │ %17.2f │\n", 
           size(tree1), sum(tree1), h(tree1), 
           (double)TotalSum(tree1, 0) / size(tree1));
    printf("└────────────┴────────────────┴──────────┴───────────────────┘\n\n");

    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║               ДВОЙНАЯ КОСВЕННОСТЬ                           ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("Обход: ");
    Obhod(tree2);
    printf("\n");
    printf("┌────────────┬────────────────┬──────────┬───────────────────┐\n");
    printf("│   Размер   │ Контр. сумма   │  Высота  │ Средняя высота    │\n");
    printf("├────────────┼────────────────┼──────────┼───────────────────┤\n");
    printf("│ %10d │ %14d │ %8d │ %17.2f │\n", 
           size(tree2), sum(tree2), h(tree2), 
           (double)TotalSum(tree2, 0) / size(tree2));
    printf("└────────────┴────────────────┴──────────┴───────────────────┘\n\n");

    // Сводная таблица сравнения
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                  СВОДНАЯ ТАБЛИЦА (n=%d)                     ║\n", n);
    printf("╠══════════════════════╦════════╦══════════════╦════════╦══════════════╣\n");
    printf("║      Тип дерева      ║ Размер ║ Сумма данных ║ Высота ║ Ср. высота   ║\n");
    printf("╠══════════════════════╬════════╬══════════════╬════════╬══════════════╣\n");
    printf("║ Идеальное СДП        ║ %6d ║ %12d ║ %6d ║ %11.2f ║\n", 
           size(idealtree), sum(idealtree), h(idealtree), 
           (double)TotalSum(idealtree, 0) / size(idealtree));
    printf("║ Рекурсивное СДП      ║ %6d ║ %12d ║ %6d ║ %11.2f ║\n", 
           size(tree1), sum(tree1), h(tree1), 
           (double)TotalSum(tree1, 0) / size(tree1));
    printf("║ Двойная косвенность  ║ %6d ║ %12d ║ %6d ║ %11.2f ║\n", 
           size(tree2), sum(tree2), h(tree2), 
           (double)TotalSum(tree2, 0) / size(tree2));
    printf("╚══════════════════════╩════════╩══════════════╩════════╩══════════════╝\n");
    
    freeTree(idealtree);
    freeTree(tree1);
    freeTree(tree2);
    
    return 0;
}
