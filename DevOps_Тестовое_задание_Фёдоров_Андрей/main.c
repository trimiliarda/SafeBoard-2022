#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define risk2 "rm -rf ~/Documents"
#define risk1 "system(\"launchctl load /Library/LaunchAgents/com.malware.agent\")"
#define risk_JS "<script>evil_script()</script>"

//  Решил сделать глобальными переменными считать кол-во угроз, файлы и ошибки.
//  Можно структурой, в идеале класс (ООП), то решил реализовать так.

int countFILES = 0;
int errorFILES = 0;
int JS = 0;
int Unix = 0;
int macOS = 0;

time_t start, end;  //  засекаю время работы

// int scan_util (char * path);
int scan_util_pro (char * path);
void loads (int k, int len);
int scan_factor (int * c);
int scan_js(int * c);
void show_res (void);

int main (int argc, char ** argv) {
    if (argc == 2) {    //  проверка наличия наличия аргумента
        start = time(NULL);
        scan_util_pro (argv[1]);
        show_res ();    //  вывод результата работы программы
    }
    return 0;
}

//  прохожу все файлы в директории
int scan_util_pro (char * path) {
    char command[] = "\' >> ~/buff.txt\0", name[] = "../../buff.txt\0";
    size_t SIZE, i;
    struct dirent ** list;
    struct dirent * ent;
    int response, k, js_flag = 0;

    response = scandir (path, &list, NULL, alphasort);  //  сканируем директорию

    if (response < 0) {
        perror ("problemo!..");
    } else {
        k = response;
        while (k--) {
            ent = list[k];
            if (ent->d_type == 8) { //  проверяем файл перед нами или нет (директория или ещё что)
                js_flag = 0;    //  проверяем расширение файла .js или нет
                if (ent->d_name[ent->d_namlen - 3] == '.' && ent->d_name[ent->d_namlen - 2] == 'j' && ent->d_name[ent->d_namlen - 1] == 's') js_flag = 1;
                FILE * myFile;
                system ("touch ~/buff.txt");    //  создаём файел-буфер, с которым, возможно, придётся нам работать (очень вероятно)

                if (access (ent->d_name, ent->d_namlen) == -1) {    //  проверям права доступа
                    char * c_path = malloc (sizeof (char) * (ent->d_namlen + strlen (path) + 20));
                    strcpy (c_path, "cat \'");
                    for (i = 0; path[i]; i++) c_path[i + 5] = path[i]; c_path[i + 5] = '/'; c_path[i + 6] = '\0';
                    SIZE = strlen (c_path); 
                    for (i = 0; i < ent->d_namlen; i++) c_path[i + SIZE] = ent->d_name[i]; c_path[i + SIZE] = '\0';                
                    SIZE = strlen (c_path); for (i = 0; command[i]; i++) c_path[i + SIZE] = command[i]; c_path[i + SIZE] = '\0';

                    system (c_path);    //  копируем всё в наш буфер, если не имеем доступ к файлу

                    myFile = fopen (name, "r");
                } else {    //  если доступ имеем, то просто открываем файл
                    printf ("%s\n", ent->d_name);
                    myFile = fopen (ent->d_name, "r");
                }

                if (myFile) {   //
                    countFILES++;   //  подсчитываем кол-во открытых => проверенных файлов
                    int c;
                    while (!feof (myFile)) {
                        c = fgetc(myFile);

                        if (js_flag) {  //  если файл с расширением .js  то дополнительная проверка
                            if (scan_js (&c)) break;
                            if (scan_factor (&c)) break;
                        } else {
                            if (scan_factor (&c)) break;
                        }
                    }
                    fclose (myFile);
                } else {    //  если не получилось прочитать файл, то записываем ошибку..
                    errorFILES++;
                    printf ("can\'t open file...\n\n");
                }
                system ("rm ~/buff.txt\0");
            } else {
                // printf ("directory\n");
            }
            free (list[k]);
            loads (k, response);    //  вывожу загрузку - номер файла / кол-во всех файлов в дир-е
        }
        free (list);
    }
    return 0;
}

