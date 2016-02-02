//This file is located from within the parser directory

/* NON is used to tell the computer that no action should be taken.. 
also to fill the action array with good data */

typedef double creal_t; //the main creal datatype
typedef double creal_d; //this is the upcomming datatype right here, alwayse use a double data type for now

void quitWithError(char const *string); //this is used to quit with an error
void quitWithSuccess(char const *string); //this is used to quit with success

/* Comparisons */ 

creal_d greaterThan(creal_d a, creal_d b);
creal_d greaterThanEqualTo(creal_d a, creal_d b);
creal_d lesserThan(creal_d a, creal_d b);
creal_d lesserThanEqualTo(creal_d a, creal_d b);
creal_d equalTo(creal_d a, creal_d b);
creal_d notEqualTo(creal_d a, creal_d b);
creal_d wasteCpuNullAction(creal_d a, creal_d b);
void zeroArray(creal_t array[], int count);



/* Expression Strings */

//~ int setBracketPointsForExpression(struct helperDataForActions helperList[], int index,struct garbageList *gList);
//~ 
//~ 
//~ int takeOutBackSlashesFromString(const char *string);
//~ int findTargetBetweenBrackets(struct helperDataForActions helperList[], int index);

//~ int concatStrings(struct helperDataForActions helperList[], int index, char **string); //used to morph the strings
//~ int concatStringsP(struct helperDataForActions helperList[], int index, char **string); //used to morph the strings
//~ 
//~ int setBracketPointsForExpressionP(struct helperDataForProperties helperList[], int index, struct garbageList *gList);
//~ int findTargetBetweenBracketsP(struct helperDataForProperties helperList[], int index);

//~ json_array_size(root)


/* Execution */ 

void executeActions(void); //deprecated
//~ 
//~ int processProperties(creal_t properties[] ,struct helperDataForProperties propertiesHelper[], uint count, uint cpuNum,	char *threadPipePaths[]); //process the properties
//~ 
//~ 
//~ int processActionsAndConditions(creal_t properties[], struct pre_actionlist  crealActions[], struct helperDataForActions actionHelper[], struct executionStructure executionStruct[], uint count); //used to process actions
//~ 


/* Constants for Actions */ 


//This lookup table is for conditional data
creal_d (*conditionLookUpTables[7])(creal_d , creal_d ) =
{
	&wasteCpuNullAction,
	&equalTo,
	&greaterThan,
	&lesserThan,
	&greaterThanEqualTo,
	&lesserThanEqualTo,
	&notEqualTo
	
};

char const NONACTION = 0x00; //nothing a type of NULL action

char const INCREASE = 0x01; //used for addition

char const DECREASE = 0x02; //used for deletion

char const CHANGE = 0x03; //used for changing

char const MAINTAIN = 0x04; //used for maintaining

char const TRANSPORT = 0x05; //used for transporting



/*Constants for relations */

char const NONRELATION = 0x00; //nothing a type of NULL action

char const EQUAL = 0x01; //used for addition

char const GREATERTHAN = 0x02; //used for deletion

char const LESSERTHAN = 0x03; //used for changing

char const GREATERTHANEQUALTO = 0x04; //used for maintaining

char const LESSERTHANEQUALTO = 0x05; //used for transporting

char const NOTEQUAL = 0x06; //used for transporting



//These are garbage collection directives

char const HOST = 0x00; //These are used to change some garbage collection function

char const DEVICE = 0x01; //These are used to change some garbage collection function




	


//other libraries are below

//~ #include "simplePrintf/cuPrintf.cu" //including the cuda simple printf stuff

//the data structures
//~ #include "libs/data_structures.h"
//~ 
//~ //creal node structures
//~ #include "libs/creal_node_structures.h"

//the json stuff
//~ #include "libs/json_parse.h"

//cuda functions
//~ #include "libs/cuda_functions.h"






/*
void setupTest(char charArray[])
{
	
			charArray[0] = ADD;
			charArray[1] = ADD;
			charArray[2] = ADD;
			charArray[3] = ADD;
			charArray[4] = ADD;
			charArray[5] = ADD;
			
}
*/

//~ void crealTimeFunction(struct timeStructure *time)
//~ {
		//~ 
		//~ time->milliseconds = time->milliseconds + .001;
	//~ 
//~ }
//~ 
//~ 
//~ void displayCrealTime(struct timeStructure *time)
//~ {
		//~ 
		//~ printf("%f\n",time->milliseconds);
