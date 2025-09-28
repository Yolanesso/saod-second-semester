#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct TreeNode {
    char *word;
    int count;
    struct TreeNode *left;
    struct TreeNode *right;
};

const char *c_keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "int", "long", "register", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
};
const int KEYWORDS_COUNT = 32;

int is_keyword(const char *word) {
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (strcmp(word, c_keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void AddWord(struct TreeNode **root, const char *word) {
    struct TreeNode **p = root;
    
    while (*p != NULL) {
        int cmp = strcmp(word, (*p)->word);
        
        if (cmp < 0) {
            p = &((*p)->left);
        } else if (cmp > 0) {
            p = &((*p)->right);
        } else {
            (*p)->count++;
            return;
        }
    }
    
    *p = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    (*p)->word = strdup(word);
    (*p)->count = 1;
    (*p)->left = NULL;
    (*p)->right = NULL;
}

void PrintTree(struct TreeNode *root) {
    if (root != NULL) {
        PrintTree(root->left);
        printf("%-15s: %d\n", root->word, root->count);
        PrintTree(root->right);
    }
}

void FreeTree(struct TreeNode **root) {
    if (*root == NULL) return;
    
    FreeTree(&((*root)->left));
    FreeTree(&((*root)->right));
    
    free((*root)->word);
    free(*root);
    *root = NULL;
}

char* get_next_word(FILE *file) {
    static char buffer[100];
    int ch, i = 0;
    
    while ((ch = fgetc(file)) != EOF && !isalpha(ch));
    
    if (ch == EOF) return NULL;
    
    buffer[i++] = tolower(ch);
    while ((ch = fgetc(file)) != EOF && isalpha(ch) && i < 99) {
        buffer[i++] = tolower(ch);
    }
    buffer[i] = '\0';
    
    return buffer;
}

int main() {
    FILE *file;
    char filename[100];
    struct TreeNode *root = NULL;
    
    printf("Введите имя файла с кодом на Си: ");
    scanf("%s", filename);
    
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Ошибка открытия файла!\n");
        return 1;
    }
    
    printf("⏳ Анализ файла...\n");
    
    char *word;
    int total_words = 0, keyword_count = 0;
    
    while ((word = get_next_word(file)) != NULL) {
        total_words++;
        
        if (is_keyword(word)) {
            AddWord(&root, word);
            keyword_count++;
        }
    }
    
    fclose(file);
    
    printf("\n📊 Результаты анализа:\n");
    printf("Всего слов: %d\n", total_words);
    printf("Ключевых слов: %d\n", keyword_count);
    printf("Уникальных ключевых слов: \n\n");
    
    printf("СЛОВО          ЧАСТОТА\n");
    printf("---------------------\n");
    PrintTree(root);
    
    FreeTree(&root);
    
    return 0;
}