//  поиск подстрок в файле
int scan_factor(int * c) {
    static int r1 = 0, r2 = 0;

//если находим совпадение символов в файле и опасной строке, то инкрем. индекс соответствующей угрозы

    if (risk2[r2] == (char) *c && r2 < 18) {r2++;}
    else if (r2 == 18) {r2 = 0; Unix++; return 1;}   // если дошли до конца строки => угрозу нашли => выходим из файла
    else {r2 = 0;}

    if (risk1[r1] == (char) *c && r1 < 64) {r1++;}
    else if (r1 == 64) {r1 = 0; macOS++; return 1;}  // если дошли до конца строки => угрозу нашли => выходим из файла
    else {r1 = 0;}

    return 0;
}

//  также сканируем дополнительно .js файлы
int scan_js(int * c) {
    static int js = 0;

    if (risk_JS[js] == (char) *c && js < 30) {js++;}
    else if (js == 30) {js = 0; JS++; return 1;}
    else {js = 0;}

    return 0;
}

void show_res (void) {
    end = time(NULL);
    // system ("clear");

    printf ("\n======== Scan result ========\n\n");
    printf ("Processed files: %d\n", countFILES);
    printf ("JS detects: %d\n", JS);
    printf ("Unix detects: %d\n", Unix);
    printf ("macOS detects: %d\n", macOS);
    printf ("Errors: %d\n", errorFILES);
    printf ("Exection time: %.2f seconds\n", difftime(end, start));
    printf ("\n=============================\n");
}

void loads (int k, int len) {
    system ("clear");
    printf ("\n\t[");
    for (int i = 0; i < 50; i++) printf ("%c", (i <= 50 * (len - k) / len) ? '#' : '_');    //  шкала выполнения
    printf ("]\t%d%c\n", 100 * (len - k) / len, '%');
}

// int scan_util (char * path) {
//     DIR * dir;
//     char command[] = "\' >> ~/buff.txt", name[] = "../../buff.txt";
//     size_t SIZE, i;
//     struct dirent * ent;
//     if ((dir = opendir (path)) != NULL) {
//         // printf ("\n\t%d\n\n", dir->__dd_td);
//         while ((ent = readdir (dir)) != NULL) {
            
//             if (ent->d_type == 8) {
//                 printf ("%d\t%d\t%s\n", ent->d_type, access (ent->d_name, ent->d_namlen), ent->d_name);
//                 FILE * myFile;
//                 system ("touch ~/buff.txt");

//                 if (access (ent->d_name, ent->d_namlen) == -1) {
//                     char * c_path = malloc (sizeof (char) * (ent->d_namlen + strlen (path) + 20));
//                     strcpy (c_path, "cat \'"); 
//                     // printf ("%s\n", c_path);
//                     for (i = 0; path[i]; i++) c_path[i + 5] = path[i]; c_path[i + 5] = '/'; c_path[i + 6] = '\0';
//                     SIZE = strlen (c_path); for (i = 0; i < ent->d_namlen; i++) c_path[i + SIZE] = ent->d_name[i];
//                     printf ("%s\n", c_path);
                    

//                     SIZE = strlen (c_path); for (i = 0; command[i]; i++) c_path[i + SIZE] = command[i];
//                     printf ("%s\n", c_path);
//                     system (c_path);
//                     // system ("")
                    
//                     printf ("%s\n", name);
//                     myFile = fopen (name, "r");
//                 } else {
//                     myFile = fopen (ent->d_name, "r");
//                 }

//                 if (myFile) {
//                 //     line[0] = '\0';
//                 //     fscanf (myFile, "%s", line);
//                 //     printf ("\t%s\n\n", line);
                    
//                     int c = fgetc (myFile);
//                     printf ("+\t%d\n", c);
//                     // line = malloc (sizeof (char) * 1000);

//                     // while (!feof (myFile)) {
//                     //     fscanf (myFile, "%s", line);
//                     //     printf ("%s\n", line);
//                     // }
//                     fclose (myFile);
//                 } else {
//                     printf ("can\'t open file...\n\n");
//                 }
//                 system ("rm ~/buff.txt");
//             }
//         }
//         closedir (dir);
//     } else {
//         perror ("I can\'t open directorie..");
//         return EXIT_FAILURE;
//     }
//     return 0;
// }
