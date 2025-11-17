#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TARGET_SIZE 15000  // 15 Кбайт для надежности

// Частоты букв русского языка (примерные)
const char* russian_letters = "оеаинтсрвлкмдпуяыьгзбчйхжшюцщэфъ";
const double frequencies[] = {
    0.109, 0.087, 0.080, 0.073, 0.067, 0.063, 0.055, 0.047, 0.045, 0.039,
    0.035, 0.032, 0.030, 0.028, 0.026, 0.024, 0.021, 0.019, 0.018, 0.017,
    0.016, 0.015, 0.014, 0.013, 0.012, 0.011, 0.010, 0.009, 0.008, 0.007,
    0.006, 0.005, 0.004
};

// Текст-шаблон для генерации осмысленного текста
const char* templates[] = {
    "В чащах юга жил бы цитрус? Да, но фальшивый экземпляр!",
    "Широкая электрификация южных губерний даст мощный толчок развитию сельского хозяйства.",
    "Съешь же ещё этих мягких французских булок, да выпей чаю.",
    "Любя, съешь щипцы, — вздохнёт мэр, — кайф жгуч.",
    "Эх, чужак! Общий съём цен шляп (юфть) — вдрызг!",
    "Флегматичная эта верблюдица жуёт у подъезда засыхающий горький шиповник.",
    "Аэрофотосъёмка ландшафта уже выявила земли богачей и процветающих крестьян.",
    "Шизофреничка в бегах: экс-граф? Плюш изъят. Бьём чуждый цен хвощ!"
};

void generate_russian_text(FILE* file) {
    int template_count = sizeof(templates) / sizeof(templates[0]);
    int total_letters = strlen(russian_letters);
    
    srand(time(NULL));
    
    long bytes_written = 0;
    int sentence_count = 0;
    
    while (bytes_written < TARGET_SIZE) {
        // Случайно выбираем между шаблоном и случайным текстом
        if (rand() % 3 == 0 && sentence_count < 50) {
            // Используем готовый шаблон
            const char* template = templates[rand() % template_count];
            fprintf(file, "%s ", template);
            bytes_written += strlen(template) + 1;
            sentence_count++;
        } else {
            // Генерируем случайное предложение
            int sentence_length = 10 + rand() % 20;
            
            // Первая буква заглавная
            char first_letter = russian_letters[rand() % (total_letters - 8)];
            fprintf(file, "%c", first_letter - 32); // Преобразуем в заглавную
            
            // Генерируем остальные буквы предложения
            for (int i = 1; i < sentence_length; i++) {
                // Выбираем букву с учетом частот
                double rand_val = (double)rand() / RAND_MAX;
                double cumulative = 0.0;
                int letter_index = 0;
                
                for (int j = 0; j < total_letters; j++) {
                    cumulative += frequencies[j];
                    if (rand_val <= cumulative) {
                        letter_index = j;
                        break;
                    }
                }
                
                fprintf(file, "%c", russian_letters[letter_index]);
            }
            
            // Завершаем предложение
            const char* endings[] = {". ", "! ", "? "};
            const char* ending = endings[rand() % 3];
            fprintf(file, "%s", ending);
            
            bytes_written += sentence_length + 2;
            sentence_count++;
        }
        
        // Добавляем абзац каждые 5-10 предложений
        if (sentence_count > 0 && sentence_count % (5 + rand() % 6) == 0) {
            fprintf(file, "\n\n");
            bytes_written += 2;
        }
        
        // Периодически выводим прогресс
        if (sentence_count % 20 == 0) {
            printf("Сгенерировано: %ld байт\n", bytes_written);
        }
    }
}

void generate_technical_text(FILE* file) {
    const char* tech_words[] = {
        "алгоритм", "программа", "компилятор", "интерфейс", "база данных",
        "сервер", "клиент", "фреймворк", "библиотека", "функция",
        "переменная", "константа", "массив", "структура", "объект",
        "класс", "метод", "параметр", "аргумент", "интерфейс",
        "имплементация", "инкапсуляция", "наследование", "полиморфизм"
    };
    
    const char* verbs[] = {
        "выполняет", "обрабатывает", "генерирует", "создает", "удаляет",
        "изменяет", "проверяет", "анализирует", "оптимизирует", "компилирует"
    };
    
    int word_count = sizeof(tech_words) / sizeof(tech_words[0]);
    int verb_count = sizeof(verbs) / sizeof(verbs[0]);
    
    srand(time(NULL));
    
    long bytes_written = 0;
    int paragraph_count = 0;
    
    while (bytes_written < TARGET_SIZE) {
        // Генерируем абзац
        int sentences_in_paragraph = 3 + rand() % 4;
        
        for (int s = 0; s < sentences_in_paragraph; s++) {
            // Генерируем предложение
            int words_in_sentence = 5 + rand() % 6;
            
            for (int w = 0; w < words_in_sentence; w++) {
                const char* word;
                if (w == 0) {
                    // Первое слово с заглавной буквы
                    char first_letter = tech_words[rand() % word_count][0];
                    fprintf(file, "%c", first_letter - 32);
                    word = tech_words[rand() % word_count] + 1;
                } else if (w == 1) {
                    // Второе слово - глагол
                    word = verbs[rand() % verb_count];
                } else {
                    word = tech_words[rand() % word_count];
                }
                
                fprintf(file, "%s", word);
                bytes_written += strlen(word);
                
                if (w < words_in_sentence - 1) {
                    fprintf(file, " ");
                    bytes_written++;
                }
            }
            
            // Завершаем предложение
            fprintf(file, ". ");
            bytes_written += 2;
        }
        
        // Конец абзаца
        fprintf(file, "\n\n");
        bytes_written += 2;
        paragraph_count++;
        
        // Прогресс
        if (paragraph_count % 5 == 0) {
            printf("Сгенерировано: %ld байт\n", bytes_written);
        }
    }
}

int main() {
    printf("Генератор тестового файла для кода Шеннона\n");
    printf("==========================================\n");
    
    FILE* file = fopen("russian_text.txt", "w");
    if (!file) {
        printf("Ошибка создания файла!\n");
        return 1;
    }
    
    printf("Выберите тип текста:\n");
    printf("1 - Художественный текст\n");
    printf("2 - Технический текст\n");
    printf("Ваш выбор: ");
    
    int choice;
    scanf("%d", &choice);
    
    printf("Генерация файла...\n");
    
    if (choice == 1) {
        generate_russian_text(file);
        printf("Сгенерирован художественный текст\n");
    } else {
        generate_technical_text(file);
        printf("Сгенерирован технический текст\n");
    }
    
    fclose(file);
    
    // Проверяем размер файла
    file = fopen("russian_text.txt", "rb");
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fclose(file);
    
    printf("\nФайл 'russian_text.txt' успешно создан!\n");
    printf("Размер файла: %ld байт (%.2f Кбайт)\n", 
           file_size, (double)file_size / 1024);
    printf("Файл готов для использования с программой кодирования Шеннона.\n");
    
    return 0;
}