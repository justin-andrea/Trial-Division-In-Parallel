#ifndef _PRIMES
#define _PRIMES

// function to find small primes
using namespace std;

int *primes; // the array holding the small primes
int primeslen; // length of that array

void findsmallprimes(int limit) // use limit = sqrt(n)
{
	bool it[limit+1];
	// fill with all trues
	for(int i=0; i<=limit; i++) it[i]=true;
	// 0 and 1 are not primes
	it[0]=it[1]=false;
	
	//cross off
	for(int d=2; d*d<=limit; d++)
		if(it[d]) // then cross off with it
	      for(int q=2*d; q<=limit; q=q+d) it[q]=false;
	
	int truecount=0;
	for(int i=0; i<=limit; i++) if(it[i]) truecount++;
	
	primes=new int[truecount];
	primeslen=0;
	// fill the primes array
	for(int i=0; i<=limit; i++) 
		if(it[i]) primes[primeslen++]=i;
}
#endif