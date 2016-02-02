/*These are Structure Definitions */
#ifndef STRUCT_DEFINITIONS

//Change Made 5/20/13.. double prototyping a function
//~ int printOutStaticActionList(uint actionCount, uint conditionCount,  FILE **stream ); //this is used to finish the 

#define STRUCT_DEFINITIONS /* Make sure we don't have any damn duplicates */

/* The pre action structures */
#define PRE_ACTION_STRUCT "struct pre_actions \n { \n uint aID; \n char actionType; \n uint targetID; \n actionFunctionPointer actionFunction;	\n creal_t targetValue; \n }; \n" 

/* The pre condition list structures */
#define PRE_CONDITIONLIST_STRUCT "struct pre_conditionlist \n { \n uint cID; \n char relationType; \n uint field; \n actionFunctionPointer conditionFunction; \n creal_t value; \n }; \n " 

/* This is the beginning of the main action list */ 
#define PRE_ACTION_LIST "struct pre_actionlist \n { \n uint alID; \n char actionListIsNull; \n char conditionListIsNull; \n uint numberOfActionItems; \n uint numberOfConditions; \n" 

/* This is used to close the structures */  
#define CLOSE_THE_STRUCT_CURLY " \n }; \n " 

/* These might be used to set up the action functions*/  
#define DECLARE_ACTION_FUNC "\n creal_t actionReturnFunction_" 

#define BEGIN_ACTION_FUNC "{\n" 

/* This is suppose to end the action function right here*/ 
#define END_ACTION_FUNC "\n }\n" 

/* This right here is just the bare name for the action function in order to concatenate the stuff for creal*/
#define BARE_ACTION_FUNC_NAME "actionReturnFunction_"

 
 
/* These are to be used for the property update functions within creal*/  
#define DECLARE_PROPUP_FUNC "\n creal_t propertyUpdateFunction_" 

#define BEGIN_PROPUP_FUNC "{\n" 


#define END_PROPUP_FUNC "\n }\n" 


#define BARE_PROPUP_FUNC_NAME "propertyUpdateFunction_"

 
 
