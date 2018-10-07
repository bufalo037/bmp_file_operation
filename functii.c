#include <stdio.h>
#include "functii.h"
#include <stdlib.h>
#include <string.h>

static void citire_bmp_fileheader(fileh *file,FILE **f)
{
	fread(&(file->fileMarker1),sizeof(unsigned char),1,*f);
	fread(&(file->fileMarker2),sizeof(unsigned char),1,*f);
	fread(&(file->bfSize),sizeof(unsigned int),1,*f);
	fread(&(file->unused1),sizeof(unsigned short),1,*f);
	fread(&(file->unused2),sizeof(unsigned short),1,*f);  
	fread(&(file->imageDataOffset),sizeof(unsigned int),1,*f);
}
static void citire_bmp_infoheader(infoh* const info,FILE **f)
{
	fread(&(info->biSize),sizeof(unsigned int),1,*f);
	fread(&(info->width),sizeof(signed int),1,*f);
	fread(&(info->height),sizeof(signed int),1,*f);
	fread(&(info->planes),sizeof(unsigned short),1,*f);
	fread(&(info->bitPix),sizeof(unsigned short),1,*f);
	fread(&(info->biCompression),sizeof(unsigned int),1,*f);
	fread(&(info->biSizeImage),sizeof(unsigned int),1,*f);
	fread(&(info->biXPelsPerMeter),sizeof(int),1,*f);
	fread(&(info->biYPelsPerMeter),sizeof(int),1,*f);
	fread(&(info->biClrUsed),sizeof(unsigned int),1,*f);
	fread(&(info->biClrImportant),sizeof(unsigned int),1,*f);
}
void citire_bmp_matrix(fileh* file,infoh *info,pixel** poza,
	unsigned char pad,FILE **f)
{
	fseek(*f,file->imageDataOffset,0);

	int i,j;

	for(i = 0;i < info->height;i++)
	{
		for(j = 0;j < info->width;j++)
		{
			fread(&(poza[i][j].b),sizeof(unsigned char),1,*f);
			fread(&(poza[i][j].g),sizeof(unsigned char),1,*f);
			fread(&(poza[i][j].r),sizeof(unsigned char),1,*f);
		}
		fseek(*f,pad,1);
	}
}
void citire(fileh *file, infoh *info, FILE **f)
{
	citire_bmp_fileheader(file, f);
	citire_bmp_infoheader(info, f);
}
void get_padding(infoh* info, unsigned char *padding)
{
	if((info->width % 4) == 0)
		*padding = 0;
	else
		*padding = 4 - ((info->width * 3) % 4);
}
void free_matrix(pixel*** poza, int height)
{
	int i;
	for (i = 0;i < height;i++)
		free(*(( *poza )+i));
	free(*poza);
}
void rotire180(pixel** poza, infoh* info)
{
	int i,j;
	pixel aux;
	for(i = 0;i <= ( info->height - 1 ) / 2 ;i++)
	{
		for(j = 0;j < info->width ;j++)
		{
			aux.b = poza[i][j].b;
			aux.g = poza[i][j].g;
			aux.r = poza[i][j].r;
			poza[i][j] = poza[ info->height - i - 1][j];
			poza[info->height - i - 1][j] = aux;
		}
	}
}
static void scriere_bmp_fileheader(fileh* file,FILE **g)
{
	fwrite(&(file->fileMarker1),sizeof(unsigned char),1,*g);
	fwrite(&(file->fileMarker2),sizeof(unsigned char),1,*g);
	fwrite(&(file->bfSize),sizeof(unsigned int),1,*g);
	fwrite(&(file->unused1),sizeof(unsigned short),1,*g);
	fwrite(&(file->unused2),sizeof(unsigned short),1,*g);  
	fwrite(&(file->imageDataOffset),sizeof(unsigned int),1,*g);
}
static void scriere_bmp_infoheader(infoh* info,FILE **g)
{
	fwrite(&(info->biSize),sizeof(unsigned int),1,*g);
	fwrite(&(info->width),sizeof(signed int),1,*g);
	fwrite(&(info->height),sizeof(signed int),1,*g);
	fwrite(&(info->planes),sizeof(unsigned short),1,*g);
	fwrite(&(info->bitPix),sizeof(unsigned short),1,*g);
	fwrite(&(info->biCompression),sizeof(unsigned int),1,*g);
	fwrite(&(info->biSizeImage),sizeof(unsigned int),1,*g);
	fwrite(&(info->biXPelsPerMeter),sizeof(int),1,*g);
	fwrite(&(info->biYPelsPerMeter),sizeof(int),1,*g);
	fwrite(&(info->biClrUsed),sizeof(unsigned int),1,*g);
	fwrite(&(info->biClrImportant),sizeof(unsigned int),1,*g);
}
static void bmp_format(infoh* info,pixel **poza,unsigned char pad,FILE **g)
{
	int i,j;
	char x = 0;
	for(i = 0;i < info->height;i++)
	{
		for(j = 0;j < info->width;j++)
		{
			fwrite(&(poza[i][j].b),sizeof(unsigned char),1,*g);
			fwrite(&(poza[i][j].g),sizeof(unsigned char),1,*g);
			fwrite(&(poza[i][j].r),sizeof(unsigned char),1,*g);
		}
		fwrite(&x,sizeof(char),pad,*g);

	}
}

