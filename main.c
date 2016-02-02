/*
 * Cerebro 
 * Made by Randall White
 * 
 *  
 * 
 * 
 * */ 
#include <stdio.h> //standard library
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h> //for file descriptor stuff
#include <stdbool.h> 
/*This stuff below can actually go into the made binary */ 

/* This application is called Cerebro */
//~ #include <pthread.h> // using posix threads only
//~ #include <regex.h>  //this is used for regular expression
//~ #include <omp.h>  //this is used for regular expression
//~ #include <math.h> //this is the standard math library for figureing out math functions	

/* This is the actual cool stuff right here */

#include <jansson.h> //this is jansson
#include "libcudacreal.h"

#include "struct_definitions.h" //struct definitions

/* These macros were copied verbatim from an earlier version of the creal parser */
#define SHARED_MEM_DIR_PATH "/dev/shm/creal/" //this is used to create a folder in order to facilitate multiprocessing.
#define SHARED_MEM_FILE "/dev/shm/creal/phpSHMcore0" //this is used to create a folder in order to facilitate multiprocessing.
#define PATH "/dev/shm/core" //this is used for the named pipe..
#define DEFAULT_THREAD_COUNT 4 //this is used to determine the default thread count of the application- unless there is a over ride
#define MAX_THREADS 16 //this is just put in for measure
#define ERROR_FILE "/dev/shm/php_pipe_errors" //this is used to look at the errors of the php stuff

/* These were added to facilitate pipe IPC, for expression parsing */
//Most of this shit is deprecated

#define PIPE_IN_FILE "/dev/shm/crealPipeInput"
#define PIPE_OUT_FILE "/dev/shm/crealPipeOutput"
#define MAX_PIPE_SIZE 2048

//~ #include "libs/json_parse.h"

#define USAGE "Cerebro, the maker of Creal version .07" //this is the usage of the program

#define CREAL_FILE_NAME "creal_ng.bin" //the name of the binary

#define CREAL_INCLUDE_FILE_NAME "creal_ng.h" //the name of the include file

#define CREAL_MAIN_CSOURCE_FILE_NAME "creal_ng.c" //this is the name of the main source file

/* This right here are the headers for the Creal_NG binary*/
#define CREAL_HEADERS "#include <stdio.h> \n#include <stdlib.h> \n#include <errno.h> \n#include <unistd.h> \n#include <string.h> \n#include <sys/types.h> \n#include <sys/stat.h> \n#include <time.h> \n#include <fcntl.h> \n#include <math.h> \n#include <pthread.h> \n#include <omp.h> \n#include <mqueue.h>" 

#define MAIN_PART "int main(int argc, char **argv) \n {" //this is actually the beginning of the main function of the application

#define END_PART "\n \n \n \
		\
exit(EXIT_SUCCESS);	\n}" //this is the end of the application

/* names of the structures*/
#define NAME_CREAL_NODE_STRUCTURE "crealNodes" //this is the name of the main creal node array of structures

#define NAME_CREAL_PROP_STRUCTURE "crealProperties" //this is the name of the main creal properties array of structures

#define NAME_CREAL_ACT_STRUCTURE "crealActions" //this is the name of the main creal node array of structures


/* These are used to pad the structures */

#define ACTION_AND_CONDITION_PAD 80 //these are used to add an extra amount of action, and conditions to the nodes

#define ACTIONLIST_PAD 15 //pad the action lists

#define PROPERTY_PAD 205 //pad the properties

#define CREAL_NODE_PAD 105 //pad the nodes

#define INDEX_PAD 201 //this is a pad for the index of the nodes 

	/* These are function prototypes */


	int abstractJsonData(json_t **root, FILE **fp); //this is used to print out the json data to a file or stdout
	int returnNumberOfCrealNodes(json_t **root); //used to return the expression
	int returnNumberOfCrealProperties(json_t **root); //returns the number of creal properties
	int returnHighestCrealPropertyID(json_t **root); //returns the highest creal property id in the json data
	int printOutStaticActionList(uint actionCount, uint conditionCount,  FILE **stream ); //this is to print out the action list right here
	int printOutStaticCrealNodes(uint actionListCount, uint propertiesCount, FILE **stream); //this function actually prints out the static creal node data
	int printOutNodeActionAndPropArrays(uint crealNodeCount, uint actionListCount, uint propertiesCount,  FILE **stream); //this function will output basic necissity
	int printOutMaxPreprocessedValues(uint propMax, uint actListMax, uint actAndCondMax, uint nodeMax, FILE **stream); //this is used to print out the max values to be used inside of creal_ng
	int returnNumberOfCoresOnSystem(void); //this function will return the amount of cores available on the system
	int createPipeFile(const char *path); //this will create a pipe in a specified location
	int writeOutActionConditionFunctions(json_t **root, FILE **fp); //this function is used to write out functions for actions and conditions, and assign them to function pointers
	int writeOutActionConditionFunctionsPrototypes(json_t **root, FILE **fp); //this function writes out the prototypes for the functions in the simulator
	int ctcpdCommunicationFunction(const char *inputFile, const char *outputFile, const char *inputString, char outputString[]); //this is the main function for communicating to ctcpd
	int quitCtcpd(const char *inputFile, const char *outputFile); //this will make the ctcpd daemon exit
	int readFromPipe(const char *path, char buffer[]); //read from a FIFO file (not really a pipe)
	int writeToPipe(const char *path, const char *string); //write to a FIFO file (not really a pipe)
	int printOutFromCTF(const char *path, FILE **fp); //this function will print out a ctf file and place it inside of the application
	
		/* Global variables here, sometimes we don't want things declared in the stack */
		
		char tempString[256]; //this is here to just make a path for our function in order to run our parse daemon
		pid_t crealPid; //this is used to grab the pid of the process
		pid_t pid, sid, cpid; // this is the pid for our daemon process redundant
		
		
		
int main(int argc, char **argv)
{
	
	
	
					if (argc <= 2)		
					{
						//nothing was entered to start the application correctly
						quitWithError(USAGE); //shows application usage	
						
					}
					
					if (!argv[1])
					{
						quitWithError("A path to the json file was not entered!\n"); //shows application usage	
						
					}
	
					FILE *outputFP = NULL; //this is to actually write the file
					
					/* The stuff below is used for grabing the amount of processors on the system */
					const int numCPU = returnNumberOfCoresOnSystem(); //this is the number of cpus available for use
					if (numCPU <= 0) { quitWithError("We didn't get a proper CPU count!!"); }
					crealPid = getpid(); //getting the pid of creal	
					

					/* The json processing */ 
					json_t *root; //json root
					
					json_error_t error; //json error
					
					root = json_load_file(argv[1], 0, &error); //this loads up the json data right here


					
					/* Main processing should happen here*/ 
					puts(CREAL_HEADERS); //these are the headers of creal the next generation
					puts(ETC_BEGINNING_DEFINITIONS); //these are the other headers needed
					printOutMaxPreprocessedValues(55, 55, 55, 55, &stdout);
					puts(CREAL_T_TYPE); //these are the headers of creal the next generation
					
					puts(ACTION_FUNCTION_POINTER); //the function pointer declaration
					puts(PRE_ACTION_STRUCT); //action 
					puts(PRE_CONDITIONLIST_STRUCT); //conditions
					
					
					
					puts(PRE_ACTION_LIST); //the action list
					
					printOutStaticActionList(10, 10,  &stdout ); //this is used to print out the contents of the 
					
					
					printOutStaticCrealNodes(1000,1000,&stdout); //this prints out the static creal node data
					
					puts(CREAL_CSV_STRUCTS);

					printOutNodeActionAndPropArrays(1000,1000,1000,&stdout); //print out the structures
					
					puts(MAIN_CREAL_PROCESSING_FUNCTIONS);
					
					writeOutActionConditionFunctionsPrototypes(&root, &stdout); //these are the function prototypes
					
					puts(MAIN_PART); //beginning of the 
		
					//~ 
					//~ puts(USAGE);
					//~ 
					
					
					
					puts(MAIN_GAME_LOOP);
					
					puts(END_PART);
					
					writeOutActionConditionFunctions(&root, &stdout); //this is where we write out the functions in relation to the 
					
					printOutFromCTF("code_templates/process_functions.ctf",&stdout);
					
					puts("void crealInit(void)\n{");
					abstractJsonData(&root, &stdout); //this is the main function of abstracting json data
					puts("}");
					json_decref(root); //free jansson parsing data

					//~ unlink(PIPE_IN_FILE); //delete the input pipe file
					//~ unlink(PIPE_OUT_FILE); //delete the output pipe file
					

			exit(EXIT_SUCCESS);	//beep-boop .. a success!!
		
		
}


int returnNumberOfCrealNodes(json_t **root)
{
		
		
		return (int) json_array_size(*root);
}

int returnNumberOfCrealProperties(json_t **root)
{
		
	
	json_t *jsonPropertiesList, *temp0; //json properties, actions, etc
			
		int propertiesCount = 0; //this is the property count	
					

	int count = (int) json_array_size(*root); //the count of the elements, for multithreading
	int i = 0;
			for (i = 0; i < count; i++)
			{
				
				temp0 = json_array_get(*root, i); //this is used temporary to get the value for the object

					 if(!json_is_object(temp0))
						{
							//~ printf("Array element : %d is not considered a json object!!\n",i);
							return -1;
						}
						else
						{
							jsonPropertiesList = json_object_get(temp0, "properties");
								 if(!json_is_object(jsonPropertiesList))
								{	
									//not ideal, something weird happens here where I cannot exit out of this fucking thing if something goes sour
									//~ printf("Element : %d contains properties that is not considered a json object!!\n",i);											
									//~ quitWithError("json array element was not an object!"); //this happens when json data is all messed up
								
								}
								else
								{
									//add the accumulations of properties for one unified array
									//elements in the creal stuff should be quick sorted
									
									
									
									propertiesCount += (int) json_object_size(jsonPropertiesList); //increase summation
									
					
									}
			
							
					}
		
		
		
		}
			
			
	return propertiesCount;
}


