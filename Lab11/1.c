#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_SYMBOLS 256
#define MAX_CODE_LENGTH 50

typedef struct {
    unsigned char symbol;
    double probability;
    char code[MAX_CODE_LENGTH];
    int code_length;
} SymbolInfo;

typedef struct {
    SymbolInfo symbols[MAX_SYMBOLS];
    int count;
    double kraft_sum;
    double entropy;
    double avg_length;
    double redundancy;
} FanoCode;

// КЛАССИЧЕСКИЙ АЛГОРИТМ ФАНО - оригинальная медиана
int find_median_classic(SymbolInfo* symbols, int left, int right) {
    double left_sum = 0.0;
    double right_sum = 0.0;
    
    for (int i = left; i < right; i++) {
        left_sum += symbols[i].probability;
    }
    
    right_sum = symbols[right].probability;
    int median = right;
    
    while (left_sum >= right_sum && median > left) {
        median--;
        left_sum -= symbols[median].probability;
        right_sum += symbols[median].probability;
    }
    
    return median;
}

// АЛГОРИТМ A2 - медиана для почти оптимального дерева поиска
int find_median_a2(SymbolInfo* symbols, int left, int right) {
    if (left == right) return left;
    
    double total_weight = 0.0;
    for (int i = left; i <= right; i++) {
        total_weight += symbols[i].probability;
    }
    
    double sum = 0.0;
    int root_index = left;
    
    for (int i = left; i <= right; i++) {
        double prev_sum = sum;
        sum += symbols[i].probability;
        
        if (prev_sum < total_weight / 2.0 && sum > total_weight / 2.0) {
            root_index = i;
            break;
        }
    }
    
    return root_index;
}

// ЭНТРОПИЙНЫЙ МЕТОД - минимизация потери информации
int find_median_entropy(SymbolInfo* symbols, int left, int right) {
    if (left == right) return left;
    
    double total = 0.0;
    for (int i = left; i <= right; i++) {
        total += symbols[i].probability;
    }
    
    double min_entropy_loss = 1e9;
    int best_median = left;
    
    for (int i = left; i < right; i++) {
        double left_sum = 0.0;
        for (int j = left; j <= i; j++) {
            left_sum += symbols[j].probability;
        }
        double right_sum = total - left_sum;
        
        // Энтропийный критерий - минимизируем потерю информации
        double left_entropy = (left_sum > 0) ? -left_sum * log2(left_sum) : 0;
        double right_entropy = (right_sum > 0) ? -right_sum * log2(right_sum) : 0;
        double total_entropy = -total * log2(total);
        
        double entropy_loss = total_entropy - (left_entropy + right_entropy);
        
        if (entropy_loss < min_entropy_loss) {
            min_entropy_loss = entropy_loss;
            best_median = i;
        }
    }
    
    return best_median;
}

// Рекурсивная функция построения кода Фано (общая)
void build_fano_code(SymbolInfo* symbols, int left, int right, int depth, int method) {
    if (left < right) {
        int median;
        
        switch (method) {
            case 0: // Классический метод
                median = find_median_classic(symbols, left, right);
                break;
            case 1: // Метод A2
                median = find_median_a2(symbols, left, right);
                break;
            case 2: // Энтропийный метод
                median = find_median_entropy(symbols, left, right);
                break;
            default:
                median = find_median_classic(symbols, left, right);
        }
        
        for (int i = left; i <= right; i++) {
            if (i <= median) {
                symbols[i].code[depth] = '0';
            } else {
                symbols[i].code[depth] = '1';
            }
            symbols[i].code_length = depth + 1;
        }
        
        build_fano_code(symbols, left, median, depth + 1, method);
        build_fano_code(symbols, median + 1, right, depth + 1, method);
    } else if (left == right) {
        symbols[left].code_length = depth;
    }
}

// Функция для чтения файла и подсчета частот
int read_file_and_count(const char* filename, int* frequencies, long* total_chars) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Ошибка: не удалось открыть файл %s\n", filename);
        return 0;
    }
    
    memset(frequencies, 0, MAX_SYMBOLS * sizeof(int));
    *total_chars = 0;
    
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        frequencies[ch]++;
        (*total_chars)++;
    }
    
    fclose(file);
    return 1;
}

void init_symbols(SymbolInfo* symbols, int* count, int* frequencies, long total_chars) {
    *count = 0;
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (frequencies[i] > 0) {
            symbols[*count].symbol = (unsigned char)i;
            symbols[*count].probability = (double)frequencies[i] / total_chars;
            symbols[*count].code_length = 0;
            memset(symbols[*count].code, 0, MAX_CODE_LENGTH);
            (*count)++;
        }
    }
}

void sort_symbols_by_probability(SymbolInfo* symbols, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (symbols[j].probability < symbols[j + 1].probability) {
                SymbolInfo temp = symbols[j];
                symbols[j] = symbols[j + 1];
                symbols[j + 1] = temp;
            }
        }
    }
}

