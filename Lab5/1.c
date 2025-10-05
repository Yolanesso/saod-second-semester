#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_BOLD    "\033[1m"

#define BG_BLUE       "\033[44m"
#define BG_GREEN      "\033[42m"
#define BG_YELLOW     "\033[43m"

typedef struct AVLVertex {
    int data;
    struct AVLVertex* left;
    struct AVLVertex* right;
    int bal;
} AVLVertex;

typedef struct BSTVertex {
    int data;
    struct BSTVertex* left;
    struct BSTVertex* right;
} BSTVertex;


void LL_rotate(AVLVertex** p) {
    AVLVertex* q = (*p)->left; // q - левый потомок p
    (*p)->bal = 0; // Баланс p становится 0
    q->bal = 0; // Баланс q становится 0
    (*p)->left = q->right; // Правое поддерево q становится левым поддеревом p
    q->right = *p; // p становится правым потомком q
    *p = q; // q становится новым корнем
}

void LR_rotate(AVLVertex** p) {
    AVLVertex* q = (*p)->left;  // q - левый потомок p
    AVLVertex* r = q->right; // r - правый потомок q (виновник разбалансировки)
    
    // Определяем новые балансы для p
    if (r->bal < 0) (*p)->bal = 1; // Если r был перевешен влево
    else (*p)->bal = 0; // Если r был сбалансирован или перевешен вправо
    
    if (r->bal > 0) q->bal = -1; // Если r был перевешен вправо
    else q->bal = 0;  // Если r был сбалансирован или перевешен влево
    
    r->bal = 0; // r становится сбалансированным

    q->right = r->left; // Левое поддерево r становится правым поддеревом q
    (*p)->left = r->right; // Правое поддерево r становится левым поддеревом p
    r->left = q; // q становится левым потомком r
    r->right = *p; // p становится правым потомком r
    *p = r; // r становится новым корнем
}

void RR_rotate(AVLVertex** p) {
    AVLVertex* q = (*p)->right; // q - правый потомок p
    (*p)->bal = 0; // Баланс p становится 0
    q->bal = 0; // Баланс q становится 0
    (*p)->right = q->left; // Левое поддерево q становится правым поддеревом p
    q->left = *p; // p становится левым потомком q
    *p = q; // q становится новым корнем
}

void RL_rotate(AVLVertex** p) {
    AVLVertex* q = (*p)->right; 
    AVLVertex* r = q->left;
    
    if (r->bal > 0) (*p)->bal = -1;
    else (*p)->bal = 0;
    
    if (r->bal < 0) q->bal = 1;
    else q->bal = 0;
    
    r->bal = 0; // r становится сбалансированным

    q->left = r->right; // Правое поддерево r становится левым поддеревом q
    (*p)->right = r->left;  // Левое поддерево r становится правым поддеревом p
    r->right = q; // q становится правым потомком r
    r->left = *p; // p становится левым потомком r
    *p = r; // r становится новым корнем    
}

int add_AVL(int data, AVLVertex** p) {
    if (*p == NULL) {
        *p = (AVLVertex*)malloc(sizeof(AVLVertex));
        if (*p == NULL) return 0;
        (*p)->data = data;
        (*p)->left = NULL;
        (*p)->right = NULL;
        (*p)->bal = 0;
        return 1;
    }
    
    if ((*p)->data > data) {
        if (add_AVL(data, &((*p)->left))) {
            if ((*p)->bal > 0) {
                // Случай 1A: был правый перевес → теперь сбалансировано
                (*p)->bal = 0;
                return 0;
                                // Случай 1B: был баланс → теперь левый перевес

            } else if ((*p)->bal == 0) {
                (*p)->bal = -1;
                return 1;
            } else {
                if ((*p)->left->bal < 0) {
                    LL_rotate(p);
                    return 0;
                } else {
                    LR_rotate(p);
                    return 0;
                }
            }
        }
        return 0;
    }
    else if ((*p)->data < data) {
        if (add_AVL(data, &((*p)->right))) {
            if ((*p)->bal < 0) {
                (*p)->bal = 0;
                return 0;
            } else if ((*p)->bal == 0) {
                (*p)->bal = 1;
                return 1;
            } else {
                if ((*p)->right->bal > 0) {
                    RR_rotate(p);
                    return 0;
                } else {
                    RL_rotate(p);
                    return 0;
                }
            }
        }
        return 0;
    }
    else {
        return 0;
    }
}

int add_BST(int data, BSTVertex** p) {
    if (*p == NULL) {
        *p = (BSTVertex*)malloc(sizeof(BSTVertex));
        if (*p == NULL) return 0;
        (*p)->data = data;
        (*p)->left = NULL;
        (*p)->right = NULL;
        return 1;
    }
    
    if ((*p)->data > data) {
        return add_BST(data, &((*p)->left));
    }
    else if ((*p)->data < data) {
        return add_BST(data, &((*p)->right));
    }
    else {
        return 0;
    }
}


void in_order_traversal(AVLVertex* root) {
    if (root != NULL) {
        in_order_traversal(root->left);
        printf("%d ", root->data);
        in_order_traversal(root->right);
    }
}

