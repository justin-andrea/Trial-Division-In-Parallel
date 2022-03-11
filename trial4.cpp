#include<iostream>
#include<cmath>
#include<fstream>
#include "mpi.h"
#include <vector>
#include <iomanip>
#include "bigint.h"
#include "smallprimes.h"

using namespace std;

 int *prime2;

int countprimes(int id, int np, int128 n)
{
	
	int place = 0;
	int sqrtn=sqrt(sqrt(n));
	
	findsmallprimes(sqrtn);
	
	int size = ceil(100 / np);
	prime2 = new int[size];
	
	for(int i = 0; i < size; i ++){
		prime2[i] = 0;
	}

	for(long int start=sqrtn+1+id*sqrtn; start<sqrt(n); start+=np*sqrtn)
	  {
		long int stop=start+sqrtn-1;
		if(stop>n){
			stop=n;
		}

		// find primes between start and stop (inclusive)
		
		bool it[stop-start+1];
		int itlen=stop-start+1;

	    for(int i=0; i<itlen; i++){
			it[i]=true;
		}
		
		for(int i=0; primes[i]<=sqrt(stop) && i<primeslen; i++)
		{
			int d=primes[i];
			long int q;
			if(start%d==0){
				q=start;
			}else{
				q=start+(d-(start%d));
			}
			for( ; q<=stop; q+=d){
				it[q-start]=false;
			}
		}
		
		
	    for(int i=0; i<itlen; i++){
			if(it[i]){
				if(start + i != 0){
					if(n % (start + i) == 0){
						prime2[place] = start + i;
						place++;						
					}
			}
		}
	  }
	}
	
	return place;
}

vector<int> findFrequency(int128 target, int arr[], int size){
	//divide take divided number and divide again
	vector<int> v;
	int num = arr[0];
	int128 hold = target;
	for(int i = 0; i < size; i++){
	
		hold = target;
		num = arr[i];
		if(num != 0){

		while(hold != 0){

			if(hold % num == 0){
				
			v.push_back(num);
			hold = ceil(hold / num);
			
			}else{
				hold = 0;
			}	

			}
		}
		
	}
	return v;
}

int main(int argc, char *argv[])
{
	MPI_Init(&argc,&argv);
	
	// who are we?
	int id;
	MPI_Comm_rank(MPI_COMM_WORLD,&id);
	
	// how big is the group?
	int np;
	MPI_Comm_size(MPI_COMM_WORLD,&np);

	int128 limit[5];
	
	long int target = 0;
	MPI_Status status;
	if(id==0)  // gather the input
	{
		ifstream inputFile;
		inputFile.open("input.txt");
		inputFile >> limit[0];
		inputFile.close();
		cout << "input is " << limit[0] << endl;
	}
	
	
	MPI_Bcast(limit,sizeof(int128),MPI_CHAR,0,MPI_COMM_WORLD);
	
	target = ceil(sqrt(limit[0]) * 1.0) + 1;
	
	double startTime = MPI_Wtime();
	
	int size = 0;
	
	size = countprimes(id,np,limit[0]);
	
	vector<int> vect;
	vector<int> vect2;
	
	vect = findFrequency(limit[0], prime2, size);
	
	if(id == 0 ){
		vect2 = findFrequency(limit[0], primes, primeslen);

	}
	
	//intialize each individual array
	int arr[10];
	for(int i = 0; i < 10; i++){
		arr[i] = 0; 
	}
	
	//fill array with primes and their frequencies
	if(id != 0){
	for(int i = 0; i < vect.size() ; i++){
		arr[i] = vect.at(i);
	}
	}
	
	//fill array with primes and their frequencies.  proc 0 is a special case because it also does small primes as well as big primes
	if(id == 0){
		
		int start = 0;
		for(int i = 0; i < vect2.size() ; i++){
			arr[i] = vect2.at(i);
			start++;
		}
		for(int i = 0; i < vect.size() ; i++){
		arr[start] = vect.at(i);
		start++;
	}
	}
	
	//intialize final array
	int finalArray[240];
	for(int i = 0; i < 240 ; i++){
		finalArray[i] = 0;
	}
	
	
	MPI_Gather(arr, 10, MPI_INT, finalArray, 10, MPI_INT, 0, MPI_COMM_WORLD);
	
	
	if(id == 0){
		int counter = 0;
		int128 hold = limit[0];
		for(int i = 0; i < 240; i++){
			//getting the prime above the sqrt if it exists as a factor
			if(finalArray[i] != 0){
				hold = hold / finalArray[i];
				counter++;
			}
		}
				//putting the final prime in the array if it exists

		
		ofstream outputFile;
		outputFile.open("output.txt");
		outputFile << limit[0] << " = ";
		
		bool check = true;
		for(int i = 0; i < 240; i++){
			if(finalArray[i] > 0){
				cout << "final is " << finalArray[i] << endl;
				if(check){
					outputFile << finalArray[i];
					check = false;
				}else{
					outputFile << " * " << finalArray[i];
				}
			}
		}
		//putting the final prime in the array if it exists
		if(hold > 2){
			outputFile << " * " << hold;
			cout << "final is " << hold << endl;
		}
		
		outputFile.close();
	}

	double endTime = MPI_Wtime();
	
	//get time in minutes
	double time = (endTime - startTime) / 60;
	
	if(id == 0){
		cout << "run time is: " << time << " minutes." << endl; 
	}

	MPI_Finalize(); // finish
	return 0;
}