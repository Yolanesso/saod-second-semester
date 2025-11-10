#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>

typedef struct Node {
    int key;
    int weight;
    struct Node* left;
    struct Node* right;
} Node;

typedef struct BSTCharacteristics {
    int size;
    int control_sum;
    int height;
    double weighted_height;
} BSTCharacteristics;

// Функция для создания нового узла
Node* create_node(int key, int weight) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->key = key;
    newNode->weight = weight;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// Обход дерева слева направо (in-order)
void left_to_right_traversal(Node* root, int* result, int* index) {
    if (root != NULL) {
        left_to_right_traversal(root->left, result, index);
        result[(*index)++] = root->key;
        left_to_right_traversal(root->right, result, index);
    }
}

// Вычисление характеристик дерева
void calculate_characteristics(Node* root, int depth, BSTCharacteristics* chars) {
    if (root == NULL) return;
    
    chars->size++;
    chars->control_sum += root->key;
    chars->height = (depth > chars->height) ? depth : chars->height;
    chars->weighted_height += root->weight * depth;
    
    calculate_characteristics(root->left, depth + 1, chars);
    calculate_characteristics(root->right, depth + 1, chars);
}

// Освобождение памяти дерева
void free_tree(Node* root) {
    if (root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}

// Функция для обмена элементов
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Алгоритм A1 - сортировка по убыванию весов
Node* algorithm_a1(int* keys, int* weights, int n) {
    // Создаем массив индексов и сортируем по убыванию весов
    int* indices = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) indices[i] = i;
    
    // Сортировка пузырьком по убыванию весов
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (weights[indices[j]] < weights[indices[j + 1]]) {
                swap(&indices[j], &indices[j + 1]);
            }
        }
    }
    
    // Построение дерева
    Node* root = NULL;
    for (int i = 0; i < n; i++) {
        Node* newNode = create_node(keys[indices[i]], weights[indices[i]]);
        if (root == NULL) {
            root = newNode;
        } else {
            // Вставка как в BST (по ключу)
            Node* current = root;
            Node* parent = NULL;
            while (current != NULL) {
                parent = current;
                if (newNode->key < current->key) {
                    current = current->left;
                } else {
                    current = current->right;
                }
            }
            if (newNode->key < parent->key) {
                parent->left = newNode;
            } else {
                parent->right = newNode;
            }
        }
    }
    
    free(indices);
    return root;
}

// Алгоритм A2 - рекурсивное построение с балансировкой по весам
Node* algorithm_a2_recursive(int* keys, int* weights, int left, int right) {
    if (left > right) return NULL;
    
    if (left == right) {
        return create_node(keys[left], weights[left]);
    }
    
    // Вычисляем общий вес
    int total_weight = 0;
    for (int i = left; i <= right; i++) {
        total_weight += weights[i];
    }
    
    // Находим корень - первый узел, где сумма весов превышает половину
    int sum = 0;
    int root_index = left;
    for (int i = left; i <= right; i++) {
        if (sum < total_weight / 2 && sum + weights[i] > total_weight / 2) {
            root_index = i;
            break;
        }
        sum += weights[i];
    }
    
    Node* root = create_node(keys[root_index], weights[root_index]);
    root->left = algorithm_a2_recursive(keys, weights, left, root_index - 1);
    root->right = algorithm_a2_recursive(keys, weights, root_index + 1, right);
    
    return root;
}

Node* algorithm_a2(int* keys, int* weights, int n) {
    // Создаем массивы для сортировки по ключам
    int* indices = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) indices[i] = i;
    
    // Сортировка пузырьком по ключам
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (keys[indices[j]] > keys[indices[j + 1]]) {
                swap(&indices[j], &indices[j + 1]);
            }
        }
    }
    
    // Создаем отсортированные массивы
    int* sorted_keys = (int*)malloc(n * sizeof(int));
    int* sorted_weights = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        sorted_keys[i] = keys[indices[i]];
        sorted_weights[i] = weights[indices[i]];
    }
    
    Node* root = algorithm_a2_recursive(sorted_keys, sorted_weights, 0, n - 1);
    
    free(indices);
    free(sorted_keys);
    free(sorted_weights);
    return root;
}

// Оптимальное дерево поиска (алгоритм Кнута) с возвратом матриц для проверки
Node* optimal_bst_with_check(int* keys, int* weights, int n, double* ap_value, double* aw_value) {
    if (n == 0) return NULL;
    
    // Выделяем память для матриц
    double** AW = (double**)malloc((n + 1) * sizeof(double*));
    double** AP = (double**)malloc((n + 1) * sizeof(double*));
    int** AR = (int**)malloc((n + 1) * sizeof(int*));
    
    for (int i = 0; i <= n; i++) {
        AW[i] = (double*)malloc((n + 1) * sizeof(double));
        AP[i] = (double*)malloc((n + 1) * sizeof(double));
        AR[i] = (int*)malloc((n + 1) * sizeof(int));
    }
    
    // Инициализация матрицы весов
    for (int i = 0; i <= n; i++) {
        for (int j = i + 1; j <= n; j++) {
            if (j == i + 1) {
                AW[i][j] = weights[i];
            } else {
                AW[i][j] = AW[i][j - 1] + weights[j - 1];
            }
        }
    }
    
    // Инициализация для h = 1
    for (int i = 0; i < n; i++) {
        int j = i + 1;
        AP[i][j] = AW[i][j];
        AR[i][j] = j;
    }
    
    // Вычисление для h > 1
    for (int h = 2; h <= n; h++) {
        for (int i = 0; i <= n - h; i++) {
            int j = i + h;
            int m = AR[i][j - 1];
            double min_val = AP[i][m - 1] + AP[m][j];
            
            for (int k = m + 1; k <= AR[i + 1][j]; k++) {
                double x = AP[i][k - 1] + AP[k][j];
                if (x < min_val) {
                    m = k;
                    min_val = x;
                }
            }
            
            AP[i][j] = min_val + AW[i][j];
            AR[i][j] = m;
        }
    }
    
    // Сохраняем значения для проверки
    *ap_value = AP[0][n];
    *aw_value = AW[0][n];
    
    Node* build_tree(int L, int R) {
        if (L >= R) return NULL;
        
        int k = AR[L][R];
        Node* root = create_node(keys[k - 1], weights[k - 1]);
        root->left = build_tree(L, k - 1);
        root->right = build_tree(k, R);
        
        return root;
    }
    
    Node* root = build_tree(0, n);
    
    // Освобождение памяти матриц
    for (int i = 0; i <= n; i++) {
        free(AW[i]);
        free(AP[i]);
        free(AR[i]);
    }
    free(AW);
    free(AP);
    free(AR);
    
    return root;
}