static int pix_identici(pixel pix1,pixel pix2)
{

	if(pix1.r == pix2.r && pix1.b == pix2.b && pix1.g == pix2.g)
		return 1;	
	return 0;
}
static void bin_format(infoh* info, pixel** poza, FILE **g)
{
	short int i,j;
	short int ii,jj;
	for(i = 0;i < info->height;i++)
	{
		for(j = 0;j < info->width;j++)
		{
			ii=i+1;
			jj=j+1;
			if(i == 0 || i == info->height -1 || j == 0 || j == info->width -1)
			{
				fwrite(&ii,sizeof(short int),1,*g);
				fwrite(&jj,sizeof(short int),1,*g);
				fwrite(&(poza[i][j].r),sizeof(unsigned char),1,*g);
				fwrite(&(poza[i][j].g),sizeof(unsigned char),1,*g);
				fwrite(&(poza[i][j].b),sizeof(unsigned char),1,*g);
			}
			else
			{
				if(pix_identici(poza[i][j],poza[i+1][j]) == 0 ||
				 pix_identici(poza[i][j],poza[i][j+1]) == 0 ||
				 pix_identici(poza[i][j],poza[i-1][j]) == 0 ||
				 pix_identici(poza[i][j],poza[i][j-1]) == 0 )
				{
					fwrite(&ii,sizeof(short int),1,*g);
					fwrite(&jj,sizeof(short int),1,*g);
					fwrite(&(poza[i][j].r),sizeof(unsigned char),1,*g);
					fwrite(&(poza[i][j].g),sizeof(unsigned char),1,*g);
					fwrite(&(poza[i][j].b),sizeof(unsigned char),1,*g);
				}
			}
		}
	}
}
void scriere_fisier(char* inp,char* ext,unsigned char pad,fileh* file
	,infoh* info,pixel **poza)
{

	FILE *g;
	char copy[MAX];
	
	strcpy(copy,inp);
	strcat(copy,ext);
	if( ( g = fopen(copy,"wb")) == NULL)
	{
		fprintf(stderr,"Eroare deschidere fisier %s",copy);
		exit(EXIT_FAILURE);
	}

	scriere_bmp_fileheader(file,&g);
	scriere_bmp_infoheader(info,&g);
	//printf("%ld",ftell(g));

	char *x = calloc(sizeof(char),file->imageDataOffset - ftell(g));
	if(x == NULL)
	{
		fprintf(stderr,"Eroare alocare octeti nuli");
		exit(EXIT_FAILURE);
	}	
	fwrite(x,sizeof(char),file->imageDataOffset - ftell(g),g);
	free(x);

	if(strcmp(ext,".bmp") == 0)
		bmp_format(info,poza,pad,&g);

	if(strcmp(ext,".bin") == 0)
		bin_format(info,poza,&g);

	fclose(g);

}
void black_white(infoh* info,pixel** poza,pixel **pic)
{
	int i,j;
	short int x;
	for(i = 0;i < info->height;i++)
	{
		for(j = 0;j < info->width;j++)
		{
			x = ( poza[i][j].b + poza[i][j].r + poza[i][j].g )/3;
			pic[i][j].b = x;
			pic[i][j].r = x;
			pic[i][j].g = x;

		}
	}
}
void free_matrix_c(char*** poza, const int height)
{
	int i;
	for (i = 0;i < height;i++)
		free(*(( *poza )+i));
	free(*poza);
}
void alocare_mem_poza(pixel ***poza,infoh* info)
{
	*poza = malloc( sizeof(pixel*) * info->height );
	int i;
	if(*poza == NULL)
	{
		fprintf(stderr,"Eroare la alocarea matricei de pixeli\n");
		exit(EXIT_FAILURE);
	}
	for(i = 0;i < info->height;i++)
	{
		(*poza)[i] = malloc( sizeof(pixel) * info->width );
		if( (*poza)[i] == NULL)
		{
			free_matrix(poza,i);
			fprintf(stderr,"Eroare la alocarea liniei %d a matricei de pixeli\n",i);
		}
	}
}

