#include <stdio.h>
#include "functii.h"
#include <stdlib.h>
#include <string.h>
int main()
{

	//citire--------------------------------------------------------
	FILE *input ,*f;
	char INPUT[11]="input.txt";
	//int i;
	if(( input = fopen(INPUT,"r")) == NULL)
	{
		fprintf(stderr,"Eroare deschidere input file\n");
		return -1;
	}
	char inp[MAX]; //input bmp file
	fscanf(input,"%s",inp);   

	if( (f = fopen(inp,"rb")) == NULL)
	{
		fprintf(stderr,"Eroare deschidere bmp file\n");
		return -1;
	}

	fileh file; //typedef
	infoh info; //typedef

	citire(&file,&info,&f); //functie in functii.c



	unsigned char padding;
	get_padding(&info, &padding);
	


	pixel **poza; // definit in functii.h

	alocare_mem_poza(&poza,&info);


	citire_bmp_matrix(&file,&info,poza,padding,&f); // se stocheaza poza pe heap

	fclose(f); 

	//char copie[MAX];

	inp[ strlen(inp) - 4 ]='\0';
	/*for(i = strlen(inp) - 1;i >= 0;i--)
	{
		if(inp[i] == '/')
		{
			strcpy(copie,inp + i + 1);
			strcpy(inp,copie);
			break;
		}
	}
	*/
	//task1---------------------------------------------------------

	pixel **pic;
	char copyinp[MAX];

	strcpy(copyinp,inp);
	strcat(copyinp,"_black_white");

	alocare_mem_poza(&pic,&info);

	black_white(&info, poza, pic);

	scriere_fisier(copyinp, ".bmp", padding, &file, &info, pic);

	//task2 --------------------------------------------------------
	
	char **F1,**F2,**F3;
	
	pixel** pic2;

	alocare_mem_poza(&pic2,&info);

	alocare_filtre(&F1 ,&F2, &F3);
	def_filtre(F1, F2, F3);

	rotire180(pic,&info);

	aplicare_filtre(pic, pic2, &info, F1);

	strcpy(copyinp,inp);
	strcat(copyinp,"_f1");
	rotire180(pic2,&info);
	scriere_fisier(copyinp, ".bmp", padding, &file, &info, pic2);

	aplicare_filtre(pic, pic2, &info, F2);
	strcpy(copyinp,inp);
	strcat(copyinp,"_f2");
	rotire180(pic2,&info);
	scriere_fisier(copyinp, ".bmp", padding, &file, &info, pic2);

	aplicare_filtre(pic, pic2, &info, F3);
	strcpy(copyinp,inp);
	strcat(copyinp,"_f3");
	rotire180(pic2,&info);
	scriere_fisier(copyinp, ".bmp", padding, &file, &info, pic2);

	rotire180(poza,&info); // inversul fisierului

	free_filtre(&F1);
	free_filtre(&F2);
	free_filtre(&F3);
	free_matrix(&pic2,info.height);

	//task3---------------------------------------------------------

	char** binar;

	alocare_matrice_bin(&binar,&info);

	short int threshold;
	fscanf(input,"%hd",&threshold);

	umplere_zone_pixeli(poza,pic,binar,&info,threshold);
	scriere_fisier("compressed",".bin",padding,&file,&info,pic);

	free_matrix_c(&binar,info.height);
	free_matrix(&pic,info.height);

	free_matrix(&poza,info.height);

	//task4---------------------------------------------------------

	alocare_matrice_bin(&binar,&info);

	char fbin[MAX];
	fscanf(input,"%s",fbin);
	fclose(input);

	FILE *BINARY;

	if( ( BINARY = fopen(fbin,"rb") ) == NULL)
	{
		fprintf(stderr,"eroare deschidere %s",fbin);
		exit(EXIT_FAILURE);
	}

	infoh info2;
	fileh file2;

	citire(&file2,&info2,&BINARY);
	alocare_mem_poza(&poza,&info2);
	initializare_binar_and_pic(poza,binar,&file2,&BINARY);
	color_matrix(poza,binar,&info2);
	get_padding(&info2, &padding);


	rotire180(poza,&info2);

	scriere_fisier("decompressed",".bmp",padding,&file2,&info2,poza);

	free_matrix(&poza,info2.height);
	free_matrix_c(&binar,info2.height);
	fclose(BINARY);

	return 0;
}