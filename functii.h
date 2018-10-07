#include "bmp_header.h"
#ifndef MAX
#define MAX 50
#endif
typedef struct bmp_fileheader fileh;
typedef struct bmp_infoheader infoh;

void citire(fileh *file,infoh *info,FILE **f);

typedef struct
{
	unsigned char r,g,b;
} pixel;

void free_matrix(pixel*** poza, int height);
void get_padding(infoh* info,unsigned char *padding);
void citire_bmp_matrix(fileh* file,infoh *info,pixel** poza
	,unsigned char pad,FILE **f);
void rotire180(pixel** poza, infoh* info);
void scriere_fisier(char* inp,char* ext,unsigned char pad,fileh* file
	,infoh* info,pixel **poza);
void alocare_mem_poza(pixel ***poza,infoh* info);
void black_white(infoh* info,pixel** poza,pixel **pic);
void alocare_filtre(char*** F1, char*** F2,char*** F3);
void def_filtre(char** F1,char** F2,char** F3);
void aplicare_filtre(pixel** poza,pixel **pic, infoh* info, char** f1);
void free_filtre(char ***f);
void alocare_matrice_bin(char ***poza,infoh* info);
void umplere_zone_pixeli(pixel** poza,pixel** pic,char **bin,infoh* info,
	short int threshold);
void color_matrix(pixel** pic,char** binar,infoh* info);
void initializare_binar_and_pic(pixel** pic,char** binar,fileh* file,FILE **f);
void free_matrix_c(char*** poza, const int height);