void alocare_matrice_bin(char ***poza,infoh* info)
{
	*poza = malloc( sizeof(char*) * info->height );
	int i;
	if(*poza == NULL)
	{
		fprintf(stderr,"Eroare la alocarea matricei binare\n");
		exit(EXIT_FAILURE);
	}
	for(i = 0;i < info->height;i++)
	{
		(*poza)[i] = calloc( sizeof(char) , info->width );
		if( (*poza)[i] == NULL)
		{
			free_matrix_c(poza,i);
			fprintf(stderr,"Eroare la alocarea liniei %d a matricei binare\n",i);
		}
	}
}
void alocare_filtre(char*** F1, char*** F2,char*** F3)
{
	int i;

	*F1 = malloc(sizeof(char*) * 3);
	if( *F1 == NULL)
	{
		fprintf(stderr,"Eroare alocare F1");
	}
	*F2 = malloc(sizeof(char*) * 3);
	if( *F2 == NULL)
	{
		fprintf(stderr,"Eroare alocare F2");
	}
	*F3 = malloc(sizeof(char*) * 3);
	if( *F3 == NULL)
	{
		fprintf(stderr,"Eroare alocare F3");
	}

	for(i = 0;i < 3;i++)
	{
		(*F1)[i] = malloc(sizeof(char) * 3);
		if((*F1)[i] == NULL)
		{
			fprintf(stderr,"Eroare alocare F1[%d]",i);
			exit(EXIT_FAILURE);
		}

		(*F2)[i] = malloc(sizeof(char) * 3);
		if((*F2)[i] == NULL)
		{
			fprintf(stderr,"Eroare alocare F2[%d]",i);
			exit(EXIT_FAILURE);
		}

		(*F3)[i] = malloc(sizeof(char) * 3);
		if((*F3)[i] == NULL)
		{
			fprintf(stderr,"Eroare alocare F3[%d]",i);
			exit(EXIT_FAILURE);
		}
	}
}
void def_filtre(char** F1,char** F2,char** F3)
{
	F1[0][0]=-1;
	F1[0][1]=-1;
	F1[0][2]=-1;
	F1[1][0]=-1;
	F1[1][1]=8;
	F1[1][2]=-1;
	F1[2][0]=-1;
	F1[2][1]=-1;
	F1[2][2]=-1;

	F2[0][0]=0;
	F2[0][1]=1;
	F2[0][2]=0;
	F2[1][0]=1;
	F2[1][1]=-4;
	F2[1][2]=1;
	F2[2][0]=0;
	F2[2][1]=1;
	F2[2][2]=0;

	F3[0][0]=1;
	F3[0][1]=0;
	F3[0][2]=-1;
	F3[1][0]=0;
	F3[1][1]=0;
	F3[1][2]=0;
	F3[2][0]=-1;
	F3[2][1]=0;
	F3[2][2]=+1;
}

