#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>


char dir[1000], answer[1000];
size_t length;


int ff (char * file);
int search (char * direct, size_t size, char * file);
void scroll (char * path, size_t n);
size_t set_len(char * name);

int main (int argc, char **argv) 
{
    if (argc == 2) {        //  проверка кол-ва аргументов
        ff (argv[1]);
        if (answer[0]) {    //если нашли директорию, то выводим содержимое
            length = set_len (answer);
            strcpy (dir, answer);
            scroll (dir, strlen (dir));
        }
    } 
    
    return 0;
}

//  рекурсивный ВЫВОД директории
void scroll (char * path, size_t n) {   //  
    struct dirent ** list;
    size_t i;
    int response, k;
    response = scandir (path, &list, NULL, alphasort);

    if (response < 0)
        perror ("\nmain.c:39:16: scandir: return -1 it's problem");
    else {
        k = response;
        while (k--) {
            size_t i, len = strlen (list[k]->d_name);
            char * new_path;
            new_path = malloc (sizeof (char) * (len + n + 1));
            if (new_path) {
                strcpy (new_path, path);    //  доращиваем новый путь
                new_path[n - 1] = '/';

                for (i = 0; i < len; i++) new_path[i + n] = list[k]->d_name[i]; //  доращиваем новый путь
                new_path[len + n] = '\0';

                for (i = length; new_path[i]; i++) answer[i - length] = new_path[i];    // создаём выводимую строку
                answer[i - length] = '\0';
                
                printf ("%s\n", answer);    //  вывод нужной части пути

                if (list[k]->d_type == 4 && list[k]->d_name[0] != '.') {
                    new_path[len + n] = '/'; new_path[len + n + 1] = '\0';
                    if (access (new_path, i + n) == 0) scroll (new_path, len + n + 1); // проверяю права доступа
                }
                free (list[k]);
                free (new_path);
            }
        }
        free (list);
    }
}

//  рекурсивный обход директории. ПОИСК!
int search (char * direct, size_t size, char * file) {
    struct dirent **namelist;
    size_t i;
    int response, k;

    response = scandir (direct, &namelist, NULL, alphasort);

    if (response < 0)
        perror ("\nmain.c:79:16: scandir: return -1 it's problem");
    else {
        k = response;
        while (k-- && !answer[0]) { //  ищем пробегаясь по всей папке
            if (strcmp (namelist[k]->d_name, file) == 0) {
                strcpy (answer, direct);
                for (i = 0; namelist[k]->d_name[i]; i++) answer[i + size] = namelist[k]->d_name[i];
                answer[i++ + size] = '/'; answer[i + size] = '\0';
            }
        }
        k = response;
        while (k--) {   //  если пока ещё не нашли, то заглядываем в другие папки
            if (namelist[k]->d_type == 4 && namelist[k]->d_name[0] != '.' && !answer[0]) {
                for (i = 0; namelist[k]->d_name[i]; i++) direct[i + size] = namelist[k]->d_name[i];
                direct[i + size] = '/'; direct[++i + size] = '\0';

                if (access (direct, i + size) == 0) search (direct, i + size, file); // проверяю права доступа
            }
            free (namelist[k]);
        }
        free (namelist);
    }
    return 0;
}

//  подшотовка к поиску
int ff (char * file) {
    size_t i, n = 999;
    answer[0] = '\0';   //  answer использую и для записи пути, если файл нашёлся, и как флаг для проверки найден/нет
    getcwd (dir, n);

    for (i = 1; i < n; i++) if (dir[i] == '/') break;   // у меня мак, поэтому я начинаю проверять с "/User/"

    dir[++i] = '\0';                //  дабы избежать возможных проблем.
    search (dir, i, file);          //  начинаем поиск файла от корня.
    return 0;
}

//  получаем длинну ЧАСТИ пути, которую не надо выводить
size_t set_len(char * name) {
    size_t res = strlen (name) - 1;
    while (res > 0 && name[--res] != '/') {}
    return res;
}