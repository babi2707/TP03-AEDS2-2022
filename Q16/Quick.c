
/****************************************
 * 
 * @author Barbara Luciano Araujo
 * Matricula: 748190
 * TP03 - Questão 16 - Quicksort com LISTA DINÂMICA DUPLAMENTE ENCADEADA em C
 * 
****************************************/

// ------------------------- Includes -------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <err.h>
// ------------------------------------------------------------




// ------------------------- Definições -------------------------
#define MAX_MOVIES          100
#define MAX_FIELD_SIZE      100
#define MAX_KEYWORDS        50
#define MAX_LINE_SIZE       250
#define FDR_PREFIX          "./tmp/filmes/"
// --------------------------------------------------------------




// ------------------------- Structs -------------------------

// ---------- data ----------

typedef struct {
    int year,
    month,
    day;
} Date; 

// ---------------------------

// ---------- filme ----------

typedef struct {
    char name[MAX_FIELD_SIZE],
        original_title[MAX_FIELD_SIZE],
        genre[MAX_FIELD_SIZE], 
        original_language[MAX_FIELD_SIZE], 
        situation[MAX_FIELD_SIZE],
        keywords[MAX_KEYWORDS][MAX_FIELD_SIZE];
    Date release_date;
    int duration, count_keywords;
    float budget;
} Movie;

// ---------------------------

// -----------------------------------------------------------




// ------------------------- variáveis globais -------------------------
Movie movies[MAX_MOVIES];
int count_movies = 0;
// ---------------------------------------------------------------------



// ------------------------- funções -------------------------

// ---------- is Fim ----------
bool isFim(char *str) { return str[0] == 'F' && str[1] == 'I' && str[2] == 'M'; }
// ----------------------------

// ---------- remover quebra de linha ----------
char *remove_line_break(char *line) {
    while (*line != '\r' && *line != '\n') line++;
    *line = '\0';
    return line;
}
// ----------------------------------------------

// ---------- ler a linha ----------

char *freadline(char *line, int max_size, FILE *file) { return remove_line_break(fgets(line, max_size, file)); }
char *readline(char *line, int max_size) { return freadline(line, max_size, stdin); }

// ---------------------------------

// ---------- index of ----------
long int indexOf(char *str, char *search) {
    long int pos = strcspn(str, search);
    return pos == strlen(str) ? -1 : pos;
}
// ------------------------------

// ---------- substring ----------
char *substring(char *string, int position, int length) {
    char *p;
    int c;
    p = malloc(length+1);
    if(p == NULL) {
        printf("Unable to allocate memory.\n");
        exit(1);
    }
    for(c = 0; c < length; c++) {
        *(p+c) = *(string+position-1);      
        string++;  
    }
    *(p+c) = '\0';
    return p;
}
// -------------------------------

// ---------- substituir ----------
void str_replace(char *target, const char *needle, const char *replacement) {
    char buffer[1024] = { 0 };
    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t needle_len = strlen(needle);
    size_t repl_len = strlen(replacement);

    while(1) {
        const char *p = strstr(tmp, needle);
        if(p == NULL) {
            strcpy(insert_point, tmp);
            break;
        }
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;
        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;
        tmp = p + needle_len;
    }
    strcpy(target, buffer);
}
// --------------------------------

// ---------- primeiro digito ----------
int firstDigit(const char *str, int start) {
    for(int i = start; i != strlen(str); i++) if(str[i] >= '0' && str[i] <= '9') return i;
    return -1;
}
// -------------------------------------