int returnHighestCrealPropertyID(json_t **root)
{
		
	const char *key = NULL;
	json_t *jsonPropertiesList, *temp0; //json properties, actions, etc
	void *iter; //used for iteration
	
	
					int highestPropertyNumber = 0; //used to compare
					int tempValue = 0; //used for temp value

	int count = (int) json_array_size(*root); //the count of the elements, for multithreading
	int i = 0;
			for (i = 0; i < count; i++)
			{
				
				temp0 = json_array_get(*root, i); //this is used temporary to get the value for the object

					 if(!json_is_object(temp0))
						{
							//~ printf("Array element : %d is not considered a json object!!\n",i);
							return -1;
						}
						else
						{
							jsonPropertiesList = json_object_get(temp0, "properties");
								 if(!json_is_object(jsonPropertiesList))
								{	
									//not ideal, something weird happens here where I cannot exit out of this fucking thing if something goes sour
									//~ printf("Element : %d contains properties that is not considered a json object!!\n",i);											
									//~ quitWithError("json array element was not an object!"); //this happens when json data is all messed up
								
								}
								else
								{
									//add the accumulations of properties for one unified array
									//elements in the creal stuff should be quick sorted
									
										iter = json_object_iter(jsonPropertiesList); //this is needed, the golden ticket
										while(iter)
										{
											key = json_object_iter_key(iter);
											
											tempValue = atoi(key);
											if (tempValue > highestPropertyNumber)
											{
												highestPropertyNumber = tempValue; //assign 
												
											}
											//~ puts(key); //use this to debug the key stuff
											
											
										iter = json_object_iter_next(jsonPropertiesList, iter); //this iterates through the json data	
										}
									
									//~ propertiesCount += (int) json_object_size(jsonPropertiesList); //increase summation
									
					
									}
			
							
					}
		
		
		
		}
		//~ json_decref(jsonPropertiesList); //free jansson parsing data
		//~ json_decref(temp0); //free jansson parsing data
		//~ free(iter); //free jansson parsing data
			
	return highestPropertyNumber; //return the highest number
}

int returnHighestCrealActionID(json_t **root)
{
		
	const char *key = NULL;
	json_t *jsonPropertiesList, *temp0; //json properties, actions, etc
	void *iter; //used for iteration
	
	
					int highestPropertyNumber = 0; //used to compare
					int tempValue = 0; //used for temp value

	int count = (int) json_array_size(*root); //the count of the elements, for multithreading
	int i = 0;
			for (i = 0; i < count; i++)
			{
				
				temp0 = json_array_get(*root, i); //this is used temporary to get the value for the object

					 if(!json_is_object(temp0))
						{
							//~ printf("Array element : %d is not considered a json object!!\n",i);
							return -1;
						}
						else
						{
							jsonPropertiesList = json_object_get(temp0, "action list");
								 if(!json_is_object(jsonPropertiesList))
								{	
									//not ideal, something weird happens here where I cannot exit out of this fucking thing if something goes sour
									//~ printf("Element : %d contains properties that is not considered a json object!!\n",i);											
									//~ quitWithError("json array element was not an object!"); //this happens when json data is all messed up
								
								}
								else
								{
									//add the accumulations of properties for one unified array
									//elements in the creal stuff should be quick sorted
									
										iter = json_object_iter(jsonPropertiesList); //this is needed, the golden ticket
										while(iter)
										{
											key = json_object_iter_key(iter);
											
											tempValue = atoi(key);
											if (tempValue > highestPropertyNumber)
											{
												highestPropertyNumber = tempValue; //assign 
												
											}
											//~ puts(key); //use this to debug the key stuff
											
											
										iter = json_object_iter_next(jsonPropertiesList, iter); //this iterates through the json data	
										}
									
									//~ propertiesCount += (int) json_object_size(jsonPropertiesList); //increase summation
									
					
									}
			
							
					}
		
		
		
		}
			
		//~ json_decref(jsonPropertiesList); //free jansson parsing data
		//~ json_decref(temp0); //free jansson parsing data
		//~ free(iter); //free jansson parsing data
		
	return highestPropertyNumber; //return the highest number
}


