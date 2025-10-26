#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Структура вершины дерева
typedef struct Vertex {
    int data;
    struct Vertex *left;
    struct Vertex *right;
    int bal;
} Vertex;

// === ЗАДАНИЕ 5: Функция вычисления количества уровней ===
int countLevels(Vertex *root) {
    if (root == NULL) {
        return 0;
    }
    
    int leftLevels = countLevels(root->left);
    int rightLevels = countLevels(root->right);
    
    return (leftLevels > rightLevels ? leftLevels : rightLevels) + 1;
}

// Функция для детального анализа уровней
void analyzeLevels(Vertex *root, int currentLevel, int levelCounts[]) {
    if (root == NULL) {
        return;
    }
    
    levelCounts[currentLevel]++;
    analyzeLevels(root->left, currentLevel + 1, levelCounts);
    analyzeLevels(root->right, currentLevel + 1, levelCounts);
}

// Функция для вывода распределения вершин по уровням
void printLevelDistribution(Vertex *root) {
    int height = countLevels(root);
    int *levelCounts = (int*)calloc(height, sizeof(int));
    
    analyzeLevels(root, 0, levelCounts);
    
    printf("\nРаспределение вершин по уровням:\n");
    printf("Уровень | Количество вершин\n");
    printf("--------+------------------\n");
    for (int i = 0; i < height; i++) {
        printf("   %2d   |       %2d\n", i + 1, levelCounts[i]);
    }
    
    free(levelCounts);
}

int B2INSERT(int D, Vertex **p, int *VR, int *HR) {
    if (*p == NULL) {
        *p = (Vertex*)malloc(sizeof(Vertex));
        if (*p == NULL) return 0;
        (*p)->data = D;
        (*p)->left = (*p)->right = NULL;
        (*p)->bal = 0;
        *VR = 1;
        return 1;
    }
    else if ((*p)->data > D) {
        if (!B2INSERT(D, &((*p)->left), VR, HR)) return 0;
        
        if (*VR == 1) {
            if ((*p)->bal == 0) {
                Vertex *q = (*p)->left;
                (*p)->left = q->right;
                q->right = *p;
                *p = q;
                (*p)->bal = 1;
                *VR = 0;
                *HR = 1;
            }
            else {
                (*p)->bal = 0;
                *VR = 1;
                *HR = 0;
            }
        }
        else {
            *HR = 0;
        }
    }
    else if ((*p)->data < D) {
        if (!B2INSERT(D, &((*p)->right), VR, HR)) return 0;
        
        if (*VR == 1) {
            (*p)->bal = 1;
            *HR = 1;
            *VR = 0;
        }
        else {
            if (*HR == 1) {
                if ((*p)->bal == 1) {
                    Vertex *q = (*p)->right;
                    (*p)->bal = 0;
                    q->bal = 0;
                    (*p)->right = q->left;
                    q->left = *p;
                    *p = q;
                    *VR = 1;
                    *HR = 0;
                }
                else {
                    *HR = 0;
                }
            }
        }
    }
    else {
        return 0;
    }
    
    return 1;
}

int insertDBD(Vertex **root, int data) {
    int VR = 1, HR = 1;
    return B2INSERT(data, root, &VR, &HR);
}

// === АВЛ Дерево для сравнения ===

void LL_rotate(Vertex **p) {
    Vertex *q = (*p)->left;
    (*p)->bal = 0;
    q->bal = 0;
    (*p)->left = q->right;
    q->right = *p;
    *p = q;
}

void RR_rotate(Vertex **p) {
    Vertex *q = (*p)->right;
    (*p)->bal = 0;
    q->bal = 0;
    (*p)->right = q->left;
    q->left = *p;
    *p = q;
}

