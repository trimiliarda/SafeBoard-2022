#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>


char dir[1000], answer[1000];


int ff (char * file);
int search (char * direct, size_t size, char * file);
int compare (char * s1, char * s2);

int main (int argc, char **argv) {
    if (argc == 2) {        //  если только имя найти
        ff (argv[1]);
    } else if (argc == 3) { //  в заданной директории ищем
        ff (argv[2]);
        if (answer[0]) {    //  если директорию нашли, то и имя ищем
            strcpy (dir, answer);
            answer[0] = '\0';
            size_t n = strlen (dir);
            dir[n++] = '/'; dir[n] = '\0';
            search (dir, n, argv[1]);
        }
    }
    
    if (answer[0]) printf ("%s\n", answer); //  если ничего не нашло (помним, answer это ещё и флаг! коммент к 69-ой стр.)
    return 0;                               //  то ничего и не выводим
}

//  поиск имени в заданной директории и ниже. рекурсией.
int search (char * direct, size_t size, char * file) {
    struct dirent **namelist;
    size_t i;
    int response, k;

    response = scandir (direct, &namelist, NULL, alphasort);

    if (response < 0) {
        perror ("\nmain.c:42:16: scandir: return -1 it's problem");
    } else {
        k = response;
        while (k-- && !answer[0]) { //  пробегаемся по всей папке, вдруг что найдём.
            if (strcmp (namelist[k]->d_name, file) == 0) {
                strcpy (answer, direct);
                for (i = 0; namelist[k]->d_name[i]; i++) answer[i + size] = namelist[k]->d_name[i];
            }
        }
        k = response;
        while (k--) {   //  если ничего не нашли, то заглядываем в другие директории 
            if (namelist[k]->d_type == 4 && namelist[k]->d_name[0] != '.' && !answer[0]) {
                for (i = 0; namelist[k]->d_name[i]; i++) direct[i + size] = namelist[k]->d_name[i];
                direct[i + size] = '/'; direct[++i + size] = '\0';

                if (access (direct, i + size) == 0) search (direct, i + size, file); // проверяю прав доступа
            }
            free (namelist[k]);
        }
        free (namelist);
    }
    return 0;
}

//  подготовка к поиску
int ff (char * file) {
    size_t i, n = 999;
    answer[0] = '\0';   //  answer использую и для записи пути, если файл нашёлся, и как флаг для проверки найден/нет
    getcwd (dir, n);

    for (i = 1; i < n; i++) if (dir[i] == '/') break;   // у меня мак, поэтому я начинаю проверять с "/User/"

    dir[++i] = '\0';        //  дабы избежать возможных проблем.
    search (dir, i, file);  //  начинаем поиск файла от корня.
    return 0;
}

//  если встречаем маску, то придумываем что делать, иначе strcmp.  пока не придкмал
int compare (char * str, char * exp) {
    int i, check = 0;
    for (i = 0; str[i]; i++) if (str[i] == '*') check = 1;
    if (check) {
        int res;
        size_t i = 0, j = 0;
        while (1)
        {
            if (str[i] == exp[j]) {i++; j++; continue;}
            if (exp[j] == '*') {
                while (exp[j] != '*') {j++;}
                while (exp[j] != str[i]) {i++;}
            }
            break;
        }
    } else {
        return strcmp (str, exp);
    }
    
    return 0;
}