int abstractJsonData(json_t **root, FILE **fp)
{
	uint numOfCrealNodes  = (uint) json_array_size(*root);
	const char *key, *key0;
	key = key0  = NULL; //hehe
	json_t *jsonPropertiesList, *temp0, *temp1, *value, *type, *id, *properties, *actions, *conditions, 
	*actionItems, *tempForActionAndConditions, *tempForActionAndConditions2, *tempForActionAndConditionsObjects; //json properties, actions, etc
	void *iter, *iter0; //used for iteration
	char *text = NULL; //this is used to slurp all of the json data into a string
	char *text2 = NULL; //this is used to slurp all of the json data into a string
	char *text3 = NULL; //this is used to slurp all of the json data into a string
	char *text4 = NULL; //this is used to slurp all of the json data into a string
	int stringLength = 0;
	int check = 0;
	//~ int count = (int) json_array_size(*root); //the count of the elements, for multithreading
	int i = 0;
	int n = 0;
	int k, p, o, m, c, t; //these are extras needed
	k = p = o = m = c = t = 0; //setting the counter variables
	
	int insideActionBlockIncrementer = 0; //this is used in for iteration inside of the action block
	
	int propertyCounter = 0;
	int actionCounter = 0;
	int actionItemCounter = 0;
	int conditionCounter = 0;
	/* setting up some temporary variables unsigned integers*/
	
	creal_t tempPropVar0, tempPropVar1, tempPropVar2, targetValue, conditionsValue;
	tempPropVar0 = tempPropVar1 = tempPropVar2 = targetValue = conditionsValue = 0;
	
	uint tempVar, idVar, propVar, actVar, condVar, numOfPropertiesInNode, numOfActionsInNode, numOfConditionsInNode, numOfActionItems, actionType, targetID, field, relation;
	
	tempVar = idVar = propVar = actVar = condVar = numOfPropertiesInNode = numOfConditionsInNode = numOfActionItems = relation = 0; //assigning everything to zero
	
	
	//~ static char inputPipePath[256], outputPipePath[256]; //these are static variable so we don't allocate too much on the damn stack
	//~ sprintf(inputPipePath,"%s%d",PIPE_IN_FILE,crealPid); //creating a path for the pipe file
	//~ sprintf(outputPipePath,"%s%d",PIPE_OUT_FILE,crealPid); //creating a path for the pipe file
	//~ 
	//~ 
	//~ puts(inputPipePath);
	//~ puts(outputPipePath);
	//~ 
	//~ sprintf(tempString,"ctcpd %s %s & \0", PIPE_IN_FILE, PIPE_OUT_FILE); //this is used to create an argu
					
	//~ puts(tempString); //this is used to debug
					
	//~ system(tempString); //start the parsing agent right here
	//~ sleep(199);
	
	
			for (i = 0; i < numOfCrealNodes; i++)
			{
				actionItemCounter = conditionCounter = actionCounter = propertyCounter = 0; //reset the property, and action counters of the creal node
			
				  //~ 0;
				  
				temp0 = json_array_get(*root, i); //this is used temporary to get the value for the object

					 if(!(json_is_object(temp0)))
						{
							
							return -1; //json data is messed up and it is not an object!
						}
						else
						{
								id = json_object_get(temp0, "id");
								
								if (!(json_is_string(id)))
								{
									
									return -1;  //the id value was not 
								}
								else
								{
									
									json_unpack(id,"s",&text); //this unpacks the value right into value
									
									/*This is where you output the print functions */
									
									//~ puts(text); //debug 
									
									idVar = (uint) atoi(text); //This is the id value, this value will be used too print out data for print
									
									//~ fprintf(*fp,"%s[%d];\n",NAME_CREAL_NODE_STRUCTURE,idVar); //this is where we start parsing values //this is debug for now
									
								}//end of else
								
								/* now we are parsing the objects, make sure to use for loop iterations*/ 
									
									properties = json_object_get(temp0, "properties");
									
								
									if(!(json_is_object(properties)))
									{
										
										//~ return -1; //the properties objects are not formatted right, they need to be corrected
									}
									else
									{
										numOfPropertiesInNode = (uint ) json_object_size(properties); //assigning the amount of properties in the node
										
										fprintf(*fp,"%s[%d].numberOfProperties = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,numOfPropertiesInNode); //we are assigning how much creal properties are in the node
										
										iter = json_object_iter(properties); //this is needed, the golden ticket
										
										while(iter)
										{
											key = json_object_iter_key(iter);
											temp1 = json_object_iter_value(iter);
												
											if(!(json_is_object(temp1)))
												{												
												
														//~ return -1; //this happens when json data is all messed up
												
												}
												else
												{
													propVar = atoi(key); //hopefully this is good
													//~ printf("The array index of the property value of this node is : %d, it is connected to %d\n ",propertyCounter, propVar); //debug
													//~ puts(key); //debug
													//stuff goes here
													
													fprintf(*fp,"%s[%d].properties[%d] = &%s[%d];\n",NAME_CREAL_NODE_STRUCTURE,idVar,propertyCounter,NAME_CREAL_PROP_STRUCTURE,propVar); //we are assigning how much creal properties are in the node
													
													propertyCounter++; //increase the property counter
													
													
													type =	json_object_get(temp1,"type");
													if(!(json_is_string(type)))
													{
														
														//~ return -1; //this portion is not a string at all
													
													}
													else
													{
														json_unpack(type,"s",&text); //this unpacks the value right into value
																if (strcmp(text, "number") == 0)
																{
																		//making sure we only get numerical data from Json
																		
																		value =	json_object_get(temp1,"value"); //getting the value from the data 
																		
																		json_unpack(value,"s",&text); //this unpacks the value right into value
																		
																		//assign value, this helps us take away this will have to be taken away later
																		tempPropVar0 = atof(text);
																		fprintf(*fp,"%s[%d] = %f;\n",NAME_CREAL_PROP_STRUCTURE,propVar,tempPropVar0); //we are assigning how much creal properties are in the node
																		//~ puts(text);
																	
																}
																
														//this is where we do something with the type
														
														
														
													}
													
												}
												
										   iter = json_object_iter_next(properties, iter); //this iterates through the json data	
										}
								
									}//end of else
											
								
								
								
								/* These are were the actions take place */ 
								actions = json_object_get(temp0, "action list");
								
								if(!(json_is_object(actions)))
								{
									
									//~ return -1; //something is wrong with the action stuff
								}
								else
								{
									//Actions are taking place right here 
									
										numOfActionsInNode = (uint) json_object_size(actions);
										
										fprintf(*fp,"%s[%d].numberOfActions = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,numOfActionsInNode); //we are assigning how much creal properties are in the node
										
										iter = json_object_iter(actions); //this is needed, the golden ticket
										
										while(iter)
										{
											
											//Now this is set up to take the actions and conditions
											key = json_object_iter_key(iter);
											
											temp1 = json_object_iter_value(iter);
											
											if(!(json_is_object(temp1)))
												{												
												
														//~ return -1; //this happens when json data is all messed up
												
												}
												else
												{
													/* This is where all the action processing taking place */
											
													//~ puts(key); //debug
													actVar = (uint) atoi(key); //add the action index
													
													/*Changed from reverse reference to a regular reference */ 
													
													
													//~ fprintf(*fp,"%s[%d] = &%s[%d].actionList[%d];\n",NAME_CREAL_ACT_STRUCTURE,actVar,NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter); //we are assigning how much creal properties are in the node
													//~ fprintf(*fp,"%s[%d] = &%s[%d].actionList[%d];\n",NAME_CREAL_ACT_STRUCTURE,actVar,NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter); //we are assigning how much creal properties are in the node
												
													//below is the type of format that will be used for references with creal actions
													fprintf(*fp, "%s[%d].actionsList[%d] = &%s[%d];\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,NAME_CREAL_ACT_STRUCTURE,actVar);
													
													/* Assigning the action id to the original array of structures*/
													
													//~ fprintf(*fp,"%s[%d].actionsList[%d].alID = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,actVar); //we are assigning how much creal properties are in the node
													//this is how we are going to set up the action right here
													fprintf(*fp,"%s[%d].alID = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,actVar); //we are assigning how much creal properties are in the node
													
													
													
													
												
												//Iterate  through everything in the action list right here, get all of the elements
												
													
												//the actual actions right here 
												actionItems = json_object_get(temp1, "actions");
												
												if(!(json_is_array(actionItems)))
												{												
													//right here we should return zero for the number of action items
													//~ puts("This (Action) is not an object you clod!"); //this is bead
													fprintf(*fp,"%s[%d].numberOfActionItems = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,0); //THIS HAS BEEN REDONE
												}
												else
												{
														//the number of elements right here
														numOfActionItems = (uint) json_array_size(actionItems); //remember these actions are arrays
														//~ fprintf(*fp,"%s[%d] = &%s[%d].actionList[%d].numberOfActions = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,numOfActionItems); //we are assigning how much creal properties are in the node
														
														//~ fprintf(*fp,"%s[%d].actionlist[%d].numberOfActionItems = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,numOfActionItems); //this is where we are assigning the number of action items
														fprintf(*fp,"%s[%d].numberOfActionItems = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,numOfActionItems); //THIS HAS BEEN REDONE
														//RIGHT HERE ,NAME_CREAL_ACT_STRUCTURE,actVar,actVar
														//~ insideActionBlockIncrementer
														for (insideActionBlockIncrementer = 0; insideActionBlockIncrementer < numOfActionItems; insideActionBlockIncrementer++)
														{
															
															//Actions!
															
															/* This right here is for iterating into the action items */
															
															tempForActionAndConditions = json_array_get(actionItems, insideActionBlockIncrementer); //this is used temporary to get the value for the object
															//~ "action_type" //unpack right here
																	
															//For action type
															tempForActionAndConditionsObjects = json_object_get(tempForActionAndConditions, "action_type");
													
															if(!(json_is_string(tempForActionAndConditionsObjects)))
																{										
																	//Do an action here to signify that the condition is null		
																		puts("OH BOY"); //not good	
													
																}
																
															//We want to unpack a string variable here
															json_unpack(tempForActionAndConditionsObjects,"s",&text3); //this unpacks the value right into value
															
															//~ puts(text3);
															
															actionType = (uint) atoi(text3); //getting the value of the action_type
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].actions[%d].actionType = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,actionType); //this is where we are assigning the number of action items	
															//~ fprintf(*fp,"%s[%d].actions[%d].actionType = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,actionCounter,insideActionBlockIncrementer,actionType); //this is where we are assigning the number of action items	
															
															//changed 5/20/13
															fprintf(*fp,"%s[%d].actions[%d].actionType = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,actionType); //this is where we are assigning the number of action items	
															
															
															
															//For target ID 
															tempForActionAndConditionsObjects = json_object_get(tempForActionAndConditions, "target_id");
													
															if(!(json_is_string(tempForActionAndConditionsObjects)))
																{												
																		puts("OH BOY"); //not good	
													
																}
																
															//We want to unpack a string variable here
															json_unpack(tempForActionAndConditionsObjects,"s",&text3); //this unpacks the value right into value
															
															//~ puts(text3);
															
															targetID = (uint) atoi(text3); //getting the value of target_id
															
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].actions[%d].targetID = %d ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,targetID); //this is where we are assigning the number of action items	
															
															//Changed 5/20/13
															fprintf(*fp,"%s[%d].actions[%d].targetID = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,targetID); //this is where we are assigning the number of action items	
															//~ fprintf(*fp,"%s[%d].actionlist[%d].actions[%d].targetID = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,tempPropVar1); //this is where we are assigning the number of action items
															
															
															//For target value
															tempForActionAndConditionsObjects = json_object_get(tempForActionAndConditions, "target_value");
															
															if(!(json_is_string(tempForActionAndConditionsObjects)))
																{												
																		puts("OH BOY"); //not good	
													
																}
																
															//We want to unpack a string variable here
															json_unpack(tempForActionAndConditionsObjects,"s",&text3); //this unpacks the value right into value
															
															//~ puts(text3);
															
															targetValue = (creal_t) atof(text3); //getting the value target value
															
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].actions[%d].targetValue = %f ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,targetValue); //this is where we are assigning the number of action items	
															
															//Changed 5/20/13
															fprintf(*fp,"%s[%d].actions[%d].targetValue = %f;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,targetValue); //this is where we are assigning the number of action items	
															
															
															/* Added 6/3/13... Making sure we make functions */
															/* //~ actionFunction_AID_INCREMENTOR() - This right here is the action function */
															
															fprintf(*fp,"%s[%d].actions[%d].actionFunction = &actionFunction_%d_%d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,actVar,insideActionBlockIncrementer); //this is where we are assigning the number of action items	
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].actions[%d].targetValue = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,tempPropVar1); //this is where we are assigning the number of action items
																
																
															//~ fprintf(*fp,"%s[%d].actionlist[%d].numberOfActionItems = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,numOfActionItems); //this is where we are assigning the number of action items
															
														}
															//~ fprintf(*fp,"Number of Action Items %d \n",numOfActionItems); //we are assigning how much creal properties are in the node
													
													
												}//end of the else statement
												
												
												//Good work
													
												
												
												//also iterate through the conditions
												conditions = json_object_get(temp1, "conditions");
												
												if(!(json_is_array(conditions)))
												{												
													//right here we should return zero for the number of conditions 
													//~ puts("This (Condition) is not an object you clod!"); //this is bead
													fprintf(*fp,"%s[%d].numberOfConditions = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,0); //this is where we are assigning the number of conditions within an action list
												}
												else
												{
													
								
													//the number of elements right here
								
													numOfConditionsInNode = (uint) json_array_size(conditions); //remember these conditions are arrays
													
													//Changed 5/20/13
													fprintf(*fp,"%s[%d].numberOfConditions = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,numOfConditionsInNode); //this is where we are assigning the number of conditions within an action list
													
													//~ fprintf(*fp,"%s[%d].numberOfActionItems = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,numOfActionItems); //THIS HAS BEEN REDONE
													
													for (insideActionBlockIncrementer = 0; insideActionBlockIncrementer < numOfConditionsInNode; insideActionBlockIncrementer++)
														{
															
															//conditions!!
															
															
															/* This right here is for iterating into the action items */
															
															tempForActionAndConditions = json_array_get(conditions, insideActionBlockIncrementer); //this is used temporary to get the value for the object
															//For the field of the conditions
															tempForActionAndConditionsObjects = json_object_get(tempForActionAndConditions, "field");
													
															if(!(json_is_string(tempForActionAndConditionsObjects)))
																{												
																		puts("OH BOY"); //not good	
													
																}
																
															//We want to unpack a string variable here
															json_unpack(tempForActionAndConditionsObjects,"s",&text3); //this unpacks the value right into value
															
															//~ puts(text3);
															field = (uint) atoi(text3); //getting the value of the action_type
															
															//Changed 5/20/13
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].conditions[%d].field = %d ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,field); //this is where we are assigning the number of action items	
															fprintf(*fp,"%s[%d].conditions[%d].field = %d ;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,field); //this is the new shit
																
																
															//For the relation of the conditions
															tempForActionAndConditionsObjects = json_object_get(tempForActionAndConditions, "relation");
													
															if(!(json_is_string(tempForActionAndConditionsObjects)))
																{												
																		puts("OH BOY"); //not good	
													
																}
																
															//We want to unpack a string variable here
															json_unpack(tempForActionAndConditionsObjects,"s",&text3); //this unpacks the value right into value
															
															//~ puts(text3);
															relation = (uint) atoi(text3); //getting the value of the action_type
															
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].conditions[%d].relationType = %d ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,relation); //this is where we are assigning the number of action items	
															//~ fprintf(*fp,"%s[%d].actionlist[%d].conditions[%d].relationType = %d ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,relation); //this is where we are assigning the number of action items	
															fprintf(*fp,"%s[%d].conditions[%d].relationType = %d ;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,relation); //this is the new shit
															//For the value of the conditions
															tempForActionAndConditionsObjects = json_object_get(tempForActionAndConditions, "value");
													
															if(!(json_is_string(tempForActionAndConditionsObjects)))
																{												
																		puts("OH BOY"); //not good	
													
																}
																
															//We want to unpack a string variable here
															json_unpack(tempForActionAndConditionsObjects,"s",&text3); //this unpacks the value right into value
															
															//~ puts(text3);
															
															conditionsValue = (creal_t) atof(text3); //getting the value of the action_type
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].conditions[%d].value = %f ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,conditionsValue); //this is where we are assigning the number of action items	
															//~ fprintf(*fp,"%s[%d].actionlist[%d].conditions[%d].value = %f ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,conditionsValue); //this is where we are assigning the number of action items	
															//changed 5/20/13
															fprintf(*fp,"%s[%d].conditions[%d].value = %f ;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,conditionsValue); //this is the new shit
															
															
															//~ puts("This right here is the incrementer for conditions");
															//~ fprintf(*fp,"%s[%d].actionlist[%d].numberOfActionItems = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,numOfActionItems); //this is where we are assigning the number of action items
															
															/* 6/4/13 -=This stuff is added right here for now=- 
															 * conditionFunction_AID_INCREMENTOR() //
															 * ->actionFunction = &actionFunction_%d_%d
															 * */
															fprintf(*fp,"%s[%d].conditions[%d].conditionFunction = &conditionFunction_%d_%d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,actVar,insideActionBlockIncrementer); //this is the new shit
														}
														
								
													//~ fprintf(*fp,"Number of Conditions %d \n",numOfConditionsInNode); //we are assigning how much creal properties are in the node
														
														
												}//end of the else statement
												
												
												
												//I AM RIGHT LEGEND
												
												//Make sure to increment the counter!! or else it all goes astray!
												
												actionCounter++; //increment the action counter
												
													
												}
												
											iter = json_object_iter_next(actions, iter); //this iterates through the json data
											
										
										}
										
								}//end of else statement
								
								/* 
								This is where we are going to work on the conditions
								conditions = json_object_get(temp0, "conditions");
								
								if(!(json_is_object(conditions)))
								{
									
									//~ return -1; //something is wrong with the action stuff
								}
								else
								{
									numOfActionsInNode = (uint) json_object_size(actions);
									
								}//end of else statement
								
								This needs to be more thought out of right here
								
								*/
								
						}
				
			}
	
	
	
	//this is the end of the function
}


int printOutStaticActionList(uint actionCount, uint conditionCount,  FILE **stream )
{
	
	/* This is where we pring out what the hell is going on here */
	
	fprintf(*stream, " struct pre_actions actions[%d]; \n struct pre_conditionlist conditions[%d];  \n", actionCount, conditionCount);
	
	fprintf(*stream, "%s\n\n\n", CLOSE_THE_STRUCT_CURLY);
	

	return 0; //everything is ok
} 	//this is used to finish the 


int printOutNodeActionAndPropArrays(uint crealNodeCount, uint actionListCount, uint propertiesCount,  FILE **stream)
{
		/* This portion of the code will print out everything related to the creal nodes */
		
		/* Creal Nodes*/
		fprintf(*stream,"%s%d%s;\n",CREAL_NODE, crealNodeCount, END_CREAL_BRACKET);
		/* Creal Properties */
		fprintf(*stream,"%s%d%s;\n",CREAL_PROP, crealNodeCount, END_CREAL_BRACKET);
		/* Summing Vector for properties... its a temporary buffer to actually change the properties */
		fprintf(*stream,"%s%d%s;\n",CREAL_SUM_VECTORS, crealNodeCount, END_CREAL_BRACKET);
		/* Creal Actions*/
		fprintf(*stream,"%s%d%s;\n\n",CREAL_ACTIONS, crealNodeCount, END_CREAL_BRACKET);
		return 0; //everything is ok
	
}


int printOutStaticCrealNodes(uint actionListCount, uint propertiesCount, FILE **stream)
{
	/* This function prints out the static creal node data*/
	/* Top portion*/
	fprintf(*stream,"%s\n",CREAL_NODE_STRUCTURE_TOP);
	
	/* Property IDs*/
	fprintf(*stream,"\t\t%s%d%s;\n",CREAL_NODE_STRUCTURE_PROP_ID,propertiesCount,END_CREAL_NODE_STRUCTURE_BRACKET);
	
	/* Creal Properties*/
	fprintf(*stream,"\t\t%s%d%s;\n",CREAL_NODE_STRUCTURE_PROPERTIES,propertiesCount,END_CREAL_NODE_STRUCTURE_BRACKET);
	
	/* Action Lists*/
	fprintf(*stream,"\t\t%s%d%s;\n",CREAL_NODE_STRUCTURE_ACTIONLIST,actionListCount,END_CREAL_NODE_STRUCTURE_BRACKET);
	
	/*Close the structure */
	
	fprintf(*stream, "%s;\n\n", END_CREAL_NODE_STRUCTURE_STRUCTDEC);
	return 0; //everything is ok
}


int printOutMaxPreprocessedValues(uint propMax, uint actListMax, uint actAndCondMax, uint nodeMax, FILE **stream)
{
	/* This function prints out precompiler macro declarations for creal_ng*/
	
	fprintf(*stream,"%s %d\n",CREAL_MAX_PROPERTIES,propMax);
	fprintf(*stream,"%s %d\n",CREAL_MAX_ACTION_LISTS,actListMax);
	fprintf(*stream,"%s %d\n",CREAL_MAX_ACTIONS_AND_CONDITIONS,actAndCondMax);
	fprintf(*stream,"%s %d\n",CREAL_MAX_CREAL_NODES,nodeMax);
	
	
	return 1;
}


int returnNumberOfCoresOnSystem(void)
{
	
	/* This function is used to find the number of cpus on the system, its a shell function */
	return sysconf( _SC_NPROCESSORS_ONLN );  //this is used to get the cpu count
}

int createPipeFile(const char *path)
{

		/* This function is used to create a pipe file with the argument going to a specified path*/
		unlink(path); //this is redundant right here
		
		if (mkfifo(path, 0777) < 0) //make the pipe
		{
			
			/* */
			return -1;
		}
		 
		
		/* Below is a very long long functional call, but it allows us to set permissions on the file to everything open */
		chmod(path, S_ISUID |  S_ISGID | S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
	
		return 1;
}


int writeOutActionConditionFunctions(json_t **root, FILE **fp)
{
	
	
	
	//This function is actually used for grabbing the json data and making the functions for this application 
	uint numOfCrealNodes  = (uint) json_array_size(*root);
	const char *key, *key0;
	key = key0  = NULL; //hehe
	json_t *jsonPropertiesList, *temp0, *temp1, *value, *type, *id, *properties, *actions, *conditions, 
	*actionItems, *tempForActionAndConditions, *tempForActionAndConditions2, *tempForActionAndConditionsObjects; //json properties, actions, etc
	void *iter, *iter0; //used for iteration
	char *text = NULL; //this is used to slurp all of the json data into a string
	char *text2 = NULL; //this is used to slurp all of the json data into a string
	char *text3 = NULL; //this is used to slurp all of the json data into a string
	char *text4 = NULL; //this is used to slurp all of the json data into a string
	int stringLength = 0;
	int check = 0;
	//~ int count = (int) json_array_size(*root); //the count of the elements, for multithreading
	int i = 0;
	int n = 0;
	int k, p, o, m, c, t; //these are extras needed
	k = p = o = m = c = t = 0; //setting the counter variables
	
	int insideActionBlockIncrementer = 0; //this is used in for iteration inside of the action block
	
	int propertyCounter = 0;
	int actionCounter = 0;
	int actionItemCounter = 0;
	int conditionCounter = 0;
	/* setting up some temporary variables unsigned integers*/
	
	creal_t tempPropVar0, tempPropVar1, tempPropVar2, targetValue, conditionsValue;
	tempPropVar0 = tempPropVar1 = tempPropVar2 = targetValue = conditionsValue = 0;
	
	uint tempVar, idVar, propVar, actVar, condVar, numOfPropertiesInNode, numOfActionsInNode, numOfConditionsInNode, numOfActionItems, actionType, targetID, field, relation;
	
	tempVar = idVar = propVar = actVar = condVar = numOfPropertiesInNode = numOfConditionsInNode = numOfActionItems = relation = 0; //assigning everything to zero
	
	
	static char inputPipePath[256], outputPipePath[256]; //these are static variable so we don't allocate too much on the damn stack
	
	char ctcpdExecutionString[256]; //this is used to execute ctcpd with its arguments
	char tempStringBuffer[2048]; //this is a temporary string buffer
	char tempStringBuffer2[2048]; //this is a temporary string buffer
	
	
	FILE *inputPipeFd, *outputPipeFd;	
	FILE *inputFd, *outputFd;	
	
	inputFd = outputFd = inputPipeFd = outputPipeFd = NULL; //set everything to zero right here
	

	
	sprintf(inputPipePath,"%s%d",PIPE_IN_FILE,crealPid); //creating a path for the pipe file
	sprintf(outputPipePath,"%s%d",PIPE_OUT_FILE,crealPid); //creating a path for the pipe file
		
		 /* testing the making of the files */
			outputFd = fopen(outputPipePath, "wr");
		
			inputFd = fopen(inputPipePath, "wr");
	
		
	
	if (!createPipeFile(inputPipePath)) //creating the pipe input file
					{
						perror("Could not create an input Pipe file for processing! Error!\n"); //exit out of this function if everything goes really bad!!		
						return -1; //get out of here, things went bad
					}
	if (!createPipeFile(outputPipePath)) //creating the pipe input file
					{
						perror("Could not create an output Pipe file for processing! Error!\n"); //exit out of this function if everything goes really bad!!		
						return -1; //get out of here, things went bad
					}
	
	
	sprintf(ctcpdExecutionString,"./ctcpd %s %s", inputPipePath, outputPipePath); //this is used to create the command for ctcpd
	
	system(ctcpdExecutionString); //start the parsing agent right here				
	
	//~ sleep(1); //we need this right here to make sure everything parses properly
	
	
	
	/*
	 * 
	//~ puts(tempStringBuffer);
	inputPipeFd = fopen(inputPipePath,"w"); //input pipe open
	
	fprintf(inputPipeFd,"%s;","6+6"); //this is the default thing we do for writing the functions
	fflush(inputPipeFd);
	fclose(inputPipeFd);
	
	outputPipeFd = fopen(outputPipePath,"r"); //output pipe file
	
	fgets(tempStringBuffer,2048,outputPipeFd); //grabs the file data
	
	
	puts(tempStringBuffer); //output the temporary string buffer
	
	fflush(outputPipeFd);
	fclose(outputPipeFd);
	
	 
	 
	 For debug purposes
	//~ puts(inputPipePath);
	//~ puts(outputPipePath);
	* 
	* 
	*/ 
	
	
	
					
	
	
	
	
			for (i = 0; i < numOfCrealNodes; i++)
			{
				actionItemCounter = conditionCounter = actionCounter = propertyCounter = 0; //reset the property, and action counters of the creal node
			
				  //~ 0;
				  
				temp0 = json_array_get(*root, i); //this is used temporary to get the value for the object

					 if(!(json_is_object(temp0)))
						{
							
							return -1; //json data is messed up and it is not an object!
						}
						else
						{
								id = json_object_get(temp0, "id");
								
								if (!(json_is_string(id)))
								{
									
									return -1;  //the id value was not 
								}
								else
								{
									
									json_unpack(id,"s",&text); //this unpacks the value right into value
									
									/*This is where you output the print functions */
									
									//~ puts(text); //debug 
									
									idVar = (uint) atoi(text); //This is the id value, this value will be used too print out data for print
									
									//~ fprintf(*fp,"%s[%d];\n",NAME_CREAL_NODE_STRUCTURE,idVar); //this is where we start parsing values //this is debug for now
									
									}//end of else
								
								/* now we are parsing the objects, make sure to use for loop iterations*/ 
									
									properties = json_object_get(temp0, "properties");
									
								
									if(!(json_is_object(properties)))
									{
										
										//~ return -1; //the properties objects are not formatted right, they need to be corrected
									}
									else
									{
										numOfPropertiesInNode = (uint ) json_object_size(properties); //assigning the amount of properties in the node
										
										//~ fprintf(*fp,"%s[%d].numberOfProperties = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,numOfPropertiesInNode); //we are assigning how much creal properties are in the node
										
										iter = json_object_iter(properties); //this is needed, the golden ticket
										
										while(iter)
										{
											key = json_object_iter_key(iter);
											temp1 = json_object_iter_value(iter);
												
											if(!(json_is_object(temp1)))
												{												
												
														//~ return -1; //this happens when json data is all messed up
												
												}
												else
												{
													propVar = atoi(key); //hopefully this is good
													//~ printf("The array index of the property value of this node is : %d, it is connected to %d\n ",propertyCounter, propVar); //debug
													//~ puts(key); //debug
													//stuff goes here
													
													//~ fprintf(*fp,"%s[%d].properties[%d] = &%s[%d];\n",NAME_CREAL_NODE_STRUCTURE,idVar,propertyCounter,NAME_CREAL_PROP_STRUCTURE,propVar); //we are assigning how much creal properties are in the node
													
													propertyCounter++; //increase the property counter
													
													
													type =	json_object_get(temp1,"type");
													if(!(json_is_string(type)))
													{
														
														//~ return -1; //this portion is not a string at all
													
													}
													else
													{
														json_unpack(type,"s",&text); //this unpacks the value right into value
																if (strcmp(text, "number") == 0)
																{
																		//making sure we only get numerical data from Json
																		
																		value =	json_object_get(temp1,"value"); //getting the value from the data 
																		
																		json_unpack(value,"s",&text); //this unpacks the value right into value
																		
																		//assign value, this helps us take away this will have to be taken away later
																		tempPropVar0 = atof(text);
																		//~ fprintf(*fp,"%s[%d] = %f;\n",NAME_CREAL_PROP_STRUCTURE,propVar,tempPropVar0); //we are assigning how much creal properties are in the node
																		//~ puts(text);
																	
																}
																
														//this is where we do something with the type
														
														
														
													}
													
												}
												
										   iter = json_object_iter_next(properties, iter); //this iterates through the json data	
										}
								
									}//end of else
											
								
								
								
								/* These are were the actions take place */ 
								actions = json_object_get(temp0, "action list");
								
								if(!(json_is_object(actions)))
								{
									
									//~ return -1; //something is wrong with the action stuff
								}
								else
								{
									//Actions are taking place right here 
									
										numOfActionsInNode = (uint) json_object_size(actions);
										
										//~ fprintf(*fp,"%s[%d].numberOfActions = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,numOfActionsInNode); //we are assigning how much creal properties are in the node
										
										iter = json_object_iter(actions); //this is needed, the golden ticket
										
										while(iter)
										{
											
											//Now this is set up to take the actions and conditions
											key = json_object_iter_key(iter);
											
											temp1 = json_object_iter_value(iter);
											
											if(!(json_is_object(temp1)))
												{												
												
														//~ return -1; //this happens when json data is all messed up
												
												}
												else
												{
													/* This is where all the action processing taking place */
											
													//~ puts(key); //debug
													actVar = (uint) atoi(key); //add the action index
													
													/*Changed from reverse reference to a regular reference */ 
													
													
													//~ fprintf(*fp,"%s[%d] = &%s[%d].actionList[%d];\n",NAME_CREAL_ACT_STRUCTURE,actVar,NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter); //we are assigning how much creal properties are in the node
													//~ fprintf(*fp,"%s[%d] = &%s[%d].actionList[%d];\n",NAME_CREAL_ACT_STRUCTURE,actVar,NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter); //we are assigning how much creal properties are in the node
												
													//below is the type of format that will be used for references with creal actions
													//~ fprintf(*fp, "%s[%d].actionsList[%d] = &%s[%d];\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,NAME_CREAL_ACT_STRUCTURE,actVar);
													
													/* Assigning the action id to the original array of structures*/
													
													//~ fprintf(*fp,"%s[%d].actionsList[%d].alID = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,actVar); //we are assigning how much creal properties are in the node
													//this is how we are going to set up the action right here
													//~ fprintf(*fp,"%s[%d].alID = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,actVar); //we are assigning how much creal properties are in the node
													
													
													
													
												
												//Iterate  through everything in the action list right here, get all of the elements
												
													
												//the actual actions right here 
												actionItems = json_object_get(temp1, "actions");
												
												if(!(json_is_array(actionItems)))
												{												
													//right here we should return zero for the number of action items
													//~ puts("This (Action) is not an object you clod!"); //this is bead
												}
												else
												{
														//the number of elements right here
														numOfActionItems = (uint) json_array_size(actionItems); //remember these actions are arrays
														//~ fprintf(*fp,"%s[%d] = &%s[%d].actionList[%d].numberOfActions = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,numOfActionItems); //we are assigning how much creal properties are in the node
														
														//~ fprintf(*fp,"%s[%d].actionlist[%d].numberOfActionItems = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,numOfActionItems); //this is where we are assigning the number of action items
														//~ fprintf(*fp,"%s[%d].numberOfActionItems = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,numOfActionItems); //THIS HAS BEEN REDONE
														//RIGHT HERE ,NAME_CREAL_ACT_STRUCTURE,actVar,actVar
														//~ insideActionBlockIncrementer
														for (insideActionBlockIncrementer = 0; insideActionBlockIncrementer < numOfActionItems; insideActionBlockIncrementer++)
														{
															
															//Actions!
															
															/* This right here is for iterating into the action items */
															
															tempForActionAndConditions = json_array_get(actionItems, insideActionBlockIncrementer); //this is used temporary to get the value for the object
															//~ "action_type" //unpack right here
																	
															//For action type
															tempForActionAndConditionsObjects = json_object_get(tempForActionAndConditions, "action_type");
													
															if(!(json_is_string(tempForActionAndConditionsObjects)))
																{										
																	//Do an action here to signify that the condition is null		
																		puts("OH BOY"); //not good	
													
																}
																
															//We want to unpack a string variable here
															json_unpack(tempForActionAndConditionsObjects,"s",&text3); //this unpacks the value right into value
															
															//~ puts(text3);
															
															actionType = (uint) atoi(text3); //getting the value of the action_type
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].actions[%d].actionType = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,actionType); //this is where we are assigning the number of action items	
															//~ fprintf(*fp,"%s[%d].actions[%d].actionType = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,actionCounter,insideActionBlockIncrementer,actionType); //this is where we are assigning the number of action items	
															
															//changed 5/20/13
															//~ fprintf(*fp,"%s[%d].actions[%d].actionType = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,actionType); //this is where we are assigning the number of action items	
															
															
															
															//For target ID 
															tempForActionAndConditionsObjects = json_object_get(tempForActionAndConditions, "target_id");
													
															if(!(json_is_string(tempForActionAndConditionsObjects)))
																{												
																		puts("OH BOY"); //not good	
													
																}
																
															//We want to unpack a string variable here
															json_unpack(tempForActionAndConditionsObjects,"s",&text3); //this unpacks the value right into value
															
															//~ puts(text3);
															
															targetID = (uint) atoi(text3); //getting the value of target_id
															
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].actions[%d].targetID = %d ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,targetID); //this is where we are assigning the number of action items	
															
															//Changed 5/20/13
															//~ fprintf(*fp,"%s[%d].actions[%d].targetID = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,targetID); //this is where we are assigning the number of action items	
															//~ fprintf(*fp,"%s[%d].actionlist[%d].actions[%d].targetID = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,tempPropVar1); //this is where we are assigning the number of action items
															
															
															//For target value
															tempForActionAndConditionsObjects = json_object_get(tempForActionAndConditions, "target_value");
															
															if(!(json_is_string(tempForActionAndConditionsObjects)))
																{												
																		puts("OH BOY"); //not good	
													
																}
																
															//We want to unpack a string variable here
															json_unpack(tempForActionAndConditionsObjects,"s",&text3); //this unpacks the value right into value
															
															
															
															//~ targetValue = (creal_t) atof(text3); //getting the value target value
															
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].actions[%d].targetValue = %f ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,targetValue); //this is where we are assigning the number of action items	
															
															//Changed 5/20/13
															//~ fprintf(*fp,"%s[%d].actions[%d].targetValue = %f;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,targetValue); //this is where we are assigning the number of action items	
															
															
															/* Added 6/3/13... Making sure we make functions */
															/* //~ actionFunction_AID_INCREMENTOR() - This right here is the action function */
															/*
															 * THIS IS WHERE WE begin to write out the custome functions for actions  
															 * 
															 * //~ fprintf(*fp,"%s[%d].actions[%d]->actionFunction = &actionFunction_%d_%d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,actVar,insideActionBlockIncrementer); //this is where we are assigning the number of action items	
															 *  */
															
															
															//~ puts(text3);
															
															
																memset(tempStringBuffer,0,2048); //set the memory to zero right here
																memset(tempStringBuffer2,0,2048); //set the memory to zero right here
																sprintf(tempStringBuffer2,"%s;",text3);
																writeToPipe(inputPipePath,tempStringBuffer2);
																readFromPipe(outputPipePath,tempStringBuffer);
																fprintf(*fp,"creal_t actionFunction_%d_%d(void)\n{\n return %s;\n}\n",actVar,insideActionBlockIncrementer,tempStringBuffer); //this is where we are assigning the number of action items	
															
															 //~ puts(tempStringBuffer); //debugging
															
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].actions[%d].targetValue = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,tempPropVar1); //this is where we are assigning the number of action items
																
																
															//~ fprintf(*fp,"%s[%d].actionlist[%d].numberOfActionItems = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,numOfActionItems); //this is where we are assigning the number of action items
															
														}
															//~ fprintf(*fp,"Number of Action Items %d \n",numOfActionItems); //we are assigning how much creal properties are in the node
													
													
												}//end of the else statement
												
												
												//Good work
													
												
												
												//also iterate through the conditions
												conditions = json_object_get(temp1, "conditions");
												
												if(!(json_is_array(conditions)))
												{												
													//right here we should return zero for the number of conditions 
													//~ puts("This (Condition) is not an object you clod!"); //this is bead
												}
												else
												{
													
								
													//the number of elements right here
								
													numOfConditionsInNode = (uint) json_array_size(conditions); //remember these conditions are arrays
													
													//Changed 5/20/13
													//~ fprintf(*fp,"%s[%d].numberOfConditions = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,numOfConditionsInNode); //this is where we are assigning the number of conditions within an action list
													
													//~ fprintf(*fp,"%s[%d].numberOfActionItems = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,numOfActionItems); //THIS HAS BEEN REDONE
													
													for (insideActionBlockIncrementer = 0; insideActionBlockIncrementer < numOfConditionsInNode; insideActionBlockIncrementer++)
														{
															
															//conditions!!
															
															
															/* This right here is for iterating into the action items */
															
															tempForActionAndConditions = json_array_get(conditions, insideActionBlockIncrementer); //this is used temporary to get the value for the object
															//For the field of the conditions
															tempForActionAndConditionsObjects = json_object_get(tempForActionAndConditions, "field");
													
															if(!(json_is_string(tempForActionAndConditionsObjects)))
																{												
																		puts("OH BOY"); //not good	
													
																}
																
															//We want to unpack a string variable here
															json_unpack(tempForActionAndConditionsObjects,"s",&text3); //this unpacks the value right into value
															
															//~ puts(text3);
															field = (uint) atoi(text3); //getting the value of the action_type
															
															//Changed 5/20/13
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].conditions[%d].field = %d ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,field); //this is where we are assigning the number of action items	
															//~ fprintf(*fp,"%s[%d].conditions[%d].field = %d ;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,field); //this is the new shit
																
																
															//For the relation of the conditions
															tempForActionAndConditionsObjects = json_object_get(tempForActionAndConditions, "relation");
													
															if(!(json_is_string(tempForActionAndConditionsObjects)))
																{												
																		puts("OH BOY"); //not good	
													
																}
																
															//We want to unpack a string variable here
															json_unpack(tempForActionAndConditionsObjects,"s",&text3); //this unpacks the value right into value
															
															//~ puts(text3);
															relation = (uint) atoi(text3); //getting the value of the action_type
															
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].conditions[%d].relationType = %d ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,relation); //this is where we are assigning the number of action items	
															//~ fprintf(*fp,"%s[%d].actionlist[%d].conditions[%d].relationType = %d ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,relation); //this is where we are assigning the number of action items	
															//~ fprintf(*fp,"%s[%d].conditions[%d].relationType = %d ;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,relation); //this is the new shit
															//For the value of the conditions
															tempForActionAndConditionsObjects = json_object_get(tempForActionAndConditions, "value");
													
															if(!(json_is_string(tempForActionAndConditionsObjects)))
																{												
																		puts("OH BOY"); //not good	
													
																}
																
															//We want to unpack a string variable here
															json_unpack(tempForActionAndConditionsObjects,"s",&text3); //this unpacks the value right into value
															
															//~ puts(text3);
															
															conditionsValue = (creal_t) atof(text3); //getting the value of the action_type
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].conditions[%d].value = %f ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,conditionsValue); //this is where we are assigning the number of action items	
															//~ fprintf(*fp,"%s[%d].actionlist[%d].conditions[%d].value = %f ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,conditionsValue); //this is where we are assigning the number of action items	
															//changed 5/20/13
															//~ fprintf(*fp,"%s[%d].conditions[%d].value = %f ;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,conditionsValue); //this is the new shit
															
															
																memset(tempStringBuffer,0,2048); //set the memory to zero right here
																memset(tempStringBuffer2,0,2048); //set the memory to zero right here
																sprintf(tempStringBuffer2,"%s;",text3);
																writeToPipe(inputPipePath,tempStringBuffer2);
																readFromPipe(outputPipePath,tempStringBuffer);
																fprintf(*fp,"creal_t conditionFunction_%d_%d(void)\n{\n return %s;\n}\n",actVar,insideActionBlockIncrementer,tempStringBuffer); //this is where we are assigning the number of action items	
																
																//~ puts(tempStringBuffer);
															
															//~ puts("This right here is the incrementer for conditions");
															//~ fprintf(*fp,"%s[%d].actionlist[%d].numberOfActionItems = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,numOfActionItems); //this is where we are assigning the number of action items
															
															/* 6/4/13 -=This stuff is added right here for now=- 
															 * conditionFunction_AID_INCREMENTOR() //
															 * ->actionFunction = &actionFunction_%d_%d
															 * */
															//~ fprintf(*fp,"%s[%d].conditions[%d]->conditionFunction = &conditionFunction_%d_%d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,actVar,insideActionBlockIncrementer); //this is the new shit
														}
														
								
													//~ fprintf(*fp,"Number of Conditions %d \n",numOfConditionsInNode); //we are assigning how much creal properties are in the node
														
														
												}//end of the else statement
												

												
												//Make sure to increment the counter!! or else it all goes astray!
												
												actionCounter++; //increment the action counter
												
													
												}
												
											iter = json_object_iter_next(actions, iter); //this iterates through the json data
											
										
										}
										
								}//end of else statement
								
								/* This is where we are going to work on the conditions
								conditions = json_object_get(temp0, "conditions");
								
								if(!(json_is_object(conditions)))
								{
									
									//~ return -1; //something is wrong with the action stuff
								}
								else
								{
									numOfActionsInNode = (uint) json_object_size(actions);
									
								}//end of else statement
								
								This needs to be more thought out of right here
								
								*/
								
						}
				
			}
	
			quitCtcpd(inputPipePath,outputPipePath); //this quits the the ctcpd daemon	
			
			unlink(inputPipePath); //make sure to unlink the files
			
			unlink(outputPipePath); //make sure to unlink the files
	
	//this is the end of the function
	return 1;
}


