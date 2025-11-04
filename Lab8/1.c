#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_N 100

typedef struct Node {
    int key;
    int weight;
    struct Node *left;
    struct Node *right;
} Node;

int AW[MAX_N+1][MAX_N+1];
int AP[MAX_N+1][MAX_N+1];
int AR[MAX_N+1][MAX_N+1];
int weights[MAX_N];

Node* createNode(int key, int weight) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->key = key;
    newNode->weight = weight;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

void computeAW(int n) {
    int i, j;
    for (i = 0; i <= n; i++) {
        AW[i][i] = 0;
        for (j = i+1; j <= n; j++) {
            AW[i][j] = AW[i][j-1] + weights[j-1];
        }
    }
}

void computeAPAR(int n) {
    int i, j, k, h;
    
    for (i = 0; i <= n; i++) {
        for (j = 0; j <= n; j++) {
            AP[i][j] = 0;
            AR[i][j] = 0;
        }
    }
    
    for (i = 0; i < n; i++) {
        j = i + 1;
        AP[i][j] = AW[i][j];
        AR[i][j] = j;
    }
    
    for (h = 2; h <= n; h++) {
        for (i = 0; i <= n - h; i++) {
            j = i + h;

            int m = AR[i][j-1];     
            int min_val = AP[i][m-1] + AP[m][j];
            
            int max_k = (AR[i+1][j] > 0) ? AR[i+1][j] : j;
            for (k = m+1; k <= max_k; k++) {
                int x = AP[i][k-1] + AP[k][j];
                if (x < min_val) {
                    m = k;
                    min_val = x;
                }
            }
            
            AP[i][j] = min_val + AW[i][j];
            AR[i][j] = m;
        }
    }
}

Node* createTree(int L, int R) {
    if (L < R) {
        int k = AR[L][R];
        Node* root = createNode(k, weights[k-1]);
        root->left = createTree(L, k-1);
        root->right = createTree(k, R);
        return root;
    }
    return NULL;
}

void inOrderTraversal(Node* root) {
    if (root != NULL) {
        inOrderTraversal(root->left);
        printf("%d(w:%d) ", root->key, root->weight);
        inOrderTraversal(root->right);
    }
}

int treeSize(Node* root) {
    if (root == NULL) return 0;
    return 1 + treeSize(root->left) + treeSize(root->right);
}

int checkSum(Node* root) {
    if (root == NULL) return 0;
    return root->key + checkSum(root->left) + checkSum(root->right);
}

    int treeHeight(Node* root) {
    if (root == NULL) return 0;
    int leftHeight = treeHeight(root->left);
    int rightHeight = treeHeight(root->right);
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

double weightedHeight(Node* root, int level) {
    if (root == NULL) return 0;
    return root->weight * level + 
           weightedHeight(root->left, level + 1) + 
           weightedHeight(root->right, level + 1);
}

void printMatrixPartial(int matrix[MAX_N+1][MAX_N+1], int n, char* name) {
    printf("\nМатрица %s (первые 10x10 элементов):\n", name);
    printf("i\\j");
    for (int j = 0; j <= 10; j++) {
        printf("%8d", j);
    }
    printf("\n");
    
    for (int i = 0; i <= 10; i++) {
        printf("%2d ", i);
        for (int j = 0; j <= 10; j++) {
            if (j >= i) {
                printf("%8d", matrix[i][j]);
            } else {
                printf("        ");
            }
        }
        printf("\n");
    }
}

int main() {
    int n = 10;
    srand(time(NULL));
    
    for (int i = 0; i < n; i++) {
        weights[i] = rand() % 100 + 1;
    }
    
    printf("Ключи и веса вершин:\n");
    for (int i = 0; i < n; i++) {
        printf("Ключ: %d, Вес: %d\n", i+1, weights[i]);
    }
    printf("\n");

    printf("\nВычисление матриц...\n");
    computeAW(n);
    computeAPAR(n);
    
    printMatrixPartial(AW, n, "AW");
    printMatrixPartial(AP, n, "AP"); 
    printMatrixPartial(AR, n, "AR");
    
    printf("\nПостроение дерева...\n");
    Node* root = createTree(0, n);
    
    int size = treeSize(root);
    int sum = checkSum(root);
    int height = treeHeight(root);
    double wHeight = weightedHeight(root, 1);
    double avgWeightedHeight = wHeight / AW[0][n];
    
    
    printf("\n");
    printf("n=%d   Размер   Контр. Сумма    Высота  Средневзвеш.высота\n", n);
    printf("ДОП   %6d   %11d   %6d   %17.2f\n", 
           size, sum, height, avgWeightedHeight);
    
    double matrixRatio = (double)AP[0][n] / AW[0][n];
    printf("\nПроверка правильности алгоритма:\n");
    printf("AP[0,n]/AW[0,n] = %.6f\n", matrixRatio);
    printf("Средневзвешенная высота = %.6f\n", avgWeightedHeight);
    printf("Разница: %.6f\n", fabs(matrixRatio - avgWeightedHeight));
    
    return 0;
}