void LR_rotate(Vertex **p) {
    Vertex *q = (*p)->left;
    Vertex *r = q->right;
    
    if (r->bal < 0) {
        (*p)->bal = 1;
    } else {
        (*p)->bal = 0;
    }
    
    if (r->bal > 0) {
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

void RL_rotate(Vertex **p) {
    Vertex *q = (*p)->right;
    Vertex *r = q->left;
    
    if (r->bal > 0) {
        (*p)->bal = -1;
    } else {
        (*p)->bal = 0;
    }
    
    if (r->bal < 0) {
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

int insertAVL(int D, Vertex **p, int *Rost) {
    if (*p == NULL) {
        *p = (Vertex*)malloc(sizeof(Vertex));
        if (*p == NULL) return 0;
        (*p)->data = D;
        (*p)->left = NULL;
        (*p)->right = NULL;
        (*p)->bal = 0;
        *Rost = 1;
        return 1;
    }
    else if ((*p)->data > D) {
        if (!insertAVL(D, &((*p)->left), Rost)) return 0;
        
        if (*Rost == 1) {
            if ((*p)->bal > 0) {
                (*p)->bal = 0;
                *Rost = 0;
            }
            else if ((*p)->bal == 0) {
                (*p)->bal = -1;
                *Rost = 1;
            }
            else {
                if ((*p)->left->bal < 0) {
                    LL_rotate(p);
                    *Rost = 0;
                } else {
                    LR_rotate(p);
                    *Rost = 0;
                }
            }
        }
    }
    else if ((*p)->data < D) {
        if (!insertAVL(D, &((*p)->right), Rost)) return 0;
        
        if (*Rost == 1) {
            if ((*p)->bal < 0) {
                (*p)->bal = 0;
                *Rost = 0;
            }
            else if ((*p)->bal == 0) {
                (*p)->bal = 1;
                *Rost = 1;
            }
            else {
                if ((*p)->right->bal > 0) {
                    RR_rotate(p);
                    *Rost = 0;
                } else {
                    RL_rotate(p);
                    *Rost = 0;
                }
            }
        }
    }
    else {
        return 0;
    }
    
    return 1;
}

int startInsertAVL(Vertex **root, int data) {
    int Rost = 1;
    return insertAVL(data, root, &Rost);
}

int treeSize(Vertex *root) {
    if (root == NULL) return 0;
    return treeSize(root->left) + treeSize(root->right) + 1;
}

int checkSum(Vertex *root) {
    if (root == NULL) return 0;
    return checkSum(root->left) + root->data + checkSum(root->right);
}

int treeHeight(Vertex *root) {
    if (root == NULL) return 0;
    int leftHeight = treeHeight(root->left);
    int rightHeight = treeHeight(root->right);
    return (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
}

int sumHeights(Vertex *root, int currentHeight) {
    if (root == NULL) return 0;
    return currentHeight + 
           sumHeights(root->left, currentHeight + 1) + 
           sumHeights(root->right, currentHeight + 1);
}

float averageHeight(Vertex *root) {
    int size = treeSize(root);
    if (size == 0) return 0;
    int totalHeight = sumHeights(root, 1);
    return (float)totalHeight / size;
}

void freeTree(Vertex *root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

void generateUniqueRandom(int arr[], int n, int min, int max) {
    int *used = (int*)calloc(max - min + 1, sizeof(int));
    int count = 0;
    
    while (count < n) {
        int num = rand() % (max - min + 1) + min;
        if (!used[num - min]) {
            used[num - min] = 1;
            arr[count] = num;
            count++;
        }
    }
    free(used);
}

void printGeneratedNumbers(int arr[], int n) {
    printf("Сгенерированные случайные числа (%d элементов):\n", n);
    printf("========================================\n");
    for (int i = 0; i < n; i++) {
        printf("%4d", arr[i]);
        if ((i + 1) % 10 == 0) {
            printf("\n");
        }
        else {
            printf(" ");
        }
    }
    if (n % 10 != 0) {
        printf("\n");
    }
    printf("========================================\n\n");
}

void inOrderTraversal(Vertex *root) {
    if (root != NULL) {
        inOrderTraversal(root->left);
        printf("%d ", root->data);
        inOrderTraversal(root->right);
    }
}

void inOrderTraversalFormatted(Vertex *root, int *count) {
    if (root != NULL) {
        inOrderTraversalFormatted(root->left, count);
        
        printf("%4d", root->data);
        (*count)++;
        
        if (*count % 10 == 0) {
            printf("\n");
        }
        else {
            printf(" ");
        }
        
        inOrderTraversalFormatted(root->right, count);
    }
}

void printComparisonTable(int n, int sizeDBD, int sizeAVL, 
                         int sumDBD, int sumAVL, 
                         int heightDBD, int heightAVL,
                         float avgHeightDBD, float avgHeightAVL) {
    printf("n=100     Размер   Контр. Сумма    Высота  Средн.высота\n");
    printf("АВЛ      %-8d %-13d %-7d %-11.2f\n", 
           sizeAVL, sumAVL, heightAVL, avgHeightAVL);
    printf("ДБД      %-8d %-13d %-7d %-11.2f\n", 
           sizeDBD, sumDBD, heightDBD, avgHeightDBD);
}

int main() {
    srand(time(NULL));
    
    Vertex *rootDBD = NULL;
    const int NUM_VERTICES = 100;
    int elements[NUM_VERTICES];
    
    generateUniqueRandom(elements, NUM_VERTICES, 1, 1000);
    printGeneratedNumbers(elements, NUM_VERTICES);
    
    
    for (int i = 0; i < NUM_VERTICES; i++) {
        insertDBD(&rootDBD, elements[i]);
    }
    
    printf("Обход дерева слева направо (in-order):\n");
    printf("========================================\n");
    
    int count = 0;
    inOrderTraversalFormatted(rootDBD, &count);
    
    if (count % 10 != 0) {
        printf("\n");
    }
    
    printf("========================================\n");
    printf("Всего элементов в обходе: %d\n\n", count);
    
    // === ЗАДАНИЕ 5: Вывод информации об уровнях ДБД ===
    printf("=== АНАЛИЗ УРОВНЕЙ ДБД ===\n");
    int levelsDBD = countLevels(rootDBD);
    printf("Количество уровней в ДБД: %d\n", levelsDBD);
    printLevelDistribution(rootDBD);
    
    printf("=== Сравнение характеристик ДБД и АВЛ-деревьев ===\n");
    
    Vertex *rootAVL = NULL;
    for (int i = 0; i < NUM_VERTICES; i++) {
        startInsertAVL(&rootAVL, elements[i]);
    }
    
    int sizeDBD = treeSize(rootDBD);
    int sumDBD = checkSum(rootDBD);
    int heightDBD = treeHeight(rootDBD);
    float avgHeightDBD = averageHeight(rootDBD);
    
    int sizeAVL = treeSize(rootAVL);
    int sumAVL = checkSum(rootAVL);
    int heightAVL = treeHeight(rootAVL);
    float avgHeightAVL = averageHeight(rootAVL);
    
    freeTree(rootDBD);
    freeTree(rootAVL);
    
    return 0;
}