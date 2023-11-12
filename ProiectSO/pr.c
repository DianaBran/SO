#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>

#define BMP_HEADER_SIZE 54 //54 octeti pentru dimensiunea  header-ului

//declararea functiilor
void processBMPFile(const char *filePath, int statFd);
void writeStatistics(const char *filePath, int statFd, const char *info);
//deschiderea fisierului 
void processBMPFile(const char *filePath, int statFd) {
    int file = open(filePath, O_RDONLY);
    if (file < 0) {
        perror("Error opening BMP file");
        return;
    }
 //citirea header-ului
    unsigned char header[BMP_HEADER_SIZE];
    if (read(file, header, BMP_HEADER_SIZE) != BMP_HEADER_SIZE) {
        perror("Error reading BMP header");
        close(file);
        return;
    }

    int width = *(int *)&header[18];
    int height = *(int *)&header[22];

    char info[100];
    snprintf(info, sizeof(info), "BMP file: Width: %d, Height: %d", width, height);

    writeStatistics(filePath, statFd, info);

    close(file);
}

//scriem informatiile formate in statistici
void writeStatistics(const char *filePath, int statFd, const char *info) {
    char buffer[256];
    int len = snprintf(buffer, sizeof(buffer), "File: %s, %s\n", filePath, info);
    if (write(statFd, buffer, len) != len) {
        perror("Error writing to statistica.txt");
    }
}

int main(int argc, char *argv[]) {
    // Verifică numărul argumentelor
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }
   //deschidem directorul 
    DIR *dir = opendir(argv[1]);
    if (dir == NULL) {
        perror("Error opening directory");
        return EXIT_FAILURE;
    }
    // Deschide fișierul pentru statistici
    int statFd = open("statistica.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (statFd == -1) {
        perror("Error opening statistica.txt");
        return EXIT_FAILURE;
    }
    struct dirent *entry;
    //parcurgem directorul
    while ((entry = readdir(dir)) != NULL) {
        // Exclude directorul curent (.) și părintele (..)
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char filePath[PATH_MAX];
        snprintf(filePath, PATH_MAX, "%s/%s", argv[1], entry->d_name);

        // Verifică dacă fișierul este un BMP și îl procesează
        if (strstr(entry->d_name, ".bmp")) {
            processBMPFile(filePath, statFd);
        }
    }

    closedir(dir);
    close(statFd);

    return EXIT_SUCCESS;
}