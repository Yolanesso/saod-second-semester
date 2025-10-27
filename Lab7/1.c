#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Цветовые коды
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_MAGENTA "\033[1;35m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_WHITE   "\033[1;37m"
#define COLOR_GRAY    "\033[1;90m"

#define BG_BLUE       "\033[44m"
#define BG_GREEN      "\033[42m"
#define BG_YELLOW     "\033[43m"

typedef struct Vertex {
    int data;
    struct Vertex *left;
    struct Vertex *right;
    int bal;
} Vertex;

int countLevels(Vertex *root) {
    if (root == NULL) {
        return 0;
    }
    
    int leftLevels = countLevels(root->left);
    int rightLevels = countLevels(root->right);
    
    return (leftLevels > rightLevels ? leftLevels : rightLevels) + 1;
}

void analyzeLevels(Vertex *root, int currentLevel, int levelCounts[]) {
    if (root == NULL) {
        return;
    }
    
    levelCounts[currentLevel]++;
    analyzeLevels(root->left, currentLevel + 1, levelCounts);
    analyzeLevels(root->right, currentLevel + 1, levelCounts);
}

void printLevelDistribution(Vertex *root) {
    int height = countLevels(root);
    int *levelCounts = (int*)calloc(height, sizeof(int));
    
    analyzeLevels(root, 0, levelCounts);
    
    printf(COLOR_CYAN "\n┌────────────────────────────────────────┐" COLOR_RESET);
    printf(COLOR_CYAN "\n│" COLOR_YELLOW "      Распределение вершин по уровням     " COLOR_CYAN "│" COLOR_RESET);
    printf(COLOR_CYAN "\n├──────────┬─────────────────────────────┤" COLOR_RESET);
    printf(COLOR_CYAN "\n│" COLOR_WHITE "  Уровень " COLOR_CYAN "│" COLOR_WHITE "     Количество вершин     " COLOR_CYAN "│" COLOR_RESET);
    printf(COLOR_CYAN "\n├──────────┼─────────────────────────────┤" COLOR_RESET);
    
    for (int i = 0; i < height; i++) {
        printf(COLOR_CYAN "\n│" COLOR_GREEN "    %2d    " COLOR_CYAN "│" COLOR_WHITE "             %2d             " COLOR_CYAN "│" COLOR_RESET, 
               i + 1, levelCounts[i]);
    }
    
    printf(COLOR_CYAN "\n└──────────┴─────────────────────────────┘" COLOR_RESET);
    printf("\n");
    
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
        
        //Высота увеличилась и узел был сбалансирован
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
            //Высота увеличилась и узел уже имел перевес
            else {
                (*p)->bal = 0;
                *VR = 1;
                *HR = 0;
            }
        }
        else {
            *HR = 0; //Высота левого поддерева НЕ увеличилась, поэтому балансировка на этом уровне НЕ требуется
        }
    }
    else if ((*p)->data < D) {
        if (!B2INSERT(D, &((*p)->right), VR, HR)) return 0;
        
        if (*VR == 1) { 
            (*p)->bal = 1; // Отмечаем перевес вправо
            *HR = 1; // Требуется балансировка
            *VR = 0; // Высота не растет дальше
        }
        else {
        //Требуется балансировка и есть перевес вправо
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
                    *HR = 0; // Балансировка не требуется - узел стал сбалансированным
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
    printf(COLOR_CYAN "\n╔══════════════════════════════════════════════════════════════╗" COLOR_RESET);
    printf(COLOR_CYAN "\n║" COLOR_YELLOW "         СГЕНЕРИРОВАННЫЕ СЛУЧАЙНЫЕ ЧИСЛА (%d элементов)       " COLOR_CYAN "║" COLOR_RESET, n);
    printf(COLOR_CYAN "\n╠══════════════════════════════════════════════════════════════╣" COLOR_RESET);
    
    for (int i = 0; i < n; i++) {
        printf(COLOR_GREEN "%4d" COLOR_RESET, arr[i]);
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
    printf(COLOR_CYAN "╚══════════════════════════════════════════════════════════════╝" COLOR_RESET);
    printf("\n\n\n");
}

void inOrderTraversalFormatted(Vertex *root, int *count) {
    if (root != NULL) {
        inOrderTraversalFormatted(root->left, count);
        
        printf(COLOR_GREEN "%4d" COLOR_RESET, root->data);
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

    printf(COLOR_CYAN "\n┌────────────────────────────────────────────────────────────────┐" COLOR_RESET);
    printf(COLOR_CYAN "\n│" COLOR_YELLOW "                   ОБХОД ДЕРЕВА СЛЕВА НАПРАВО                   " COLOR_CYAN "│" COLOR_RESET);
    printf(COLOR_CYAN "\n└────────────────────────────────────────────────────────────────┘" COLOR_RESET);
    printf("\n");
    
    int count = 0;
    inOrderTraversalFormatted(rootDBD, &count);
    
    if (count % 10 != 0) {
        printf("\n");
    }
    
    printf(COLOR_CYAN "┌────────────────────────────────────────────────────────────────┐" COLOR_RESET);
    printf(COLOR_CYAN "\n│" COLOR_WHITE "         Всего элементов в обходе: " COLOR_GREEN "%-3d" COLOR_WHITE "                     " COLOR_CYAN "│" COLOR_RESET, count);
    printf(COLOR_CYAN "\n└────────────────────────────────────────────────────────────────┘" COLOR_RESET);
    printf("\n");
    
    printf(COLOR_CYAN "\n┌────────────────────────────────────────────────────────────────┐" COLOR_RESET);
    printf(COLOR_CYAN "\n│" COLOR_YELLOW "                     АНАЛИЗ УРОВНЕЙ ДБД                       " COLOR_CYAN "│" COLOR_RESET);
    printf(COLOR_CYAN "\n└────────────────────────────────────────────────────────────────┘" COLOR_RESET);
    
    int levelsDBD = countLevels(rootDBD);
    printf(COLOR_GREEN "\n🎯 Количество уровней в ДБД: " COLOR_YELLOW "%d" COLOR_RESET, levelsDBD);
    printLevelDistribution(rootDBD);
    
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
    
    printf(COLOR_CYAN "\n╔══════════════════════════════════════════════════════════════╗" COLOR_RESET);
    printf(COLOR_CYAN "\n║" COLOR_YELLOW "                   СРАВНИТЕЛЬНАЯ ТАБЛИЦА                    " COLOR_CYAN "║" COLOR_RESET);
    printf(COLOR_CYAN "\n╠══════════╦══════════╦══════════════╦═════════╦══════════════╣" COLOR_RESET);
    printf(COLOR_CYAN "\n║" COLOR_WHITE "  n=100   " COLOR_CYAN "║" COLOR_WHITE "  Размер  " COLOR_CYAN "║" COLOR_WHITE " Контр. Сумма " COLOR_CYAN "║" COLOR_WHITE " Высота  " COLOR_CYAN "║" COLOR_WHITE " Средн.высота " COLOR_CYAN "║" COLOR_RESET);
    printf(COLOR_CYAN "\n╠══════════╬══════════╬══════════════╬═════════╬══════════════╣" COLOR_RESET);
    printf(COLOR_CYAN "\n║" COLOR_YELLOW "   АВЛ    " COLOR_CYAN "║" COLOR_GREEN " %8d " COLOR_CYAN "║" COLOR_GREEN " %12d " COLOR_CYAN "║" COLOR_GREEN " %7d " COLOR_CYAN "║" COLOR_GREEN " %12.2f " COLOR_CYAN "║" COLOR_RESET, 
           sizeAVL, sumAVL, heightAVL, avgHeightAVL);
    printf(COLOR_CYAN "\n║" COLOR_MAGENTA "   ДБД    " COLOR_CYAN "║" COLOR_GREEN " %8d " COLOR_CYAN "║" COLOR_GREEN " %12d " COLOR_CYAN "║" COLOR_GREEN " %7d " COLOR_CYAN "║" COLOR_GREEN " %12.2f " COLOR_CYAN "║" COLOR_RESET, 
           sizeDBD, sumDBD, heightDBD, avgHeightDBD);
    printf(COLOR_CYAN "\n╚══════════╩══════════╩══════════════╩═════════╩══════════════╝" COLOR_RESET);
    printf("\n");
    freeTree(rootDBD);
    freeTree(rootAVL);
    
    return 0;
}