// ---------- remover tags ---------- 
char *extractOnlyText(char *html, char *text) {
    char *start = text;
    int contagem = 0;
    while (*html != '\0') {
        if (*html == '<') {
            if (
                (*(html + 1) == 'p') ||
                (*(html + 1) == 'b' && *(html + 2) == 'r') ||
                (*(html + 1) == '/' && *(html + 2) == 'h' && *(html + 3) == '1') ||
                (*(html + 1) == '/' && *(html + 2) == 't' && *(html + 3) == 'h') ||
                (*(html + 1) == '/' && *(html + 2) == 't' && *(html + 3) == 'd')
            ) break;
            else contagem++;
        }
        else if (*html == '>') contagem--;
        else if (contagem == 0 && *html != '"') {
            if (*html == '&') html = strchr(html, ';');
            else if (*html != '\r' && *html != '\n') *text++ = *html;
        }
        html++;
    }
    *text = '\0';
    return *start == ' ' ? start + 1 : start;
}
// ----------------------------------

// ---------- funções da classe Movies ----------

// ----- imprimir -----
void movie_print(Movie *movie) {
    printf("%s %s %02i/%02i/%04i %i %s %s %s %g [",
    movie -> name,
    movie -> original_title,
    movie -> release_date.day, movie -> release_date.month, movie -> release_date.year,
    movie -> duration,
    movie -> genre,
    movie -> original_language,
    movie -> situation,
    movie -> budget);
    for(int i = 0; i < movie -> count_keywords; i++) {
        if(i == movie -> count_keywords - 1) printf("%s]\n", movie -> keywords[i]);
        else printf("%s, ", movie -> keywords[i]);
    }
    if(movie -> count_keywords == 0) printf("]\n");
}
// --------------------