void aplicare_filtre(pixel** poza,pixel **pic, infoh* info, char** f1)
{
	int i,j;
	//int k,l;
	//f1 filtru oarecare 3x3
	int summr, summg, summb;
	for(i = 0;i < info->height; i++)
	{
		
		for(j = 0;j < info->width; j++)
		{
			summr = 0;
			summg = 0;
			summb = 0;
		
			if(j - 1 >= 0)
			{
				summr = summr + f1[1][0]*poza[i][j-1].r;
				summg = summg + f1[1][0]*poza[i][j-1].g;
				summb = summb + f1[1][0]*poza[i][j-1].b;

				if(i - 1 >= 0)
				{
					summr = summr + f1[0][0]*poza[i-1][j-1].r;
					summg = summg + f1[0][0]*poza[i-1][j-1].g;
					summb = summb + f1[0][0]*poza[i-1][j-1].b;
				}
				if(i + 1 < info->height)
				{
					summr = summr + f1[2][0]*poza[i+1][j-1].r;
					summg = summg + f1[2][0]*poza[i+1][j-1].g;
					summb = summb + f1[2][0]*poza[i+1][j-1].b;
				}
			}
			if(i - 1 >= 0)
			{
				summr = summr + f1[0][1]*poza[i-1][j].r;
				summg = summg + f1[0][1]*poza[i-1][j].g;
				summb = summb + f1[0][1]*poza[i-1][j].b;
			}

				summr = summr + f1[1][1]*poza[i][j].r;
				summg = summg + f1[1][1]*poza[i][j].g;
				summb = summb + f1[1][1]*poza[i][j].b;

			if(i + 1 < info->height)
			{
				summr = summr + f1[2][1]*poza[i+1][j].r;
				summg = summg + f1[2][1]*poza[i+1][j].g;
				summb = summb + f1[2][1]*poza[i+1][j].b;
			}
			if(j + 1 < info->width)
			{
				summr = summr + f1[1][2]*poza[i][j+1].r;
				summg = summg + f1[1][2]*poza[i][j+1].g;
				summb = summb + f1[1][2]*poza[i][j+1].b;

				if(i - 1 >= 0)
				{
					summr = summr + f1[0][2]*poza[i-1][j+1].r;
					summg = summg + f1[0][2]*poza[i-1][j+1].g;
					summb = summb + f1[0][2]*poza[i-1][j+1].b;
				}
				if(i + 1 < info->height)
				{
					summr = summr + f1[2][2]*poza[i+1][j+1].r;
					summg = summg + f1[2][2]*poza[i+1][j+1].g;
					summb = summb + f1[2][2]*poza[i+1][j+1].b;
				}
			}
			if(summr < 0)
				summr = 0;
			if(summr > 255)
				summr = 255;

			if(summg < 0)
				summg = 0;
			if(summg > 255)
				summg = 255;

			if(summb < 0)
				summb = 0;
			if(summb > 255)
				summb = 255;

			pic[i][j].r = summr;
			pic[i][j].b = summb;
			pic[i][j].g = summg;
		}
	}
}