/* These right here were just added 5/17/13 */
 #define ACTION_FUNCTION_POINTER "typedef creal_t (*actionFunctionPointer)( void); \n"
 
 #define CREAL_T_TYPE "typedef double creal_t; \n"
 
 #define CREAL_NODE "struct crealNodeStruct crealNodes["
 #define CREAL_PROP "creal_t crealProperties["
 #define CREAL_ACTIONS "struct pre_actionlist crealActions["
 #define CREAL_SUM_VECTORS "creal_t sumVector["
 #define END_CREAL_BRACKET "]"
 
 
 #define CREAL_NODE_STRUCTURE_TOP " \
  struct crealNodeStruct { \n \
	\n \
		/* May need to modify the datatypes */ \n \
		\n \
		uint iD; //this is the id\n \
		uint numberOfProperties; //the number of properties\n \
		uint numberOfActions; //the number of actions \n \
		char notNull; //this is to make sure that the data structure is not null\n \
		"
/* These are used for the creal nodes */
	#define CREAL_NODE_STRUCTURE_PROP_ID "uint pID["		
	#define CREAL_NODE_STRUCTURE_PROPERTIES "creal_t *properties["		
	#define CREAL_NODE_STRUCTURE_ACTIONLIST "struct pre_actionlist  *actionsList["
	#define END_CREAL_NODE_STRUCTURE_BRACKET "]"
	#define END_CREAL_NODE_STRUCTURE_STRUCTDEC "}"



/* These definitions are used for setting up the maxes for the application*/ 
	#define CREAL_MAX_PROPERTIES "#define MAX_PROPERTIES" 
	#define CREAL_MAX_ACTION_LISTS "#define MAX_ACTION_LISTS" 
	#define CREAL_MAX_ACTIONS_AND_CONDITIONS "#define MAX_ACTIONS_AND_CONDITIONS" 
	#define CREAL_MAX_CREAL_NODES "#define MAX_CREAL_NODES" 

		//~ uint pID[5]; //these are the property ids 
		//~ creal_t *properties[5]; //these are the properties right here
		//~ struct pre_actionlist  *actionsList[5]; //this is the actions
		//~ 
					//~ //this is the creal node data structure
	//~ 
//~ }; 
 //~ 
 //~ "
 //~ 
 /* These are the structures for CSVS, they are really for making a format for processing the data faster */
#define CREAL_CSV_STRUCTS "\n \
struct csvNodeDataStruct{ \n \
\n \
			uint nodeNumber; //the number of the node\n \
			uint count; //the count of all the properties\n \
			creal_t *properties; //the properties of the node\n \
\n \
};\n \
\n \
\n \
struct csvStruct\n \
{\n \
\n \
	struct csvNodeDataStruct *nodes;\n \
	uint count; //the count of all the nodes\n \
	long double currentTimeValue;\n \
\n \
};\n \
"
 
/* These are the core functions for writing the nodes */ 
 #define MAIN_CREAL_PROCESSING_FUNCTIONS "\n \
void printOutDataInCrealNodes(struct crealNodeStruct nodes[]); //this will be used to print out all of the junk\n \
 int processActionsAndConditions(struct crealNodeStruct nodes[], creal_t properties[], creal_t returnVector[]); //this is the main function to process the creal nodes\n \
 int processProperties(creal_t returnVector[], creal_t properties[]); //process the properties\n \
 void crealInit(void); //this function will be used to initialize all of the data structures\n \
 /* These functions below are to grab file information, could be placed in a wrapper function */\n \
 \n \
 int grabRangesFromFile(const char *string, uint rangePointer[]);\n \
 int returnLineCountOfFile(const char *string);\n \
 //this function will write out \n \
  \n \
 /* Add these selection function to cerebro */\n \
 int processSelectedActionsAndConditions(struct crealNodeStruct nodes[], creal_t properties[], creal_t returnVector[], uint rangePointer[], uint rangeSize); //this is the main function to process the creal nodes... through a selection!\n \
 void printOutSelectedDataInCrealNodes(struct crealNodeStruct nodes[], uint rangePointer[], uint rangeSize); //this will only utilize selections\n \
\n \
 uint evaluateConditions(struct crealNodeStruct *node, creal_t properties[], uint actionListIndex); //this is ok\n \
 uint processActions(struct crealNodeStruct *node, creal_t properties[], creal_t returnVector[], uint actionListIndex); //this is ok too\n \
 \n \
 int writeOutDataFileFor(struct crealNodeStruct nodes[], uint rangePointer[], uint rangeIndex, long double *currenTime);\n \
 int transferDataFileDataFileIndex(struct crealNodeStruct *node, uint nodeIndex, long double *currenTime); \n \
 int processProperties(creal_t returnVector[], creal_t properties[]); //process the properties\n \
 int writeSelectedDataFile(struct crealNodeStruct *node, uint nodeIndex, uint propIndex, long double *currentTime);\n \
 \n \
 /* These are purely functional types of functions for multiprocessing */\n \
 uint findNumOfProperties(struct crealNodeStruct *node); //this is used to find the number of properties\n \
 uint findNumOfActions(uint rangeIndex, struct crealNodeStruct nodes[]); //this is used to find the number of actions\n \
 uint findNumOfActionItems(uint rangeIndex,uint listIndex, struct crealNodeStruct nodes[]); //this is used to find the number of action items\n \
 uint findNumOfConditions(uint rangeIndex,uint listIndex, struct crealNodeStruct nodes[]); //this is used to find the number of action items\n \
 \n \
 uint returnValueOfProperties(uint rangeIndex, struct crealNodeStruct nodes[], creal_t returnArray[]); //this is used to find the number of properties\n \
 \n \
 uint returnActionsTargetValues(uint rangeIndex,uint listIndex, struct crealNodeStruct nodes[], creal_t returnArray[]); //returns action target values\n \
 uint returnActionsTargetIDs(uint rangeIndex,uint listIndex, struct crealNodeStruct nodes[], uint returnArray[]); //returns actions target ids\n \
 uint returnActionsActionTypes(uint rangeIndex,uint listIndex, struct crealNodeStruct nodes[], uint returnArray[]); //returns action types\n \
 \n \
 uint returnConditionFields(uint rangeIndex,uint listIndex, struct crealNodeStruct nodes[], uint returnArray[]); //returns condition fields\n \
 uint returnConditionRelationTypes(uint rangeIndex,uint listIndex, struct crealNodeStruct nodes[], uint returnArray[]); //returns condition relations\n \
 uint returnConditionValues(uint rangeIndex,uint listIndex, struct crealNodeStruct nodes[], creal_t returnArray[]); //returns condition values\n \
 \n \
"



/* These are misc definitions declared at the beginning of the code base */

#define ETC_BEGINNING_DEFINITIONS "\n \
#define NONRELATION 0 //no action \n \
#define EQUAL 1 //used for addition \n \
#define GREATERTHAN 2 //used for deletion \n \
#define LESSERTHAN 3 //used for changing \n \
#define GREATERTHANEQUALTO 4 //used for maintaining \n \
#define LESSERTHANEQUALTO 5 //used for transporting \n \
#define NOTEQUAL 6 //used for transporting \n \
 \n \
#define NONACTION 0 //no action \n \
#define INCREASE 1 //used for addition \n \
#define DECREASE 2 //used for deletion \n \
#define CHANGE 3 //used for changing \n \
#define MAINTAIN 4 //used for maintaining \n \
#define TRANSPORT 5 //used for transporting \n \
#define NOTEQUAL 6 //used for transporting \n \
 \n \
  #define USAGE \"Input a file with a range, and input a time\" \n \
uint minRange, maxRange, speedAdjustVar; //these are used for taking the maximum and minumum values, also adjusting the speed of the application \n \
long double crealTime, timeIncrementer; //these are used for keeping up with time \n \
uint numOfRange; \n \
long double count; //the counting variable \n \
"
	 

/* This is the main game loop right here */
#define MAIN_GAME_LOOP "\n \
					if (argc <= 2)		\n \
					{ \n \
						\n \
							\n \
								//nothing was entered to start the application correctly\n \
								fputs(USAGE, stderr); //shows application usage	\n \
								exit(EXIT_FAILURE); // exit the application\n \
								\n \
								\n \
					}\n \
\n \
\n \
				//The ranges, add this into cerebro\n \
				uint *rangeArray, range; //for the ranges\n \
				range = (uint) returnLineCountOfFile(argv[1]); //geting the line count of the file\n \
				rangeArray = (uint *) alloca(sizeof(uint) * range); //allocating the array for functions\n \
\n \
				if (grabRangesFromFile(argv[1], rangeArray) < 0)\n \
				{\n \
						\n \
						\n \
							exit(EXIT_FAILURE);\n \
					\n \
				}\n \
				\n \
				\n \
				crealInit(); //initialize the creal properties\n \
				\n \
\n \
	\n \
		crealTime = atoi(argv[2]);\n \
		\n \
\n \
 		for (count = 0; count < crealTime; count++)\n \
 		{\n \
				memset(sumVector,0,1000*sizeof(creal_t)); //set the memory to zero\n \
				\n \
 				srand(time(0)); //allowing the application to generate a random seed\n \
 								\n \
 				processSelectedActionsAndConditions(crealNodes, crealProperties, sumVector,rangeArray, range);\n \
 				\n \
 				processProperties(sumVector,crealProperties);\n \
 				\n \
 				writeOutDataFileFor(crealNodes, rangeArray, range, &count);\n \
 				\n \
 		}\n \
\n \
\n \
"
 
#endif //end the if statements to stop duplications



