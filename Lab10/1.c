#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_SYMBOLS 256
#define MAX_CODE_LENGTH 32
#define MIN_FILE_SIZE 10240
#define TEXT_TO_ENCODE_SIZE 100

typedef struct {
    unsigned char symbol;
    double probability;
    int code[MAX_CODE_LENGTH];
    int code_length;
    int shannon_length;
} SymbolInfo;

typedef struct {
    unsigned char symbol;
    int code[MAX_CODE_LENGTH];
    int code_length;
} CodeTable;

int count_symbols(const char* filename, int* frequencies, long* total_symbols) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Ошибка: не удалось открыть файл %s\n", filename);
        return 0;
    }
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (file_size < MIN_FILE_SIZE) {
        printf("Ошибка: файл слишком маленький (%ld байт). Требуется минимум %d байт.\n", 
               file_size, MIN_FILE_SIZE);
        fclose(file);
        return 0;
    }
    
    memset(frequencies, 0, MAX_SYMBOLS * sizeof(int));
    *total_symbols = 0;
    
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        frequencies[ch]++;
        (*total_symbols)++;
    }
    
    fclose(file);
    return 1;
}

void calculate_probabilities(int* frequencies, long total_symbols, SymbolInfo* symbols, int* symbol_count) {
    *symbol_count = 0;
    
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (frequencies[i] > 0) {
            (*symbol_count)++;
        }
    }
    
    int index = 0;
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (frequencies[i] > 0) {
            symbols[index].symbol = (unsigned char)i;
            symbols[index].probability = (double)frequencies[i] / total_symbols;
            index++;
        }
    }
}

int compare_probabilities(const void* a, const void* b) {
    const SymbolInfo* sa = (const SymbolInfo*)a;
    const SymbolInfo* sb = (const SymbolInfo*)b;
    
    if (sa->probability > sb->probability) return -1;
    if (sa->probability < sb->probability) return 1;
    return 0;
}

int calculate_shannon_length(double probability) {
    if (probability <= 0) return 0;
    return (int)ceil(-log2(probability));
}

void build_shannon_codes_exact(SymbolInfo* symbols, int n) {
    double P[MAX_SYMBOLS] = {0};
    double Q[MAX_SYMBOLS] = {0};
    int L[MAX_SYMBOLS] = {0};
    
    for (int i = 1; i <= n; i++) {
        P[i] = symbols[i-1].probability;
    }
    
    for (int i = 1; i <= n; i++) {
        Q[i] = Q[i-1] + P[i];
        L[i] = calculate_shannon_length(P[i]);
        symbols[i-1].shannon_length = L[i];
    }
    
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= L[i]; j++) {
            Q[i-1] = Q[i-1] * 2;
            symbols[i-1].code[j-1] = (int)floor(Q[i-1]);
            if (Q[i-1] >= 1.0) {
                Q[i-1] = Q[i-1] - 1.0;      
            }
        }
        symbols[i-1].code_length = L[i];
    }
}


void create_code_table(SymbolInfo* symbols, int symbol_count, CodeTable* code_table) {
    for (int i = 0; i < symbol_count; i++) {
        code_table[i].symbol = symbols[i].symbol;
        code_table[i].code_length = symbols[i].code_length;
        for (int j = 0; j < symbols[i].code_length; j++) {
            code_table[i].code[j] = symbols[i].code[j];
        }
    }
}

int find_symbol_code(unsigned char symbol, CodeTable* code_table, int symbol_count, int* code, int* code_length) {
    for (int i = 0; i < symbol_count; i++) {
        if (code_table[i].symbol == symbol) {
            *code_length = code_table[i].code_length;
            for (int j = 0; j < code_table[i].code_length; j++) {
                code[j] = code_table[i].code[j];
            }
            return 1;
        }
    }
    return 0;
}

int encode_text(const char* text, int text_length, CodeTable* code_table, int symbol_count, 
                int* encoded_bits, int* encoded_length) {
    *encoded_length = 0;
    
    for (int i = 0; i < text_length; i++) {
        unsigned char symbol = text[i];
        int code[MAX_CODE_LENGTH];
        int code_len;
        
        if (find_symbol_code(symbol, code_table, symbol_count, code, &code_len)) {
            for (int j = 0; j < code_len; j++) {
                encoded_bits[*encoded_length] = code[j];
                (*encoded_length)++;
            }
        } else {
            printf("Ошибка: символ '0x%02X' не найден в кодовой таблице!\n", symbol);
            return 0;
        }
    }
    
    return 1;
}