int writeOutActionConditionFunctionsPrototypes(json_t **root, FILE **fp)
{
	
	
	
	//This function makes prototypes for the functions declared in this program 
	uint numOfCrealNodes  = (uint) json_array_size(*root);
	const char *key, *key0;
	key = key0  = NULL; //hehe
	json_t *jsonPropertiesList, *temp0, *temp1, *value, *type, *id, *properties, *actions, *conditions, 
	*actionItems, *tempForActionAndConditions, *tempForActionAndConditions2, *tempForActionAndConditionsObjects; //json properties, actions, etc
	void *iter, *iter0; //used for iteration
	char *text = NULL; //this is used to slurp all of the json data into a string
	char *text2 = NULL; //this is used to slurp all of the json data into a string
	char *text3 = NULL; //this is used to slurp all of the json data into a string
	char *text4 = NULL; //this is used to slurp all of the json data into a string
	int stringLength = 0;
	int check = 0;
	//~ int count = (int) json_array_size(*root); //the count of the elements, for multithreading
	int i = 0;
	int n = 0;
	int k, p, o, m, c, t; //these are extras needed
	k = p = o = m = c = t = 0; //setting the counter variables
	
	int insideActionBlockIncrementer = 0; //this is used in for iteration inside of the action block
	
	int propertyCounter = 0;
	int actionCounter = 0;
	int actionItemCounter = 0;
	int conditionCounter = 0;
	/* setting up some temporary variables unsigned integers*/
	
	creal_t tempPropVar0, tempPropVar1, tempPropVar2, targetValue, conditionsValue;
	tempPropVar0 = tempPropVar1 = tempPropVar2 = targetValue = conditionsValue = 0;
	
	uint tempVar, idVar, propVar, actVar, condVar, numOfPropertiesInNode, numOfActionsInNode, numOfConditionsInNode, numOfActionItems, actionType, targetID, field, relation;
	
	tempVar = idVar = propVar = actVar = condVar = numOfPropertiesInNode = numOfConditionsInNode = numOfActionItems = relation = 0; //assigning everything to zero
	
	
	static char inputPipePath[256], outputPipePath[256]; //these are static variable so we don't allocate too much on the damn stack
	
	char ctcpdExecutionString[256]; //this is used to execute ctcpd with its arguments
	char tempStringBuffer[2048]; //this is a temporary string buffer
	char tempStringBuffer2[2048]; //this is a temporary string buffer
	
	
	//~ FILE *inputPipeFd, *outputPipeFd;	
	//~ FILE *inputFd, *outputFd;	
	//~ 
	//~ inputFd = outputFd = inputPipeFd = outputPipeFd = NULL; //set everything to zero right here
	//~ 
//~ 
	//~ 
	//~ sprintf(inputPipePath,"%s%d",PIPE_IN_FILE,crealPid); //creating a path for the pipe file
	//~ sprintf(outputPipePath,"%s%d",PIPE_OUT_FILE,crealPid); //creating a path for the pipe file
		//~ 
		 //~ /* testing the making of the files */
			//~ outputFd = fopen(outputPipePath, "wr");
		//~ 
			//~ inputFd = fopen(inputPipePath, "wr");
	//~ 
		//~ 
	//~ 
	//~ if (!createPipeFile(inputPipePath)) //creating the pipe input file
					//~ {
						//~ perror("Could not create an input Pipe file for processing! Error!\n"); //exit out of this function if everything goes really bad!!		
						//~ return -1; //get out of here, things went bad
					//~ }
	//~ if (!createPipeFile(outputPipePath)) //creating the pipe input file
					//~ {
						//~ perror("Could not create an output Pipe file for processing! Error!\n"); //exit out of this function if everything goes really bad!!		
						//~ return -1; //get out of here, things went bad
					//~ }
	//~ 
	//~ 
	//~ sprintf(ctcpdExecutionString,"ctcpd %s %s", inputPipePath, outputPipePath); //this is used to create the command for ctcpd
	//~ 
	//~ system(ctcpdExecutionString); //start the parsing agent right here				
	
	//~ sleep(1); //we need this right here to make sure everything parses properly
	
	
	
	/*
	 * 
	//~ puts(tempStringBuffer);
	inputPipeFd = fopen(inputPipePath,"w"); //input pipe open
	
	fprintf(inputPipeFd,"%s;","6+6"); //this is the default thing we do for writing the functions
	fflush(inputPipeFd);
	fclose(inputPipeFd);
	
	outputPipeFd = fopen(outputPipePath,"r"); //output pipe file
	
	fgets(tempStringBuffer,2048,outputPipeFd); //grabs the file data
	
	
	puts(tempStringBuffer); //output the temporary string buffer
	
	fflush(outputPipeFd);
	fclose(outputPipeFd);
	
	 
	 
	 For debug purposes
	//~ puts(inputPipePath);
	//~ puts(outputPipePath);
	* 
	* 
	*/ 
	
	
	
					
	
	
	
	
			for (i = 0; i < numOfCrealNodes; i++)
			{
				actionItemCounter = conditionCounter = actionCounter = propertyCounter = 0; //reset the property, and action counters of the creal node
			
				  //~ 0;
				  
				temp0 = json_array_get(*root, i); //this is used temporary to get the value for the object

					 if(!(json_is_object(temp0)))
						{
							
							return -1; //json data is messed up and it is not an object!
						}
						else
						{
								id = json_object_get(temp0, "id");
								
								if (!(json_is_string(id)))
								{
									
									return -1;  //the id value was not 
								}
								else
								{
									
									json_unpack(id,"s",&text); //this unpacks the value right into value
									
									/*This is where you output the print functions */
									
									//~ puts(text); //debug 
									
									idVar = (uint) atoi(text); //This is the id value, this value will be used too print out data for print
									
									//~ fprintf(*fp,"%s[%d];\n",NAME_CREAL_NODE_STRUCTURE,idVar); //this is where we start parsing values //this is debug for now
									
									}//end of else
								
								/* now we are parsing the objects, make sure to use for loop iterations*/ 
									
									properties = json_object_get(temp0, "properties");
									
								
									if(!(json_is_object(properties)))
									{
										
										//~ return -1; //the properties objects are not formatted right, they need to be corrected
									}
									else
									{
										numOfPropertiesInNode = (uint ) json_object_size(properties); //assigning the amount of properties in the node
										
										//~ fprintf(*fp,"%s[%d].numberOfProperties = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,numOfPropertiesInNode); //we are assigning how much creal properties are in the node
										
										iter = json_object_iter(properties); //this is needed, the golden ticket
										
										while(iter)
										{
											key = json_object_iter_key(iter);
											temp1 = json_object_iter_value(iter);
												
											if(!(json_is_object(temp1)))
												{												
												
														//~ return -1; //this happens when json data is all messed up
												
												}
												else
												{
													propVar = atoi(key); //hopefully this is good
													//~ printf("The array index of the property value of this node is : %d, it is connected to %d\n ",propertyCounter, propVar); //debug
													//~ puts(key); //debug
													//stuff goes here
													
													//~ fprintf(*fp,"%s[%d].properties[%d] = &%s[%d];\n",NAME_CREAL_NODE_STRUCTURE,idVar,propertyCounter,NAME_CREAL_PROP_STRUCTURE,propVar); //we are assigning how much creal properties are in the node
													
													propertyCounter++; //increase the property counter
													
													
													type =	json_object_get(temp1,"type");
													if(!(json_is_string(type)))
													{
														
														//~ return -1; //this portion is not a string at all
													
													}
													else
													{
														json_unpack(type,"s",&text); //this unpacks the value right into value
																if (strcmp(text, "number") == 0)
																{
																		//making sure we only get numerical data from Json
																		
																		value =	json_object_get(temp1,"value"); //getting the value from the data 
																		
																		json_unpack(value,"s",&text); //this unpacks the value right into value
																		
																		//assign value, this helps us take away this will have to be taken away later
																		tempPropVar0 = atof(text);
																		//~ fprintf(*fp,"%s[%d] = %f;\n",NAME_CREAL_PROP_STRUCTURE,propVar,tempPropVar0); //we are assigning how much creal properties are in the node
																		//~ puts(text);
																	
																}
																
														//this is where we do something with the type
														
														
														
													}
													
												}
												
										   iter = json_object_iter_next(properties, iter); //this iterates through the json data	
										}
								
									}//end of else
											
								
								
								
								/* These are were the actions take place */ 
								actions = json_object_get(temp0, "action list");
								
								if(!(json_is_object(actions)))
								{
									
									//~ return -1; //something is wrong with the action stuff
								}
								else
								{
									//Actions are taking place right here 
									
										numOfActionsInNode = (uint) json_object_size(actions);
										
										//~ fprintf(*fp,"%s[%d].numberOfActions = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,numOfActionsInNode); //we are assigning how much creal properties are in the node
										
										iter = json_object_iter(actions); //this is needed, the golden ticket
										
										while(iter)
										{
											
											//Now this is set up to take the actions and conditions
											key = json_object_iter_key(iter);
											
											temp1 = json_object_iter_value(iter);
											
											if(!(json_is_object(temp1)))
												{												
												
														//~ return -1; //this happens when json data is all messed up
												
												}
												else
												{
													/* This is where all the action processing taking place */
											
													//~ puts(key); //debug
													actVar = (uint) atoi(key); //add the action index
													
													/*Changed from reverse reference to a regular reference */ 
													
													
													//~ fprintf(*fp,"%s[%d] = &%s[%d].actionList[%d];\n",NAME_CREAL_ACT_STRUCTURE,actVar,NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter); //we are assigning how much creal properties are in the node
													//~ fprintf(*fp,"%s[%d] = &%s[%d].actionList[%d];\n",NAME_CREAL_ACT_STRUCTURE,actVar,NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter); //we are assigning how much creal properties are in the node
												
													//below is the type of format that will be used for references with creal actions
													//~ fprintf(*fp, "%s[%d].actionsList[%d] = &%s[%d];\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,NAME_CREAL_ACT_STRUCTURE,actVar);
													
													/* Assigning the action id to the original array of structures*/
													
													//~ fprintf(*fp,"%s[%d].actionsList[%d].alID = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,actVar); //we are assigning how much creal properties are in the node
													//this is how we are going to set up the action right here
													//~ fprintf(*fp,"%s[%d].alID = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,actVar); //we are assigning how much creal properties are in the node
													
													
													
													
												
												//Iterate  through everything in the action list right here, get all of the elements
												
													
												//the actual actions right here 
												actionItems = json_object_get(temp1, "actions");
												
												if(!(json_is_array(actionItems)))
												{												
													//right here we should return zero for the number of action items
													//~ puts("This (Action) is not an object you clod!"); //this is bead
												}
												else
												{
														//the number of elements right here
														numOfActionItems = (uint) json_array_size(actionItems); //remember these actions are arrays
														//~ fprintf(*fp,"%s[%d] = &%s[%d].actionList[%d].numberOfActions = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,numOfActionItems); //we are assigning how much creal properties are in the node
														
														//~ fprintf(*fp,"%s[%d].actionlist[%d].numberOfActionItems = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,numOfActionItems); //this is where we are assigning the number of action items
														//~ fprintf(*fp,"%s[%d].numberOfActionItems = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,numOfActionItems); //THIS HAS BEEN REDONE
														//RIGHT HERE ,NAME_CREAL_ACT_STRUCTURE,actVar,actVar
														//~ insideActionBlockIncrementer
														for (insideActionBlockIncrementer = 0; insideActionBlockIncrementer < numOfActionItems; insideActionBlockIncrementer++)
														{
															
															//Actions!
															
															/* This right here is for iterating into the action items */
															
															tempForActionAndConditions = json_array_get(actionItems, insideActionBlockIncrementer); //this is used temporary to get the value for the object
															//~ "action_type" //unpack right here
																	
															//For action type
															tempForActionAndConditionsObjects = json_object_get(tempForActionAndConditions, "action_type");
													
															if(!(json_is_string(tempForActionAndConditionsObjects)))
																{										
																	//Do an action here to signify that the condition is null		
																		puts("OH BOY"); //not good	
													
																}
																
															//We want to unpack a string variable here
															json_unpack(tempForActionAndConditionsObjects,"s",&text3); //this unpacks the value right into value
															
															//~ puts(text3);
															
															actionType = (uint) atoi(text3); //getting the value of the action_type
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].actions[%d].actionType = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,actionType); //this is where we are assigning the number of action items	
															//~ fprintf(*fp,"%s[%d].actions[%d].actionType = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,actionCounter,insideActionBlockIncrementer,actionType); //this is where we are assigning the number of action items	
															
															//changed 5/20/13
															//~ fprintf(*fp,"%s[%d].actions[%d].actionType = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,actionType); //this is where we are assigning the number of action items	
															
															
															
															//For target ID 
															tempForActionAndConditionsObjects = json_object_get(tempForActionAndConditions, "target_id");
													
															if(!(json_is_string(tempForActionAndConditionsObjects)))
																{												
																		puts("OH BOY"); //not good	
													
																}
																
															//We want to unpack a string variable here
															json_unpack(tempForActionAndConditionsObjects,"s",&text3); //this unpacks the value right into value
															
															//~ puts(text3);
															
															targetID = (uint) atoi(text3); //getting the value of target_id
															
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].actions[%d].targetID = %d ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,targetID); //this is where we are assigning the number of action items	
															
															//Changed 5/20/13
															//~ fprintf(*fp,"%s[%d].actions[%d].targetID = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,targetID); //this is where we are assigning the number of action items	
															//~ fprintf(*fp,"%s[%d].actionlist[%d].actions[%d].targetID = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,tempPropVar1); //this is where we are assigning the number of action items
															
															
															//For target value
															tempForActionAndConditionsObjects = json_object_get(tempForActionAndConditions, "target_value");
															
															if(!(json_is_string(tempForActionAndConditionsObjects)))
																{												
																		puts("OH BOY"); //not good	
													
																}
																
															//We want to unpack a string variable here
															json_unpack(tempForActionAndConditionsObjects,"s",&text3); //this unpacks the value right into value
															
															
															
															//~ targetValue = (creal_t) atof(text3); //getting the value target value
															
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].actions[%d].targetValue = %f ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,targetValue); //this is where we are assigning the number of action items	
															
															//Changed 5/20/13
															//~ fprintf(*fp,"%s[%d].actions[%d].targetValue = %f;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,targetValue); //this is where we are assigning the number of action items	
															
															
															/* Added 6/3/13... Making sure we make functions */
															/* //~ actionFunction_AID_INCREMENTOR() - This right here is the action function */
															/*
															 * THIS IS WHERE WE begin to write out the custome functions for actions  
															 * 
															 * //~ fprintf(*fp,"%s[%d].actions[%d]->actionFunction = &actionFunction_%d_%d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,actVar,insideActionBlockIncrementer); //this is where we are assigning the number of action items	
															 *  */
															
															
															//~ puts(text3);
															
															
																//~ memset(tempStringBuffer,0,2048); //set the memory to zero right here
																//~ memset(tempStringBuffer2,0,2048); //set the memory to zero right here
																//~ sprintf(tempStringBuffer2,"%s;",text3);
																//~ writeToPipe(inputPipePath,tempStringBuffer2);
																//~ readFromPipe(outputPipePath,tempStringBuffer);
																fprintf(*fp,"creal_t actionFunction_%d_%d(void);\n",actVar,insideActionBlockIncrementer); //this is where we are assigning the number of action items	
															
															 //~ puts(tempStringBuffer); //debugging
															
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].actions[%d].targetValue = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,tempPropVar1); //this is where we are assigning the number of action items
																
																
															//~ fprintf(*fp,"%s[%d].actionlist[%d].numberOfActionItems = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,numOfActionItems); //this is where we are assigning the number of action items
															
														}
															//~ fprintf(*fp,"Number of Action Items %d \n",numOfActionItems); //we are assigning how much creal properties are in the node
													
													
												}//end of the else statement
												
												
												//Good work
													
												
												
												//also iterate through the conditions
												conditions = json_object_get(temp1, "conditions");
												
												if(!(json_is_array(conditions)))
												{												
													//right here we should return zero for the number of conditions 
													//~ puts("This (Condition) is not an object you clod!"); //this is bead
												}
												else
												{
													
								
													//the number of elements right here
								
													numOfConditionsInNode = (uint) json_array_size(conditions); //remember these conditions are arrays
													
													//Changed 5/20/13
													//~ fprintf(*fp,"%s[%d].numberOfConditions = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,numOfConditionsInNode); //this is where we are assigning the number of conditions within an action list
													
													//~ fprintf(*fp,"%s[%d].numberOfActionItems = %d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,numOfActionItems); //THIS HAS BEEN REDONE
													
													for (insideActionBlockIncrementer = 0; insideActionBlockIncrementer < numOfConditionsInNode; insideActionBlockIncrementer++)
														{
															
															//conditions!!
															
															
															/* This right here is for iterating into the action items */
															
															tempForActionAndConditions = json_array_get(conditions, insideActionBlockIncrementer); //this is used temporary to get the value for the object
															//For the field of the conditions
															tempForActionAndConditionsObjects = json_object_get(tempForActionAndConditions, "field");
													
															if(!(json_is_string(tempForActionAndConditionsObjects)))
																{												
																		puts("OH BOY"); //not good	
													
																}
																
															//We want to unpack a string variable here
															json_unpack(tempForActionAndConditionsObjects,"s",&text3); //this unpacks the value right into value
															
															//~ puts(text3);
															field = (uint) atoi(text3); //getting the value of the action_type
															
															//Changed 5/20/13
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].conditions[%d].field = %d ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,field); //this is where we are assigning the number of action items	
															//~ fprintf(*fp,"%s[%d].conditions[%d].field = %d ;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,field); //this is the new shit
																
																
															//For the relation of the conditions
															tempForActionAndConditionsObjects = json_object_get(tempForActionAndConditions, "relation");
													
															if(!(json_is_string(tempForActionAndConditionsObjects)))
																{												
																		puts("OH BOY"); //not good	
													
																}
																
															//We want to unpack a string variable here
															json_unpack(tempForActionAndConditionsObjects,"s",&text3); //this unpacks the value right into value
															
															//~ puts(text3);
															relation = (uint) atoi(text3); //getting the value of the action_type
															
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].conditions[%d].relationType = %d ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,relation); //this is where we are assigning the number of action items	
															//~ fprintf(*fp,"%s[%d].actionlist[%d].conditions[%d].relationType = %d ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,relation); //this is where we are assigning the number of action items	
															//~ fprintf(*fp,"%s[%d].conditions[%d].relationType = %d ;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,relation); //this is the new shit
															//For the value of the conditions
															tempForActionAndConditionsObjects = json_object_get(tempForActionAndConditions, "value");
													
															if(!(json_is_string(tempForActionAndConditionsObjects)))
																{												
																		puts("OH BOY"); //not good	
													
																}
																
															//We want to unpack a string variable here
															json_unpack(tempForActionAndConditionsObjects,"s",&text3); //this unpacks the value right into value
															
															//~ puts(text3);
															
															conditionsValue = (creal_t) atof(text3); //getting the value of the action_type
															
															//~ fprintf(*fp,"%s[%d].actionlist[%d].conditions[%d].value = %f ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,conditionsValue); //this is where we are assigning the number of action items	
															//~ fprintf(*fp,"%s[%d].actionlist[%d].conditions[%d].value = %f ;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,insideActionBlockIncrementer,conditionsValue); //this is where we are assigning the number of action items	
															//changed 5/20/13
															//~ fprintf(*fp,"%s[%d].conditions[%d].value = %f ;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,conditionsValue); //this is the new shit
															
															
																//~ memset(tempStringBuffer,0,2048); //set the memory to zero right here
																//~ memset(tempStringBuffer2,0,2048); //set the memory to zero right here
																//~ sprintf(tempStringBuffer2,"%s;",text3);
																//~ writeToPipe(inputPipePath,tempStringBuffer2);
																//~ readFromPipe(outputPipePath,tempStringBuffer);
																fprintf(*fp,"creal_t conditionFunction_%d_%d(void);\n",actVar,insideActionBlockIncrementer); //this is where we are assigning the number of action items	
																
																//~ puts(tempStringBuffer);
															
															//~ puts("This right here is the incrementer for conditions");
															//~ fprintf(*fp,"%s[%d].actionlist[%d].numberOfActionItems = %d;\n",NAME_CREAL_NODE_STRUCTURE,idVar,actionCounter,numOfActionItems); //this is where we are assigning the number of action items
															
															/* 6/4/13 -=This stuff is added right here for now=- 
															 * conditionFunction_AID_INCREMENTOR() //
															 * ->actionFunction = &actionFunction_%d_%d
															 * */
															//~ fprintf(*fp,"%s[%d].conditions[%d]->conditionFunction = &conditionFunction_%d_%d;\n",NAME_CREAL_ACT_STRUCTURE,actVar,insideActionBlockIncrementer,actVar,insideActionBlockIncrementer); //this is the new shit
														}
														
								
													//~ fprintf(*fp,"Number of Conditions %d \n",numOfConditionsInNode); //we are assigning how much creal properties are in the node
														
														
												}//end of the else statement
												

												
												//Make sure to increment the counter!! or else it all goes astray!
												
												actionCounter++; //increment the action counter
												
													
												}
												
											iter = json_object_iter_next(actions, iter); //this iterates through the json data
											
										
										}
										
								}//end of else statement
								
								/* This is where we are going to work on the conditions
								conditions = json_object_get(temp0, "conditions");
								
								if(!(json_is_object(conditions)))
								{
									
									//~ return -1; //something is wrong with the action stuff
								}
								else
								{
									numOfActionsInNode = (uint) json_object_size(actions);
									
								}//end of else statement
								
								This needs to be more thought out of right here
								
								*/
								
						}
				
			}
	
			//~ quitCtcpd(inputPipePath,outputPipePath); //this quits the the ctcpd daemon	
			//~ 
			//~ unlink(inputPipePath); //make sure to unlink the files
			//~ 
			//~ unlink(outputPipePath); //make sure to unlink the files
	
	//this is the end of the function
	return 1;
}



