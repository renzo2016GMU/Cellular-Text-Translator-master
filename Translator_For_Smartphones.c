
#Renzo Tejada

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define TRUE 1
#define FALSE 0

typedef struct dictionary
{
	char english[161];
	char textspeak[161];
}dictionary;

void menu( int *menuChoice );
char *readFileName();
dictionary * readDictFile(char *fileName, int *numPairs);
void translate(int numPairs, dictionary *dictArray, char engORtext);
dictionary *  add(dictionary *dictArray ,int *numPairs);

int main()
{
	dictionary *dictArray = NULL;
	char notDone = TRUE;
	int menuChoice = 0;	
	int numPairs = 0;
	char *fileName;
	while(notDone)
	{
		menu( &menuChoice );
		switch(menuChoice)
		{
			case 1:												//Read in dictionary file
				if(dictArray != NULL)
				{
					free(dictArray);							//To avoid memory leak											
					dictArray = NULL;
				}
				printf("Enter the name of a dictionary file: ");
				fileName = readFileName();
				dictArray = readDictFile(fileName, &numPairs);
				free(fileName);									//Free fileName memory on heap
				break;
			case 2:												//Translate from English to TextSpeak
				translate(numPairs, dictArray, 'E');
				break;
			case 3:												//Translate from TextSpeak to English
				translate(numPairs, dictArray, 'T');
				break;
			case 4:												//Add word to dictionary file
				dictArray =	add(dictArray, &numPairs);
				break;
			case 5:												//Exit	
				notDone = FALSE;
				if(dictArray != NULL)
				{
					free(dictArray);							//Always free memory at the end						
				}			
				break;
		}
	}
	return 0;
}

void menu( int *menuChoice)					//Print Menu and prompt the user
{
	int i;
	char notValid = TRUE;
	for(i = 0; i < 41; i++)
		printf("_");
	printf("\n|%-40s|\n|%-40s|\n|%-40s|\n|%-40s|\n|%-40s|\n", " 1. Read in a dictionary file", " 2. Translate from English to TextSpeak",
			" 3. Translate from TextSpeak to English", " 4. Add word to dictionary file", " 5. Exit");
	for(i = 0; i < 41; i++)
		printf("-");
	while(notValid)
	{
		printf("\nEnter a choice from the menu: ");
		scanf("%d", menuChoice);
		getchar();
		if( (*menuChoice >= 1) && (*menuChoice <= 5) )			//Check choice input range
			notValid = FALSE;
		else
			printf("<<< Invalid input range >>>\n");
	}
	printf("\n");
}
char * readFileName()
{
	char temp[200];
	char *string;

	if( fgets(temp, sizeof(temp), stdin) == 0 )								//Check input errors
	{
		fprintf(stderr, "Error returned from fgets()\n");
		exit(1);
	}
	if( sscanf(temp, "%s", temp) != 1 )										//Check scanning errors
	{
		fprintf(stderr, "Error scanning string from temp\n");
		exit(1);
	}
	string = (char *)malloc( (strlen(temp) + 5) *  sizeof (char) );			//Create just enough memory to hold file Name
	if(string == NULL)														//Check memory allocation
	{
		fprintf(stderr, "Memory allocation failed.\n");
		exit(1);
	}
	strcpy(string,temp);
	printf("\n");
	return string;
}
dictionary * readDictFile(char *fileName, int *numPairs)
{
	FILE *file = NULL;
	dictionary *dictArray = NULL;
	int i;
	char string[161];

	file = fopen(fileName, "r");											//Open file in read mode
	if(file == NULL)														//Check file opened properly
	{
		printf("Error opening file\n");
		return NULL;
	}
	if( fgets(string, sizeof(string), file) == 0)							//Check read number of Pairs
	{
		fprintf(stderr, "Error returned from fgets()(read number of pairs)\n");
		fclose(file);
		exit(1);
	}
	if( sscanf(string, "%d", numPairs) != 1)						
	{
		fprintf(stderr, "Error scanning string(Read number of pairs)\n");
		fclose(file);
		exit(1);
	}
	dictArray = (dictionary *)malloc(*numPairs * sizeof(dictionary));		//Malloc for numPair dictionary
	if(dictArray == NULL)													//Check memory allocation
	{
		fprintf(stderr, "Memory allocation failed.\n");
		exit(1);
	}
	for(i = 0; i < *numPairs; i++)
	{
		fgets(string, sizeof(string), file);								//Read English phrase
		strncpy(dictArray[i].english, string, strlen(string)-1);			//Copy phrase to array. -1 for \n
		dictArray[i].english[strlen(string)-1] = '\0';						//strncpy(): Length is reached before the entire string is copied
		fgets(string, sizeof(string), file);								//Read TextSpeak phrase	-1 for \n
		strncpy(dictArray[i].textspeak, string, strlen(string)-1);			//Copy phrase to array
		dictArray[i].textspeak[strlen(string)-1] = '\0';					//strncpy(): Length is reached before the entire string is copied
	}
	fclose(file);															//Close file
	return dictArray;														//Return dictionary array
}