int read_first_100_chars(const char* filename, char* buffer, int buffer_size) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Ошибка: не удалось открыть файл %s\n", filename);
        return 0;
    }
    
    int chars_read = 0;
    int ch;
    while (chars_read < buffer_size && (ch = fgetc(file)) != EOF) {
        buffer[chars_read++] = (char)ch;
    }
    
    fclose(file);
    return chars_read;
}

void print_encoded_sequence(int* encoded_bits, int encoded_length, int original_length) {
    printf("\nЗакодированная последовательность (%d бит):\n", encoded_length);
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    
    int bits_per_line = 50;
    int lines = (encoded_length + bits_per_line - 1) / bits_per_line;
    
    for (int line = 0; line < lines; line++) {
        int start = line * bits_per_line;
        int end = start + bits_per_line;
        if (end > encoded_length) end = encoded_length;
        
        printf("│ ");
        for (int i = start; i < end; i++) {
            printf("%d", encoded_bits[i]);
            if ((i - start + 1) % 5 == 0 && i < end - 1) {
                printf(" ");
            }
        }
        
        for (int i = end - start; i < bits_per_line; i++) {
            if ((i + 1) % 5 == 0 && i < bits_per_line - 1) {
                printf(" ");
            }
        }
        printf(" │\n");
    }
    
    printf("└─────────────────────────────────────────────────────────────┘\n");
    
    int original_bits = original_length * 8;
    double compression_ratio = (double)original_bits / encoded_length;
    double compression_percentage = (1 - (double)encoded_length / original_bits) * 100;
    
    printf("\nСтатистика кодирования:\n");
    printf("Исходный текст: %d символов = %d бит\n", original_length, original_bits);
    printf("Закодированный текст: %d бит\n", encoded_length);
    printf("Коэффициент сжатия: %.3f\n", compression_ratio);
    printf("Степень сжатия: %.2f%%\n", compression_percentage);
    printf("Экономия: %d бит\n", original_bits - encoded_length);
}

int check_kraft_inequality(SymbolInfo* symbols, int symbol_count) {
    double kraft_sum = 0.0;
    
    for (int i = 0; i < symbol_count; i++) {
        if (symbols[i].code_length > 0) {
            kraft_sum += pow(2, -symbols[i].code_length);
        }
    }
    
    printf("Сумма Крафта: %.6f\n", kraft_sum);
    
    if (kraft_sum <= 1.0) {
        printf("Неравенство Крафта ВЫПОЛНЕНО (∑ 2^(-l_i) = %.6f ≤ 1)\n", kraft_sum);
        return 1;
    } else {
        printf("Неравенство Крафта НЕ ВЫПОЛНЕНО (∑ 2^(-l_i) = %.6f > 1)\n", kraft_sum);
        return 0;
    }
}

double calculate_entropy(SymbolInfo* symbols, int symbol_count) {
    double entropy = 0;
    for (int i = 0; i < symbol_count; i++) {
        if (symbols[i].probability > 0) {
            entropy -= symbols[i].probability * log2(symbols[i].probability);
        }
    }
    return entropy;
}

double calculate_average_code_length(SymbolInfo* symbols, int symbol_count) {
    double average_length = 0;
    for (int i = 0; i < symbol_count; i++) {
        average_length += symbols[i].probability * symbols[i].code_length;
    }
    return average_length;
}

double calculate_redundancy(double entropy, double average_length) {
    return average_length - entropy;
}

double calculate_relative_redundancy(double entropy, double average_length) {
    if (average_length == 0) return 0;
    return (average_length - entropy) / average_length * 100;
}

void print_code_table(SymbolInfo* symbols, int symbol_count) {
    printf("\nКод Шеннона\n");
    printf("Символ\tВероятность символа\tКодовое слово\tДлина кодового слова\n");
    printf("-----------------------------------------------------------------\n");
    
    for (int i = 0; i < symbol_count && i < 20; i++) {
        unsigned char ch = symbols[i].symbol;
        if (isprint(ch) && !isspace(ch)) {
            printf("'%c'\t", ch);
        } else if (ch == ' ') {
            printf("' '\t");
        } else if (ch == '\n') {
            printf("\\\\n\t");
        } else if (ch == '\t') {
            printf("\\\\t\t");
        } else if (ch == '\r') {
            printf("\\\\r\t");
        } else {
            printf("0x%02X\t", ch);
        }
        
        printf("%.6f\t\t", symbols[i].probability);
        
        for (int j = 0; j < symbols[i].code_length; j++) {
            printf("%d", symbols[i].code[j]);
        }
        
        printf("\t\t%d\n", symbols[i].code_length);
    }
    
    if (symbol_count > 20) {
        printf("... (и еще %d символов)\n", symbol_count - 20);
    }
}

