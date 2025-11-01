#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_WORDS 500
#define MAX_LEN 100

typedef struct {
    char name[MAX_LEN];
    int used;
} Variable;

int is_keyword(const char *word) {
    const char *keywords[] = {
        "int", "float", "char", "double", "if", "else", "for", "while",
        "return", "void", "include", "printf", "scanf", "main", "break",
        "continue", NULL
    };
    for (int i = 0; keywords[i]; i++)
        if (strcmp(word, keywords[i]) == 0) return 1;
    return 0;
}

int is_valid_identifier(const char *word) {
    if (!isalpha(word[0]) && word[0] != '_') return 0;
    for (int i = 1; word[i]; i++) {
        if (!isalnum(word[i]) && word[i] != '_')
            return 0;
    }
    return 1;
}

int find_variable(Variable vars[], int count, const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(vars[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char line[256];
    int line_no = 0;
    int first = 1;
    int open_braces = 0, open_parens = 0;
    Variable declared[MAX_WORDS];
    int declared_count = 0;

    printf("[");

    while (fgets(line, sizeof(line), stdin)) {
        line_no++;

        // Clean line (remove non-printable chars)
        char clean[256];
        int ci = 0;
        for (int i = 0; line[i]; i++) {
            if (isprint(line[i])) clean[ci++] = line[i];
        }
        clean[ci] = '\0';
        if (strlen(clean) == 0) continue;

        // Track braces & parentheses, skip inside quotes
        int in_string = 0;
        for (int i = 0; clean[i]; i++) {
            if (clean[i] == '"' && (i == 0 || clean[i - 1] != '\\'))
                in_string = !in_string;
            if (!in_string) {
                if (clean[i] == '{') open_braces++;
                if (clean[i] == '}') open_braces--;
                if (clean[i] == '(') open_parens++;
                if (clean[i] == ')') open_parens--;
            }
        }

        // Check for missing semicolon (ignore blocks, directives, loops, conditionals)
        if (strchr(clean, '{') == NULL && strchr(clean, '}') == NULL &&
            strstr(clean, "#include") == NULL &&
            strstr(clean, "if") == NULL && strstr(clean, "for") == NULL &&
            strstr(clean, "while") == NULL && strchr(clean, ';') == NULL) {
            if (!first) printf(",");
            printf("{\"type\":\"Syntax Error\",\"line\":%d,\"message\":\"Missing semicolon ';'\"}", line_no);
            first = 0;
        }

        // Tokenization (ignore strings)
        char temp[256];
        int ti = 0;
        in_string = 0;
        for (int i = 0; clean[i]; i++) {
            if (clean[i] == '"' && (i == 0 || clean[i - 1] != '\\'))
                in_string = !in_string;
            if (!in_string)
                temp[ti++] = clean[i];
        }
        temp[ti] = '\0';

        char *token = strtok(temp, " ;=(){}+-*/,<>\n\t");
        char prev_token[MAX_LEN] = "";

        while (token) {
            // Skip numeric constants
            int all_digits = 1;
            for (int i = 0; token[i]; i++) {
                if (!isdigit(token[i])) { all_digits = 0; break; }
            }
            if (all_digits) {
                token = strtok(NULL, " ;=(){}+-*/,<>\n\t");
                continue;
            }

            // Handle declarations
            if ((strcmp(prev_token, "int") == 0 || strcmp(prev_token, "float") == 0 ||
                 strcmp(prev_token, "char") == 0 || strcmp(prev_token, "double") == 0) &&
                is_valid_identifier(token) && !is_keyword(token)) {
                if (find_variable(declared, declared_count, token) == -1) {
                    strcpy(declared[declared_count].name, token);
                    declared[declared_count].used = 0;
                    declared_count++;
                }
            }
            // Invalid identifier like 3abc
            else if (isdigit(token[0])) {
                if (!first) printf(",");
                printf("{\"type\":\"Lexical Error\",\"line\":%d,\"message\":\"Invalid identifier '%s'\"}", line_no, token);
                first = 0;
            }
            // Undeclared variable usage
            else if (is_valid_identifier(token) && !is_keyword(token)) {
                int idx = find_variable(declared, declared_count, token);
                if (idx == -1) {
                    if (!first) printf(",");
                    printf("{\"type\":\"Semantic Error\",\"line\":%d,\"message\":\"Undeclared variable '%s'\"}", line_no, token);
                    first = 0;
                } else {
                    declared[idx].used = 1;
                }
            }

            strcpy(prev_token, token);
            token = strtok(NULL, " ;=(){}+-*/,<>\n\t");
        }
    }

    // Final unbalanced checks
    if (open_braces != 0) {
        if (!first) printf(",");
        printf("{\"type\":\"Syntax Error\",\"line\":0,\"message\":\"Unbalanced braces '{}'\"}");
        first = 0;
    }
    if (open_parens != 0) {
        if (!first) printf(",");
        printf("{\"type\":\"Syntax Error\",\"line\":0,\"message\":\"Unbalanced parentheses '()'\"}");
        first = 0;
    }

    printf("]");
    return 0;
}