//~ 
//~ 
//~ }
//~ 


//this string is just used to brand the application...


char const *crealString = "Creal A Wonderful Application\n\n \
";



void quitWithError(char const *string)
{
		//this is a wrapper function for quiting
		perror(string); //print the string
		puts("\n"); // add a new line
		exit(EXIT_FAILURE); //exit with failure 

}

//~ int greaterThanEqualTo(int a, int b);
//~ int lesserThan(int a, int b);
//~ int lesserThanEqualTo(int a, int b);
//~ int equalTo(int a, int b);
//~ int wasteCpuNullAction(int a, int b);



creal_d greaterThan(creal_d a, creal_d b)
{
	if (a > b)
	{
		return 1; //true
	}
	else
	{
		return 0; //false
	}
	
}

creal_d greaterThanEqualTo(creal_d a, creal_d b)
{
	if (a >= b)
	{
		return 1; //true
	}
	else
	{
		return 0; //false
	}
	
}

creal_d lesserThan(creal_d a, creal_d b)
{
	if (a < b)
	{
		return 1; //true
	}
	else
	{
		return 0; //false
	}
	
}

creal_d lesserThanEqualTo(creal_d a, creal_d b)
{
	if (a <= b)
	{
		return 1; //true
	}
	else
	{
		return 0; //false
	}
	
}

creal_d equalTo(creal_d a, creal_d b)
{
	if (a == b)
	{
		return 1; //true
	}
	else
	{
		return 0; //false
	}
	
}

creal_d notEqualTo(creal_d a, creal_d b)
{
	if (a != b)
	{
		return 1; //true
	}
	else
	{
		return 0; //false
	}
	
}

creal_d wasteCpuNullAction(creal_d a, creal_d b)
{
	
	//this does nothing
	
		return 1; //true
	
	
}