void translate(int numPairs, dictionary *dictArray, char engORtext)
{
	char inputString[161];													//Hold string without punctuation
	char inputPunct[161];													//Hold string with punctuation in the last character
	char finalString[161] = "";
	char lastChar[2];
	char notFound = TRUE;
	char hasPunct = FALSE;
	int i;
	char *token;
	char *nextToken;
	const char s[15] = " ";

	if(engORtext == 'E')
		printf("Enter a word or phrase in English: ");
	else
		printf("Enter a word or phrase in TextSpeak: ");
	if( fgets(inputString, sizeof(inputString), stdin) == 0 )				//Check error from reading input
	{
		fprintf(stderr, "error returned from fgets()\n");
		exit(1);
	}
	inputString[strlen(inputString)-1] = '\0';								//Delete nextLine character
	lastChar[0] = inputString[strlen(inputString)-1];						//Assign last character
	lastChar[1] = '\0';
	
	for( i = 0; i < strlen(inputString); i++ )									//To lower case
		inputString[i] = tolower(inputString[i]);

	if( (lastChar[0] == '.') || (lastChar[0] == '?') || (lastChar[0] == '!') )		//Check for last character
	{
		hasPunct = TRUE;
		strcpy(inputPunct,inputString);										//String with punctuation
		inputString[strlen(inputString)-1] = '\0';							//String without punctuation
	}		
	for(i = 0; i < numPairs && notFound; i++)								//Searching the dictionary
	{
		if(engORtext == 'E')
			notFound = strcmp(inputString, dictArray[i].english);			//String without punctuation found it , return False to notFound
		else
			notFound = strcmp(inputString, dictArray[i].textspeak);
		if(!notFound)														//Found it!!!
		{
			if(engORtext == 'E')
				strcpy(finalString, dictArray[i].textspeak);
			else
				strcpy(finalString, dictArray[i].english);
			if(hasPunct)													//With punctuation
				strcat(finalString,lastChar);
		}
		if(notFound && hasPunct)											//If hasFound and string contains punctuation, we do search with punctuation
		{
			if(engORtext == 'E')
				notFound = strcmp(inputPunct, dictArray[i].english);
			else
				notFound = strcmp(inputPunct, dictArray[i].textspeak);
			if(!notFound)
			{
				if(engORtext == 'E')
					strcpy(finalString, dictArray[i].textspeak);
				else
					strcpy(finalString, dictArray[i].english);
			}
		}
	}
	if(!notFound)															//If phrase is found
	{
		if(engORtext == 'E')
			printf("Corresponding TextSpeak phrase: %s\n", finalString);
		else
			printf("Corresponding English phrase: %s\n", finalString);
	}
	else																	//else break into separate words
	{
		char temp[161];
		char addLastChar = FALSE;
		nextToken = strtok(inputString, s);									//Pointer to next word
		token = nextToken;													//Pointer to current word
		while( token != NULL )
		{
			notFound = TRUE;
			nextToken = strtok(NULL, s);									//Get next Word
			for(i = 0; i < numPairs && notFound; i++)						//String without punctuation found it, return False to notFound
			{
				if(engORtext == 'E')
					notFound = strcmp(token, dictArray[i].english);
				else
					notFound = strcmp(token, dictArray[i].textspeak);
			}
			if( !notFound && (nextToken == NULL) && hasPunct)				//Finish searching for last word, check for add last character(After searching withoutP and found)
				addLastChar = TRUE;
			if( notFound && (nextToken == NULL) && hasPunct )				//If has a punctuation and notFound, check the last word in sentence with punctuation
			{
				strcpy(temp, token);
				strcat(temp, lastChar);									//Create string with punctuation
				for(i = 0; i < numPairs && notFound; i++)
				{
					if(engORtext == 'E')
						notFound = strcmp(temp, dictArray[i].english);
					else
						notFound = strcmp(temp, dictArray[i].textspeak);
				}
			}
			if( notFound && (nextToken == NULL) && hasPunct)			//Finish searching for last word, check for add last character(After checking withP and notFound)
				addLastChar = TRUE;
			strcat(finalString," ");
			if(!notFound)												//Found it
			{
				if(engORtext == 'E')
					strcat(finalString, dictArray[i-1].textspeak);	
				else
					strcat(finalString, dictArray[i-1].english);	
				if(addLastChar)
					strcat(finalString, lastChar);						//Add last character
			}
			else
			{
				strcat(finalString, token);								//Not found, stay the same word
				if(addLastChar)
					strcat(finalString, lastChar);						//Add last character
			}
			token = nextToken;
		}
		if(engORtext == 'E')
			printf("Corresponding TextSpeak phrase:%s\n", finalString);										//Print translation to console
		else
			printf("Corresponding English phrase:%s\n", finalString);										//Print translation to console

	}
}