double calculate_kraft_sum(SymbolInfo* symbols, int count) {
    double kraft_sum = 0.0;
    for (int i = 0; i < count; i++) {
        kraft_sum += pow(2, -symbols[i].code_length);
    }
    return kraft_sum;
}

double calculate_entropy(SymbolInfo* symbols, int count) {
    double entropy = 0.0;
    for (int i = 0; i < count; i++) {
        if (symbols[i].probability > 0) {
            entropy -= symbols[i].probability * log2(symbols[i].probability);
        }
    }
    return entropy;
}

double calculate_average_length(SymbolInfo* symbols, int count) {
    double avg_length = 0.0;
    for (int i = 0; i < count; i++) {
        avg_length += symbols[i].probability * symbols[i].code_length;
    }
    return avg_length;
}

double calculate_total_probability(SymbolInfo* symbols, int count) {
    double total = 0.0;
    for (int i = 0; i < count; i++) {
        total += symbols[i].probability;
    }
    return total;
}

const char* get_method_name(int method) {
    switch (method) {
        case 0: return "Классический код Фано";
        case 1: return "Код Фано с медианой A2";
        case 2: return "Код Фано с энтропийной медианой";
        default: return "Неизвестный метод";
    }
}

void print_code_table(SymbolInfo* symbols, int count, const char* method_name) {
    printf("\n%s\n", method_name);
    printf("Символ\t\tВероятность\tКодовое слово\tДлина\n");
    printf("------------------------------------------------------------\n");
    
    for (int i = 0; i < count && i < 20; i++) {
        if (symbols[i].symbol >= 0xC0 && symbols[i].symbol <= 0xDF) {
            printf("RU-0x%02X\t", symbols[i].symbol);
        }
        else if (symbols[i].symbol >= 32 && symbols[i].symbol <= 126) {
            printf("'%c'\t\t", symbols[i].symbol);
        } else {
            printf("0x%02X\t\t", symbols[i].symbol);
        }
        
        printf("%.6f\t%s\t\t%d\n", 
               symbols[i].probability, 
               symbols[i].code, 
               symbols[i].code_length);
    }
    if (count > 20) {
        printf("... (и еще %d символов)\n", count - 20);
    }
    
    // Вывод суммы вероятностей
    double total_prob = calculate_total_probability(symbols, count);
    printf("------------------------------------------------------------\n");
    printf("Сумма вероятностей:\t%.6f\n", total_prob);
}