void free_filtre(char ***f)
{
	int i;
	for(i = 0;i <= 2;i++)
		free((*f)[i]);
	free(*f);
}
void umplere_zone_pixeli(pixel** poza,pixel** pic,char **bin,infoh* info,
	short int threshold)
{
	// nu am facut recursiv ca pt unele cazuri ar fi depasit stiva de 1MB
	// cele 2 malloc-uri ocupa aproximativ 24 MB de heap
	int i,j;
	unsigned char r,g,b;
	short int re,gr,bl;
	short int *jj,*ii;
	int poz;
	int poz_max;
	jj = malloc(sizeof(short int) * 2500 * 2500);
	if(jj == NULL)
	{
		fprintf(stderr,"Eroare alocare jj");
		exit(EXIT_FAILURE);
	}

	ii = malloc(sizeof(short int) * 2500 * 2500);
	if(ii == NULL)
	{
		fprintf(stderr,"Eroare alocare ii");
		exit(EXIT_FAILURE);
	}

	for(i = 0;i < info->height;i++)
	{
		for(j = 0;j < info->width;j++)
		{
			if(bin[i][j] == 0)
			{
				poz = 0;
				poz_max = 0;
				bin[i][j] = 1;
				r = poza[i][j].r;
				g = poza[i][j].g;
				b = poza[i][j].b;
				ii[poz]=i;
				jj[poz]=j;

				pic[ ii[poz] ][ jj[poz] ].r = r;
				pic[ ii[poz] ][ jj[poz] ].g = g;
				pic[ ii[poz] ][ jj[poz] ].b = b;

				while(poz <= poz_max)
				{
					// i-1 j
					if( ii[poz] - 1 >= 0 && bin[ ii[poz] - 1 ][ jj[poz] ] == 0)
					{
						if(poza[ ii[poz] - 1 ][ jj[poz] ].r - r >= 0)
						{
							re = poza[ ii[poz] - 1 ][ jj[poz] ].r - r;
						}

						else

						{
							re = r - poza[ ii[poz] - 1 ][ jj[poz] ].r;
						}

						if(poza[ ii[poz] - 1 ][ jj[poz] ].g - g >= 0)
						{
							gr = poza[ ii[poz] - 1 ][ jj[poz] ].g - g;
						}

						else

						{
							gr = g - poza[ ii[poz] - 1 ][ jj[poz] ].g;
						}

						if(poza[ ii[poz] - 1 ][ jj[poz] ].b - b >= 0)
						{
							bl = poza[ ii[poz] - 1 ][ jj[poz] ].b - b;
						}

						else
						{
							bl = b - poza[ ii[poz] - 1 ][ jj[poz] ].b;
						}

						if( (re + gr + bl) <= threshold )
						{
							poz_max++;
							ii[poz_max] = ii[poz] - 1;
							jj[poz_max] = jj[poz];
							bin[ ii[poz_max] ][ jj[poz_max] ] = 1;
							pic[ ii[poz_max] ][ jj[poz_max] ].r = r;
							pic[ ii[poz_max] ][ jj[poz_max] ].g = g;
							pic[ ii[poz_max] ][ jj[poz_max] ].b = b;
						}
					}
					// i+1 j
					if( ii[poz] + 1 < info->height && 
						bin[ ii[poz] + 1 ][ jj[poz] ] == 0)
					{
						if(poza[ ii[poz] + 1 ][ jj[poz] ].r - r >= 0)
						{
							re = poza[ ii[poz] + 1 ][ jj[poz] ].r - r;
						}

						else

						{
							re = r - poza[ ii[poz] + 1 ][ jj[poz] ].r;
						}

						if(poza[ ii[poz] + 1 ][ jj[poz] ].g - g >= 0)
						{
							gr = poza[ ii[poz]  + 1 ][ jj[poz] ].g - g;
						}

						else

						{
							gr = g - poza[ ii[poz] + 1 ][ jj[poz] ].g;
						}

						if(poza[ ii[poz] + 1 ][ jj[poz] ].b - b >= 0)
						{
							bl = poza[ ii[poz] + 1 ][ jj[poz] ].b - b;
						}

						else

						{
							bl = b - poza[ ii[poz] + 1 ][ jj[poz] ].b;
						}

						if( (re + gr + bl) <= threshold )
						{
							poz_max++;
							ii[poz_max] = ii[poz] + 1;
							jj[poz_max] = jj[poz];
							bin[ ii[poz_max] ][ jj[poz_max] ] = 1;
							pic[ ii[poz_max] ][ jj[poz_max] ].r = r;
							pic[ ii[poz_max] ][ jj[poz_max] ].g = g;
							pic[ ii[poz_max] ][ jj[poz_max] ].b = b;
						}
					}
					// i j-1
					if( jj[poz] - 1 >= 0 && bin[ ii[poz] ][ jj[poz] - 1 ] == 0)
					{
						if(poza[ ii[poz] ][ jj[poz] - 1 ].r - r >= 0)
						{
							re = poza[ ii[poz] ][ jj[poz] - 1 ].r - r;
						}

						else

						{
							re = r - poza[ ii[poz] ][ jj[poz] - 1 ].r;
						}

						if(poza[ ii[poz] ][ jj[poz] - 1 ].g - g >= 0)
						{
							gr = poza[ ii[poz] ][ jj[poz] - 1 ].g - g;
						}

						else

						{
							gr = g - poza[ ii[poz] ][ jj[poz] - 1 ].g;
						}

						if(poza[ ii[poz] ][ jj[poz] - 1 ].b - b >= 0)
						{
							bl = poza[ ii[poz] ][ jj[poz] - 1 ].b - b;
						}

						else
						{
							bl = b - poza[ ii[poz] ][ jj[poz] - 1 ].b;
						}

						if( (re + gr + bl) <= threshold )
						{
							poz_max++;
							ii[poz_max] = ii[poz];
							jj[poz_max] = jj[poz] - 1;
							bin[ ii[poz_max] ][ jj[poz_max] ] = 1;
							pic[ ii[poz_max] ][ jj[poz_max] ].r = r;
							pic[ ii[poz_max] ][ jj[poz_max] ].g = g;
							pic[ ii[poz_max] ][ jj[poz_max] ].b = b;
						}
					}
					// i j+1
					if( jj[poz] + 1 < info->width &&
						 bin[ ii[poz] ][ jj[poz] + 1 ] == 0)
					{
						if(poza[ ii[poz] ][ jj[poz] + 1 ].r - r >= 0)
						{
							re = poza[ ii[poz] ][ jj[poz] + 1 ].r - r;
						}

						else

						{
							re = r - poza[ ii[poz] ][ jj[poz] + 1 ].r;
						}

						if(poza[ ii[poz] ][ jj[poz] + 1 ].g - g >= 0)
						{
							gr = poza[ ii[poz] ][ jj[poz] + 1 ].g - g;
						}

						else

						{
							gr = g - poza[ ii[poz] ][ jj[poz] + 1 ].g;
						}

						if(poza[ ii[poz] ][ jj[poz] + 1 ].b - b >= 0)
						{
							bl = poza[ ii[poz] ][ jj[poz] + 1 ].b - b;
						}

						else

						{
							bl = b - poza[ ii[poz] ][ jj[poz] + 1 ].b;
						}

						if( (re + gr + bl) <= threshold )
						{
							poz_max++;
							ii[poz_max] = ii[poz];
							jj[poz_max] = jj[poz] + 1;
							bin[ ii[poz_max] ][ jj[poz_max] ] = 1;
							pic[ ii[poz_max] ][ jj[poz_max] ].r = r;
							pic[ ii[poz_max] ][ jj[poz_max] ].g = g;
							pic[ ii[poz_max] ][ jj[poz_max] ].b = b;
						}
					}
					poz++;
				}
			}
		}
	}
	free(ii);
	free(jj);
}