int tree_size(AVLVertex* avl_root, BSTVertex* bst_root, int is_avl) {
    if (is_avl) {
        if (avl_root == NULL) return 0;
        return tree_size(avl_root->left, NULL, 1) + 
               tree_size(avl_root->right, NULL, 1) + 1;
    } else {
        if (bst_root == NULL) return 0;
        return tree_size(NULL, bst_root->left, 0) + 
               tree_size(NULL, bst_root->right, 0) + 1;
    }
}

int control_sum(AVLVertex* avl_root, BSTVertex* bst_root, int is_avl) {
    if (is_avl) {
        if (avl_root == NULL) return 0;
        return avl_root->data + 
               control_sum(avl_root->left, NULL, 1) + 
               control_sum(avl_root->right, NULL, 1);
    } else {
        if (bst_root == NULL) return 0;
        return bst_root->data + 
               control_sum(NULL, bst_root->left, 0) + 
               control_sum(NULL, bst_root->right, 0);
    }
}

int tree_height(AVLVertex* avl_root, BSTVertex* bst_root, int is_avl) {
    if (is_avl) {
        if (avl_root == NULL) return 0;
        int left_h = tree_height(avl_root->left, NULL, 1);
        int right_h = tree_height(avl_root->right, NULL, 1);
        return (left_h > right_h ? left_h : right_h) + 1;
    } else {
        if (bst_root == NULL) return 0;
        int left_h = tree_height(NULL, bst_root->left, 0);
        int right_h = tree_height(NULL, bst_root->right, 0);
        return (left_h > right_h ? left_h : right_h) + 1;
    }
}

void tree_height_sum(AVLVertex* avl_root, BSTVertex* bst_root, int is_avl, 
                    int current_height, int* total_height, int* node_count) {
    if (is_avl) {
        if (avl_root == NULL) return;
        (*total_height) += current_height;
        (*node_count) += 1;
        tree_height_sum(avl_root->left, NULL, 1, current_height + 1, total_height, node_count);
        tree_height_sum(avl_root->right, NULL, 1, current_height + 1, total_height, node_count);
    } else {
        if (bst_root == NULL) return;
        (*total_height) += current_height;
        (*node_count) += 1;
        tree_height_sum(NULL, bst_root->left, 0, current_height + 1, total_height, node_count);
        tree_height_sum(NULL, bst_root->right, 0, current_height + 1, total_height, node_count);
    }
}

double average_height(AVLVertex* avl_root, BSTVertex* bst_root, int is_avl) {
    int total_height = 0;
    int node_count = 0;
    tree_height_sum(avl_root, bst_root, is_avl, 1, &total_height, &node_count);
    return node_count > 0 ? (double)total_height / node_count : 0.0;
}


void generate_unique_random(int arr[], int n, int min, int max) {
    int* used = (int*)calloc(max - min + 1, sizeof(int));
    int count = 0;
    
    while (count < n) {
        int num = min + rand() % (max - min + 1);
        if (!used[num - min]) {
            used[num - min] = 1;
            arr[count] = num;
            count++;
        }
    }
    free(used);
}

void free_AVL(AVLVertex* root) {
    if (root != NULL) {
        free_AVL(root->left);
        free_AVL(root->right);
        free(root);
    }
}

void free_BST(BSTVertex* root) {
    if (root != NULL) {
        free_BST(root->left);
        free_BST(root->right);
        free(root);
    }
}

void print_separator() {
    printf(COLOR_CYAN "╔════════════════════════════════════════════════════════════════════════════════╗\n" COLOR_RESET);
}

void print_middle_separator() {
    printf(COLOR_CYAN "╠════════════════════════════════════════════════════════════════════════════════╣\n" COLOR_RESET);
}

void print_bottom_separator() {
    printf(COLOR_CYAN "╚════════════════════════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
}

void print_header(const char* text) {
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_BOLD BG_BLUE " %-76s " COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET, text);
}

void print_success(const char* text) {
    printf(COLOR_GREEN " ✓ %s\n" COLOR_RESET, text);
}

void print_info(const char* text) {
    printf(COLOR_BLUE " ℹ %s\n" COLOR_RESET, text);
}

void print_warning(const char* text) {
    printf(COLOR_YELLOW " ⚠ %s\n" COLOR_RESET, text);
}


