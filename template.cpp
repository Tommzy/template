/** This is an implementation of a Markov model for comparison with TP
	resolutes.  Refer to following TP model
	2Markovs SeedingOFF Random Number consumerDist.trex
	Make sure to populate the dist_pWellToDead array if you want to
	recreate sampled run from TP.
	**/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float probtoprob(float p1, float p2 );
int writeToFile(float* array, int size);


//1. input array of probability, out put the state
int getState(float* prob, int length){
	float randnum = rand()/(double)RAND_MAX;
	int i;
	int state;
	for(i=0;i<length;i++){
		if(i==0){
			continue;
		}
		prob[i]=prob[i]+prob[i-1];
		if(randum<prob[i]){
			state = i;
			break;
		}
	}
	return state;
	
}
int main(void){
	srand(time(NULL));

	int yearCount = 120;

	float cWell = 100;
	float cSick=500;
	float uWell = 0.95f;
	//Check if this pWellToDead parameter is sampled or should be fixed.
	float pWellToDead = 0.25;

	int cycles_per_year;
	int trial_count;
	int sample_count;
	printf("Please enter cycles per year\n");
	scanf( "%d", &cycles_per_year );
	printf("Please enter trial count\n");
	scanf( "%d", &trial_count );
	printf("Please enter sample count\n");
	scanf( "%d", &sample_count );

	printf("Running %d Samples of %d Trials\n", sample_count,trial_count);

	int count = 0;

	float* sampleMeans;
	sampleMeans = (float*)malloc(sizeof(float)*sample_count);

	int clean;

	double startTime = clock();//start counting cpu time
	double total=0;
	double min;
	double max=0;
	int initValue=1;
	int initBenefit=0;
	int next_state=1;

	double prob1 = probtoprob(0.25,(double)1.00/(double)cycles_per_year);
	double prob2 = probtoprob(0.5,(double)1.00/(double)cycles_per_year);
	double prob3 = prob1+prob2;
	double prob4 = probtoprob(0.35,(double)1.00/(double)cycles_per_year);


	for (count=0; count<sample_count; count++){

		double *batchCost;
		batchCost = (double*)malloc(sizeof(double)*trial_count);

		int batch;
		double totalcost=0;
		int actbatch=0;

		for(batch=0;batch<trial_count;batch++){
			double cost = 0.00;
			int stage;
			if(initBenefit==1){
				cost+=100;
			}

			for(stage = 0; stage<=(yearCount*cycles_per_year);stage++){
				float randnum = rand()/(double)RAND_MAX;


				switch(next_state){
				case 1:
					if(randnum<prob1){
						next_state=3;
					}else if(
						(randnum>=prob1)
						&&
						(randnum<prob3)){
							next_state=2;
					}else{
						next_state=1;
					}
					cost=cost+cWell/cycles_per_year;
					break;
				case 2:
					if(randnum<prob4){
						next_state=3;
					}else{
						next_state=2;
					}
					cost=cost+cSick/cycles_per_year;
					break;
				case 3:
					break;
				}

				if(next_state==3){
					next_state=1;
					break;
				}

			}

			batchCost[batch]=cost;
			totalcost+=cost;
			actbatch++;
		}
		float val= totalcost/actbatch;//the mean
		sampleMeans[count] =val;

		//init value for the min and max
		if(initValue==1){
			min=val;
			max=val;
			initValue=0;
		}else{
			if(val<min){
				min=val;
			}
			if(val>max){
				max=val;
			}
		}
		total+=val;

		free(batchCost);
	}

	double totalSampleMean= total/sample_count;
	double stDevOfMeans;
	int i;
	double sum=0;
	for (i = 0; i<sample_count;i++){
		sum+=(pow((sampleMeans[i]-totalSampleMean),2)/sample_count);
	}
	stDevOfMeans = sqrt(sum);
	double cputimeDiff = (clock() - startTime)/CLOCKS_PER_SEC;

	printf("Mean of Means: %5.2f\n", totalSampleMean);
	printf("Std Dev of Means: %5.2f\n", stDevOfMeans);
	printf("Run Time (sec) : %5.2f\n", cputimeDiff);
	printf("Min of Means: %5.2f\n",min);
	printf("Max of Means: %5.2f\n",max);
	printf("Computing Finished\n");

	//char c;
	//c = getchar();
	//printf("Character entered:");
	//putchar(c);

	getchar();
	getchar();

	writeToFile(sampleMeans,sample_count);

	return 0;
}


float probtoprob(float p1, float p2 ){
	float prob;
	prob = 1-(pow((1 - p1), p2));
	return prob;
}


int writeToFile(float* array, int size){
	FILE *f = fopen("SampleMeans.csv", "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	int i;
	fprintf(f, "count, SampleMean\n");
	for(i=0;i<size;i++){
		fprintf(f,"%d, %5.5f\n",i+1,array[i]);
	}
	fclose(f);
}
