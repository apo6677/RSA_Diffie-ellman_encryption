#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <gmp.h>
#include <time.h>


int main(int argc, char **argv)
{
	int c;


	unsigned long int w = 1;
	int u = 0;

	unsigned long int P = 0;
	unsigned long int Q = 0;
	unsigned long int ii, iii = 0;
	unsigned long int plnchr, cphchr = 0;

	int A = 0;
	int B = 0;
	int C = 0;
	
	mpz_t p, q, n, lambda, e, plainchar, cipherchar, flag2, flag1, d, I, k, l;

	mpz_init(p);
	mpz_init(d);
	mpz_init(q);
	mpz_init(n);
	mpz_init(lambda);
	mpz_init(e);
	mpz_init(plainchar);
	mpz_init(cipherchar);
	mpz_init(flag2);
	mpz_init(flag1);
	mpz_init(I);
	mpz_init(k);
	mpz_init(l);


	char* pub_k = "public.key";
	char* pr_k = "private.key";
	char* a = NULL;

	const char* ivar = NULL;
	const char* ovar = NULL;
	const char* kvar = NULL;

	int gflag = 0;
	int dflag = 0;
	int eflag = 0;
	int hflag = 0;
	
	int opterr = 0;

	while ((c = getopt (argc, argv, "i:o:k: g d e h")) != -1){
		switch(c){
			case 'i':		// the user provides the filename of plaintext
			ivar = optarg;
			break;

			case 'o':		// the user provides the filename where ciphertext will be stored
			ovar = optarg;
			break;

			case 'k':		// the user provides the filename from which the keys will be retrieved
			kvar = optarg;
			break;

			case 'g':		// perform rsa key generation and store keys to seperate keyfiles
			gflag = 1;
			break;

			case 'd':		// perform rsa decryption and store plaintext to output
			dflag = 1;
			break;

			case 'e':		// perform rsa encryption and store ciphertext to output
			eflag = 1;
			break;

			case 'h':
			hflag = 1;
			printf("\nThis help message\n");
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

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
	// if the user typed g then gflag is raised to 1 so that key generation may follow.

	if(gflag==1){
		
		//step 1 and 2
		printf("Give the first prime number, p:\n");
		scanf("%ld", &P);
		printf("Give the second prime number, q:\n");
		scanf("%ld", &Q);

		mpz_set_ui(p,P);
		mpz_set_ui(q,Q);
			
		//step 3
		mpz_mul(n, p, q);

		//step 4
		mpz_set_ui(I,w);
		mpz_sub(k, p, I);
		mpz_sub(l, q, I);

		mpz_mul(lambda, k, l);
		unsigned long int L = mpz_get_ui(lambda);

		//step 5
		
		//mpz_set_ui(e,u);
		
		while(B==0){
			srand(time(NULL));
			unsigned long int E	= rand()%(L - w + 1) + w;
			mpz_set_ui(e,E);

			u = mpz_probab_prime_p(e, 48);
			if(u == 2){			
				mpz_gcd(flag2, e, lambda);
				if(mpz_get_ui(flag2)==1){
					mpz_mod(flag1, e, lambda);
					if(mpz_get_ui(flag1)!=0){B = 1;}
				}
				if(B==0){mpz_add(e, e, I);}
			}
	
		}
		mpz_invert(d, e, lambda);

		FILE* file1 = fopen(pub_k, "w");
		gmp_fprintf(file1, "%Zd, %Zd", n, d);
		fclose(file1);

		file1 = fopen(pr_k, "w");
		gmp_fprintf(file1, "%Zd, %Zd", n, e);
		fclose(file1);
	}

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

	// if the user typed e then eflag is raised to 1 so that encryption may follow.

	if(eflag==1){
		FILE* file = fopen(ivar, "r");			//input file is opened

		if(file == NULL){
			printf("\nUnable to open file");	
		}

		FILE* file1= fopen(ovar, "w");			//output file is opened

		if(file1 == NULL){
			printf("\nUnable to open file");
		}

		FILE* file2= fopen(kvar, "r");			//public.key file is opened

		if(file2 == NULL){
			printf("\nUnable to open file");
		}

//process to recover the public key begins----------------------------------------------------------------

		fseek(file2, 0, SEEK_END);
		A = ftell(file2);
		fseek(file2, 0, SEEK_SET);

		ii = fgetc(file2);
		while(ii!= 44){
			if(ii!=44){iii = 10*iii + ii - 48;}
			ii = fgetc(file2);
		}
		
		mpz_set_ui(n,iii);

		fgetc(file2); // to get the space after the coma (,) at file private.key, whose ascii number is 44, to get the first character of the private key

		iii = 0;
		while(ii!= EOF){
			if(ii!=44){iii = 10*iii + ii - 48;}
			ii = fgetc(file2);
		}

		mpz_set_ui(e,iii);

		fclose(file2);
//process to recover the public key is ended----------------------------------------------------------------


//encryption process begins---------------------------------------------------------------------------------
		fseek(file, 0, SEEK_END);
		A = ftell(file);
		fseek(file, 0, SEEK_SET);

		unsigned long int Cpch[A];

		for(B = 0; B<A; B++){
			ii = fgetc(file);
			mpz_set_ui(plainchar,ii);
			mpz_powm(cipherchar, plainchar, e, n);
			cphchr = mpz_get_ui(cipherchar);
			Cpch[B] = cphchr;
		}
		fwrite(Cpch, sizeof(long int), A, file1);


		fclose(file);
		fclose(file1);
//encryption process is ended---------------------------------------------------------------------------------
	}

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------


	// if the user typed d then dflag is raised to 1 so that decryption may follow.
	if(dflag==1){
		FILE* file = fopen(ivar, "r");

		if(file == NULL){
			printf("\nUnable to open file");
		}

		FILE* file1= fopen(ovar, "w");

		if(file1 == NULL){
			printf("\nUnable to open file");
		}

		FILE* file2= fopen(kvar, "r");

		if(file2 == NULL){
			printf("\nUnable to open file");
		}

//process to recover the private key begins----------------------------------------------------------------

		fseek(file2, 0, SEEK_END);
		A = ftell(file2);
		fseek(file2, 0, SEEK_SET);

		ii = fgetc(file2);
		while(ii!= 44){
			if(ii!=44){iii = 10*iii + ii - 48;}
			ii = fgetc(file2);
		}
		
		mpz_set_ui(n,iii);

		fgetc(file2); // to get the space after the coma (,) at file private.key, whose ascii number is 44, to get the first character of the private key

		iii = 0;
		while(ii!= EOF){
			if(ii!=44){iii = 10*iii + ii - 48;}
			ii = fgetc(file2);
		}

		mpz_set_ui(d,iii);

		fclose(file2);

//process to recover the private key is ended----------------------------------------------------------------


//decryption process begins---------------------------------------------------------------------------------
		fseek(file, 0, SEEK_END);
		A = ftell(file);
		fseek(file, 0, SEEK_SET);

		unsigned long int Plch[A/8];
		fread(Plch, sizeof(unsigned long int), A/8, file);
		
		char c;
		for(B = 0; B<A/8; B++){
			mpz_set_ui(cipherchar,Plch[B]);
			mpz_powm(plainchar, cipherchar, d, n);
			ii = mpz_get_ui(plainchar);
			fprintf(file1, "%c", (char)ii);

		}
		fclose(file);
		fclose(file1);
//decryption process is ended---------------------------------------------------------------------------------
	}

}