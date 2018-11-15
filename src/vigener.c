/*
 * Vigener crack application
 * Copyright (C) 2013 Alexander Pyshchev aka Yftul
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "vigener.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

FILE *f;
char* fname;
char* key;
int   estimatedkeylengh;
long  filesize, i, j, k, textsize;

// Frequency distribution of symbols in English text
double eng_freq[] = { 8.167, 1.492, 2.782, 4.253, 12.702, 2.228, 2.015, 6.094, 6.996, 0.153, 0.772, 4.025, 2.406,
                     6.749, 7.507, 1.929, 0.095, 5.987, 6.327, 9.056, 2.758, 0.978, 2.360, 0.150, 1.974, 0.074 };
double eng_maxval = 12.702; // Maximum value from previous table (for normalizing)
double eng_kappa = 0.0667; // Index of coincidence for English text

// th 1.52       en 0.55       ng 0.18
// he 1.28       ed 0.53       of 0.16
// in 0.94       to 0.52       al 0.09
// er 0.94       it 0.50       de 0.09
// an 0.82       ou 0.50       se 0.08
// re 0.68       ea 0.47       le 0.08
// nd 0.63       hi 0.46       sa 0.06
// at 0.59       is 0.46       si 0.05
// on 0.57       or 0.43       ar 0.04
// nt 0.56       ti 0.34       ve 0.04
// ha 0.56       as 0.33       ra 0.04
// es 0.56       te 0.27       ld 0.02
// st 0.55       et 0.19       ur 0.02

double eng_bigram_freq[26][26] =// A     B     C     D     E     F     G     H     I     J     K     L     M     N     O     P     Q     R     S     T     U     V     W     X     Y     Z
			/* A */  {{0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.09, 0.00, 0.82, 0.00, 0.00, 0.00, 0.04, 0.33, 0.59, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* B */	  {0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* C */	  {0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* D */	  {0.00, 0.00, 0.00, 0.00, 0.09, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* E */	  {0.47, 0.00, 0.00, 0.53, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.55, 0.00, 0.00, 0.00, 0.94, 0.56, 0.19, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* F */	  {0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* G */	  {0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* H */	  {0.56, 0.00, 0.00, 0.00, 1.28, 0.00, 0.00, 0.00, 0.46, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* I */	  {0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.94, 0.00, 0.00, 0.00, 0.00, 0.46, 0.50, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* J */	  {0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* K */	  {0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* L */	  {0.00, 0.00, 0.00, 0.02, 0.08, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* M */	  {0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* N */	  {0.00, 0.00, 0.00, 0.63, 0.00, 0.00, 0.18, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.56, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* O */	  {0.00, 0.00, 0.00, 0.00, 0.00, 0.16, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.57, 0.00, 0.00, 0.00, 0.43, 0.00, 0.00, 0.50, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* P */	  {0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* Q */	  {0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* R */	  {0.04, 0.00, 0.00, 0.00, 0.68, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* S */	  {0.06, 0.00, 0.00, 0.00, 0.08, 0.00, 0.00, 0.00, 0.05, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.55, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* T */	  {0.00, 0.00, 0.00, 0.00, 0.27, 0.00, 0.00, 1.52, 0.34, 0.00, 0.00, 0.00, 0.00, 0.00, 0.52, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* U */	  {0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.02, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* V */	  {0.00, 0.00, 0.00, 0.00, 0.04, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* W */	  {0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* X */	  {0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* Y */	  {0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},
			/* Z */	  {0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00}};

//double delta = 0.01; // Cut-off value 
double symbolfrequences[MAXKEYLENGTH][27]; // Frequences of symbols + maximum value for each key position
double floatacc, funcminimum;
int    funcminimumnumber;

char* plaintext;
char* cryptedtext; 
char* decryptedtext;
char *shiftedtext[MAXKEYLENGTH];
char *symbolgroups[MAXKEYLENGTH];
int allocateflag;

int Kasiskikeylen, Fridmankeylen;

typedef struct tqueue
{
//	struct queue *prevelem;
	struct tqueue *nextelem;
	long  value;
} tqueuedist;

struct tqueue *qbegindist = NULL;
double keylenestimate[2][MAXKEYLENGTH];

/*
 Add to queue distances
*/
tqueuedist* adddistance(long dist)
{
	struct tqueue *qdistptr;
	qdistptr = malloc(sizeof(tqueuedist));
	if ( NULL != qdistptr )
	{
		//if (qbegindist != NULL) qbegindist->prevelem = qdistptr;
		qdistptr->nextelem = qbegindist;
		//qdistptr->prevelem = NULL;
		qbegindist = qdistptr;
		qdistptr->value = dist;
		return qdistptr;
	}
	else
	{
		return NULL;
	}
}

/*
 Free queue memory
*/
void clearqueue(struct tqueue *qbegin)
{
	struct tqueue *qdistptr;
	qdistptr = qbegin;
	while( NULL != qbegin )
	{
		qdistptr = qbegin->nextelem;
		free(qbegin);
		qbegin = qdistptr;
	}
}

/*
 Greatest common divisor
*/
unsigned long gcd( unsigned long n, unsigned long m )
{
	static long cacheGCD[CACHESIZE][CACHESIZE] = {0};
	unsigned long cached = 0;
	if ( ( 0 == n  ) || ( 0 == m ) )
	{
		return 0;
	}

	if ( ( n < CACHESIZE ) && ( m < CACHESIZE ) )
	{
		cached = cacheGCD[n - 1][m - 1];
	}

	if ( 0 == cached )
	{
		cached = (n > m)?gcd(n - m, m):(n < m)?gcd(n, m - n):n;
		cacheGCD[n - 1][m - 1] = cached;
	}

	return cached;
}

/*
 Estimate of key length
*/
int Kasiskiexam(char* text, int textlen, int* keylen)
{
	long tripletpos, i, test;
	struct tqueue *qdistptr1, *qdistptr2;
	long keylenacc[MAXKEYLENGTH];
	memset(keylenacc, 0, sizeof(keylenacc));
 
	for (tripletpos = 0; tripletpos < textlen - 3; tripletpos++)
	{
		for (i = 0; i < textlen - 3; i++)
		{
			if (
				   (text[tripletpos] == text[i])
				&& (text[tripletpos + 1] == text[i + 1])
				&& (text[tripletpos + 2] == text[i + 2])
			)
			{
				if (i != tripletpos)
				{
					if (adddistance(abs(i - tripletpos)) == NULL)
					{
						return EXIT_FAILURE; 
					};
	    			}
			}
		}
  	}
 
	qdistptr1 = qbegindist;
	while(qdistptr1 != NULL)
	{
		qdistptr2 = qbegindist;
		while(qdistptr2 != NULL)
		{
			test = gcd(qdistptr1->value, qdistptr2->value);
			if ((test > 1) && (test <= MAXKEYLENGTH))
			{
				keylenacc[test - 1]++; 
			}
			qdistptr2 = qdistptr2->nextelem;
		}
		qdistptr1 = qdistptr1->nextelem;
	}

#ifdef DEBUG
	qdistptr2 = qbegindist;
	while(qdistptr2 != NULL)
	{
		fprintf(stdout,"%i ",(int)(qdistptr2->value));
		qdistptr2 = qdistptr2->nextelem;
	}
	fprintf(stdout,"\n");
	for (i = 0; i < MAXKEYLENGTH; i++)
	{
		fprintf(stdout,"%i ",(int)keylenacc[i]);
	}
	fprintf(stdout,"\n");
#endif
 
	test = 0;
	for (i = 0; i < MAXKEYLENGTH; i++)
	{
		if (test < keylenacc[i])
		{
			test = keylenacc[i];
			*keylen = i + 1;
		} 
	}
	if (test != 0)
	{
		for (i = 0; i < MAXKEYLENGTH; i++)
		{
			keylenestimate[0][i] = (double)keylenacc[i] / test;
		}
	}

	return EXIT_SUCCESSFULL;
}

int Fridmanexam(char* text, int textlen, int* keylen)
{
	long i, j, acc;
	double indexes[MAXKEYLENGTH + 1];
	double test;
	// Shift cryptedtext to array shiftedtext
	// Find indexes of coincidence for each string
	for (i = 0; i < MAXKEYLENGTH; i++)
	{
		for ( j = 0; j < textlen; j++ )
		{
			shiftedtext[i][(j + i + 1) % textlen] = text[j];
		}
		acc = 0; 
		for (j = 0; j < textlen; j++)
		{
			if (shiftedtext[i][j] == text[j])
			{
				acc++;
			}
		} 
		indexes[i] = (double)acc / textlen;
	}

	for ( i = 0; i <= MAXKEYLENGTH; i++)
	{
		if ( test < indexes[i] )
		{
			test = indexes[i];
			*keylen = i + 1;
		} 
	}

	if ( 0 != test )
	{
		for (i = 0; i < MAXKEYLENGTH; i++)
		{
			keylenestimate[1][i] = (double)indexes[i] / test;
		}
	}
#ifdef DEBUG
	for (i = 0; i < MAXKEYLENGTH; i++)
	{
		fprintf(stdout,"String %i index is %5.4f,  %s\n",(int)(i + 1), indexes[i], shiftedtext[i]);
	}
#endif
}

/*
 Vigenere encryption procedure
*/
void encrypt(char* text, char* key)
{
	int i = 0;
	int keylen = strlen(key);
	while (text[i])
	{
		if ((text[i] >= MINSYMBOL) && (text[i] <= MAXSYMBOL))
		{
			text[i] = ((text[i] - MINSYMBOL) + (key[i % keylen] - MINSYMBOL)) % 26 + MINSYMBOL;
		}
		i++;
	}
}

/*
 Vigenere decryption procedure
*/
void decrypt(char* text, char* key)
{
	int i = 0;
	int keylen = strlen(key);
	while (text[i])
	{
		if ((text[i] >= MINSYMBOL) && (text[i] <= MAXSYMBOL))
		{
			text[i] = (26 + (text[i] - MINSYMBOL) - (key[i % keylen] - MINSYMBOL)) % 26 + MINSYMBOL;
		}
		i++;
	}
}

/*
  This procedure returns all non-significant characters to decrypted text
*/
void restoretextstructure(char* text, char* source, char* dest)
{
	int i = 0;
	int j = 0;
	while (source[i])
	{
		if ((ADDUCTION(source[i]) >= MINSYMBOL) && (ADDUCTION(source[i]) <= MAXSYMBOL))
		{
			dest[i] = text[j++];
		}
		else
		{
			dest[i] = source[i];
		} 
		i++; 
	}
}

/*
 Check & free resources
*/
void prepare_exit(void)
{
	if ( NULL != f )
	{
		fclose(f);
	}
	if ( NULL != fname )
	{
		free(fname);
	}
	if ( NULL != cryptedtext )
	{
		free(cryptedtext);
	}
	if ( NULL != plaintext )
	{
		free(plaintext);
	}
	if ( NULL != key )
	{
		free(key);
	}
	for (i = 0; i < MAXKEYLENGTH; i++)
	{
		if ( NULL != shiftedtext[i] )
		{
			free(shiftedtext[i]);
		}
	}
	for (i = 0; i < MAXKEYLENGTH; i++)
	{
		if ( NULL != symbolgroups[i] )
		{
			free(symbolgroups[i]);
		}
	}
	if ( NULL != qbegindist )
	{
		clearqueue(qbegindist);
	}
}

int main(int argc, char** argv)
{
	// Get commandline parameter   
	if (argc != 2)
	{
		fprintf(stdout,"Usage: vigener datafile.txt\n");
		return EXIT_FAILURE;
	}

	if (strlen(argv[1]) >= FILENAME_MAX)
	{
		fprintf(stdout, "Filename is too long!\n");
	}
	else
	{
		fname = malloc(strlen(argv[1]) + 1);
		if ( NULL != fname )
		{
			strcpy(fname, argv[1]);
		}
		else
		{
			prepare_exit();
			return EXIT_FAILURE;
		} 
	}

	// Open the target file
	f = fopen(fname, "r");
	if (f == NULL)
	{
		fprintf(stdout,"Error opening file '%s': exiting.\n",fname);
		prepare_exit();
		return EXIT_FAILURE;
	}

	// Take target file size in bytes
	fseek(f, 0, SEEK_END);
	filesize = ftell(f);
	fseek(f, 0, SEEK_SET);

	// Fill source text variables
	// plaintext will contents all target file matter
	// cryptedtext will contents meaningful matter
	// textsize - is size of cryptedtext variable in bytes
	if ( filesize > 0 )
	{
		cryptedtext = malloc(filesize);
		decryptedtext = malloc(filesize);
		plaintext = malloc(filesize);
		allocateflag = 1;
		for (i = 0; i < MAXKEYLENGTH; i++)
		{
			shiftedtext[i] = malloc(filesize);
			if (shiftedtext[i] == NULL)
			{
				allocateflag = 0;
			}
		}

		if (
			   ( NULL != cryptedtext )
			&& ( NULL != plaintext )
			&& ( NULL != decryptedtext )
			&& allocateflag
		)
		{
			j = 0;
			for (i = 0; i < filesize; i++)
			{
				plaintext[i] = fgetc(f);
				cryptedtext[j] = ADDUCTION(plaintext[i]);
				if ((cryptedtext[j] >= MINSYMBOL) && (cryptedtext[j] <= MAXSYMBOL))
				{
					j++;
				}
			        else
				{
					cryptedtext[j] = 0;
				}
			}

			textsize = j;

#ifdef DEBUG
			fprintf(stdout,"%s\n",plaintext);
			fprintf(stdout,"%s\n",cryptedtext);
#endif
		}
		else
		{
			prepare_exit();
			fprintf(stdout,"Can't allocate memory to read file content: exiting");
			return EXIT_FAILURE;
		}
	}
	else
	{
		fprintf(stdout,"Datafile is empty");
	}
	memset(keylenestimate, 0, sizeof(keylenestimate));
	Kasiskiexam(cryptedtext, textsize, &Kasiskikeylen);
	Fridmanexam(cryptedtext, textsize, &Fridmankeylen);
 
	floatacc = 0;
	for (i = 0; i < MAXKEYLENGTH; i++)
	{
		if (floatacc < keylenestimate[0][i] + keylenestimate[1][i])
		{
			floatacc = keylenestimate[0][i] + keylenestimate[1][i];
			estimatedkeylengh = i + 1;
		}
	}
   
#ifdef DEBUG
	fprintf(stdout,"Kasiskikeylen is %i\n", Kasiskikeylen);
	fprintf(stdout,"Fridmankeylen is %i\n", Fridmankeylen);
	for (j = 0; j < 2; j++)
	{
		fprintf(stdout,"Estimates %i:", (int)j);
		for (i = 0; i < MAXKEYLENGTH; i++)
		{
			fprintf(stdout," %5.4f", keylenestimate[j][i]);
		}
		fprintf(stdout,"\n"); 
	}
	fprintf(stdout,"Consolidated length is %i\n", estimatedkeylengh); 
#endif
 
	for (i = 0; i < MAXKEYLENGTH; i++)
	{
		symbolgroups[i] = malloc(textsize);
		if (symbolgroups[i] == NULL)
		{
			prepare_exit();
			return EXIT_FAILURE;
		}
	}

	memset(symbolfrequences, 0, sizeof(symbolfrequences));
	for (i = 0; i < MAXKEYLENGTH; i++)
	{
		memset(symbolgroups[i], 0, textsize);
	}

	key = malloc(MAXKEYLENGTH + 1);
	if ( NULL == key )
	{
		prepare_exit();
		return EXIT_FAILURE;
	}
	memset(key, 0, MAXKEYLENGTH + 1);

	for (i = 0; i < textsize; i++)
	{
		symbolgroups[i % estimatedkeylengh][i / estimatedkeylengh] = cryptedtext[i]; 
		symbolfrequences[i % estimatedkeylengh][cryptedtext[i] - MINSYMBOL] += 1;
		if (symbolfrequences[i % estimatedkeylengh][cryptedtext[i] - MINSYMBOL] > symbolfrequences[i % estimatedkeylengh][26])
		{
			symbolfrequences[i % estimatedkeylengh][26] = symbolfrequences[i % estimatedkeylengh][cryptedtext[i] - MINSYMBOL];
		}
	}

	for (j = 0; j < estimatedkeylengh; j++)
	{
		for (k = 0; k < 26; k++)
		{
			symbolfrequences[j][k] = eng_maxval * symbolfrequences[j][k] / symbolfrequences[j][26];
		}
#ifdef DEBUG
		fprintf(stdout," %f", floatacc);
#endif
		for (k = 0; k < 26; k++)
		{
			floatacc = 0;
			for (i = 0; i < 26; i++)
			{
				floatacc += abs(symbolfrequences[j][(i + k) % 26] - eng_freq[i]);
			}
#ifdef DEBUG
			fprintf(stdout," %f", floatacc);
#endif
			if ((funcminimum > floatacc) || (key[j] == 0))
			{
				funcminimum = floatacc;
				key[j] = k + MINSYMBOL;
			}
		}
#ifdef DEBUG
		fprintf(stdout,"\n");
#endif
	}

#ifdef DEBUG
	for (i = 0; i < estimatedkeylengh; i++)
	{
		fprintf(stdout,"Group %i,  %s\n", (int)(i + 1), symbolgroups[i]);
		for (j = 0; j < 26; j++)
		{
			fprintf(stdout,"%4.3f ", symbolfrequences[i][j]);
		}
		fprintf(stdout,"\n");
	}
	fprintf(stdout,"Probable key is: %s\n", key);
#endif

	decrypt(cryptedtext, key);
	restoretextstructure(cryptedtext, plaintext, decryptedtext);
 
	fprintf(stdout,"Decrypted text: %s\n", decryptedtext);
	fprintf(stdout,"Key: %s \n", key);
 
	prepare_exit();
	return EXIT_SUCCESSFULL;
}
