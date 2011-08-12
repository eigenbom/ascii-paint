#include <cstdio>
#include "asc_file.hpp"
#include "txt2asc.h"

int txt2asc(char* filename) {
    FILE *fp;

    fp = fopen(filename, "r");

    if(fp == NULL)
        return -1;

    int width = 80;
    int height = 50;
    int x = 0;
    int y = 0;

    printf("\nwidth = %i\n", width);
    printf("height = %i\n", height);

    AscFile asc;
    asc.setSize(width, height);

    for(int x = 0; x < width; x++) {
        for(int y = 0; y < height; y++) {
            asc.setChar(x, y, ' ');
            asc.setFore(x, y, 255, 255, 255);
            asc.setBack(x, y, 0, 0, 0);
        }
    }

    char c = ' ';
    while(c != EOF) {
        c = getc(fp);

        x = 0;
        while(c != '\n' && c != EOF) {
            asc.setChar(x, y, c);
            asc.setFore(x, y, 255, 255, 255);
            asc.setBack(x, y, 0, 0, 0);
            x++;
            c = getc(fp);
        }
    y++;
    }

    fclose(fp);
    asc.save("output.asc");

    return 0;

}

int main() {
    printf("Enter filename: ");
    char filename[1000];
    gets(filename);
    printf("Converting %s\n", filename);
    if(txt2asc(filename) == -1)
        printf("Error!\n");
    return 0;
}