// ----- ler o HTML -----
void movie_readHtml(Movie *movie, char *filename) {
    FILE *html_file;
    char *line_html = NULL;
    char path[MAX_LINE_SIZE];
    size_t len = 0;
    ssize_t read;

    strcpy(path, FDR_PREFIX);
    strcat(path, filename);

    html_file = fopen(path, "r");

    if(html_file == NULL) exit(EXIT_FAILURE);

    // ----- variáveis de movie -----
    char *name = NULL, 
    *original_title = NULL,
    *genre = NULL,
    *original_language = NULL,
    *situation = NULL,
    *keywords = NULL;

    Date release_date;

    release_date.day = 0;
    int duration = -1;
    float budget = -1;
    // ------------------------------
    
    // ----- ler cada linha do HTML -----
    while((read = getline(&line_html, &len, html_file)) != -1) {

        // ---- nome ----
        if(name == NULL) {
            if(strstr(line_html, "<title>") != NULL) { 
                name = strstr(line_html, "<title>") + 7;
                strcpy(movie -> name, name);
                str_replace(movie -> name, "&#8212;", "—");
                movie -> name[strlen(movie -> name) - 46] = '\0';
            }
        }
        // --------------

        // ---- titulo original ----
        if(original_title == NULL) {
            if(strstr(line_html, "<p class=\"wrap\">") != NULL) {
                original_title = strstr(line_html, "</strong> ") + 10;
                original_title[strlen(original_title) - 5] = '\0';
                strcpy(movie -> original_title, original_title);
            }
        }
        // -------------------------

        // ---- data de lançamento ----
        if(release_date.day == 0) {
            if(strstr(line_html, "<span class=\"release\">") != NULL) {    
                // Skip one line
                read = getline(&line_html, &len, html_file);
                char *day, *month, *year;
                day = substring(line_html, 9, 2);
                month = substring(line_html, 12, 2);
                year = substring(line_html, 15, 4);

                movie -> release_date.day = atoi(day);
                movie -> release_date.month = atoi(month);
                movie -> release_date.year = atoi(year);
            }
        }
        // ----------------------------

        // ---- duração ----
        if(duration == -1) {
            if(strstr(line_html, "<span class=\"runtime\">") != NULL) { 
                // Skip two lines
                read = getline(&line_html, &len, html_file);
                read = getline(&line_html, &len, html_file);
                int h_pos = indexOf(line_html, "h"),
                    hours = 0,
                    minutes = 0;

                if(h_pos != -1) hours = atoi(substring(line_html, firstDigit(line_html, 0), h_pos));
    
                minutes = atoi(substring(line_html, firstDigit(line_html, h_pos == -1 ? 0 : h_pos), strlen(line_html) - 1));
                duration = (hours * 60) + minutes;
                movie -> duration = duration;
            }
        }
        // -----------------

        // ---- gênero ----
        if(genre == NULL) {
            if(strstr(line_html, "<span class=\"genres\">") != NULL) {
                // Skip two lines
                read = getline(&line_html, &len, html_file);
                read = getline(&line_html, &len, html_file);
                extractOnlyText(line_html, movie -> genre);
                genre = substring(movie -> genre, 7, strlen(movie -> genre));
                strcpy(movie -> genre, genre);
            }
        }
        // ----------------

        // ---- idioma original ----
        if(original_language == NULL) {
            if(strstr(line_html, "<bdi>Idioma original</bdi>") != NULL) {   
                strcpy(movie -> original_language, line_html);
                original_language = substring(movie -> original_language, 50, strlen(line_html) - 54);
                strcpy(movie -> original_language, original_language);
            }
        }
        // -------------------------

        // ---- situação ----
        if(situation == NULL) {
            if(strstr(line_html, "<bdi>Situação</bdi>") != NULL) { 
                strcpy(movie -> situation, line_html);
                situation = substring(movie -> situation, 44, strlen(line_html) - 44);
                strcpy(movie -> situation, situation);
            }
        }
        // ------------------

        // ---- orçamento ----
        if(budget == -1) {
            if(strstr(line_html, "<bdi>Orçamento</bdi>") != NULL) {
                char *p_budget, e_budget[strlen(line_html)];
                strcpy(e_budget, line_html);
                p_budget = substring(e_budget, 45, strlen(line_html) - 49);
                if(!strcmp(p_budget, "-")) movie -> budget = 0;
                else
                {
                    strcpy(e_budget, p_budget);
                    str_replace(e_budget, "$", "");
                    str_replace(e_budget, ",", "");
                    movie -> budget = atof(e_budget);
                }
            }
        }
        // -------------------

        // ---- keywords ----
        if(keywords == NULL) {
            if(strstr(line_html, "<h4><bdi>Palavras-chave</bdi></h4>") != NULL) { 
                // Skip two lines until keywords starts
                for(int i = 0; i < 2; i++) read = getline(&line_html, &len, html_file);
                char tmp_line[strlen(line_html)];
                strcpy(tmp_line, line_html);       
                keywords = substring(tmp_line, 5, strlen(line_html) - 5);
                if(strcmp(keywords, "<p><bdi>Nenhuma palavra-chave foi adicionada.</bdi></p>")) {
                    // Skip more two lines until keywords starts
                    for(int x = 0; x < 2; x++) read = getline(&line_html, &len, html_file);

                    while(true) {         
                        if(strstr(line_html, "</ul>") != NULL) break;

                        if(strstr(line_html, "<li>") != NULL) {               
                            extractOnlyText(line_html, tmp_line);
                            keywords = substring(tmp_line, 9, strlen(line_html) - 8);
                            strcpy(movie -> keywords[movie -> count_keywords++], keywords);
                        }

                        read = getline(&line_html, &len, html_file);
                    }
                }
            }
        }
        // ------------------

        // ---- verifica se a variável é null ----
        if(original_title == NULL) {
            strcpy(movie -> original_title, movie -> name);
        }
        // ---------------------------------------
    }
    // ----------------------------------

    fclose(html_file); // fecha o arquivo
    if(line_html) free(line_html); // libera a memória
}
// ----------------------

// ----- inicializações -----
void movie_inicio (Movie *movie) {

    strcpy(movie -> name, "");
    strcpy(movie -> original_title, "");
    strcpy(movie -> genre, "");
    strcpy(movie -> original_language, "");
    strcpy(movie -> situation, "");

    movie -> release_date.day = 0;
    movie -> release_date.month = 0;
    movie -> release_date.year = 0;
    movie -> duration = 0;
    movie -> count_keywords = 0;
    movie -> budget = 0;

    for(int i = 0; i < MAX_KEYWORDS; i++) {
        strcpy(movie -> keywords[i], "");
    }

}
// --------------------------