int main() {
    AVLVertex* avl_root = NULL;
    BSTVertex* bst_root = NULL;
    const int NUM_VERTICES = 100;
    int values[NUM_VERTICES];
    
    srand(time(NULL));
    
    print_separator();
    print_header("                    🌳 АВЛ-ДЕРЕВО ПОИСКА 🌳                    ");
    print_middle_separator();
    print_middle_separator();
    
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_BOLD " %-76s " COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET, "🎲 Генерация 100 случайных уникальных чисел...");
    generate_unique_random(values, NUM_VERTICES, 1, 1000);
    print_success("Числа сгенерированы успешно!");
    
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_BOLD " %-76s " COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET, "🏗️  Построение АВЛ-дерева и ИСДП...");
    for (int i = 0; i < NUM_VERTICES; i++) {
        add_AVL(values[i], &avl_root);
        add_BST(values[i], &bst_root);
    }
    print_success("Деревья успешно построены! 🎉");
    print_middle_separator();
    
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_BOLD BG_GREEN " %-76s " COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET, "📋 ЗАДАНИЕ 3: Обход АВЛ-дерева слева направо");
    printf(COLOR_CYAN "║" COLOR_RESET " %-76s " COLOR_CYAN "║\n" COLOR_RESET, "");
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_YELLOW " 📊 Отсортированные значения: " COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET "   ");
    in_order_traversal(avl_root);
    printf(COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET " %-76s " COLOR_CYAN "║\n" COLOR_RESET, "");
    print_middle_separator();
    
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_BOLD BG_GREEN " %-76s " COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET, "📊 ЗАДАНИЕ 4: Сравнительная таблица характеристик");
    printf(COLOR_CYAN "║" COLOR_RESET " %-76s " COLOR_CYAN "║\n" COLOR_RESET, "");
    
    int avl_size = tree_size(avl_root, NULL, 1);
    int avl_sum = control_sum(avl_root, NULL, 1);
    int avl_height = tree_height(avl_root, NULL, 1);
    double avl_avg_height = average_height(avl_root, NULL, 1);
    
    int bst_size = tree_size(NULL, bst_root, 0);
    int bst_sum = control_sum(NULL, bst_root, 0);
    int bst_height = tree_height(NULL, bst_root, 0);
    double bst_avg_height = average_height(NULL, bst_root, 0);
    
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_BOLD " %-76s " COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET, "┌──────────┬──────────┬──────────────┬──────────┬───────────────┐");
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_BOLD " %-76s " COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET, "│  Дерево  │  Размер  │  Контр.Сумма │  Высота  │  Средн.высота │");
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_BOLD " %-76s " COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET, "├──────────┼──────────┼──────────────┼──────────┼───────────────┤");
    
    char bst_line[100], avl_line[100];
    snprintf(bst_line, sizeof(bst_line), "│" COLOR_RED "  ИСДП   " COLOR_RESET "│  %-6d  │  %-10d  │  %-6d  │  %-11.2f  │", 
             bst_size, bst_sum, bst_height, bst_avg_height);
    snprintf(avl_line, sizeof(avl_line), "│" COLOR_GREEN "  АВЛ    " COLOR_RESET "│  %-6d  │  %-10d  │  %-6d  │  %-11.2f  │", 
             avl_size, avl_sum, avl_height, avl_avg_height);
    
    printf(COLOR_CYAN "║" COLOR_RESET " %-76s " COLOR_CYAN "║\n" COLOR_RESET, bst_line);
    printf(COLOR_CYAN "║" COLOR_RESET " %-76s " COLOR_CYAN "║\n" COLOR_RESET, avl_line);
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_BOLD " %-76s " COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET, "└──────────┴──────────┴──────────────┴──────────┴───────────────┘");
    printf(COLOR_CYAN "║" COLOR_RESET " %-76s " COLOR_CYAN "║\n" COLOR_RESET, "");
    print_middle_separator();
    
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_BOLD BG_YELLOW " %-76s " COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET, "📈 АНАЛИТИКА И СРАВНЕНИЕ");
    printf(COLOR_CYAN "║" COLOR_RESET " %-76s " COLOR_CYAN "║\n" COLOR_RESET, "");
    
    double height_ratio = (double)bst_height / avl_height;
    double avg_height_diff = bst_avg_height - avl_avg_height;
    int height_saving = bst_height - avl_height;
    
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_BOLD " 🎯 Эффективность балансировки АВЛ-дерева:" COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_GREEN "   ✅ Высота ИСДП больше высоты АВЛ в %.2f раз" COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET, height_ratio);
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_GREEN "   ✅ Средняя высота ИСДП больше на %.2f единиц" COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET, avg_height_diff);
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_GREEN "   ✅ Экономия по высоте: %d уровней 🎉" COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET, height_saving);
    printf(COLOR_CYAN "║" COLOR_RESET " %-76s " COLOR_CYAN "║\n" COLOR_RESET, "");
    
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_BOLD " 📊 Теоретические значения для АВЛ-дерева:" COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_BLUE "   📏 Максимальная теоретическая высота: ~%.2f" COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET, 1.44 * log2(NUM_VERTICES + 2));
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_BLUE "   📏 Минимальная теоретическая высота: ~%.2f" COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET, log2(NUM_VERTICES + 1));
    printf(COLOR_CYAN "║" COLOR_RESET COLOR_BLUE "   📏 Фактическая высота: %d" COLOR_RESET COLOR_CYAN "║\n" COLOR_RESET, avl_height);
    printf(COLOR_CYAN "║" COLOR_RESET " %-76s " COLOR_CYAN "║\n" COLOR_RESET, "");
    
    print_middle_separator();
    
    free_AVL(avl_root);
    free_BST(bst_root);
    
    return 0;
}