#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// Цвета для консоли
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_MAGENTA "\033[1;35m"
#define COLOR_CYAN    "\033[1;36m"

// Структура вершины АВЛ-дерева
typedef struct Vertex {
    int data;
    int bal;
    struct Vertex *left;
    struct Vertex *right;
} Vertex;

// Флаги для балансировки
bool decrease;

// Функция создания новой вершины
Vertex* createVertex(int value) {
    Vertex* newVertex = (Vertex*)malloc(sizeof(Vertex));
    if (newVertex != NULL) {
        newVertex->data = value;
        newVertex->bal = 0;
        newVertex->left = NULL;
        newVertex->right = NULL;
    }
    return newVertex;
}

// LL-поворот
void LL_rotation(Vertex **p) {
    Vertex *q = (*p)->left;
    
    if (q->bal == 0) {
        q->bal = 1;
        (*p)->bal = -1;
        decrease = false;
    } else {
        q->bal = 0;
        (*p)->bal = 0;
    }
    
    (*p)->left = q->right;
    q->right = *p;
    *p = q;
}

// RR-поворот
void RR_rotation(Vertex **p) {
    Vertex *q = (*p)->right;
    
    if (q->bal == 0) {
        q->bal = -1;
        (*p)->bal = 1;
        decrease = false;
    } else {
        q->bal = 0;
        (*p)->bal = 0;
    }
    
    (*p)->right = q->left;
    q->left = *p;
    *p = q;
}

// LR-поворот
void LR_rotation(Vertex **p) {
    Vertex *q = (*p)->left;
    Vertex *r = q->right;
    
    if (r->bal == -1) {
        (*p)->bal = 1;
    } else {
        (*p)->bal = 0;
    }
    
    if (r->bal == 1) {
        q->bal = -1;
    } else {
        q->bal = 0;
    }
    
    r->bal = 0;
    q->right = r->left;
    (*p)->left = r->right;
    r->left = q;
    r->right = *p;
    *p = r;
}

// RL-поворот
void RL_rotation(Vertex **p) {
    Vertex *q = (*p)->right;
    Vertex *r = q->left;
    
    if (r->bal == 1) {
        (*p)->bal = -1;
    } else {
        (*p)->bal = 0;
    }
    
    if (r->bal == -1) {
        q->bal = 1;
    } else {
        q->bal = 0;
    }
    
    r->bal = 0;
    q->left = r->right;
    (*p)->right = r->left;
    r->right = q;
    r->left = *p;
    *p = r;
}

// Балансировка после удаления из левого поддерева
void BL(Vertex **p) {
    if ((*p) == NULL) return;
    
    if ((*p)->bal == -1) {
        (*p)->bal = 0;
    } else if ((*p)->bal == 0) {
        (*p)->bal = 1;
        decrease = false;
    } else if ((*p)->bal == 1) {
        if ((*p)->right != NULL && (*p)->right->bal >= 0) {
            RR_rotation(p);
        } else if ((*p)->right != NULL) {
            RL_rotation(p);
        }
    }
}

// Балансировка после удаления из правого поддерева
void BR(Vertex **p) {
    if ((*p) == NULL) return;
    
    if ((*p)->bal == 1) {
        (*p)->bal = 0;
    } else if ((*p)->bal == 0) {
        (*p)->bal = -1;
        decrease = false;
    } else if ((*p)->bal == -1) {
        if ((*p)->left != NULL && (*p)->left->bal <= 0) {
            LL_rotation(p);
        } else if ((*p)->left != NULL) {
            LR_rotation(p);
        }
    }
}

// Вспомогательная функция для удаления (поиск максимального элемента в левом поддереве)
void del(Vertex **r, Vertex **q) {
    if ((*r)->right != NULL) {
        del(&((*r)->right), q);
        if (decrease) {
            BR(r);
        }
    } else {
        (*q)->data = (*r)->data;
        *q = *r;
        *r = (*r)->left;
        decrease = true;
    }
}

// Основная функция удаления
void DELETE(int x, Vertex **p) {
    if (*p == NULL) {
        return;
    } else if (x < (*p)->data) {
        DELETE(x, &((*p)->left));
        if (decrease) {
            BL(p);
        }
    } else if (x > (*p)->data) {
        DELETE(x, &((*p)->right));
        if (decrease) {
            BR(p);
        }
    } else {
        Vertex *q = *p;
        if (q->left == NULL) {
            *p = q->right;
            decrease = true;
        } else if (q->right == NULL) {
            *p = q->left;
            decrease = true;
        } else {
            del(&(q->left), &q);
            if (decrease) {
                BL(p);
            }
        }
        free(q);
    }
}

// Простая функция вставки (без балансировки при создании)
bool insertSimple(Vertex **root, int value) {
    if (*root == NULL) {
        *root = createVertex(value);
        return true;
    }
    
    if (value < (*root)->data) {
        return insertSimple(&((*root)->left), value);
    } else if (value > (*root)->data) {
        return insertSimple(&((*root)->right), value);
    }
    
    return false;
}

// Обход дерева слева направо (симметричный обход)
void inOrderTraversal(Vertex *root) {
    if (root != NULL) {
        inOrderTraversal(root->left);
        printf("%d ", root->data);
        inOrderTraversal(root->right);
    }
}

