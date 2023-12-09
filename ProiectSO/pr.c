#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#define BMP_HEADER_SIZE 54 // Dimensiunea header-ului pentru un fișier BMP.

// Declarația funcțiilor.
void processBMPFile(const char *filePath, int statFd);
void writeStatistics(const char *filePath, int statFd, const char *info);
void writeFileDetails(const char *filePath, int statFd);
int isBMP(const unsigned char *header);

// Funcția pentru procesarea fișierelor BMP.
void processBMPFile(const char *filePath, int statFd) 
{
    int file = open(filePath, O_RDONLY);
    if (file < 0) 
    {
        perror("Error opening BMP file");
        return;
    }

    unsigned char header[BMP_HEADER_SIZE];
    if (read(file, header, BMP_HEADER_SIZE) != BMP_HEADER_SIZE) 
    {
        perror("Error reading BMP header");
        close(file);
        return;
    }

    // Verifică dacă fișierul este un BMP valid.
    if (!isBMP(header))
     {
        fprintf(stderr, "File %s is not a valid BMP file\n", filePath);
        close(file);
        return;
    }

    int width = *(int *)&header[18];
    int height = *(int *)&header[22];

    char info[100];
    snprintf(info, sizeof(info), "BMP file: Width: %d, Height: %d", width, height);
    writeStatistics(filePath, statFd, info);
    writeFileDetails(filePath, statFd);
    
    close(file);
}

// Funcția pentru scrierea statisticilor.
void writeStatistics(const char *filePath, int statFd, const char *info)
 {
    char buffer[256];
    int len = snprintf(buffer, sizeof(buffer), "File: %s, %s\n", filePath, info);
    if (write(statFd, buffer, len) != len)
     {
        perror("Error writing to statistica.txt");
    }
}

// Funcția pentru scrierea detaliilor fișierului.
void writeFileDetails(const char *filePath, int statFd) 
{
    struct stat fileStat;
    if (stat(filePath, &fileStat) < 0)
     {
        perror("Error getting file stats");
        return;
    }

    char buffer[1024];
    int length;

    length = snprintf(buffer, sizeof(buffer), "File: %s\nSize: %ld\nUser ID: %d\nNumber of Links: %d\nLast Modified: %s\n\n",
                      filePath, fileStat.st_size, fileStat.st_uid, (int)fileStat.st_nlink, ctime(&fileStat.st_mtime));
    write(statFd, buffer, length);

    length = snprintf(buffer, sizeof(buffer), "Access Rights: User [%c%c%c] Group [%c%c%c] Others [%c%c%c]\n",
                      (fileStat.st_mode & S_IRUSR) ? 'R' : '-',
                      (fileStat.st_mode & S_IWUSR) ? 'W' : '-',
                      (fileStat.st_mode & S_IXUSR) ? 'X' : '-',
                      (fileStat.st_mode & S_IRGRP) ? 'R' : '-',
                      (fileStat.st_mode & S_IWGRP) ? 'W' : '-',
                      (fileStat.st_mode & S_IXGRP) ? 'X' : '-',
                      (fileStat.st_mode & S_IROTH) ? 'R' : '-',
                      (fileStat.st_mode & S_IWOTH) ? 'W' : '-',
                      (fileStat.st_mode & S_IXOTH) ? 'X' : '-');
    write(statFd, buffer, length);
}

// Funcția de verificare dacă fișierul este un BMP valid.
int isBMP(const unsigned char *header) 
{
    return header[0] == 'B' && header[1] == 'M';
}

int main(int argc, char *argv[]) 
{
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    DIR *dir = opendir(argv[1]);
    if (dir == NULL) 
    {
        perror("Error opening directory");
        return EXIT_FAILURE;
    }

    int statFd = open("statistica.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (statFd == -1) {
        perror("Error opening statistica.txt");
        closedir(dir);
        return EXIT_FAILURE;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
     {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char filePath[PATH_MAX];
        snprintf(filePath, PATH_MAX, "%s/%s", argv[1], entry->d_name);

         struct stat path_stat;
        stat(filePath, &path_stat);

        if (S_ISREG(path_stat.st_mode))
         {
            // Este un fișier obișnuit
            if (strstr(entry->d_name, ".bmp"))
             {
                processBMPFile(filePath, statFd);
            } else {
                writeFileDetails(filePath, statFd);
            }
        } else if (S_ISDIR(path_stat.st_mode))
         {
           // Este un director
          char info[1024];

          // Formatarea informațiilor despre director
         int length = snprintf(info, sizeof(info), 
         "Directory: %s\nUser ID: %d\nGroup ID: %d\nSize: %ld bytes\nLast Modified: %s",
         entry->d_name, path_stat.st_uid, path_stat.st_gid, path_stat.st_size, ctime(&path_stat.st_mtime));

         // Scrierea drepturilor de acces
          length += snprintf(info + length, sizeof(info) - length,
          "\nAccess Rights: User [%c%c%c] Group [%c%c%c] Others [%c%c%c]\n",
           (path_stat.st_mode & S_IRUSR) ? 'R' : '-',
           (path_stat.st_mode & S_IWUSR) ? 'W' : '-',
           (path_stat.st_mode & S_IXUSR) ? 'X' : '-',
           (path_stat.st_mode & S_IRGRP) ? 'R' : '-',
           (path_stat.st_mode & S_IWGRP) ? 'W' : '-',
           (path_stat.st_mode & S_IXGRP) ? 'X' : '-',
           (path_stat.st_mode & S_IROTH) ? 'R' : '-',
           (path_stat.st_mode & S_IWOTH) ? 'W' : '-',
           (path_stat.st_mode & S_IXOTH) ? 'X' : '-');

         writeStatistics(filePath, statFd, info);
         }
    }

      closedir(dir);
      close(statFd);

     return EXIT_SUCCESS;
}