void print_summary_table(int kraft_result, double entropy, double avg_length, double redundancy, double rel_redundancy) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                      РЕЗУЛЬТАТЫ АНАЛИЗА                     ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ Характеристика                    │ Значение                 ║\n");
    printf("╠═══════════════════════════════════╪══════════════════════════╣\n");
    printf("║ Код Шеннона                       │ Построен                 ║\n");
    printf("║ Неравенство Крафта                │ %s ║\n", 
           kraft_result ? "Выполнено          " : "Не выполнено      ");
    printf("║ Энтропия исходного текста         │ %-8.6f бит/символ    ║\n", entropy);
    printf("║ Средняя длина кодового слова      │ %-8.6f бит/символ    ║\n", avg_length);
    printf("║ Избыточность кода Шеннона         │ %-8.6f бит/символ    ║\n", redundancy);
    printf("║ Относительная избыточность        │ %-8.2f%%             ║\n", rel_redundancy);
    printf("╚═══════════════════════════════════╧══════════════════════════╝\n");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Использование: %s <имя_файла>\n", argv[0]);
        return 1;
    }
    
    const char* filename = argv[1];
    int frequencies[MAX_SYMBOLS];
    long total_symbols;
    SymbolInfo symbols[MAX_SYMBOLS];
    int symbol_count;
    
    if (!count_symbols(filename, frequencies, &total_symbols)) {
        return 1;
    }
    
    printf("Файл прочитан успешно. Всего символов: %ld\n", total_symbols);
    
    calculate_probabilities(frequencies, total_symbols, symbols, &symbol_count);
    
    qsort(symbols, symbol_count, sizeof(SymbolInfo), compare_probabilities);
    
    printf("\nПостроение кода Шеннона (точное соответствие псевдокоду):\n");
    build_shannon_codes_exact(symbols, symbol_count);
    
    printf("\nПроверка неравенства Крафта:\n");
    int kraft_result = check_kraft_inequality(symbols, symbol_count);
    
    double entropy = calculate_entropy(symbols, symbol_count);
    double avg_length = calculate_average_code_length(symbols, symbol_count);
    double redundancy = calculate_redundancy(entropy, avg_length);
    double rel_redundancy = calculate_relative_redundancy(entropy, avg_length);
    
    print_code_table(symbols, symbol_count);
    print_summary_table(kraft_result, entropy, avg_length, redundancy, rel_redundancy);
    
    printf("\n══════════════════════════════════════════════════════════════\n");
    printf("                КОДИРОВАНИЕ ТЕКСТА ИЗ 100 СИМВОЛОВ\n");
    printf("══════════════════════════════════════════════════════════════\n");
    
    char text_to_encode[TEXT_TO_ENCODE_SIZE + 1];
    int actual_chars = read_first_100_chars(filename, text_to_encode, TEXT_TO_ENCODE_SIZE);
    
    if (actual_chars > 0) {
        printf("┌─────────────────────────────────────────────────────────────┐\n");
        printf("│ ");
        for (int i = 0; i < actual_chars; i++) {
            unsigned char ch = text_to_encode[i];
            if (isprint(ch) && !isspace(ch)) {
                printf("%c", ch);
            } else if (ch == ' ') {
                printf(" ");
            } else if (ch == '\n') {
                printf("¶");
            } else if (ch == '\t') {
                printf("→");
            } else {
                printf("·");
            }
        }
        printf(" │\n");
        printf("└─────────────────────────────────────────────────────────────┘\n");
        
        CodeTable code_table[MAX_SYMBOLS];
        create_code_table(symbols, symbol_count, code_table);
        
        int encoded_bits[MAX_CODE_LENGTH * TEXT_TO_ENCODE_SIZE];
        int encoded_length;
        
        if (encode_text(text_to_encode, actual_chars, code_table, symbol_count, 
                       encoded_bits, &encoded_length)) {
            print_encoded_sequence(encoded_bits, encoded_length, actual_chars);
        }
    } else {
        printf("Не удалось прочитать текст для кодирования из файла.\n");
    }
    
    return 0;
}