/*

void zeroArray(creal_d array[], int count)
{
	
int i = 0;
		for (i = 0; i < count; i++)
		{
			array[i] = 0; //nullifying everything
			
		}	
	
}




int setBracketPointsForExpression(struct helperDataForActions helperList[], int index, struct garbageList *gList)
	{
	
	//This function finds the bracket information in the strings and then adds it to a helper list
	//~ asprintf(
	if (helperList[index].expressionString == NULL)
	{
		return -1;
		
	}
	uint beginning, end, sumation, internalCounter;
	
	beginning = sumation = internalCounter = end = 0; //secret
	
	struct queue internQueue = {0}; //this is used to pass some values into things
	initQueue(&internQueue); //initialize the internal queue
	 
		uint stringLength = strlen(helperList[index].expressionString); //use this to get the length of the string to process
		int i = 0; 
		
				for(i = 0; i < stringLength; i++)
				{
					
					if (helperList[index].expressionString[i] == '[')
					{
						internalCounter++;
						beginning = i;
						
					}
					
					if (helperList[index].expressionString[i] == ']')
					{
						internalCounter++;
						end = i;
					}
					
					if (internalCounter == 2)
					{
						//this is where the stuff happens
						
						sumation++; //this makes sure we declare the proper size of the beginning and end stuff
						internalCounter = 0; // make sure to set it back to zero
						enqueueForInternal(&internQueue,beginning,end); //enqueue the stuff
						
					}
					
				}
				if (!(sumation <= 0))
				{
					helperList[index].endBeginOfBrackets = (struct bracketFindingStructForActionHelper *) calloc(sumation, sizeof(struct bracketFindingStructForActionHelper));
					addElementToGarbageList(helperList[index].endBeginOfBrackets,gList);
					helperList[index].countOfBracketList = sumation; // the count of the bracket list
					
				}
				else
				{
					
					helperList[index].endBeginOfBrackets = NULL; //Nullify this pointer
					helperList[index].countOfBracketList = 0; // Zero! Nothing is here
				}
		
		
		
			for (i = 0; i < sumation; i++)
			{
				
				//redundant, refactor later to make it more efficient
				beginning = internQueue.head->crealNodeIndex; //queue to temp variable
				end = internQueue.head->crealNodeIndexExtra; //queue to temp variable
				
				helperList[index].endBeginOfBrackets[i].beginning = beginning; //confusing but just look at it
				helperList[index].endBeginOfBrackets[i].end = end; //confusing but just look at it
				
				
				//~ helperList.endBeginOfBrackets.beginning = internQueue->head->crealNodeIndex;
				//~ helperList.endBeginOfBrackets.end = internQueue->head->crealNodeIndexExtra;

					
				dequeueForInternal(&internQueue); //make sure we dequeue everything
			}
			
					return sumation;//everything was good
			

	}
	
	
int findTargetBetweenBrackets(struct helperDataForActions helperList[], int index)
{
	
	if (helperList[index].expressionString == NULL)
	{
		return -1;
		
	}
		//this is used to find the target between the brackets
		int i, tempVarForLength, k, j, tempPlacerBegin, tempPlacerEnd, whileLoopCounter; //this is the counter variable
		tempPlacerBegin = tempPlacerEnd= j = k = i = tempVarForLength = whileLoopCounter = 0; //set all the variable to zero
		//
		//
		char string[128]; //I doubt that things will get this huge for indexes for the array indexes
		char string2[128]; //I doubt that things will get this huge for indexes for the array indexes
		
		
		
				for (i = 0; i < helperList[index].countOfBracketList; i++)
				{
						
						memset(string, 0 ,sizeof(char) * 128); //clear out the memory everytime we are adjusting shit
						
				
						printf("The begning of the bracket at index: %d\n : %d\n The ending of the bracket at index : %d\n %d\n\n", index, helperList[index].endBeginOfBrackets[i].beginning, index, helperList[index].endBeginOfBrackets[i].end);
						k = 0;
						tempPlacerBegin = helperList[index].endBeginOfBrackets[i].beginning + 1;
						tempPlacerEnd = helperList[index].endBeginOfBrackets[i].end - 1;
						
						if (helperList[index].endBeginOfBrackets[i].beginning == 0)
						{
							tempVarForLength = helperList[index].endBeginOfBrackets[i].end - 1; //find the absolute value of the variables
								
								//~ for (k = 0; k < tempVarForLength; k++)
								//~ {
										//~ 
									//~ string[k] += helperList[index].expressionString[helperList[index].endBeginOfBrackets[i].beginning + (k + 1)];
									//~ 
								//~ }
						}
						else
						{
						
							tempVarForLength = helperList[index].endBeginOfBrackets[i].end - helperList[index].endBeginOfBrackets[i].beginning;
								
								//~ for (k = 0; k < tempVarForLength; k++)
								//~ {
									//~ 
									//~ 
									//~ string[k] += helperList[index].expressionString[helperList[index].endBeginOfBrackets[i].beginning + (k + 1)];
									//~ 
								//~ }
							
						}
						
								while(1)
								{
									string[k] += helperList[index].expressionString[tempPlacerBegin]; //append the string
									
									if (tempPlacerBegin == tempPlacerEnd)
									{
										break;
									}
									else
									{
										tempPlacerBegin++; //increment the counter
										k++; //increment k 
									}
								}
									string[tempVarForLength] += '\0'; //add a null terminating string
									
									
									if (string[0] == 'p')
									{
										//right here is where we change the formating 
										memset(string2, 0 ,sizeof(char) * 128); //clear out the memory everytime we are adjusting shit
										 j = k = 0;
										
											while (string[k] != '\0')
											{
												//~ || 'i' || 'd' || ':'
												if (string[k] != 'p' )
												{
													if (string[k] != 'i' )
													{
															if (string[k] != 'd' )
															{
																	if (string[k] != ':' )
																	{
																	string2[j] += string[k];
																	j++; //increment j
															
															}
														}
													
													}
												}
													k++; //increment k
											}
											 
											j++;
											string2[j] += '\0';
											memset(string, 0 ,sizeof(char) * 128); //clear out the memory everytime we are adjusting shit
											strcpy(string,string2);
											
									}
							helperList[index].endBeginOfBrackets[i].targetID = (uint) atoi(string); //assign the value to node index
							puts(string); //put here for debugging
				
				} //end of the for loop
				
				
				
		
		return 1; //whoops
}

int setBracketPointsForExpressionP(struct helperDataForProperties helperList[], int index, struct garbageList *gList)
	{
	
	//This function finds the bracket information in the strings and then adds it to a helper list
	//~ asprintf(
	if (helperList[index].expressionString == NULL)
	{
		return -1;
		
	}
	uint beginning, end, sumation, internalCounter;
	
	beginning = sumation = internalCounter = end = 0; //secret
	
	struct queue internQueue = {0}; //this is used to pass some values into things
	initQueue(&internQueue); //initialize the internal queue
	 
		uint stringLength = strlen(helperList[index].expressionString); //use this to get the length of the string to process
		int i = 0; 
		
				for(i = 0; i < stringLength; i++)
				{
					
					if (helperList[index].expressionString[i] == '[')
					{
						internalCounter++;
						beginning = i;
						
					}
					
					if (helperList[index].expressionString[i] == ']')
					{
						internalCounter++;
						end = i;
					}
					
					if (internalCounter == 2)
					{
						//this is where the stuff happens
						
						sumation++; //this makes sure we declare the proper size of the beginning and end stuff
						internalCounter = 0; // make sure to set it back to zero
						enqueueForInternal(&internQueue,beginning,end); //enqueue the stuff
						
					}
					
				}
				if (!(sumation <= 0))
				{
					helperList[index].endBeginOfBrackets = (struct bracketFindingStructForActionHelper *) calloc(sumation, sizeof(struct bracketFindingStructForActionHelper));
					addElementToGarbageList(helperList[index].endBeginOfBrackets,gList);
					helperList[index].countOfBracketList = sumation; // the count of the bracket list
					
				}
				else
				{
					
					helperList[index].endBeginOfBrackets = NULL; //Nullify this pointer
					helperList[index].countOfBracketList = 0; // Zero! Nothing is here
				}
		
		
		
			for (i = 0; i < sumation; i++)
			{
				
				//redundant, refactor later to make it more efficient
				beginning = internQueue.head->crealNodeIndex; //queue to temp variable
				end = internQueue.head->crealNodeIndexExtra; //queue to temp variable
				
				helperList[index].endBeginOfBrackets[i].beginning = beginning; //confusing but just look at it
				helperList[index].endBeginOfBrackets[i].end = end; //confusing but just look at it
				
				
				//~ helperList.endBeginOfBrackets.beginning = internQueue->head->crealNodeIndex;
				//~ helperList.endBeginOfBrackets.end = internQueue->head->crealNodeIndexExtra;

					
				dequeueForInternal(&internQueue); //make sure we dequeue everything
			}
			
					return sumation;//everything was good
			

	}
	
	
int findTargetBetweenBracketsP(struct helperDataForProperties helperList[], int index)
{
	
	if (helperList[index].expressionString == NULL)
	{
		return -1;
		
	}
		//this is used to find the target between the brackets
		int i, tempVarForLength, k, j, tempPlacerBegin, tempPlacerEnd, whileLoopCounter; //this is the counter variable
		tempPlacerBegin = tempPlacerEnd= j = k = i = tempVarForLength = whileLoopCounter = 0; //set all the variable to zero
		//
		//
		char string[128]; //I doubt that things will get this huge for indexes for the array indexes
		char string2[128]; //I doubt that things will get this huge for indexes for the array indexes
		
		
		
				for (i = 0; i < helperList[index].countOfBracketList; i++)
				{
						
						memset(string, 0 ,sizeof(char) * 128); //clear out the memory everytime we are adjusting shit
						
				
						printf("The begning of the bracket at index: %d\n : %d\n The ending of the bracket at index : %d\n %d\n\n", index, helperList[index].endBeginOfBrackets[i].beginning, index, helperList[index].endBeginOfBrackets[i].end);
						k = 0;
						tempPlacerBegin = helperList[index].endBeginOfBrackets[i].beginning + 1;
						tempPlacerEnd = helperList[index].endBeginOfBrackets[i].end - 1;
						
						if (helperList[index].endBeginOfBrackets[i].beginning == 0)
						{
							tempVarForLength = helperList[index].endBeginOfBrackets[i].end - 1; //find the absolute value of the variables
								
								//~ for (k = 0; k < tempVarForLength; k++)
								//~ {
										//~ 
									//~ string[k] += helperList[index].expressionString[helperList[index].endBeginOfBrackets[i].beginning + (k + 1)];
									//~ 
								//~ }
						}
						else
						{
						
							tempVarForLength = helperList[index].endBeginOfBrackets[i].end - helperList[index].endBeginOfBrackets[i].beginning;
								
								//~ for (k = 0; k < tempVarForLength; k++)
								//~ {
									//~ 
									//~ 
									//~ string[k] += helperList[index].expressionString[helperList[index].endBeginOfBrackets[i].beginning + (k + 1)];
									//~ 
								//~ }
							
						}
						
								while(1)
								{
									string[k] += helperList[index].expressionString[tempPlacerBegin]; //append the string
									
									if (tempPlacerBegin == tempPlacerEnd)
									{
										break;
									}
									else
									{
										tempPlacerBegin++; //increment the counter
										k++; //increment k 
									}
								}
									string[tempVarForLength] += '\0'; //add a null terminating string
									
									
									if (string[0] == 'p')
									{
										//right here is where we change the formating 
										memset(string2, 0 ,sizeof(char) * 128); //clear out the memory everytime we are adjusting shit
										 j = k = 0;
										
											while (string[k] != '\0')
											{
												//~ || 'i' || 'd' || ':'
												if (string[k] != 'p' )
												{
													if (string[k] != 'i' )
													{
															if (string[k] != 'd' )
															{
																	if (string[k] != ':' )
																	{
																	string2[j] += string[k];
																	j++; //increment j
															
															}
														}
													
													}
												}
													k++; //increment k
											}
											 
											j++;
											string2[j] += '\0';
											memset(string, 0 ,sizeof(char) * 128); //clear out the memory everytime we are adjusting shit
											strcpy(string,string2);
											
									}
							helperList[index].endBeginOfBrackets[i].targetID = (uint) atoi(string); //assign the value to node index
							puts(string); //put here for debugging
				
				} //end of the for loop
				
				
				
		
		return 1; //went good,, whoops 
}




//buffer

creal_t innerProcessProperties(struct helperDataForProperties propertiesHelper[], uint index, const char *pipePath)
{
			char buffer[2048]; //this is used as a buffer
			
			double tmpDblVar = 0;
			
			uint i, k, stringLength, j , begin, end, targetID, p, swap;
			
			swap =	i =  k = stringLength = j = begin = end = targetID = p = 0;
			
			memset(buffer,0,sizeof(char) * (2048)); //make sure to clear the memory
			
			if (propertiesHelper[index].expressionString != NULL)
				{
					
					if (propertiesHelper[index].expressionString != " ")
					{
					
						stringLength = strlen(propertiesHelper[index].expressionString); //getting the string length
					
						
						
						if ( stringLength <= 2048)
						{
							
							if (!(propertiesHelper[index].countOfBracketList <= 0))
							{
								
								for(k = 0; k < propertiesHelper[index].countOfBracketList; k++)
								{
								
								j = 0;
								p = 0;
									
									begin = propertiesHelper[index].endBeginOfBrackets[k].beginning;
									
									end = propertiesHelper[index].endBeginOfBrackets[k].end;
									
									targetID = propertiesHelper[index].endBeginOfBrackets[k].targetID; 
								
								
									
										for(; j < begin; j++)
										{
											buffer[j] += propertiesHelper[index].expressionString[j];
										
										}
										
										for (; j <= end; j++) { //nothing is going on here  }
									
										strcat(buffer, propertiesHelper[targetID].expressionString); //concatenate the string
										
										puts(buffer); //debugging the buffer right here
								}
								
							}
							
							else
							{
								
								strcpy(buffer, propertiesHelper[index].expressionString); //this is copies the string to the buffer 
							}
							
							
							return evaluateCrealEquation(buffer,index);
							
							
						}
						
					}
					
				}
	
}



//buffer

void *pthreadProcessProperties(struct helperDataForProperties propertiesHelper[], uint index, const char *pipePath)
{
			char buffer[2048]; //this is used as a buffer
			
			double tmpDblVar = 0;
			
			uint i, k, stringLength, j , begin, end, targetID, p, swap;
			
			swap =	i =  k = stringLength = j = begin = end = targetID = p = 0;
			
			memset(buffer,0,sizeof(char) * (2048)); //make sure to clear the memory
			
			if (propertiesHelper[index].expressionString != NULL)
				{
					
					if (propertiesHelper[index].expressionString != " ")
					{
					
						stringLength = strlen(propertiesHelper[index].expressionString); //getting the string length
					
						
						
						if ( stringLength <= 2048)
						{
							
							if (!(propertiesHelper[index].countOfBracketList <= 0))
							{
								
								for(k = 0; k < propertiesHelper[i].countOfBracketList; k++)
								{
								
								j = 0;
								p = 0;
									
									begin = propertiesHelper[i].endBeginOfBrackets[k].beginning;
									
									end = propertiesHelper[i].endBeginOfBrackets[k].end;
									
									targetID = propertiesHelper[i].endBeginOfBrackets[k].targetID; 
								
								
									
										for(; j < begin; j++)
										{
											buffer[j] += propertiesHelper[i].expressionString[j];
										
										}
										
										for (; j <= end; j++) { // nothing is going on here  }
									
										strcat(buffer, propertiesHelper[targetID].expressionString); //concatenate the string
										
										puts(buffer); //debugging the buffer right here
								}
								
							}
							
							else
							{
								
								strcpy(buffer, propertiesHelper[i].expressionString); //this is copies the string to the buffer 
							}
							
							
							//~ return evaluateCrealEquation(buffer,pipePath);
							
							
						}
						
					}
					
				}
	
}


int processProperties(creal_t properties[] ,struct helperDataForProperties propertiesHelper[], uint count, uint cpuNum,	char *threadPipePaths[])
{
			if (count <= 0) {return -1;} //something bad happened here
			
			//~ pthread_t threads[cpuNum]; //set up thread array
			
			//This is it all here
			//This is it all here
			
			char buffer[2048]; //this is used as a buffer
			
			double tmpDblVar = 0;
			
			uint i, k, stringLength, j , begin, end, targetID, p, swap;
			
			swap =	i =  k = stringLength = j = begin = end = targetID = p = 0;
			
			memset(buffer,0,sizeof(char) * (2048)); //make sure to clear the memory
			
			#pragma omp parallel for
			for(i = 0; i < count; i++)
			{
				
			properties[i] =	innerProcessProperties(propertiesHelper, i,threadPipePaths[j]);
				j++;
				
				if (j > cpuNum)
				{
					j = 0; //reset it
					
				}
				//~ if (propertiesHelper[i].expressionString != NULL)
				//~ {
					//~ 
					//~ if (propertiesHelper[i].expressionString != " ")
					//~ {
						//~ 
						//~ memset(buffer,0,sizeof(char) * (2048)); //make sure to clear the memory
						//~ 
						//~ stringLength = strlen(propertiesHelper[i].expressionString); //getting the string length
					//~ 
						//~ targetID = 0; //make sure to reset this
						//~ 
						//~ if ( stringLength <= 2048)
						//~ {
							//~ 
							//~ if (!(propertiesHelper[i].countOfBracketList <= 0))
							//~ {
								//~ j = 0;
								//~ p = 0;
	//~ 
								//~ for(k = 0; k < propertiesHelper[i].countOfBracketList; k++)
								//~ {
									//~ 
									//~ begin = propertiesHelper[i].endBeginOfBrackets[k].beginning;
									//~ 
									//~ end = propertiesHelper[i].endBeginOfBrackets[k].end;
									//~ 
									//~ targetID = propertiesHelper[i].endBeginOfBrackets[k].targetID; 
								//~ 
								//~ 
									//~ 
										//~ for(; j < begin; j++)
										//~ {
											//~ buffer[j] += propertiesHelper[i].expressionString[j];
										//~ 
										//~ }
										//~ 
										//~ for (; j <= end; j++) { // nothing is going on here }
										//~ 
										//~ 
										//~ 
										//~ 
										//~ 
										//~ strcat(buffer, propertiesHelper[targetID].expressionString); //concatenate the string
										//~ 
										//~ puts(buffer); //debugging the buffer right here
										//~ 
										//~ 
									//~ 
								//~ }
							//~ 
								//~ 
							//~ }
							//~ 
							//~ else
							//~ {
								//~ strcpy(buffer, propertiesHelper[i].expressionString); //this is copies the string to the buffer 
								//~ //just run the code anyways ..meaning there were no brackets to process right here
								//~ 
			//~ 
									//properties[i] = (int) tmpDblVar;
									//printf("%g\n", tmpDblVar);
								//~ 
							//~ }
									//~ 
									//~ 
									//~ 
									//~ tmpDblVar = evaluateCrealEquation(buffer,"/dev/shm/core0_thread0");
							//~ 
			//~ 
						//~ }
						//~ 
						//~ 
			//~ 
					//~ }
					//~ 
					//~ 
			//~ 
				//~ }
				
			
				
			
			}
			
			
			
			return 0; //something good happened here

}//end of the function
		


		
int processActionsAndConditions(creal_t properties[], struct pre_actionlist  crealActions[], 
struct actionConditionsMetaHelp helpersForActions[], struct executionStructure executionStruct[], uint count)
{
	
			if (count <= 0) {return -1;} //something bad happened here
			
			//This is it all here
			//This is it all here
			
			char buffer[2048]; //this is used as a buffer
			
			double tmpDblVar = 0;
			
			uint i, k, stringLength, j , begin, end, targetID, p, swap, makeAction, conditionalTruthSum;
			
			
			swap =	i =  k = stringLength = j = begin = end = targetID = makeAction = conditionalTruthSum = p = 0;
			
			
			memset(buffer,0,sizeof(char) * (2048)); //make sure to clear the memory
			
			
				for(i = 0; i < count; i++)
				{
					
						memset(buffer,0,sizeof(char) * (2048)); //make sure to clear the memory
						
						
					
						makeAction = 0; //make sure to reset this right here
						
						targetID = 0; //assign the target id
						
						conditionalTruthSum = 0; //make sure to reset this right here
						
						//~ crealActions[i].
						if (crealActions[i].conditionListIsNull == 1)
						{
							makeAction = 1; //set up the actions to take place
							
							
							
						}
						 //~ if (crealActions[i].conditionlist != NULL) //deprecated.. I think
						else
						{
							
							
							for(k = 0; k < crealActions[i].numberOfConditions; k++)
							{
									//BEWARE, this is a lookup table.. its very efficient though
									
									conditionalTruthSum = conditionLookUpTables[crealActions[i].conditionlist[k].relationType]
									
									(properties[crealActions[i].conditionlist[k].field], crealActions[i].conditionlist[k].value);
									
									
								
							}
							
							if (conditionalTruthSum == crealActions[i].numberOfConditions)
							{
								
									makeAction = 1;
							}
							
							
						}//end of the else statement
						
					if (makeAction == 1)
					{
						
				for(k = 0; k < crealActions[i].numberOfActions; k++)
				{
					targetID = crealActions[i].actionsList[k].targetID;
					
						switch (crealActions[i].actionsList[k].actionType)
						{
							
							case INCREASE:
							{
								//increase
						
									executionStruct[targetID].actionType = INCREASE;
									
									
								//~ uint actionType;
								//~ uint targetID;
								//~ uint targetValue;
								//~ 
								
								break;
								
							}
							
							case DECREASE:
							{
								//decrease
								
								executionStruct[targetID].actionType = DECREASE;
							
							
								break;
								
							}
							
							case CHANGE:
							{
								//change
								//~ tmpDblVar = 0; //use this to determine what needs to be done
								executionStruct[targetID].actionType = NONACTION; //CHANGE THIS RIGHT HERE
								//~ tmpDblVar = evaluateCrealEquation(buffer,"/dev/shm/core0_thread0");
								
								
								break;
								
								
								
							}
							
							case MAINTAIN:
							{
								//maintain
								
								executionStruct[targetID].actionType = NONACTION;
							
							break;
							
								
								
							}
							
							case TRANSPORT:
							{
								//transport
								
								executionStruct[targetID].actionType = NONACTION;
							
							break;
								
								
							}
							
							//~ default: 
							//~ {
								//~ 
								//~ executionStruct[targetID].actionType = NONACTION;
							//~ }
							
							
						}
					
					executionStruct[targetID].targetID = crealActions[i].actionsList[k].targetID;
					executionStruct[targetID].targetValue = crealActions[i].actionsList[k].targetValue;
					
					} //end of innner for loop
						
						//~ crealActions[i].actionsList[k]
									
							
							
							makeAction = 0; //reset the variable
							
					} //end of the condition for the makeAction Variable
					
					
					
					
				}//end of the for loop


	
		return 0;	//everything went good here
		
}


int concatStrings(struct helperDataForActions helperList[], int index, char **string)
{
	//deprecated ... use strcat
	if (helperList[index].expressionString != NULL)
	{
			char buffer[128]; //this is used as a buffer
			char buffer2[128]; //this is used as a buffer
			char *buffer3;
			char *buffer4;
			char *bufferString;
			int i;
			i = 0;
			//~ asprintf(&bufferString
			for(i = 0; i < helperList[index].countOfBracketList; i++)
			{
				
				
			}

			return 0;
	}
	
	else
	{
		
			return -1; //error
			
			
	}
}











int concatStringsP(struct pre_actionlist crealActions[] , struct helperDataForProperties helperList[], uint count)
{
	
	return 0;
}


void executeActions(void)
{
	
	
}


// This below is used to  
* 
* 
* */ 


mode_t read_umask (void) {
    mode_t mask = umask (0);
    umask (mask);
    return mask;
}


