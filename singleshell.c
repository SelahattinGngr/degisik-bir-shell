#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

// inbuf için open max değeri
#define INBUF_SIZE 256

// write_output fonksiyonu tanımlanır
void write_output(char *command)
{
    // shell.log dosyası "a" modunda açılır
    FILE *fp;
    fp = fopen("shell.log", "a");
    // dosya açılamazsa hata mesajı verilip fonksiyondan çıkılır
    if (fp == NULL)
    {
        printf("Dosya açma hatası\n");
        return;
    }
    // zaman bilgisi elde edilir ve tm yapısına atanır
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // log dosyasına tarih, saat ve komut yazılır
    // bu işlevi yazmak için chatgpt den yardım aldım ....
    fprintf(fp, "%d-%02d-%02d %02d:%02d:%02d : %s\n",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, command);
    // dosya kapatılır
    fclose(fp);
}

// main fonksiyonu tanımlanır ve programın argümanları argc ve argv olarak alınır

int main(int argc, char *argv[])
{
    // -Wall -Werror -Wextra flaglarından geçmesi için main parametrelerini void e cast ettim ...
	(void)argc;
	(void)argv;
    // inbuf adında bir karakter dizisi tanımlanır ve değerleri sıfırlanır
    char inbuf[INBUF_SIZE] = {'\0'};
    // nbyte adında bir değişken tanımlanır ve başlangıç değeri atanmaz
    int nbyte; /* input byte count */
    // myshell'in PID'i ekrana yazdırılır
    printf("myshell id: %d\n", getpid());

    // Şimdiki zaman bilgisi alınır ve tm_info struct'ına atanır
    time_t now;
    struct tm *tm_info;
    char timebuf[26];
    time(&now);
    tm_info = localtime(&now);
    // Şimdiki zaman formatlanarak timebuf'a yazılır ve dosyaya yazdırılır
    strftime(timebuf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    write_output(timebuf);

    // Sonsuz bir döngü başlatılır
    while (1)
    {
        // Terminal ekranında $ işareti yazdırılır
        write(STDOUT_FILENO, "$", 2);

        // Kullanıcıdan girdi alınır ve nbyte değişkeninde tutulur
        if ((nbyte = read(STDIN_FILENO, inbuf, INBUF_SIZE - 1)) <= 0)
        {
            // Eğer okunan byte sayısı 0 ya da negatifse hata mesajı yazdırılır ve program sonlandırılır
            perror("input <=0 byte");
            exit(1);
        }
        else
        {
            // Okunan byte sayısı 0 dan büyükse, inbuf stringinin sonuna null karakteri eklenir
            inbuf[nbyte - 1] = '\0';
            write_output(inbuf);
        }

        if (strncmp(inbuf, "exit", 4) == 0)
        {
            // Geçerli zaman bilgisini almak için time() fonksiyonu kullanılır
            time(&now);
            // Geçerli zaman bilgisini yerel saat bilgilerine dönüştürmek için localtime() fonksiyonu kullanılır
            tm_info = localtime(&now);
            // strftime() fonksiyonu, zaman bilgisini belirli bir formata göre biçimlendirir ve timebuf dizisine yazdırır
            strftime(timebuf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
            // write_output() fonksiyonu, zaman bilgisini kullanarak log dosyasına belirli bir formatta tarih ve saat bilgisini yazar
            write_output(timebuf);
            // Programın normal bir şekilde sonlandırılması için exit() fonksiyonu kullanılır
            exit(0);
        }

        pid_t child_pid = fork();
        printf("process id: %d - parent id: %d\n", getpid(), getppid());

        if (child_pid == 0) //çocuk işlemdeyiz
        {
            // Çocuk işlem mesajını yazdır.
            printf("Child process!!!\n");
            // Çocuk işlemin çalıştıracağı komut ve argümanları içeren bir dizi tanımlanır.
            char *args[] = {"/bin/sh", "-c", inbuf, NULL};
            // execvp fonksiyonu, işlemi farklı bir programla değiştirerek çalıştırır.
            // args[0] programın tam yolunu, args ise programın argümanlarını içerir.
            int r = execvp(args[0], args);

            // Eğer komutun çalıştırılması başarısız olursa, alternatif bir yöntem kullanılabilir.
            if (r == -1)
            {
                // Komut dosyası veya betiği doğrudan çalıştırmaya çalış.
                char command[255] = {'\0'};
                strcat(command, "./");
                strcat(command, inbuf);

                // Yeni bir argüman dizisi tanımla. 
                char *args[] = {command, NULL};
                // Bu sefer farklı bir programın çalıştırılmasını deneyin.
                r = execvp(args[0], args);
                // Eğer bu da başarısız olursa, bir hata mesajı yazdırılır.
                if (r == -1)
                {
                    char error_msg[284] = {'\0'};
                    sprintf(error_msg, "Error executing %s command", inbuf);
                    perror(error_msg);
                    exit(1);
                }
            }
        }
        else if (child_pid > 0)
        {
            // Ebeveyn işlemi : işlemin bitmesini bekler...
            wait(NULL);
        }
        else
        {
            // hata durumunda process i kapatır...
            perror("fork() hata");
            exit(1);
        }
    }
    return (0);
}