int quitCtcpd(const char *inputFile, const char *outputFile)
{
	
	
	
			//this function communicates through names pipes to the ctcpd daemon
			
	FILE *inputPipeFd, *outputPipeFd;	
	//~ 
	inputPipeFd = outputPipeFd = NULL; //set everything to zero right here
			
					inputPipeFd = fopen(inputFile,"w"); //input pipe open
					if (inputPipeFd == NULL) { return -1;} //something went wrong right here
					
	
	fprintf(inputPipeFd,"%s;","Q"); //this is the default thing we do for writing the functions
	fflush(inputPipeFd);
	fclose(inputPipeFd);
	
	
	//~ 
	//~ outputPipeFd = fopen(outputFile,"r"); //output pipe file
	//~ if (outputPipeFd == NULL) { return -1;} //something went wrong here
	//~ 
	//~ //grabs the file data
	//~ fflush(outputPipeFd);
	//~ if (fgets(outputString,2048,outputPipeFd) == NULL) { return -1;} //something went wrong here	
//~ 
	//~ fflush(outputPipeFd);
	//~ fclose(outputPipeFd);
	//~ 
	//~ 
	
	return 1;
}


int writeToPipe(const char *path, const char *string)
{
	
		FILE *fp = NULL;
		
	 fp = fopen(path,"w");
	 
	 if (fp == NULL) 
	 {
		 
			return -1;
			
	 }
	 
	 fputs(string,fp);
	 fclose(fp);
	 return 1;
}

int readFromPipe(const char *path, char buffer[])
{
	
		FILE *fp = NULL;
		
	 fp = fopen(path,"r");
	 
	 if (fp == NULL) 
	 {
		 
			return -1;
			
	 }
	 
	 while(fgets(buffer,MAX_PIPE_SIZE,fp) != NULL) {

		}
	 
	 fclose(fp);
	 return 1;
	 
}

int printOutFromCTF(const char *path, FILE **fp)
{
		/* This function right here gets a ctf file 
		 * and prints it out to the code base you wany */ 
		 
		size_t read, len;
		char *line = NULL;
        len = 0;
        
        
		FILE *tempFilePointer = NULL; //get a file pointer

		tempFilePointer = fopen(path,"r");
	 
		 if (tempFilePointer == NULL) 
		 {
			 
				return -1;
				
		 }
	
	 while ((read = getline(&line, &len, tempFilePointer)) != -1) {
               fprintf(*fp,"%s", line);
               
           }

	
	fclose(tempFilePointer);
	free(line);
	return 1;
	
}