void print_analysis_results(FanoCode* fano, const char* method_name) {
    printf("\nАнализ %s\n", method_name);
    printf("------------------------------------------------------------\n");
    printf("Неравенство Крафта:\t%.6f\n", fano->kraft_sum);
    printf("Энтропия:\t\t%.6f\n", fano->entropy);
    printf("Средняя длина:\t\t%.6f\n", fano->avg_length);
    printf("Избыточность:\t\t%.6f\n", fano->redundancy);
    
    if (fano->kraft_sum <= 1.0 + 1e-10) {
        printf("✓ Неравенство Крафта выполняется\n");
    } else {
        printf("✗ Неравенство Крафта не выполняется\n");
    }
    
    printf("Эффективность:\t\t%.2f%%\n", (fano->entropy / fano->avg_length) * 100);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Использование: %s <имя_файла>\n", argv[0]);
        return 1;
    }
    
    const char* filename = argv[1];
    int frequencies[MAX_SYMBOLS];
    long total_chars;
    
    if (!read_file_and_count(filename, frequencies, &total_chars)) {
        return 1;
    }
    
    printf("Размер файла: %ld байт\n", total_chars);
    
    if (total_chars < 10240) {
        printf("Предупреждение: файл меньше 10 КБ\n");
    }
    
    // Создаем три копии для трех методов
    FanoCode fano_classic, fano_a2, fano_entropy;
    
    init_symbols(fano_classic.symbols, &fano_classic.count, frequencies, total_chars);
    init_symbols(fano_a2.symbols, &fano_a2.count, frequencies, total_chars);
    init_symbols(fano_entropy.symbols, &fano_entropy.count, frequencies, total_chars);
    
    sort_symbols_by_probability(fano_classic.symbols, fano_classic.count);
    sort_symbols_by_probability(fano_a2.symbols, fano_a2.count);
    sort_symbols_by_probability(fano_entropy.symbols, fano_entropy.count);
    
    printf("Обнаружено %d уникальных символов\n", fano_classic.count);
    
    // Вычисляем и выводим сумму вероятностей до кодирования
    double total_prob_before = calculate_total_probability(fano_classic.symbols, fano_classic.count);
    printf("Сумма вероятностей до кодирования: %.6f\n", total_prob_before);
    
    // Построение кодов тремя методами
    build_fano_code(fano_classic.symbols, 0, fano_classic.count - 1, 0, 0);
    build_fano_code(fano_a2.symbols, 0, fano_a2.count - 1, 0, 1);
    build_fano_code(fano_entropy.symbols, 0, fano_entropy.count - 1, 0, 2);
    
    // Вычисление характеристик для классического метода
    fano_classic.kraft_sum = calculate_kraft_sum(fano_classic.symbols, fano_classic.count);
    fano_classic.entropy = calculate_entropy(fano_classic.symbols, fano_classic.count);
    fano_classic.avg_length = calculate_average_length(fano_classic.symbols, fano_classic.count);
    fano_classic.redundancy = fano_classic.avg_length - fano_classic.entropy;
    
    // Вычисление характеристик для метода A2
    fano_a2.kraft_sum = calculate_kraft_sum(fano_a2.symbols, fano_a2.count);
    fano_a2.entropy = calculate_entropy(fano_a2.symbols, fano_a2.count);
    fano_a2.avg_length = calculate_average_length(fano_a2.symbols, fano_a2.count);
    fano_a2.redundancy = fano_a2.avg_length - fano_a2.entropy;
    
    // Вычисление характеристик для энтропийного метода
    fano_entropy.kraft_sum = calculate_kraft_sum(fano_entropy.symbols, fano_entropy.count);
    fano_entropy.entropy = calculate_entropy(fano_entropy.symbols, fano_entropy.count);
    fano_entropy.avg_length = calculate_average_length(fano_entropy.symbols, fano_entropy.count);
    fano_entropy.redundancy = fano_entropy.avg_length - fano_entropy.entropy;
    
    // Вывод результатов для всех методов
    print_code_table(fano_classic.symbols, fano_classic.count, get_method_name(0));
    print_analysis_results(&fano_classic, get_method_name(0));
    
    print_code_table(fano_a2.symbols, fano_a2.count, get_method_name(1));
    print_analysis_results(&fano_a2, get_method_name(1));
    
    print_code_table(fano_entropy.symbols, fano_entropy.count, get_method_name(2));
    print_analysis_results(&fano_entropy, get_method_name(2));
    
    // Сравнение всех методов
    printf("\nСРАВНЕНИЕ МЕТОДОВ ПОСТРОЕНИЯ КОДА ФАНО\n");
    printf("============================================\n");
    printf("Метод\t\t\t\tСредняя длина\tИзбыточность\tЭффективность\n");
    printf("------------------------------------------------------------\n");
    printf("%s\t\t%.6f\t%.6f\t%.2f%%\n", 
           get_method_name(0), fano_classic.avg_length, fano_classic.redundancy, 
           (fano_classic.entropy / fano_classic.avg_length) * 100);
    printf("%s\t\t%.6f\t%.6f\t%.2f%%\n", 
           get_method_name(1), fano_a2.avg_length, fano_a2.redundancy,
           (fano_a2.entropy / fano_a2.avg_length) * 100);
    printf("%s\t%.6f\t%.6f\t%.2f%%\n", 
           get_method_name(2), fano_entropy.avg_length, fano_entropy.redundancy,
           (fano_entropy.entropy / fano_entropy.avg_length) * 100);
    
    // Определение лучшего метода
    printf("\nАНАЛИЗ РЕЗУЛЬТАТОВ:\n");
    printf("------------------------------------------------------------\n");
    
    double min_length = fmin(fano_classic.avg_length, fmin(fano_a2.avg_length, fano_entropy.avg_length));
    
    if (fabs(fano_classic.avg_length - min_length) < 1e-10) {
        printf("✓ Классический метод показал наилучшую среднюю длину\n");
    }
    if (fabs(fano_a2.avg_length - min_length) < 1e-10) {
        printf("✓ Метод A2 показал наилучшую среднюю длину\n");
    }
    if (fabs(fano_entropy.avg_length - min_length) < 1e-10) {
        printf("✓ Энтропийный метод показал наилучшую среднюю длину\n");
    }
    
    // Сравнение энтропийного метода с другими
    double diff_vs_classic = fano_entropy.avg_length - fano_classic.avg_length;
    double diff_vs_a2 = fano_entropy.avg_length - fano_a2.avg_length;
    
    printf("\nСравнение энтропийного метода:\n");
    if (fabs(diff_vs_classic) < 1e-10) {
        printf("• Энтропийный метод = Классический метод\n");
    } else if (diff_vs_classic < 0) {
        printf("• Энтропийный метод ЛУЧШЕ классического на %.6f\n", -diff_vs_classic);
    } else {
        printf("• Энтропийный метод ХУЖЕ классического на %.6f\n", diff_vs_classic);
    }
    
    if (fabs(diff_vs_a2) < 1e-10) {
        printf("• Энтропийный метод = Метод A2\n");
    } else if (diff_vs_a2 < 0) {
        printf("• Энтропийный метод ЛУЧШЕ метода A2 на %.6f\n", -diff_vs_a2);
    } else {
        printf("• Энтропийный метод ХУЖЕ метода A2 на %.6f\n", diff_vs_a2);
    }
    
    return 0;
}