void initializare_binar_and_pic(pixel** pic,char** binar,fileh* file,FILE **f)
{
	short int i,j;
	unsigned char r,g,b;
	fseek(*f, file->imageDataOffset, 0);

	while(feof(*f) == 0)
	{
		fread(&i,sizeof(short int),1,*f);
		fread(&j,sizeof(short int),1,*f);
		fread(&r,sizeof(unsigned char),1,*f);
		fread(&g,sizeof(unsigned char),1,*f);
		fread(&b,sizeof(unsigned char),1,*f);

		pic[i-1][j-1].r = r;
		pic[i-1][j-1].g = g;
		pic[i-1][j-1].b = b;
		binar[i-1][j-1] = 1;
	}
}
void color_matrix(pixel** pic,char** binar,infoh* info)
{
	int i,j;
	for(i = 1;i < info->height - 1;i++)
	{
		for(j = 1;j < info->width - 1;j++)
		{
			if(binar[i][j] == 0 )
			{
				if(binar[i-1][j] == 1)
				{
					binar[i][j] = 1;
					pic[i][j] = pic[i-1][j];
				}

			}
			
			if(binar[i][j] == 0 )
			{
				if(binar[i+1][j] == 1)
				{
					binar[i][j] = 1;
					pic[i][j] = pic[i+1][j];
				}
				
			}

			if(binar[i][j] == 0 )
			{
				if(binar[i][j+1] == 1)
				{
					binar[i][j] = 1;
					pic[i][j] = pic[i][j+1];
				}
				
			}

			if(binar[i][j] == 0 )
			{
				if(binar[i][j-1] == 1)
				{
					binar[i][j] = 1;
					pic[i][j] = pic[i][j-1];
				}

			}
		}
	}
}