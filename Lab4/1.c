#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct vertex
{
    int data;
    struct vertex *left;
    struct vertex *right;
};

void DeleteVertex(struct vertex **Root, int D)
{
    struct vertex **p = Root;
    
    while (*p != NULL)
    {
        if (D < (*p)->data)
            p = &((*p)->left);
        else if (D > (*p)->data)
            p = &((*p)->right);
        else
            break;
    }
    
    if (*p != NULL)
    {
        struct vertex *q = *p;
        
        if (q->left == NULL)
        {
            *p = q->right;
        }
        else if (q->right == NULL)
        {
            *p = q->left;
        }
        else
        {
            struct vertex *r = q->left;
            struct vertex *s = q;
            
            if (r->right == NULL)
            {
                r->right = q->right;
                *p = r;
            }
            else
            {
                while (r->right != NULL)
                {
                    s = r;
                    r = r->right;
                }
                
                s->right = r->left;
                r->left = q->left;
                r->right = q->right;
                *p = r;
            }
        }
        
        free(q);
        printf("🟢 Вершина %d успешно удалена из дерева\n", D);
    }
    else
    {
        printf("🔴 Вершина %d не найдена в дереве\n", D);
    }
}

void addDoubleIndirect(struct vertex **root, int data)
{
    struct vertex **p = root;

    while (*p != NULL)
    {
        if (data < (*p)->data)
            p = &((*p)->left);
        else if (data > (*p)->data)
            p = &((*p)->right);
        else
            return;
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

void LeftToRightTraversal(struct vertex *p)
{
    if (p != NULL)
    {
        LeftToRightTraversal(p->left);
        printf("%d ", p->data);
        LeftToRightTraversal(p->right);
    }
}

void freeTree(struct vertex *p)
{
    if (p == NULL)
        return;
    freeTree(p->left);
    freeTree(p->right);
    free(p);
}

int countNodes(struct vertex *p)
{
    if (p == NULL)
        return 0;
    return 1 + countNodes(p->left) + countNodes(p->right);
}


int main()
{
    srand(time(NULL));
    
    int size_arr = 25;
    int A[size_arr];
    
    FillRand(size_arr, A);
    
    struct vertex *root = NULL;
    for (int i = 0; i < size_arr; i++)
    {
        addDoubleIndirect(&root, A[i]);
    }
    
    printf("┌─────────────────────────────────────────────┐\n");
    printf("│        УДАЛЕНИЕ ВЕРШИН ИЗ СДП              │\n");
    printf("└─────────────────────────────────────────────┘\n\n");
    
    printf("📊 ИСХОДНОЕ ДЕРЕВО:\n");
    printf("├─ Количество вершин: %d\n", countNodes(root));
    printf("├─ Обход слева направо: ");
    LeftToRightTraversal(root);
    printf("\n└─ Элементы: ");
    for (int i = 0; i < size_arr; i++) {
        printf("%d", A[i]);
        if (i < size_arr - 1) printf(", ");
    }
    printf("\n\n");
    
    printf("═══════════════════════════════════════════════\n\n");
    
    for (int i = 1; i <= 10; i++)
    {
        int key;
        printf("🚀 ШАГ %d/10\n", i);
        printf("├─ Введите ключ для удаления: ");
        scanf("%d", &key);
        
        printf("├─ Результат: ");
        DeleteVertex(&root, key);
        
        printf("├─ Обход слева направо: ");
        if (root != NULL) {
            LeftToRightTraversal(root);
        } else {
            printf("∅ (дерево пустое)");
        }
        printf("\n├─ Количество вершин: %d\n", countNodes(root));

        printf("\n");
        
        if (root == NULL)
        {
            printf("└─ ⚠️  Дерево стало пустым\n");
            break;
        }
        printf("\n");
    }
    
    printf("═══════════════════════════════════════════════\n\n");
    
    printf("🎯 РЕЗУЛЬТАТЫ УДАЛЕНИЯ:\n");
    printf("├─ Финальный обход слева направо: ");
    if (root != NULL) {
        LeftToRightTraversal(root);
    } else {
        printf("∅");
    }
    printf("\n\n");

    
    freeTree(root);
    return 0;
}