dictionary * add(dictionary *dictArray, int *numPairs)		//Need to update numPairs in this function
{
	char englishStr[161];
	char textspeakStr[161];
	char tempString[161];
	char *fileName;
	FILE *file = NULL;
	int i;
	printf("Enter an English word or phrase: ");									//Prompt for an English word
	fgets(tempString, sizeof(tempString), stdin);
	strncpy(englishStr, tempString, strlen(tempString));
	englishStr[strlen(tempString)] = '\0';	
	printf("\n");
	printf("Enter the corresponding TextSpeak version of the phrase: ");			//Prompt for corresponding TextSpeak
	fgets(tempString, sizeof(tempString), stdin);
	strncpy(textspeakStr, tempString, strlen(tempString));
	textspeakStr[strlen(tempString)] = '\0';	
	printf("\n");
	for( i = 0; i < strlen(englishStr); i++ )										//To lower case
		englishStr[i] = tolower(englishStr[i]);
	for( i = 0; i < strlen(textspeakStr); i++ )
		textspeakStr[i] = tolower(textspeakStr[i]);


	printf("Enter the name of a dictionary file: ");
	fileName = readFileName();

	file = fopen(fileName, "w");													//Open file in write mode
	if(file == NULL)
	{
		printf("Error opening file\n");
		exit(1);
	}
	*numPairs = *numPairs + 1;														//increment number of Pairs
	fprintf(file, "%d\n", *numPairs);												//Print formatted number to file

	for(i = 0; i < (*numPairs) - 1; i++)													//Write current dictionary in memory to file
	{
		fprintf(file, "%s\n", dictArray[i].english);
		fprintf(file, "%s\n", dictArray[i].textspeak);
	}

	fprintf(file, "%s", englishStr);												//Add new phrases							
	fprintf(file, "%s", textspeakStr);

	fclose(file);
	free(dictArray);																//Free old memory
	dictArray = readDictFile(fileName, numPairs);									//Read the updated version of dictionary file into memory
	free(fileName);																	//Free fileName memory on heap
	return dictArray;
}
