#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#define BMP_HEADER_SIZE 54

void writeStatistics(int fd, const char *filename, int width, int height, struct stat *fileStat);

void writeStatistics(int fd, const char *filename, int width, int height, struct stat *fileStat) {
    char buffer[1024];
    int length;

    length = snprintf(buffer, sizeof(buffer), 
                      "nume fisier: %s\ninaltime: %d\nlungime: %d\ndimensiune: %ld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %scontorul de legaturi: %ld\ndrepturi de acces user: %c%c%c\ndrepturi de acces grup: %c%c%c\ndrepturi de acces altii: %c%c%c\n",
                      filename, height, width, fileStat->st_size, fileStat->st_uid, ctime(&fileStat->st_mtime),
                      fileStat->st_nlink,
                      (fileStat->st_mode & S_IRUSR) ? 'R' : '-',
                      (fileStat->st_mode & S_IWUSR) ? 'W' : '-',
                      (fileStat->st_mode & S_IXUSR) ? 'X' : '-',
                      (fileStat->st_mode & S_IRGRP) ? 'R' : '-',
                      (fileStat->st_mode & S_IWGRP) ? 'W' : '-',
                      (fileStat->st_mode & S_IXGRP) ? 'X' : '-',
                      (fileStat->st_mode & S_IROTH) ? 'R' : '-',
                      (fileStat->st_mode & S_IWOTH) ? 'W' : '-',
                      (fileStat->st_mode & S_IXOTH) ? 'X' : '-');

    write(fd, buffer, length);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fisier_intrare>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    unsigned char header[BMP_HEADER_SIZE];
    if (read(fd, header, BMP_HEADER_SIZE) != BMP_HEADER_SIZE) {
        perror("Error reading BMP header");
        close(fd);
        return EXIT_FAILURE;
    }

    int width = *(int *)&header[18];
    int height = *(int *)&header[22];
    close(fd);

    struct stat fileStat;
    if (stat(argv[1], &fileStat) < 0) {
        perror("Error getting file stats");
        return EXIT_FAILURE;
    }

    int statFd = open("statistica.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (statFd == -1) {
        perror("Error opening statistica.txt");
        return EXIT_FAILURE;
    }

    writeStatistics(statFd, argv[1], width, height, &fileStat);
    close(statFd);

    return EXIT_SUCCESS;
}