// Обертка для обратной совместимости
Node* optimal_bst(int* keys, int* weights, int n) {
    double ap, aw;
    return optimal_bst_with_check(keys, weights, n, &ap, &aw);
}

void print_separator() {
    printf("+-----------------+-----------+-----------------+---------+---------------------+\n");
}

void print_table_header() {
    print_separator();
    printf("| %-15s | %-9s | %-15s | %-7s | %-19s |\n", 
           "Алгоритм", "Размер", "Контр.Сумма", "Высота", "Ср.взвеш.высота");
    print_separator();
}

void print_table_row(const char* algorithm, BSTCharacteristics chars) {
    printf("| %-15s | %-9d | %-15d | %-7d | %-19.2f |\n", 
           algorithm, chars.size, chars.control_sum, chars.height, chars.weighted_height);
}

int main() {
    const int n = 100;
    int keys[n];
    int weights[n];
    double total_weight = 0;
    
    srand(42);
    
    // Генерация ключей и весов
    for (int i = 0; i < n; i++) {
        keys[i] = i + 1;
        weights[i] = rand() % 100 + 1;
        total_weight += weights[i];
    }
    
    printf("СРАВНЕНИЕ АЛГОРИТМОВ ПОСТРОЕНИЯ ДЕРЕВЬЕВ ПОИСКА\n");
    printf("Количество вершин: %d\n", n);
    printf("Сумма всех весов: %.0f\n\n", total_weight);
    
    // Тестирование оптимального дерева с проверкой
    double ap_value, aw_value;
    Node* optimal_root = optimal_bst_with_check(keys, weights, n, &ap_value, &aw_value);
    BSTCharacteristics optimal_chars = {0, 0, 0, 0.0};
    calculate_characteristics(optimal_root, 1, &optimal_chars);
    
    // Вычисление средневзвешенной высоты из матриц
    double h_avg_from_matrix = ap_value / aw_value;
    
    printf("ПРОВЕРКА ПРАВИЛЬНОСТИ ОПТИМАЛЬНОГО АЛГОРИТМА:\n");
    printf("AP[0,%d] = %.2f\n", n, ap_value);
    printf("AW[0,%d] = %.2f\n", n, aw_value);
    printf("AP[0,%d] / AW[0,%d] = %.6f\n", n, n, h_avg_from_matrix);
    printf("Средневзвешенная высота по дереву = %.6f\n", optimal_chars.weighted_height / aw_value);
    printf("Разница = %.10f\n\n", fabs(h_avg_from_matrix - optimal_chars.weighted_height / aw_value));
    
    // ПРАВИЛЬНОЕ вычисление средневзвешенной высоты - делим на сумму весов, а не на n!
    optimal_chars.weighted_height /= aw_value;  // aw_value = total_weight
    
    // Тестирование алгоритма A1
    Node* a1_root = algorithm_a1(keys, weights, n);
    BSTCharacteristics a1_chars = {0, 0, 0, 0.0};
    calculate_characteristics(a1_root, 1, &a1_chars);
    a1_chars.weighted_height /= aw_value;  // Делим на общую сумму весов
    
    // Тестирование алгоритма A2
    Node* a2_root = algorithm_a2(keys, weights, n);
    BSTCharacteristics a2_chars = {0, 0, 0, 0.0};
    calculate_characteristics(a2_root, 1, &a2_chars);
    a2_chars.weighted_height /= aw_value;  // Делим на общую сумму весов
    
    // Вывод таблицы
    print_table_header();
    print_table_row("ДОП", optimal_chars);
    print_table_row("A1", a1_chars);
    print_table_row("A2", a2_chars);
    print_separator();
    
    // Вывод обходов деревьев (только первых 10 элементов для читаемости)
    printf("\nОБХОДЫ ДЕРЕВЬЕВ СЛЕВА НАПРАВО:\n");
    printf("---------------------------------------------------\n");
    
    int* traversal = (int*)malloc(n * sizeof(int));
    int index;
    
    printf("ДОП: ");
    index = 0;
    left_to_right_traversal(optimal_root, traversal, &index);
    for (int i = 0; i < 100; i++) printf("%d ", traversal[i]);
    printf("\n\n");
    
    printf("А1:  ");
    index = 0;
    left_to_right_traversal(a1_root, traversal, &index);
    for (int i = 0; i < 100; i++) printf("%d ", traversal[i]);
    printf("\n\n");
    
    printf("А2:  ");
    index = 0;
    left_to_right_traversal(a2_root, traversal, &index);
    for (int i = 0; i < 100; i++) printf("%d ", traversal[i]);
    printf("\n\n");

    
    // Освобождение памяти
    free_tree(optimal_root);
    free_tree(a1_root);
    free_tree(a2_root);
    free(traversal);
    
    return 0;
}