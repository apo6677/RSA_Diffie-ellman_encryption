#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <gmp.h>


int main(int argc, char **argv)
{
	int c;

	const char* ovar = NULL;
	const char* pvar = NULL;
	const char* gvar = NULL;
	const char* avar = NULL;
	const char* bvar = NULL;

	int hflag = 0;
	
	int opterr = 0;

	while ((c = getopt (argc, argv, "o:p:g:a:b:h")) != -1){
		switch(c){
			case 'o':
			ovar = optarg;
			break;

			case 'p':
			pvar = optarg;
			break;

			case 'g':
			gvar = optarg;
			break;

			case 'a':
			avar = optarg;
			fprintf(stderr, "%s", avar);
			break;

			case 'b':
			bvar = optarg;
			break;

			case 'h':
			hflag = 1;
			printf("\nThis help message");
			break;

			case '?':
				if(isprint(optopt))
					fprintf(stderr, "Uknown option '-%c'.\n", optopt);
				else
					fprintf(stderr,"Uknown option character'\\x%x'.\n", optopt );
				return 1;
				default: abort();
		}
		
	}
	mpz_t Pvar, Gvar, Avar, Bvar, A, B, S1, S2;

	mpz_init(Pvar);
	mpz_init(Gvar);
	mpz_init(Avar);
	mpz_init(Bvar);
	mpz_init(A);
	mpz_init(B);
	mpz_init(S1);
	mpz_init(S2);

	mpz_set_str(Pvar, pvar, 0);
	mpz_set_str(Gvar, gvar, 0);
	mpz_set_str(Avar, avar, 0);
	mpz_set_str(Bvar, bvar, 0);

	mpz_powm(A, Gvar, Avar, Pvar);
	
	mpz_powm(B, Gvar, Bvar, Pvar);
	

	mpz_powm(S1, B, Avar, Pvar);
	mpz_powm(S2, A, Bvar, Pvar);

	int Err = mpz_cmp(S1, S2);

	if(Err!=0){
		printf("\nThe inserted numbers were not primes, try again!");
	}
	else{printf("\nall done\n");}

	FILE* file = fopen(ovar, "w");
	gmp_fprintf(file, "%Zd, %Zd, %Zd", A, B, S1);
	fclose(file);
}