// ---------------------------------------------

// -----------------------------------------------------------

// ------------------------------ Lista Dupla ------------------------------

// --------------- celula ---------------
typedef struct CelulaDupla
{
    Movie elemento; // elemento a ser inserido na celula
    struct CelulaDupla* prox; // ponteiro que aponta para celula prox
    struct CelulaDupla* ant; // ponteiro que aponta para celula ant
} CelulaDupla;

CelulaDupla* newCelulaDupla (Movie elemento) {
    CelulaDupla* nova = (CelulaDupla*)malloc(sizeof(CelulaDupla));
    nova->elemento = elemento;
    nova->ant = nova->prox = NULL;

    return nova;
}

CelulaDupla* primeiro;
CelulaDupla* ultimo;

void start() { 
    Movie tmp;

    primeiro = newCelulaDupla(tmp);
    ultimo = primeiro;
}
// --------------------------------------

// --------------- inserir ---------------

void insertFim (Movie x) {

    ultimo->prox = newCelulaDupla(x);
    ultimo->prox->ant = ultimo;
    ultimo = ultimo->prox;
    
}

// ---------------------------------------

// --------------- tamanho ---------------

int tamanho () {

    int tam = 0;
    CelulaDupla* i;

    for (i = primeiro; i != ultimo; i = i->prox, tam++);

    return tam;    
}

// ---------------------------------------

// --------------- mostrar ---------------

void mostrar () {
    CelulaDupla* i;

    for (i = primeiro->prox; i != NULL; i = i->prox) {
        movie_print(&(i->elemento));
    }
}

// ---------------------------------------

// --------------- swap ---------------

void swap (CelulaDupla* i, CelulaDupla* j) {
    Movie tmp = i->elemento;
    i->elemento = j->elemento;
    j->elemento = tmp;
}

// ---------------------------------------

// --------------- quickSort ---------------

void quickSort(CelulaDupla** filme, int esq, int dir) {
    int i = esq, j = dir;

    Movie pivo = filme[(esq+dir)/2]->elemento;

    while (i <= j) {
        if (strcmp(filme[i]->elemento.situation, pivo.situation) == 0) {
            while(strcmp(filme[i]->elemento.name, pivo.name) < 0) i++;
        } else {
            while (strcmp(filme[i]->elemento.situation, pivo.situation) < 0) i++;
        }

        if (strcmp(filme[j]->elemento.situation, pivo.situation) == 0) {
            while(strcmp(filme[j]->elemento.name, pivo.name) > 0) j--;
        } else {
            while (strcmp(filme[j]->elemento.situation, pivo.situation) > 0) j--;
        }

        if (i <= j) swap(filme[i++], filme[j--]);

    }

    if (esq < j) quickSort(filme, esq, j);
    if (i < dir) quickSort(filme, i, dir);
}

void sort () {
    int tam = tamanho(), j = 0;

    CelulaDupla* filme[tam];

    for (CelulaDupla* i = primeiro->prox; i != NULL; i = i->prox, j++) {
        filme[j] = i;
    }

    quickSort(filme, 0, tam - 1); 
}
// ---------------------------------------

// -------------------------------------------------------------------

int main() {

    char line[MAX_LINE_SIZE];

    readline(line, MAX_LINE_SIZE);

    // ----- leitura do arquivo -----

    while(!isFim(line)) {

        Movie movieMain;

        movie_inicio(&movieMain);
        movie_readHtml(&movieMain, line);
        insertFim(movieMain);
        readline(line, MAX_LINE_SIZE);

    }

    // ------------------------------

    sort();
    mostrar();

    
    return EXIT_SUCCESS;
}