// Функция для подсчета высоты дерева
int treeHeight(Vertex *root) {
    if (root == NULL) return 0;
    int leftHeight = treeHeight(root->left);
    int rightHeight = treeHeight(root->right);
    return (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
}

// Функция для подсчета количества вершин
int countVertices(Vertex *root) {
    if (root == NULL) return 0;
    return countVertices(root->left) + countVertices(root->right) + 1;
}

// Функция для проверки сбалансированности дерева
bool isTreeBalanced(Vertex *root) {
    if (root == NULL) return true;
    
    int leftHeight = treeHeight(root->left);
    int rightHeight = treeHeight(root->right);
    int balanceFactor = leftHeight - rightHeight;
    
    if (balanceFactor < -1 || balanceFactor > 1) {
        return false;
    }
    
    return isTreeBalanced(root->left) && isTreeBalanced(root->right);
}

// Функция для освобождения памяти
void freeTree(Vertex *root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

// Вывод разделительной линии
void printSeparator() {
    printf(COLOR_CYAN "---------------------------------------------------------------\n" COLOR_RESET);
}

// Вывод заголовка
void printHeader(const char* title) {
    printf("\n" COLOR_MAGENTA ">>> %s\n" COLOR_RESET, title);
}

int main() {
    Vertex *root = NULL;
    srand(time(NULL));
    
    printHeader("АВЛ-ДЕРЕВО - ДЕМОНСТРАЦИЯ УДАЛЕНИЯ");
    
    // Создаем начальное АВЛ-дерево с ровно 10 вершинами
    printf(COLOR_YELLOW "Создаем АВЛ-дерево с 10 вершинами...\n" COLOR_RESET);
    
    int values[10];
    int added = 0;
    
    // Генерируем 10 уникальных значений
    printf(COLOR_GREEN "Добавленные значения: " COLOR_RESET);
    while (added < 10) {
        int value = rand() % 50 + 1; // от 1 до 50
        bool duplicate = false;
        
        // Проверяем на дубликаты
        for (int i = 0; i < added; i++) {
            if (values[i] == value) {
                duplicate = true;
                break;
            }
        }
        
        if (!duplicate) {
            values[added] = value;
            insertSimple(&root, value);
            printf("%d ", value);
            added++;
        }
    }
    printf("\n");
    
    printSeparator();
    printHeader("НАЧАЛЬНОЕ СОСТОЯНИЕ ДЕРЕВА");
    
    printf(COLOR_GREEN "Симметричный обход (слева направо): " COLOR_RESET);
    inOrderTraversal(root);
    printf("\n");
    
    printf(COLOR_BLUE "Высота дерева: " COLOR_RESET "%d\n", treeHeight(root));
    printf(COLOR_BLUE "Количество вершин: " COLOR_RESET "%d\n", countVertices(root));
    
    
    
    // Удаляем 10 вершин
    printSeparator();
    printHeader("ПРОЦЕСС УДАЛЕНИЯ 10 ВЕРШИН");
    
    for (int i = 0; i < 10; i++) {
        int key;
        printf("\n" COLOR_YELLOW "УДАЛЕНИЕ %d/10\n" COLOR_RESET, i + 1);
        printf(COLOR_GREEN "Введите ключ для удаления: " COLOR_RESET);
        
        if (scanf("%d", &key) != 1) {
            printf(COLOR_RED "Ошибка ввода! Пожалуйста, введите число.\n" COLOR_RESET);
            while (getchar() != '\n');
            i--;
            continue;
        }
        
        int initialHeight = treeHeight(root);
        int initialCount = countVertices(root);
        
        decrease = true;
        DELETE(key, &root);
        
        printf(COLOR_GREEN "Результат удаления %d:\n" COLOR_RESET, key);
        printf(COLOR_GREEN "Симметричный обход: " COLOR_RESET);
        inOrderTraversal(root);
        printf("\n");
        
        printf(COLOR_BLUE "Вершин: " COLOR_RESET "%d", countVertices(root));
        if (countVertices(root) < initialCount) {
            printf(COLOR_GREEN " (удалена 1 вершина)\n" COLOR_RESET);
        } else {
            printf(COLOR_YELLOW " (вершина не найдена)\n" COLOR_RESET);
        }
        
        
        if (i < 9) {
            printSeparator();
        }
    }
    
    printSeparator();
    printHeader("ФИНАЛЬНОЕ СОСТОЯНИЕ ДЕРЕВА");
    
    if (root != NULL) {
        printf(COLOR_GREEN "Финальный симметричный обход: " COLOR_RESET);
        inOrderTraversal(root);
        printf("\n");
        
        printf(COLOR_BLUE "Финальная высота: " COLOR_RESET "%d\n", treeHeight(root));
        printf(COLOR_BLUE "Финальное количество вершин: " COLOR_RESET "%d\n", countVertices(root));
        
    } else {
        printf(COLOR_RED "Дерево пусто!\n" COLOR_RESET);
    }
    
    printf(COLOR_MAGENTA "\nПрограмма завершена успешно!\n" COLOR_RESET);
    
    // Освобождаем память
    freeTree(root);
    
    return 0;
}