/*Advanced C Project - Survey Program
* by Patrick Moran
* g00179039@gmit.ie
* 08/04/2016 */

#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#define NUM_OF_PASSWROD_RECORDS 3
FILE* file_ptr;//file pointer

typedef struct login {
	//Login Structure to hold username and password
	char username[7];
	char password[7];
}login_t;

typedef struct  surveyAnsers {
	//survey structure to hold survey answers
	int ppsNum;
	char firstName[15];
	char lastName[15];
	char gender[2];
	char address[50];
	char email[40];
	int ageBracket;
	int incomeBracket;
	int excerise;
	int alcohol;
	int cigarettes;
}answers_t;

struct listElement {
	answers_t value; //structure holding values
	struct listElement* next;//pointer to the next node in the linked list
};
/*===Declare Functions===*/
int login(login_t* access, char username[], char password[]);
void addSurvey(struct listElement** head_ptr);
void addSurveyStart(struct listElement** head_ptr);
void displayAllSurveys(struct listElement* head_ptr);
void displaySurveyDetails(struct listElement* head_ptr);
void updateSurvey(struct listElement** head_ptr);
void deleteSurvey(struct listElement** head_ptr);
//===Statistics Functions====
void generateStatistics(struct listElement* head_ptr);
void generateAgeStatistics(struct listElement* head_ptr, int number);//calls the three functions underneath
void generateAgeSmokingStatistics(struct listElement* head_ptr, int number);
void generateAgeExerciseStatistics(struct listElement* head_ptr, int number);
void generateAgeAlcoholStatistics(struct listElement* head_ptr, int number);
void generateIncomeStatistics(struct listElement* head_ptr, int number);//calls the three functions underneath
void generateIncomeSmokingStatistics(struct listElement* head_ptr, int number);
void generateIncomeExerciseStatistics(struct listElement* head_ptr, int number);
void generateIncomeAlcoholStatistics(struct listElement* head_ptr, int number);
void generateGenderStatistics(struct listElement* head_ptr, int number);
//===============================================================================
void printToFile(struct listElemnt* head_ptr);
int displayLength(struct listElement* head_ptr);
int search(struct listElement* head_ptr, int pps);
void sortList(struct listElement** head_ptr);
void save(struct listElemnt* head_ptr);
void load(struct listElemnt** head_ptr);
void editMenu();
void mainMenu();
int validateEmail(char email[]);

void main() {
	//Declare Variables
	login_t* access;
	struct listElement* top;
	
	char username[10];
	char password[10];
	char pass, name;
	int grantAccess = 0;
	int i, choice, count = 0;
	int test = 0;
	int numRecords;

	top = NULL;
	
	//=======================Header=======================================================
	printf("===========================================================================\n");
	printf("\t\t\tSURVEY DATABASE PROGRAM\n");
	printf("\t\t\t    by Patrick Moran\n");
	printf("===========================================================================");
	//=====================================Login===========================================
	do {
		count++;
		printf("\nPlease enter username: ");
		scanf("%s", username);

		printf("Please enter password: ");
		for (i = 0; i < 6; i++) {//pasword must be six characters
			pass = getch();//store the first character in pass
			password[i] = pass;//that is then stored in index 0 of the password
			pass = '*'; //pass is then *
			printf("%c", pass); //only print * - Repeat until you get to the end of the password
		}
		password[i] = '\0';//put in the terminator
		printf("\n");
		
		access = (login_t*)malloc(sizeof(login_t) * NUM_OF_PASSWROD_RECORDS);
		grantAccess = login(access, username, password);

		if (count > 4) {//Username and Password entered incorrect 5 times - the program will end
			printf("\nYou entered your username or password incorrect 5 times! This program will Terminate\n");
			break;
		}
		if (grantAccess == 1) {
			printf("\nAccess to Survey Records Granted\n");//access granted
		}
		else if (grantAccess == 2) {
			break; //can't access file containing login records so terminate program
		}
		else {
			printf("\nUsername and password are incorrect!! - Please try again\n");//wrong user input
		}	
	} while (grantAccess == 0);//do while grant access remains at 0
	//=====================================================================================
	
	if (grantAccess == 1) {//Proceed with program if grant access returned as 1 otherwise the program will end
		
		file_ptr = fopen("Surveys.txt", "r");//open the file to read
		if (file_ptr != NULL) {
			fclose(file_ptr);//close the file again
			load(&top);//if there is data in the file then call load()
		}
		do {
			mainMenu();//call mainMenu() function
			scanf("%d", &choice);//scan input from user
			
			if (choice == 1) {
				addSurvey(&top);//add a survey to the list
			}
			else if (choice == 2) {
				if (top != NULL) {//A check incase I try and sort a list that doesn't exist
					sortList(&top);
				}
				displayAllSurveys(top);//display all surveys to screen
			}
			else if (choice == 3) {
				displaySurveyDetails(top);//display specific survey - either by pps or surname
			}
			else if (choice == 4) {
				updateSurvey(&top);//update an existing survey
			}
			else if (choice == 5) {
				deleteSurvey(&top);//deletes a survey by pps number
			}
			else if (choice == 6) {
				generateStatistics(top);//generates statistics for all the surveys in the linked list
			}
			else if (choice == 7) {
				//I pass a 1 into the statistics functions so the surveys only get printed to file, not to screen
				if (top != NULL) {
					printToFile(top);//prints the surveys in the list to a file called "SurveyReport.txt"
					generateAgeSmokingStatistics(top, 1);
					generateAgeExerciseStatistics(top, 1);
					generateAgeAlcoholStatistics(top, 1);
					generateIncomeSmokingStatistics(top, 1);
					generateIncomeExerciseStatistics(top, 1);
					generateIncomeAlcoholStatistics(top, 1);
					generateGenderStatistics(top, 1);
				}
				//incase this function is called and there are no surveys
				else {
					printf("\nNo Survey Records to print to File \"SurveyReport.txt\"\n");
				}
			}
			else if (choice < -1 || choice > 7){
				printf("\nSorry that is not a valid choice! Please try again.\n");
			}/*Error handling for invalid integer input from user*/
		} while (choice != -1 && choice == getchar() != EOF);
		//used getchar() != EOF incase the user entered a character instead of a number - not ideal but it does prevent the program from entering a contineous loop
		
		save(top);//save function is called just before the program terminates to update the survey information to file
	}//if
	getch();
}//main()

int login(login_t* access, char username[], char password[]) {
	int grantAccess = 0; //returns 0 if the username and the password entered is incorrect
	int i;

	file_ptr = fopen("password.txt", "r+");//open file to read usernames and passwords
	if (file_ptr != NULL) {
		for (i = 0; i < NUM_OF_PASSWROD_RECORDS; i++) {

			while (!feof(file_ptr)) {
				//Read password records from file
				fscanf(file_ptr, "%s", (access + i)->username);//read the first line of file and pass username into structure
				fscanf(file_ptr, "%s", (access + i)->password);//read second line of file and pass password into structure

				if (strcmp((username + i), (access + i)->username) == 0) {
					if (strcmp((password + i), (access + i)->password) == 0) {
						grantAccess = 1; //returns 1 if both username and password match
					}//if
				}//if
			}//while
		}
		fclose(file_ptr);//close file
	}
	else {
		printf("\nError accessing login authentification Records!\n");
		grantAccess = 2;//returns 2 incase there is a problem accessing the password file
	}
	return grantAccess;
}//login

void mainMenu() {
	printf("\n=======================================================\n");
	printf("\t\t\tMain Menu\n");
	printf("=======================================================\n");
	printf("Please Enter your Choice Below\n");
	printf("1: To add a Servey\n");
	printf("2: To Display all surveys to Screen\n");
	printf("3: To display Survey Details\n");
	printf("4: To update survey\n");
	printf("5: To delete survey!\n");
	printf("6: To generate statistics\n");
	printf("7: To print surveys to a report file\n");
	printf("-1: To Terminate!\n");
}//mainMenu

void addSurvey(struct listElement** head_ptr) {
	struct listElement *newNode;
	struct listElement *temp;
	int temp_pps;//holds the temporary pps
	int validateMail = 0; //validation variable
	int validateGender = 0;//validation variable
	char tempEmail[30];//holds the temporary email 
	
	if (*head_ptr == NULL) {
		addSurveyStart(head_ptr);//If its the first survey just add it to the start of the list
	}
	else {
		//============Personal Details=============
		printf("Q1: What is your PPS Number?: ");
		scanf("%d", &temp_pps);
		if (search(*head_ptr, temp_pps) == 0) {//call search() to test if pps num is unique
			temp = *head_ptr;//temp is now the head of the list
			
			while (temp->next != NULL) {//move to the last node
				temp = temp->next;
			}//while
			
			newNode = (struct listElement*)malloc(sizeof(struct listElement));//allocate memory for new node
		
			newNode->value.ppsNum = temp_pps;//copy the tempPps into the actual pps

			printf("Q2: What is your first name?: ");
			scanf("%s", newNode->value.firstName);
			printf("Q3: What is your last name?: ");
			scanf("%s", newNode->value.lastName);
			printf("Q4: What is your address?: ");
			scanf("%s", newNode->value.address);
			do {
				printf("Q5: What is your email address?: ");
				scanf("%s", tempEmail);
				validateMail = validateEmail(tempEmail);//call validateEmail() - returns 1 if the email is okay
				if (validateMail == 0) {
					printf("\nSorry - That email is not correct. Please try again!\n");
				}
			} while (validateMail == 0);
			strcpy(newNode->value.email, tempEmail);//put the temp email into the actual email 
			
			do {
				printf("Q6: What is your Gender(m/f)?: ");
				scanf("%s", newNode->value.gender);
				if (strcmp("m", newNode->value.gender) == 0 || strcmp("f", newNode->value.gender) == 0 || strcmp("M", newNode->value.gender) == 0 || strcmp("F", newNode->value.gender) == 0) {
					validateGender = 1;
				}
				else {
					printf("\nPlease Try again! %s is not valid input!\n", newNode->value.gender);
				}
			} while (validateGender == 0);
			//==========Option Qusetions================
			do {
				printf("\nQ7: What is your Age Bracket TEST?\n");
				printf("\tType 1 for 18-20\n");
				printf("\tType 2 for 20-30\n");
				printf("\tType 3 for 30-50\n");
				printf("\tType 4 for 50-65\n");
				printf("\tType 5 for 65+\n");
				scanf("%d", &newNode->value.ageBracket);
			} while (newNode->value.ageBracket < 1 || newNode->value.ageBracket > 5);
			do {
				printf("\nQ8: What is your Income Bracket?\n");
				printf("\tType 1 for No Income\n");
				printf("\tType 2 for < 20,000\n");
				printf("\tType 3 for < 40,000\n");
				printf("\tType 4 for < 60,000\n");
				printf("\tType 5 for < 80,000\n");
				printf("\tType 6 for < 100,000\n");
				printf("\tType 7 for > 100,000\n");
				scanf("%d", &newNode->value.incomeBracket);
			} while (newNode->value.incomeBracket < 1 || newNode->value.incomeBracket > 7);
			do {
				printf("\nQ9: How often do you excercise?\n");
				printf("\tType 1 for Never\n");
				printf("\tType 2 for Less than 3 times a week\n");
				printf("\tType 3 for less than 5 times a week\n");
				printf("\tType 4 for more than 5 times a week\n");
				scanf("%d", &newNode->value.excerise);
			} while (newNode->value.excerise < 1 || newNode->value.excerise > 4);
			do {
				printf("\nQ10: How much alchol do you consume per week?\n");
				printf("\tType 1 for None\n");
				printf("\tType 2 for Less than 2 units\n");
				printf("\tType 3 for less than 4 units\n");
				printf("\tType 4 for more than 4 units\n");
				scanf("%d", &newNode->value.alcohol);
			} while (newNode->value.alcohol < 1 || newNode->value.alcohol > 4);
			do {
				printf("\nQ11: How many cigarettes do you smoke per week?\n");
				printf("\tType 1 for None\n");
				printf("\tType 2 for Less than 20 cigarettes\n");
				printf("\tType 3 for less than 40 cigarettes\n");
				printf("\tType 4 for more than 40 cigarettes\n");
				scanf("%d", &newNode->value.cigarettes);
			} while (newNode->value.cigarettes < 1 || newNode->value.cigarettes > 4);
			newNode->next = NULL; //newNode is now pointing at null because its at the end
			temp->next = newNode;
		}
		else {
			printf("\nSorry, that PPS Number allready exists in the data base!\n");
		}
	}//else

}//addSurvey()

void addSurveyStart(struct listElement** head_ptr) {
	struct listElement *newNode;
	int temp_pps;
	int validateMail = 0;
	int validateGender = 0;
	char tempEmail[30];//holds temp email address
	printf("Q1: What is your PPS Number?: ");
	scanf("%d", &temp_pps);
	if (search(*head_ptr, temp_pps) == 0) {
		
		newNode = (struct listElement*)malloc(sizeof(struct listElement));//allocate memory
		
		newNode->value.ppsNum = temp_pps;

		printf("Q2: What is your first name?: ");
		scanf("%s", newNode->value.firstName);
		printf("Q3: What is your last name?: ");
		scanf("%s", newNode->value.lastName);
		printf("Q4: What is your address?: ");
		scanf("%s", newNode->value.address);
		do {
			printf("Q5: What is your email address?: ");
			scanf("%s", tempEmail);
			validateMail = validateEmail(tempEmail);
			if (validateMail == 0) {
				printf("\nSorry - That email is not correct. Please try again!\n");
			}
		} while (validateMail == 0);
		strcpy(newNode->value.email, tempEmail);
		do {
			printf("Q6: What is your Gender(m/f)?: ");
			scanf("%s", newNode->value.gender);
			if (strcmp("m", newNode->value.gender) == 0 || strcmp("f", newNode->value.gender) == 0 || strcmp("M", newNode->value.gender) == 0 || strcmp("F", newNode->value.gender) == 0) {
				validateGender = 1;
			}
			else {
				printf("\nPlease Try again! %s is not valid input!\n", newNode->value.gender);
			}
		} while (validateGender == 0);
		//==========Option Qusetions================
		do {
			printf("\nQ7: What is your Age Bracket?\n");
			printf("\tType 1 for 18-20\n");
			printf("\tType 2 for 20-30\n");
			printf("\tType 3 for 30-50\n");
			printf("\tType 4 for 50-65\n");
			printf("\tType 5 for 65+\n");
			scanf("%d", &newNode->value.ageBracket);
		} while (newNode->value.ageBracket < 1 || newNode->value.ageBracket > 5);
		do {
			printf("\nQ8: What is your Income Bracket?\n");
			printf("\tType 1 for No Income\n");
			printf("\tType 2 for < 20,000\n");
			printf("\tType 3 for < 40,000\n");
			printf("\tType 4 for < 60,000\n");
			printf("\tType 5 for < 80,000\n");
			printf("\tType 6 for < 100,000\n");
			printf("\tType 7 for > 100,000\n");
			scanf("%d", &newNode->value.incomeBracket);
		} while (newNode->value.incomeBracket < 1 || newNode->value.incomeBracket > 7);
		do {
			printf("\nQ9: How often do you excercise?\n");
			printf("\tType 1 for Never\n");
			printf("\tType 2 for Less than 3 times a week\n");
			printf("\tType 3 for less than 5 times a week\n");
			printf("\tType 4 for more than 5 times a week\n");
			scanf("%d", &newNode->value.excerise);
		} while (newNode->value.excerise < 1 || newNode->value.excerise > 4);
		do {
			printf("\nQ10: How much alchol do you consume per week?\n");
			printf("\tType 1 for None\n");
			printf("\tType 2 for Less than 2 units\n");
			printf("\tType 3 for less than 4 units\n");
			printf("\tType 4 for more than 4 units\n");
			scanf("%d", &newNode->value.alcohol);
		} while (newNode->value.alcohol < 1 || newNode->value.alcohol > 4);
		do {
			printf("\nQ11: How many cigarettes do you smoke per week?\n");
			printf("\tType 1 for None\n");
			printf("\tType 2 for Less than 20 cigarettes\n");
			printf("\tType 3 for less than 40 cigarettes\n");
			printf("\tType 4 for more than 40 cigarettes\n");
			scanf("%d", &newNode->value.cigarettes);
		} while (newNode->value.cigarettes < 1 || newNode->value.cigarettes > 4);

		newNode->next = *head_ptr; //newNode->next is now the head_ptr
		*head_ptr = newNode;//head_ptr is the newNode
	}
	else {
		printf("\nSorry, that PPS Number allready exists in the data base!\n");
	}
}//addSurveyStart()

void displayAllSurveys(struct listElement* head_ptr) {
	struct listElement *temp;

	temp = head_ptr;

	if (head_ptr == NULL) {//nothing to display if head Pointer is Null
		printf("\nThere are no Surveys in the DataBase!\n");
	}
	else {
		printf("\n==============================================================\n");
		printf("\t\t   All Surveys Displayed\n");
		printf("==============================================================");
		/*Loop through the link list and print elements to the screen*/
		while (temp != NULL) {
			printf("\nPPS Number: %d\n", temp->value.ppsNum);
			printf("Name: %s %s\n", temp->value.firstName, temp->value.lastName);
			printf("Address: %s\n", temp->value.address);
			printf("Email Addres: %s\n", temp->value.email);
			printf("Gender: %s\n", temp->value.gender);

			if (temp->value.ageBracket == 1)  printf("Age Bracket: 18-20 Years\n");
			if (temp->value.ageBracket == 2)  printf("Age Bracket: 20-30 Years\n");
			if (temp->value.ageBracket == 3)  printf("Age Bracket: 30-50 Years\n");
			if (temp->value.ageBracket == 4)  printf("Age Bracket: 50-65 Years\n");
			if (temp->value.ageBracket == 5)  printf("Age Bracket: 65+ Years\n");

			if (temp->value.incomeBracket == 1)  printf("Income Bracket: No Income\n");
			if (temp->value.incomeBracket == 2)  printf("Income Bracket: < 20,000\n");
			if (temp->value.incomeBracket == 3)  printf("Income Bracket: < 40,000\n");
			if (temp->value.incomeBracket == 4)  printf("Income Bracket: < 60,000\n");
			if (temp->value.incomeBracket == 5)  printf("Income Bracket: < 80,000\n");
			if (temp->value.incomeBracket == 6)  printf("Income Bracket: < 100,000\n");
			if (temp->value.incomeBracket == 7)  printf("Income Bracket: > 100,000\n");

			if (temp->value.excerise == 1)  printf("Exercise: Never\n");
			if (temp->value.excerise == 2)  printf("Exercise: Less Than 3 times per week\n");
			if (temp->value.excerise == 3)  printf("Exercise: Less than 5 times per week\n");
			if (temp->value.excerise == 4)  printf("Exercise: More than 5 times per week\n");

			if (temp->value.alcohol == 1)  printf("Alcohol Consumption: None\n");
			if (temp->value.alcohol == 2)  printf("Alcohol Consumption: Less than 2 Units\n");
			if (temp->value.alcohol == 3)  printf("Alcohol Consumption: Less than 4 units\n");
			if (temp->value.alcohol == 4)  printf("Alcohol Consumption: More than 4 units\n");

			if (temp->value.cigarettes == 1)  printf("Cigarette Consumption: None\n");
			if (temp->value.cigarettes == 2)  printf("Cigarette Consumption: Less than 20\n");
			if (temp->value.cigarettes == 3)  printf("Cigarette Consumption: Less than 40\n");
			if (temp->value.cigarettes == 4)  printf("Cigarette Consumption: More than 40\n");

			temp = temp->next;//move to the last node
		}//while
		printf("==============================================================\n");
	}
}//displayAllSurveys

void displaySurveyDetails(struct listElement* head_ptr) {
	struct listElement *temp;
	int input, tempPps;
	char tempSurname[20];
	int found = 0;

	temp = head_ptr;

	if (head_ptr == NULL) {//nothing to display if head Pointer is Null
		printf("\nThere are no Surveys in the DataBase!\n");
	}
	else {
		do {//choice, either search by ps number or surname
			printf("\nHow would you like to search for the survey.\n");
			printf("Press 1: By PPS Number\n");
			printf("Press 2: By Surname\n");
			printf("Press -1: Return to Menu\n");
			scanf("%d", &input);

			if (input == 1) {
				printf("\nPlease Enter the PPS Number: ");
				scanf("%d", &tempPps);

				while (temp != NULL && found == 0) {
					
					if (temp->value.ppsNum == tempPps) {
						found = 1;
						printf("\nPPS Number: %d\n", temp->value.ppsNum);
						printf("Name: %s %s\n", temp->value.firstName, temp->value.lastName);
						printf("Address: %s\n", temp->value.address);
						printf("Email Addres: %s\n", temp->value.email);
						printf("Gender: %s\n", temp->value.gender);

						if (temp->value.ageBracket == 1)  printf("Age Bracket: 18-20 Years\n");
						if (temp->value.ageBracket == 2)  printf("Age Bracket: 20-30 Years\n");
						if (temp->value.ageBracket == 3)  printf("Age Bracket: 30-50 Years\n");
						if (temp->value.ageBracket == 4)  printf("Age Bracket: 50-65 Years\n");
						if (temp->value.ageBracket == 5)  printf("Age Bracket: 65+ Years\n");

						if (temp->value.incomeBracket == 1)  printf("Income Bracket: No Income\n");
						if (temp->value.incomeBracket == 2)  printf("Income Bracket: < 20,000\n");
						if (temp->value.incomeBracket == 3)  printf("Income Bracket: < 40,000\n");
						if (temp->value.incomeBracket == 4)  printf("Income Bracket: < 60,000\n");
						if (temp->value.incomeBracket == 5)  printf("Income Bracket: < 80,000\n");
						if (temp->value.incomeBracket == 6)  printf("Income Bracket: < 100,000\n");
						if (temp->value.incomeBracket == 7)  printf("Income Bracket: > 100,000\n");

						if (temp->value.excerise == 1)  printf("Exercise: Never\n");
						if (temp->value.excerise == 2)  printf("Exercise: Less Than 3 times per week\n");
						if (temp->value.excerise == 3)  printf("Exercise: Less than 5 times per week\n");
						if (temp->value.excerise == 4)  printf("Exercise: More than 5 times per week\n");

						if (temp->value.alcohol == 1)  printf("Alcohol Consumption: None\n");
						if (temp->value.alcohol == 2)  printf("Alcohol Consumption: Less than 2 Units\n");
						if (temp->value.alcohol == 3)  printf("Alcohol Consumption: Less than 4 units\n");
						if (temp->value.alcohol == 4)  printf("Alcohol Consumption: More than 4 units\n");

						if (temp->value.cigarettes == 1)  printf("Cigarette Consumption: None\n");
						if (temp->value.cigarettes == 2)  printf("Cigarette Consumption: Less than 20\n");
						if (temp->value.cigarettes == 3)  printf("Cigarette Consumption: Less than 40\n");
						if (temp->value.cigarettes == 4)  printf("Cigarette Consumption: More than 40\n");
						return;
					}
					temp = temp->next;
				}//while to loop through linked list
				if (found == 0) {
					printf("\Sorry - Couldn't find PPS Number %d in the database\n", tempPps);
					return;
				}
			}//search by pps
			else if (input == 2) {
				printf("\nPlease Enter the Surname: ");
				scanf("%s", tempSurname);
				
				while (temp != NULL) {
					//Used strcmpi() so its not case sensitive
					if (strcmpi(tempSurname, temp->value.lastName) == 0) {//have a match - print survey to screen
						found = 1;
						printf("\nPPS Number: %d\n", temp->value.ppsNum);
						printf("Name: %s %s\n", temp->value.firstName, temp->value.lastName);
						printf("Address: %s\n", temp->value.address);
						printf("Email Addres: %s\n", temp->value.email);
						printf("Gender: %s\n", temp->value.gender);

						if (temp->value.ageBracket == 1)  printf("Age Bracket: 18-20 Years\n");
						if (temp->value.ageBracket == 2)  printf("Age Bracket: 20-30 Years\n");
						if (temp->value.ageBracket == 3)  printf("Age Bracket: 30-50 Years\n");
						if (temp->value.ageBracket == 4)  printf("Age Bracket: 50-65 Years\n");
						if (temp->value.ageBracket == 5)  printf("Age Bracket: 65+ Years\n");

						if (temp->value.incomeBracket == 1)  printf("Income Bracket: No Income\n");
						if (temp->value.incomeBracket == 2)  printf("Income Bracket: < 20,000\n");
						if (temp->value.incomeBracket == 3)  printf("Income Bracket: < 40,000\n");
						if (temp->value.incomeBracket == 4)  printf("Income Bracket: < 60,000\n");
						if (temp->value.incomeBracket == 5)  printf("Income Bracket: < 80,000\n");
						if (temp->value.incomeBracket == 6)  printf("Income Bracket: < 100,000\n");
						if (temp->value.incomeBracket == 7)  printf("Income Bracket: > 100,000\n");

						if (temp->value.excerise == 1)  printf("Exercise: Never\n");
						if (temp->value.excerise == 2)  printf("Exercise: Less Than 3 times per week\n");
						if (temp->value.excerise == 3)  printf("Exercise: Less than 5 times per week\n");
						if (temp->value.excerise == 4)  printf("Exercise: More than 5 times per week\n");

						if (temp->value.alcohol == 1)  printf("Alcohol Consumption: None\n");
						if (temp->value.alcohol == 2)  printf("Alcohol Consumption: Less than 2 Units\n");
						if (temp->value.alcohol == 3)  printf("Alcohol Consumption: Less than 4 units\n");
						if (temp->value.alcohol == 4)  printf("Alcohol Consumption: More than 4 units\n");

						if (temp->value.cigarettes == 1)  printf("Cigarette Consumption: None\n");
						if (temp->value.cigarettes == 2)  printf("Cigarette Consumption: Less than 20\n");
						if (temp->value.cigarettes == 3)  printf("Cigarette Consumption: Less than 40\n");
						if (temp->value.cigarettes == 4)  printf("Cigarette Consumption: More than 40\n");	
					}
					temp = temp->next;
				}//while
				if (found == 0) {
					printf("\Sorry - Couldn't find the Surname %s in the database\n", tempSurname);//can't find the Surname
				}
				return;
			}//search by surname
			else if (input < -1 || input > 2) {
				printf("\nSorry - Thats not a correct input! Please Try Again!\n");
			}
		} while (input != -1);//do While
		
	}//else
}//displaySurveyDetails()

void updateSurvey(struct listElement** head_ptr) {
	struct listElement *temp;
	int input, tempPps;
	char tempSurname[20];
	int found = 0;
	int validateMail = 0;
	int validateGender = 0;
	char tempEmail[30];

	temp = *head_ptr;

	if (head_ptr == NULL) {//nothing to display if head Pointer is Null
		printf("\nThere are no Surveys in the DataBase!\n");
	}
	else {
		do {
			printf("\nHow would you like to search for the survey you want to edit.\n");
			printf("Press 1: By PPS Number\n");
			printf("Press 2: By Surname\n");
			printf("Press -1: Return to Menu\n");
			scanf("%d", &input);

			if (input == 1) {
				printf("\nPlease Enter the PPS Number: ");
				scanf("%d", &tempPps);

				while (temp != NULL && found == 0) {

					if (temp->value.ppsNum == tempPps) {
						found = 1;
						//Print that Survey
						printf("\nPPS Number: %d\n", temp->value.ppsNum);
						printf("Name: %s %s\n", temp->value.firstName, temp->value.lastName);
						printf("Address: %s\n", temp->value.address);
						printf("Email Addres: %s\n", temp->value.email);
						printf("Gender: %s\n", temp->value.gender);

						if (temp->value.ageBracket == 1)  printf("Age Bracket: 18-20 Years\n");
						if (temp->value.ageBracket == 2)  printf("Age Bracket: 20-30 Years\n");
						if (temp->value.ageBracket == 3)  printf("Age Bracket: 30-50 Years\n");
						if (temp->value.ageBracket == 4)  printf("Age Bracket: 50-65 Years\n");
						if (temp->value.ageBracket == 5)  printf("Age Bracket: 65+ Years\n");

						if (temp->value.incomeBracket == 1)  printf("Income Bracket: No Income\n");
						if (temp->value.incomeBracket == 2)  printf("Income Bracket: < 20,000\n");
						if (temp->value.incomeBracket == 3)  printf("Income Bracket: < 40,000\n");
						if (temp->value.incomeBracket == 4)  printf("Income Bracket: < 60,000\n");
						if (temp->value.incomeBracket == 5)  printf("Income Bracket: < 80,000\n");
						if (temp->value.incomeBracket == 6)  printf("Income Bracket: < 100,000\n");
						if (temp->value.incomeBracket == 7)  printf("Income Bracket: > 100,000\n");

						if (temp->value.excerise == 1)  printf("Exercise: Never\n");
						if (temp->value.excerise == 2)  printf("Exercise: Less Than 3 times per week\n");
						if (temp->value.excerise == 3)  printf("Exercise: Less than 5 times per week\n");
						if (temp->value.excerise == 4)  printf("Exercise: More than 5 times per week\n");

						if (temp->value.alcohol == 1)  printf("Alcohol Consumption: None\n");
						if (temp->value.alcohol == 2)  printf("Alcohol Consumption: Less than 2 Units\n");
						if (temp->value.alcohol == 3)  printf("Alcohol Consumption: Less than 4 units\n");
						if (temp->value.alcohol == 4)  printf("Alcohol Consumption: More than 4 units\n");

						if (temp->value.cigarettes == 1)  printf("Cigarette Consumption: None\n");
						if (temp->value.cigarettes == 2)  printf("Cigarette Consumption: Less than 20\n");
						if (temp->value.cigarettes == 3)  printf("Cigarette Consumption: Less than 40\n");
						if (temp->value.cigarettes == 4)  printf("Cigarette Consumption: More than 40\n");

						//Edit that survey
						do {
							editMenu();
							scanf("%d", &input);//input from user
							if (input == 1) {
								printf("\nPlease Enter the first Name: ");
								scanf("%s", temp->value.firstName);
							}
							else if (input == 2) {
								printf("\nPlease Enter the Last Name: ");
								scanf("%s", temp->value.lastName);
							}
							else if (input == 3) {
								printf("\nPlease Enter the Address: ");
								scanf("%s", temp->value.address);
							}
							else if (input == 4) {
								do {
									printf("Q5: What is your email address?: ");
									scanf("%s", tempEmail);
									validateMail = validateEmail(tempEmail);
									if (validateMail == 0) {
										printf("\nSorry - That email is not correct. Please try again!\n");
									}
								} while (validateMail == 0);
								strcpy(temp->value.email, tempEmail);
							}
							else if (input == 5) {
								do {
									printf("Q6: What is your Gender(m/f)?: ");
									scanf("%s", temp->value.gender);
									if (strcmp("m", temp->value.gender) == 0 || strcmp("f", temp->value.gender) == 0 || strcmp("M", temp->value.gender) == 0 || strcmp("F", temp->value.gender) == 0) {
										validateGender = 1;
									}
									else {
										printf("\nPlease Try again! %s is not valid input!\n", temp->value.gender);
									}
								} while (validateGender == 0);
							}
							else if (input == 6) {
								do {
									printf("\nQ7: What is your Age Bracket?\n");
									printf("\tType 1 for 18-20\n");
									printf("\tType 2 for 20-30\n");
									printf("\tType 3 for 30-50\n");
									printf("\tType 4 for 50-65\n");
									printf("\tType 5 for 65+\n");
									scanf("%d", &temp->value.ageBracket);
								} while (temp->value.ageBracket < 1 || temp->value.ageBracket > 5);
							}
							else if (input == 7) {
								do {
									printf("\nQ8: What is your Income Bracket?\n");
									printf("\tType 1 for No Income\n");
									printf("\tType 2 for < 20,000\n");
									printf("\tType 3 for < 40,000\n");
									printf("\tType 4 for < 60,000\n");
									printf("\tType 5 for < 80,000\n");
									printf("\tType 6 for < 100,000\n");
									printf("\tType 7 for > 100,000\n");
									scanf("%d", &temp->value.incomeBracket);
								} while (temp->value.incomeBracket < 1 || temp->value.incomeBracket > 7);
							}
							else if (input == 8) {
								do {
									printf("\nQ9: How often do you excercise?\n");
									printf("\tType 1 for Never\n");
									printf("\tType 2 for Less than 3 times a week\n");
									printf("\tType 3 for less than 4 times a week\n");
									printf("\tType 4 for more than 5 times a week\n");
									scanf("%d", &temp->value.excerise);
								} while (temp->value.excerise < 1 || temp->value.excerise > 4);
							}
							else if (input == 9) {
								do {
									printf("\nQ10: How much alchol do you consume per week?\n");
									printf("\tType 1 for None\n");
									printf("\tType 2 for Less than 2 units\n");
									printf("\tType 3 for less than 4 units\n");
									printf("\tType 4 for more than 4 units\n");
									scanf("%d", &temp->value.alcohol);
								} while (temp->value.alcohol < 1 || temp->value.alcohol > 4);
							}
							else if (input == 10) {
								do {
									printf("\nQ11: How many cigarettes do you smoke per week?\n");
									printf("\tType 1 for None\n");
									printf("\tType 2 for Less than 20 cigarettes\n");
									printf("\tType 3 for less than 40 cigarettes\n");
									printf("\tType 4 for more than 40 cigarettes\n");
									scanf("%d", &temp->value.cigarettes);
								} while (temp->value.cigarettes < 1 || temp->value.cigarettes > 4);
							}
							else if (input == 0 || input < -1 || input > 10) {
								printf("\nSorry that is not a valid choice! Please try again.\n");
							}/*Error handling for invalid integer input from user*/
						} while (input != -1 && input == getchar() != EOF);
						return;
					}
					temp = temp->next;
				}//while to loop through linked list
				if (found == 0) {
					printf("\Sorry - Couldn't find PPS Number %d in the database\n", tempPps);
					return;
				}
			}//search by pps
			else if (input == 2) {
				printf("\nPlease Enter the Surname: ");
				scanf("%s", tempSurname);

				while (temp != NULL) {
					//print out survey(s)
					if (strcmpi(tempSurname, temp->value.lastName) == 0) {//have a match
						found = 1;
						printf("\nPPS Number: %d\n", temp->value.ppsNum);
						printf("Name: %s %s\n", temp->value.firstName, temp->value.lastName);
						printf("Address: %s\n", temp->value.address);
						printf("Email Addres: %s\n", temp->value.email);
						printf("Gender: %s\n", temp->value.gender);

						if (temp->value.ageBracket == 1)  printf("Age Bracket: 18-20 Years\n");
						if (temp->value.ageBracket == 2)  printf("Age Bracket: 20-30 Years\n");
						if (temp->value.ageBracket == 3)  printf("Age Bracket: 30-50 Years\n");
						if (temp->value.ageBracket == 4)  printf("Age Bracket: 50-65 Years\n");
						if (temp->value.ageBracket == 5)  printf("Age Bracket: 65+ Years\n");

						if (temp->value.incomeBracket == 1)  printf("Income Bracket: No Income\n");
						if (temp->value.incomeBracket == 2)  printf("Income Bracket: < 20,000\n");
						if (temp->value.incomeBracket == 3)  printf("Income Bracket: < 40,000\n");
						if (temp->value.incomeBracket == 4)  printf("Income Bracket: < 60,000\n");
						if (temp->value.incomeBracket == 5)  printf("Income Bracket: < 80,000\n");
						if (temp->value.incomeBracket == 6)  printf("Income Bracket: < 100,000\n");
						if (temp->value.incomeBracket == 7)  printf("Income Bracket: > 100,000\n");

						if (temp->value.excerise == 1)  printf("Exercise: Never\n");
						if (temp->value.excerise == 2)  printf("Exercise: Less Than 3 times per week\n");
						if (temp->value.excerise == 3)  printf("Exercise: Less than 5 times per week\n");
						if (temp->value.excerise == 4)  printf("Exercise: More than 5 times per week\n");

						if (temp->value.alcohol == 1)  printf("Alcohol Consumption: None\n");
						if (temp->value.alcohol == 2)  printf("Alcohol Consumption: Less than 2 Units\n");
						if (temp->value.alcohol == 3)  printf("Alcohol Consumption: Less than 4 units\n");
						if (temp->value.alcohol == 4)  printf("Alcohol Consumption: More than 4 units\n");

						if (temp->value.cigarettes == 1)  printf("Cigarette Consumption: None\n");
						if (temp->value.cigarettes == 2)  printf("Cigarette Consumption: Less than 20\n");
						if (temp->value.cigarettes == 3)  printf("Cigarette Consumption: Less than 40\n");
						if (temp->value.cigarettes == 4)  printf("Cigarette Consumption: More than 40\n");

						//Edit that survey
						do {
							editMenu();
							scanf("%d", &input);//input from user
							if (input == 1) {
								printf("\nPlease Enter the first Name: ");
								scanf("%s", temp->value.firstName);
							}
							else if (input == 2) {
								printf("\nPlease Enter the Last Name: ");
								scanf("%s", temp->value.lastName);
							}
							else if (input == 3) {
								printf("\nPlease Enter the Address: ");
								scanf("%s", temp->value.address);
							}
							else if (input == 4) {
								do {
									printf("Q5: What is your email address?: ");
									scanf("%s", tempEmail);
									validateMail = validateEmail(tempEmail);
									if (validateMail == 0) {
										printf("\nSorry - That email is not correct. Please try again!\n");
									}
								} while (validateMail == 0);
								strcpy(temp->value.email, tempEmail);
							}
							else if (input == 5) {
								do {
									printf("Q6: What is your Gender(m/f)?: ");
									scanf("%s", temp->value.gender);
									if (strcmp("m", temp->value.gender) == 0 || strcmp("f", temp->value.gender) == 0) {
										validateGender = 1;
									}
									else {
										printf("\nPlease Try again! %s is not valid input!\n", temp->value.gender);
									}
								} while (validateGender == 0);
							}
							else if (input == 6) {
								do {
									printf("\nQ7: What is your Age Bracket?\n");
									printf("\tType 1 for 18-20\n");
									printf("\tType 2 for 20-30\n");
									printf("\tType 3 for 30-50\n");
									printf("\tType 4 for 50-65\n");
									printf("\tType 5 for 65+\n");
									scanf("%d", &temp->value.ageBracket);
								} while (temp->value.ageBracket < 1 || temp->value.ageBracket > 5);
							}
							else if (input == 7) {
								do {
									printf("\nQ8: What is your Income Bracket?\n");
									printf("\tType 1 for No Income\n");
									printf("\tType 2 for < 20,000\n");
									printf("\tType 3 for < 40,000\n");
									printf("\tType 4 for < 60,000\n");
									printf("\tType 5 for < 80,000\n");
									printf("\tType 6 for < 100,000\n");
									printf("\tType 7 for > 100,000\n");
									scanf("%d", &temp->value.incomeBracket);
								} while (temp->value.incomeBracket < 1 || temp->value.incomeBracket > 7);
							}
							else if (input == 8) {
								do {
									printf("\nQ9: How often do you excercise?\n");
									printf("\tType 1 for Never\n");
									printf("\tType 2 for Less than 3 times a week\n");
									printf("\tType 3 for less than 4 times a week\n");
									printf("\tType 4 for more than 5 times a week\n");
									scanf("%d", &temp->value.excerise);
								} while (temp->value.excerise < 1 || temp->value.excerise > 4);
							}
							else if (input == 9) {
								do {
									printf("\nQ10: How much alchol do you consume per week?\n");
									printf("\tType 1 for None\n");
									printf("\tType 2 for Less than 2 units\n");
									printf("\tType 3 for less than 4 units\n");
									printf("\tType 4 for more than 4 units\n");
									scanf("%d", &temp->value.alcohol);
								} while (temp->value.alcohol < 1 || temp->value.alcohol > 4);
							}
							else if (input == 10) {
								do {
									printf("\nQ11: How many cigarettes do you smoke per week?\n");
									printf("\tType 1 for None\n");
									printf("\tType 2 for Less than 20 cigarettes\n");
									printf("\tType 3 for less than 40 cigarettes\n");
									printf("\tType 4 for more than 40 cigarettes\n");
									scanf("%d", &temp->value.cigarettes);
								} while (temp->value.cigarettes < 1 || temp->value.cigarettes > 4);
							}
							else if (input == 0 || input < -1 || input > 10) {
								printf("\nSorry that is not a valid choice! Please try again.\n");
							}/*Error handling for invalid integer input from user*/
						} while (input != -1 && input == getchar() != EOF);//do while
					}
					temp = temp->next;
				}//while
				if (found == 0) {
					printf("\Sorry - Couldn't find the Surname %s in the database\n", tempSurname);
				}
				return;
			}//search by surname
			else if (input < -1 || input > 2) {
				printf("\nSorry - Thats not a correct input! Please Try Again!\n");
			}
		} while (input != -1 && input == getchar() != EOF);//do While
	}//else

}//updateSurvey()

void deleteSurvey(struct listElement** head_ptr) {
	int found = 0;//found will be either 0 or 1
	int tempPps;//hold the tempPps variable
	struct listElement *temp; 
	struct listElement *prev;
	int status;
	temp = (struct listElement*)malloc(sizeof(struct listElement));
	temp = *head_ptr; //set temp to the head of the list
	prev = NULL; //set previous pointing to nothing
	
	printf("\nPlease enter the PPS number of the survey you want to delete: ");
	scanf("%d", &tempPps);
	
	while (temp != NULL && found == 0) {//loop around the linked list
		if (temp->value.ppsNum == tempPps) {//if actaul pps num and temp_pps number are the same we have a match
			found = 1; //found is 1 if successful
			printf("\nDeleting the Survey with Pps Number: %d\n", temp->value.ppsNum);
			if (prev != NULL) {
				prev->next = temp->next;
			}
			else {
				*head_ptr = temp->next;
			}
			free(temp);//delete the node
			break;//we have found and deleted our node, exit while loop
		}//if to compare
		prev = temp;//previous is now temp
		temp = temp->next;//temp is now temps next
	}//while
	if (found == 0) {
		printf("\nSorry, no Survey with PPS Number: %d in the database!\n", tempPps);//can't find the survey
	}
}//deleteSurvey()

void generateStatistics(struct listElement* head_ptr) {
	int choice;
	if (head_ptr == NULL) {
		printf("\nSorry - There are no surveys in the database!!\n");
	}
	else {
		do {
			printf("\n============================\n");
			printf("    Generate Statistics\n");
			printf("============================\n");
			printf("Type 1: For Age Statistics\n");
			printf("Type 2: For Income Statistics\n");
			printf("Type 3: For Gender Statistics\n");
			printf("Type -1: To return to main menu!\n");
			scanf("%d", &choice);
			//I pass a 0 into the statistics functions so the surveys only get printed to the screen, not to the file
			if (choice == 1) {
				generateAgeStatistics(head_ptr, 0);
				break;
			}
			else if (choice == 2) {
				generateIncomeStatistics(head_ptr, 0);
				break;
			}
			else if (choice == 3) {
				generateGenderStatistics(head_ptr, 0);
				break;
			}
			else if(choice < -1 || choice > 3){
				printf("\nSorry - That is not a valid choice! Please Try again.\n");
			}
		}while(choice != -1 && choice == getchar() != EOF);
	}
}//generateStatistics

void generateAgeStatistics(struct listElement* head_ptr, int number) {
	generateAgeSmokingStatistics(head_ptr, number);
	generateAgeExerciseStatistics(head_ptr, number);
	generateAgeAlcoholStatistics(head_ptr, number);	
}//generateAgeStatistics

void generateAgeSmokingStatistics(struct listElement* head_ptr, int number) {
	struct listElement *temp = head_ptr;
	//Counters
	int _20Count = 0, _30Count = 0, _50Count = 0, _65Count = 0, _UpWardsCount = 0;

	//integer arrays for all age bractets who don't smoke
	int _20Smoke[4] = { 0,0,0,0 }, _30Smoke[4] = { 0,0,0,0 }, _50Smoke[4] = { 0,0,0,0 }, _65Smoke[4] = { 0,0,0,0 }, _UpWardsSmoke[4] = { 0,0,0,0 };

	//variables for smokers
	int total18_20Smokers, total20_30Smokers, total30_50Smokers, total50_65Smokers, total65UpSmokers;
	int total18_20Less20, total20_30Less20, total30_50Less20, total50_65Less20, total65UpLess20;
	int total18_20Less40, total20_30Less40, total30_50Less40, total50_65Less40, total65UpLess40;
	int total18_20Greater40, total20_30Greater40, total30_50Greater40, total50_65Greater40, total65UpGreater40;
	//Total variables for smokers
	float percentage18_20Smokers, percentage20_30Smokers, percentage30_50Smokers, percentage50_65Smokers, percentage65UpSmokers;
	float percentage18_20Less20, percentage20_30Less20, percentage30_50Less20, percentage50_65Less20, percentage65UpLess20;
	float percentage18_20Less40, percentage20_30Less40, percentage30_50Less40, percentage50_65Less40, percentage65UpLess40;
	float percentage18_20Greater40, percentage20_30Greater40, percentage30_50Greater40, percentage50_65Greater40, percentage65UpGreater40;

	while (temp != NULL) {
		//counters for agebrackets
		if (temp->value.ageBracket == 1) _20Count++;
		if (temp->value.ageBracket == 2) _30Count++;
		if (temp->value.ageBracket == 3) _50Count++;
		if (temp->value.ageBracket == 4) _65Count++;
		if (temp->value.ageBracket == 5) _UpWardsCount++;

		if (temp->value.cigarettes == 1 && temp->value.ageBracket == 1) _20Smoke[0] += 1; //your 18-20 and don't smoke
		if (temp->value.cigarettes == 1 && temp->value.ageBracket == 2) _30Smoke[0] += 1; // 20-30 and don't smoke
		if (temp->value.cigarettes == 1 && temp->value.ageBracket == 3) _50Smoke[0] += 1; //30-50 and don't smoke
		if (temp->value.cigarettes == 1 && temp->value.ageBracket == 4) _65Smoke[0] += 1; //50-65 and don't smoke
		if (temp->value.cigarettes == 1 && temp->value.ageBracket == 5) _UpWardsSmoke[0] += 1; //65+ and don't smoke

		if (temp->value.cigarettes == 2 && temp->value.ageBracket == 1) _20Smoke[1] += 1; //your 18-20 and smoke < 20
		if (temp->value.cigarettes == 2 && temp->value.ageBracket == 2) _30Smoke[1] += 1; // 20-30 and and smoke < 20
		if (temp->value.cigarettes == 2 && temp->value.ageBracket == 3) _50Smoke[1] += 1; //30-50 and and smoke < 20
		if (temp->value.cigarettes == 2 && temp->value.ageBracket == 4) _65Smoke[1] += 1; //50-65 and and smoke < 20
		if (temp->value.cigarettes == 2 && temp->value.ageBracket == 5) _UpWardsSmoke[1] += 1; //65+ and smoke < 20

		if (temp->value.cigarettes == 3 && temp->value.ageBracket == 1) _20Smoke[2] += 1; //your 18-20 and smoke < 40
		if (temp->value.cigarettes == 3 && temp->value.ageBracket == 2) _30Smoke[2] += 1; // 20-30 and and smoke < 40
		if (temp->value.cigarettes == 3 && temp->value.ageBracket == 3) _50Smoke[2] += 1; //30-50 and and smoke < 40
		if (temp->value.cigarettes == 3 && temp->value.ageBracket == 4) _65Smoke[2] += 1; //50-65 and and smoke < 40
		if (temp->value.cigarettes == 3 && temp->value.ageBracket == 5) _UpWardsSmoke[2] += 1; //65+ and smoke < 40

		if (temp->value.cigarettes == 4 && temp->value.ageBracket == 1) _20Smoke[3] += 1; //your 18-20 and smoke > 20
		if (temp->value.cigarettes == 4 && temp->value.ageBracket == 2) _30Smoke[3] += 1; // 20-30 and and smoke > 20
		if (temp->value.cigarettes == 4 && temp->value.ageBracket == 3) _50Smoke[3] += 1; //30-50 and and smoke > 20
		if (temp->value.cigarettes == 4 && temp->value.ageBracket == 4) _65Smoke[3] += 1; //50-65 and and smoke > 20
		if (temp->value.cigarettes == 4 && temp->value.ageBracket == 5) _UpWardsSmoke[3] += 1; //65+ and smoke > 20
		temp = temp->next;
	}//while

	 //Calculate Smoking statistics for age brackets
	total18_20Smokers = _20Smoke[1] + _20Smoke[2] + _20Smoke[3];
	total20_30Smokers = _30Smoke[1] + _30Smoke[2] + _30Smoke[3];
	total30_50Smokers = _50Smoke[1] + _50Smoke[2] + _50Smoke[3];
	total50_65Smokers = _65Smoke[1] + _65Smoke[2] + _65Smoke[3];
	total65UpSmokers = _UpWardsSmoke[1] + _UpWardsSmoke[2] + _UpWardsSmoke[3];

	total18_20Less20 = _20Smoke[1];
	total20_30Less20 = _30Smoke[1];
	total30_50Less20 = _50Smoke[1];
	total50_65Less20 = _65Smoke[1];
	total65UpLess20 = _UpWardsSmoke[1];

	total18_20Less40 = _20Smoke[2];
	total20_30Less40 = _30Smoke[2];
	total30_50Less40 = _50Smoke[2];
	total50_65Less40 = _65Smoke[2];
	total65UpLess40 = _UpWardsSmoke[2];

	total18_20Greater40 = _20Smoke[3];
	total20_30Greater40 = _30Smoke[3];
	total30_50Greater40 = _50Smoke[3];
	total50_65Greater40 = _65Smoke[3];
	total65UpGreater40 = _UpWardsSmoke[3];

	//Totals for Smoking for all age brackets
	percentage18_20Smokers = ((float)total18_20Smokers / (float)_20Count) * 100;
	percentage20_30Smokers = ((float)total20_30Smokers / (float)_30Count) * 100;
	percentage30_50Smokers = ((float)total30_50Smokers / (float)_50Count) * 100;
	percentage50_65Smokers = ((float)total50_65Smokers / (float)_65Count) * 100;
	percentage65UpSmokers = ((float)total65UpSmokers / (float)_UpWardsCount) * 100;

	percentage18_20Less20 = ((float)total18_20Less20 / (float)_20Count) * 100;
	percentage20_30Less20 = ((float)total20_30Less20 / (float)_30Count) * 100;
	percentage30_50Less20 = ((float)total30_50Less20 / (float)_50Count) * 100;
	percentage50_65Less20 = ((float)total50_65Less20 / (float)_65Count) * 100;
	percentage65UpLess20 = ((float)total65UpLess20 / (float)_UpWardsCount) * 100;

	percentage18_20Less40 = ((float)total18_20Less40 / (float)_20Count) * 100;
	percentage20_30Less40 = ((float)total20_30Less40 / (float)_30Count) * 100;
	percentage30_50Less40 = ((float)total30_50Less40 / (float)_50Count) * 100;
	percentage50_65Less40 = ((float)total50_65Less40 / (float)_65Count) * 100;
	percentage65UpLess40 = ((float)total65UpLess40 / (float)_UpWardsCount) * 100;

	percentage18_20Greater40 = ((float)total18_20Greater40 / (float)_20Count) * 100;
	percentage20_30Greater40 = ((float)total20_30Greater40 / (float)_30Count) * 100;
	percentage30_50Greater40 = ((float)total30_50Greater40 / (float)_50Count) * 100;
	percentage50_65Greater40 = ((float)total50_65Greater40 / (float)_65Count) * 100;
	percentage65UpGreater40 = ((float)total65UpGreater40 / (float)_UpWardsCount) * 100;

	if (number == 0) {//only print to screen
		printf("==========================================================================\n");
		printf("\t\t\t  Age Statistics\n");
		printf("==========================================================================\n");
		if (_20Count == 0) printf("The total percentage of 18-20 year old people who smoke is: 0.0%%\n");
		else printf("The total percentage of 18-20 year old people who smoke is: %.2f%%\n", percentage18_20Smokers);
		if (_30Count == 0) printf("The total percentage of 20-30 year old people who smoke is: 0.0%%\n");
		else printf("The total percentage of 20-30 year old people who smoke is: %.2f%%\n", percentage20_30Smokers);
		if (_50Count == 0) printf("The total percentage of 30-50 year old people who smoke is: 0.0%%\n");
		else printf("The total percentage of 30-50 year old people who smoke is: %.2f%%\n", percentage30_50Smokers);
		if (_65Count == 0) printf("The total percentage of 50-65 year old people who smoke is:0.0%%\n");
		else printf("The total percentage of 50-65 year old people who smoke is: %.2f%%\n", percentage50_65Smokers);
		if (_UpWardsCount == 0) printf("The total percentage of 65+ year old people who smoke is: 0.0%%\n");
		else printf("The total percentage of 65+ year old people who smoke is: %.2f%%\n", percentage65UpSmokers);

		if (_20Count == 0) printf("The total percentage of 18-20 year old people who smoke less than 20 is: 0.0%%\n");
		else printf("The total percentage of 18-20 year old people who smoke less than 20 is: %.2f%%\n", percentage18_20Less20);
		if (_30Count == 0) printf("The total percentage of 20-30 year old people who smoke less than 20 is: 0.0%%\n");
		else printf("The total percentage of 20-30 year old people who smoke less than 20 is: %.2f%%\n", percentage20_30Less20);
		if (_50Count == 0) printf("The total percentage of 30-50 year old people who smoke less than 20 is: 0.0%%\n");
		else printf("The total percentage of 30-50 year old people who smoke less than 20 is: %.2f%%\n", percentage30_50Less20);
		if (_65Count == 0) printf("The total percentage of 50-65 year old people who smoke less than 20 is: 0.0%%\n");
		else printf("The total percentage of 50-65 year old people who smoke less than 20 is: %.2f%%\n", percentage50_65Less20);
		if (_UpWardsCount == 0) printf("The total percentage of 65+ year old people who smoke less than 20 is: 0.0%%\n");
		else printf("The total percentage of 65+ year old people who smoke less than 20 is: %.2f%%\n", percentage65UpLess20);

		if (_20Count == 0) printf("The total percentage of 18-20 year old people who smoke less than 40 is: 0.0%%\n");
		else printf("The total percentage of 18-20 year old people who smoke less than 40 is: %.2f%%\n", percentage18_20Less40);
		if (_30Count == 0) printf("The total percentage of 20-30 year old people who smoke less than 40 is: 0.0%%\n");
		else printf("The total percentage of 20-30 year old people who smoke less than 40 is: %.2f%%\n", percentage20_30Less40);
		if (_50Count == 0) printf("The total percentage of 30-50 year old people who smoke less than 40 is: 0.0%%\n");
		else printf("The total percentage of 30-50 year old people who smoke less than 40 is: %.2f%%\n", percentage30_50Less40);
		if (_65Count == 0) printf("The total percentage of 50-65 year old people who smoke less than 40 is: 0.0%%\n");
		else printf("The total percentage of 50-65 year old people who smoke less than 40 is: %.2f%%\n", percentage50_65Less40);
		if (_UpWardsCount == 0) printf("The total percentage of 65+ year old people who smoke less than 40 is: 0.0%%\n");
		else printf("The total percentage of 65+ year old people who smoke less than 40 is: %.2f%%\n", percentage65UpLess40);

		if (_20Count == 0) printf("The total percentage of 18-20 year old people who smoke greater than 40 is: 0.0%%\n");
		else printf("The total percentage of 18-20 year old people who smoke greater than 40 is: %.2f%%\n", percentage18_20Greater40);
		if (_30Count == 0) printf("The total percentage of 20-30 year old people who smoke greater than 40 is: 0.0%%\n");
		else printf("The total percentage of 20-30 year old people who smoke greater than 40 is: %.2f%%\n", percentage20_30Greater40);
		if (_50Count == 0) printf("The total percentage of 30-50 year old people who smoke greater than 40 is: 0.0%%\n");
		else printf("The total percentage of 30-50 year old people who smoke greater than 40 is: %.2f%%\n", percentage30_50Greater40);
		if (_65Count == 0) printf("The total percentage of 50-65 year old people who smoke greater than 40 is: 0.0%%\n");
		else printf("The total percentage of 50-65 year old people who smoke greater than 40 is: %.2f%%\n", percentage50_65Greater40);
		if (_UpWardsCount == 0) printf("The total percentage of 65+ year old people who smoke greater than 40 is: 0.0%%\n");
		else printf("The total percentage of 65+ year old people who smoke greater than 40 is: %.2f%%\n", percentage65UpGreater40);
	}
	/*PRINT SURVEY RECORDS TO THE SURVEY REPORT FILE
	*If the number passed down from the function call is a 1, we want to append the statistics to the end of the surveyReort.txt*/
	if (number == 1) {
		file_ptr = fopen("SurveyReport.txt", "a");//append statistics to the file

		if (file_ptr != NULL) {
			fprintf(file_ptr, "\n=============================================================\n");
			fprintf(file_ptr, "\t\t\tSurvey Statistics\n");
			fprintf(file_ptr, "=============================================================\n");
			fprintf(file_ptr, "\n==========================================================================\n");
			fprintf(file_ptr, "\t\t\t  Age Statistics\n");
			fprintf(file_ptr, "==========================================================================\n");
			if (_20Count == 0) fprintf(file_ptr, "The total percentage of 18-20 year old people who smoke is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 18-20 year old people who smoke is: %.2f%%\n", percentage18_20Smokers);
			if (_30Count == 0) fprintf(file_ptr, "The total percentage of 20-30 year old people who smoke is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 20-30 year old people who smoke is: %.2f%%\n", percentage20_30Smokers);
			if (_50Count == 0) fprintf(file_ptr, "The total percentage of 30-50 year old people who smoke is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 30-50 year old people who smoke is: %.2f%%\n", percentage30_50Smokers);
			if (_65Count == 0) fprintf(file_ptr, "The total percentage of 50-65 year old people who smoke is:0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 50-65 year old people who smoke is: %.2f%%\n", percentage50_65Smokers);
			if (_UpWardsCount == 0) fprintf(file_ptr, "The total percentage of 65+ year old people who smoke is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 65+ year old people who smoke is: %.2f%%\n", percentage65UpSmokers);

			if (_20Count == 0) fprintf(file_ptr, "The total percentage of 18-20 year old people who smoke less than 20 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 18-20 year old people who smoke less than 20 is: %.2f%%\n", percentage18_20Less20);
			if (_30Count == 0) fprintf(file_ptr, "The total percentage of 20-30 year old people who smoke less than 20 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 20-30 year old people who smoke less than 20 is: %.2f%%\n", percentage20_30Less20);
			if (_50Count == 0) fprintf(file_ptr, "The total percentage of 30-50 year old people who smoke less than 20 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 30-50 year old people who smoke less than 20 is: %.2f%%\n", percentage30_50Less20);
			if (_65Count == 0) fprintf(file_ptr, "The total percentage of 50-65 year old people who smoke less than 20 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 50-65 year old people who smoke less than 20 is: %.2f%%\n", percentage50_65Less20);
			if (_UpWardsCount == 0) fprintf(file_ptr, "The total percentage of 65+ year old people who smoke less than 20 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 65+ year old people who smoke less than 20 is: %.2f%%\n", percentage65UpLess20);

			if (_20Count == 0) fprintf(file_ptr, "The total percentage of 18-20 year old people who smoke less than 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 18-20 year old people who smoke less than 40 is: %.2f%%\n", percentage18_20Less40);
			if (_30Count == 0) fprintf(file_ptr, "The total percentage of 20-30 year old people who smoke less than 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 20-30 year old people who smoke less than 40 is: %.2f%%\n", percentage20_30Less40);
			if (_50Count == 0) fprintf(file_ptr, "The total percentage of 30-50 year old people who smoke less than 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 30-50 year old people who smoke less than 40 is: %.2f%%\n", percentage30_50Less40);
			if (_65Count == 0) fprintf(file_ptr, "The total percentage of 50-65 year old people who smoke less than 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 50-65 year old people who smoke less than 40 is: %.2f%%\n", percentage50_65Less40);
			if (_UpWardsCount == 0) fprintf(file_ptr, "The total percentage of 65+ year old people who smoke less than 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 65+ year old people who smoke less than 40 is: %.2f%%\n", percentage65UpLess40);

			if (_20Count == 0) fprintf(file_ptr, "The total percentage of 18-20 year old people who smoke greater than 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 18-20 year old people who smoke greater than 40 is: %.2f%%\n", percentage18_20Greater40);
			if (_30Count == 0) fprintf(file_ptr, "The total percentage of 20-30 year old people who smoke greater than 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 20-30 year old people who smoke greater than 40 is: %.2f%%\n", percentage20_30Greater40);
			if (_50Count == 0) fprintf(file_ptr, "The total percentage of 30-50 year old people who smoke greater than 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 30-50 year old people who smoke greater than 40 is: %.2f%%\n", percentage30_50Greater40);
			if (_65Count == 0) fprintf(file_ptr, "The total percentage of 50-65 year old people who smoke greater than 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 50-65 year old people who smoke greater than 40 is: %.2f%%\n", percentage50_65Greater40);
			if (_UpWardsCount == 0) fprintf(file_ptr, "The total percentage of 65+ year old people who smoke greater than 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 65+ year old people who smoke greater than 40 is: %.2f%%\n", percentage65UpGreater40);
		}
		else {
			printf("\nError accessing the file SurveyReport.txt on your system!");
		}
		fclose(file_ptr);//close the file
	}//if num = 1

}//generateAgeSmokingStatistics()

void generateAgeExerciseStatistics(struct listElement* head_ptr, int number) {
	struct listElement *temp = head_ptr;
	//Counters
	int _20Count = 0, _30Count = 0, _50Count = 0, _65Count = 0, _UpWardsCount = 0;

	//integer arrays for all age bractets who exercise
	int _20Exercise[4] = { 0,0,0,0 }, _30Exercise[4] = { 0,0,0,0 }, _50Exercise[4] = { 0,0,0,0 }, _65Exercise[4] = { 0,0,0,0 }, _UpWardsExercise[4] = { 0,0,0,0 };

	//variables for exercise
	int total18_20NoExercise, total20_30NoExercise, total30_50NoExercise, total50_65NoExercise, total65UpNoExercise;
	int total18_20Less3, total20_30Less3, total30_50Less3, total50_65Less3, total65UpLess3;
	int total18_20Less5, total20_30Less5, total30_50Less5, total50_65Less5, total65UpLess5;
	int total18_20Greater5, total20_30Greater5, total30_50Greater5, total50_65Greater5, total65UpGreater5;

	//Total variables for exercise
	float percentage18_20NoExercise, percentage20_30NoExercise, percentage30_50NoExercise, percentage50_65NoExercise, percentage65UpNoExercise;
	float percentage18_20Less3, percentage20_30Less3, percentage30_50Less3, percentage50_65Less3, percentage65UpLess3;
	float percentage18_20Less5, percentage20_30Less5, percentage30_50Less5, percentage50_65Less5, percentage65UpLess5;
	float percentage18_20Greater5, percentage20_30Greater5, percentage30_50Greater5, percentage50_65Greater5, percentage65UpGreater5;

	while (temp != NULL) {
		if (temp->value.ageBracket == 1) _20Count++;
		if (temp->value.ageBracket == 2) _30Count++;
		if (temp->value.ageBracket == 3) _50Count++;
		if (temp->value.ageBracket == 4) _65Count++;
		if (temp->value.ageBracket == 5) _UpWardsCount++;

		if (temp->value.excerise == 1 && temp->value.ageBracket == 1) _20Exercise[0] += 1; //your 18-20 and don't exercise
		if (temp->value.excerise == 1 && temp->value.ageBracket == 2) _30Exercise[0] += 1; // 20-30 and don't exercise
		if (temp->value.excerise == 1 && temp->value.ageBracket == 3) _50Exercise[0] += 1; //30-50 and don't exercise
		if (temp->value.excerise == 1 && temp->value.ageBracket == 4) _65Exercise[0] += 1; //50-65 and don't exercise
		if (temp->value.excerise == 1 && temp->value.ageBracket == 5) _UpWardsExercise[0] += 1; //65+ and don't exercise

		if (temp->value.excerise == 2 && temp->value.ageBracket == 1) _20Exercise[1] += 1; //your 18-20 and exercise less 3 
		if (temp->value.excerise == 2 && temp->value.ageBracket == 2) _30Exercise[1] += 1; // 20-30 and and exercise less 3 
		if (temp->value.excerise == 2 && temp->value.ageBracket == 3) _50Exercise[1] += 1; //30-50 and and exercise less 3 
		if (temp->value.excerise == 2 && temp->value.ageBracket == 4) _65Exercise[1] += 1; //50-65 and and exercise less 3 
		if (temp->value.excerise == 2 && temp->value.ageBracket == 5) _UpWardsExercise[1] += 1; //65+ and exercise less 3 

		if (temp->value.excerise == 3 && temp->value.ageBracket == 1) _20Exercise[2] += 1; //your 18-20 and exercise less 5 
		if (temp->value.excerise == 3 && temp->value.ageBracket == 2) _30Exercise[2] += 1; // 20-30 and and exercise less 5
		if (temp->value.excerise == 3 && temp->value.ageBracket == 3) _50Exercise[2] += 1; //30-50 and and exercise less 5
		if (temp->value.excerise == 3 && temp->value.ageBracket == 4) _65Exercise[2] += 1; //50-65 and and exercise less 5
		if (temp->value.excerise == 3 && temp->value.ageBracket == 5) _UpWardsExercise[2] += 1; //65+ and exercise less 5

		if (temp->value.excerise == 4 && temp->value.ageBracket == 1) _20Exercise[3] += 1; //your 18-20 and exercise > 5 
		if (temp->value.excerise == 4 && temp->value.ageBracket == 2) _30Exercise[3] += 1; // 20-30 and and exercise > 5 
		if (temp->value.excerise == 4 && temp->value.ageBracket == 3) _50Exercise[3] += 1; //30-50 and and exercise > 5 
		if (temp->value.excerise == 4 && temp->value.ageBracket == 4) _65Exercise[3] += 1; //50-65 and and exercise > 5 
		if (temp->value.excerise == 4 && temp->value.ageBracket == 5) _UpWardsExercise[3] += 1; //50-65 and exercise > 5 
		temp = temp->next;
	}//while

	 //Calculate Exercise statistics for age brackets
	total18_20NoExercise = _20Exercise[0];
	total20_30NoExercise = _30Exercise[0];
	total30_50NoExercise = _50Exercise[0];
	total50_65NoExercise = _65Exercise[0];
	total65UpNoExercise = _UpWardsExercise[0];
	
	total18_20Less3 = _20Exercise[1];
	total20_30Less3 = _30Exercise[1];
	total30_50Less3 = _50Exercise[1];
	total50_65Less3 = _65Exercise[1];
	total65UpLess3 = _UpWardsExercise[1];

	total18_20Less5 = _20Exercise[2];
	total20_30Less5 = _30Exercise[2];
	total30_50Less5 = _50Exercise[2];
	total50_65Less5 = _65Exercise[2];
	total65UpLess5 = _UpWardsExercise[2];

	total18_20Greater5 = _20Exercise[3];
	total20_30Greater5 = _30Exercise[3];
	total30_50Greater5 = _50Exercise[3];
	total50_65Greater5 = _65Exercise[3];
	total65UpGreater5 = _UpWardsExercise[3];

	//Total variables for Exercise
	percentage18_20NoExercise = ((float)total18_20NoExercise / (float)_20Count) * 100;
	percentage20_30NoExercise = ((float)total20_30NoExercise / (float)_30Count) * 100;
	percentage30_50NoExercise = ((float)total30_50NoExercise / (float)_50Count) * 100;
	percentage50_65NoExercise = ((float)total50_65NoExercise / (float)_65Count) * 100;
	percentage65UpNoExercise = ((float)total65UpNoExercise / (float)_UpWardsCount) * 100;

	percentage18_20Less3 = ((float)total18_20Less3 / (float)_20Count) * 100;
	percentage20_30Less3 = ((float)total20_30Less3 / (float)_30Count) * 100;
	percentage30_50Less3 = ((float)total30_50Less3 / (float)_50Count) * 100;
	percentage50_65Less3 = ((float)total50_65Less3 / (float)_65Count) * 100;
	percentage65UpLess3 = ((float)total65UpLess3 / (float)_UpWardsCount) * 100;

	percentage18_20Less5 = ((float)total18_20Less5 / (float)_20Count) * 100;
	percentage20_30Less5 = ((float)total20_30Less5 / (float)_30Count) * 100;
	percentage30_50Less5 = ((float)total30_50Less5 / (float)_50Count) * 100;
	percentage50_65Less5 = ((float)total50_65Less5 / (float)_65Count) * 100;
	percentage65UpLess5 = ((float)total65UpLess5 / (float)_UpWardsCount) * 100;

	percentage18_20Greater5 = ((float)total18_20Greater5 / (float)_20Count) * 100;
	percentage20_30Greater5 = ((float)total20_30Greater5 / (float)_30Count) * 100;
	percentage30_50Greater5 = ((float)total30_50Greater5 / (float)_50Count) * 100;
	percentage50_65Greater5 = ((float)total50_65Greater5 / (float)_65Count) * 100;
	percentage65UpGreater5 = ((float)total65UpGreater5 / (float)_UpWardsCount) * 100;
	
	if (number == 0) {
		printf("=========================================================================================\n");
		if (_20Count == 0) printf("The total percentage of 18-20 year old people who don't exercise is: 0.0%%\n");
		else printf("The total percentage of 18-20 year old people who don't exercise is: %.2f%%\n", percentage18_20NoExercise);
		if (_30Count == 0) printf("The total percentage of 20-30 year old people who don't exercise is: 0.0%%\n");
		else printf("The total percentage of 20-30 year old people who don't exercise is: %.2f%%\n", percentage20_30NoExercise);
		if (_50Count == 0) printf("The total percentage of 30-50 year old people who don't exercise is: 0.0%%\n");
		else printf("The total percentage of 30-50 year old people who don't exercise is: %.2f%%\n", percentage30_50NoExercise);
		if (_65Count == 0) printf("The total percentage of 50-65 year old people who don't exercise is:0.0%%\n");
		else printf("The total percentage of 50-65 year old people who don't exercise is: %.2f%%\n", percentage50_65NoExercise);
		if (_UpWardsCount == 0) printf("The total percentage of 65+ year old people who don't exercise is: 0.0%%\n");
		else printf("The total percentage of 65+ year old people who don't exercise is: %.2f%%\n", percentage65UpNoExercise);

		if (_20Count == 0) printf("The percentage of 18-20 year old people who exercise less than 3 times per week is: 0.0%%\n");
		else printf("The percentage of 18-20 year old people who exercise less than 3 times per week is: %.2f%%\n", percentage18_20Less3);
		if (_30Count == 0) printf("The percentage of 20-30 year old people who exercise less than 3 times per week is: 0.0%%\n");
		else printf("The percentage of 20-30 year old people who exercise less than 3 times per week is: %.2f%%\n", percentage20_30Less3);
		if (_50Count == 0) printf("The percentage of 30-50 year old people who exercise less than 3 times per week is: 0.0%%\n");
		else printf("The percentage of 30-50 year old people who exercise less than 3 times per week is: %.2f%%\n", percentage30_50Less3);
		if (_65Count == 0) printf("The percentage of 50-65 year old people who exercise less than 3 times per week is:0.0%%\n");
		else printf("The percentage of 50-65 year old people who exercise less than 3 times per week is: %.2f%%\n", percentage50_65Less3);
		if (_UpWardsCount == 0) printf("The percentage of 65+ year old people who exercise less than 3 times per week is: 0.0%%\n");
		else printf("The percentage of 65+ year old people who exercise less than 3 times per week is: %.2f%%\n", percentage65UpLess3);

		if (_20Count == 0) printf("The percentage of 18-20 year old people who exercise less than 5 times per week is: 0.0%%\n");
		else printf("The percentage of 18-20 year old people who exercise less than 5 times per week is: %.2f%%\n", percentage18_20Less5);
		if (_30Count == 0) printf("The percentage of 20-30 year old people who exercise less than 5 times per week is: 0.0%%\n");
		else printf("The percentage of 20-30 year old people who exercise less than 5 times per week is: %.2f%%\n", percentage20_30Less5);
		if (_50Count == 0) printf("The percentage of 30-50 year old people who exercise less than 5 times per week is: 0.0%%\n");
		else printf("The percentage of 30-50 year old people who exercise less than 5 times per week is: %.2f%%\n", percentage30_50Less5);
		if (_65Count == 0) printf("The percentage of 50-65 year old people who exercise less than 5 times per week is:0.0%%\n");
		else printf("The percentage of 50-65 year old people who exercise less than 5 times per week is: %.2f%%\n", percentage50_65Less5);
		if (_UpWardsCount == 0) printf("The percentage of 65+ year old people who exercise less than 5 times per week is: 0.0%%\n");
		else printf("The percentage of 65+ year old people who exercise less than 5 times per week is: %.2f%%\n", percentage65UpLess5);

		if (_20Count == 0) printf("The percentage of 18-20 year old people who exercise greater than 5 times per week is: 0.0%%\n");
		else printf("The percentage of 18-20 year old people who exercise greater than 5 times per week is: %.2f%%\n", percentage18_20Greater5);
		if (_30Count == 0) printf("The percentage of 20-30 year old people who exercise greater than 5 times per week is: 0.0%%\n");
		else printf("The percentage of 20-30 year old people who exercise greater than 5 times per week is: %.2f%%\n", percentage20_30Greater5);
		if (_50Count == 0) printf("The percentage of 30-50 year old people who exercise greater than 5 times per week is: 0.0%%\n");
		else printf("The percentage of 30-50 year old people who exercise greater than 5 times per week is: %.2f%%\n", percentage30_50Greater5);
		if (_65Count == 0) printf("The percentage of 50-65 year old people who exercise greater than 5 times per week is:0.0%%\n");
		else printf("The percentage of 50-65 year old people who exercise greater than 5 times per week is: %.2f%%\n", percentage50_65Greater5);
		if (_UpWardsCount == 0) printf("The percentage of 65+ year old people who exercise greater than 5 times per week is: 0.0%%\n");
		else printf("The percentage of 65+ year old people who exercise greater than 5 times per week is: %.2f%%\n", percentage65UpGreater5);
	}
	if (number == 1) {
		file_ptr = fopen("SurveyReport.txt", "a");//append statistics to the file

		if (file_ptr != NULL) {
			fprintf(file_ptr, "=========================================================================================\n");
			if (_20Count == 0) fprintf(file_ptr, "The total percentage of 18-20 year old people who don't exercise is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 18-20 year old people who don't exercise is: %.2f%%\n", percentage18_20NoExercise);
			if (_30Count == 0) fprintf(file_ptr, "The total percentage of 20-30 year old people who don't exercise is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 20-30 year old people who don't exercise is: %.2f%%\n", percentage20_30NoExercise);
			if (_50Count == 0) fprintf(file_ptr, "The total percentage of 30-50 year old people who don't exercise is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 30-50 year old people who don't exercise is: %.2f%%\n", percentage30_50NoExercise);
			if (_65Count == 0) fprintf(file_ptr, "The total percentage of 50-65 year old people who don't exercise is:0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 50-65 year old people who don't exercise is: %.2f%%\n", percentage50_65NoExercise);
			if (_UpWardsCount == 0) fprintf(file_ptr, "The total percentage of 65+ year old people who don't exercise is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 65+ year old people who don't exercise is: %.2f%%\n", percentage65UpNoExercise);

			if (_20Count == 0) fprintf(file_ptr, "The percentage of 18-20 year old people who exercise less than 3 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 18-20 year old people who exercise less than 3 times per week is: %.2f%%\n", percentage18_20Less3);
			if (_30Count == 0) fprintf(file_ptr, "The percentage of 20-30 year old people who exercise less than 3 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 20-30 year old people who exercise less than 3 times per week is: %.2f%%\n", percentage20_30Less3);
			if (_50Count == 0) fprintf(file_ptr, "The percentage of 30-50 year old people who exercise less than 3 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 30-50 year old people who exercise less than 3 times per week is: %.2f%%\n", percentage30_50Less3);
			if (_65Count == 0) fprintf(file_ptr, "The percentage of 50-65 year old people who exercise less than 3 times per week is:0.0%%\n");
			else fprintf(file_ptr, "The percentage of 50-65 year old people who exercise less than 3 times per week is: %.2f%%\n", percentage50_65Less3);
			if (_UpWardsCount == 0) fprintf(file_ptr, "The percentage of 65+ year old people who exercise less than 3 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 65+ year old people who exercise less than 3 times per week is: %.2f%%\n", percentage65UpLess3);

			if (_20Count == 0) fprintf(file_ptr, "The percentage of 18-20 year old people who exercise less than 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 18-20 year old people who exercise less than 5 times per week is: %.2f%%\n", percentage18_20Less5);
			if (_30Count == 0) fprintf(file_ptr, "The percentage of 20-30 year old people who exercise less than 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 20-30 year old people who exercise less than 5 times per week is: %.2f%%\n", percentage20_30Less5);
			if (_50Count == 0) fprintf(file_ptr, "The percentage of 30-50 year old people who exercise less than 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 30-50 year old people who exercise less than 5 times per week is: %.2f%%\n", percentage30_50Less5);
			if (_65Count == 0) fprintf(file_ptr, "The percentage of 50-65 year old people who exercise less than 5 times per week is:0.0%%\n");
			else fprintf(file_ptr, "The percentage of 50-65 year old people who exercise less than 5 times per week is: %.2f%%\n", percentage50_65Less5);
			if (_UpWardsCount == 0) fprintf(file_ptr, "The percentage of 65+ year old people who exercise less than 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 65+ year old people who exercise less than 5 times per week is: %.2f%%\n", percentage65UpLess5);

			if (_20Count == 0) fprintf(file_ptr, "The percentage of 18-20 year old people who exercise greater than 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 18-20 year old people who exercise greater than 5 times per week is: %.2f%%\n", percentage18_20Greater5);
			if (_30Count == 0) fprintf(file_ptr, "The percentage of 20-30 year old people who exercise greater than 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 20-30 year old people who exercise greater than 5 times per week is: %.2f%%\n", percentage20_30Greater5);
			if (_50Count == 0) fprintf(file_ptr, "The percentage of 30-50 year old people who exercise greater than 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 30-50 year old people who exercise greater than 5 times per week is: %.2f%%\n", percentage30_50Greater5);
			if (_65Count == 0) fprintf(file_ptr, "The percentage of 50-65 year old people who exercise greater than 5 times per week is:0.0%%\n");
			else fprintf(file_ptr, "The percentage of 50-65 year old people who exercise greater than 5 times per week is: %.2f%%\n", percentage50_65Greater5);
			if (_UpWardsCount == 0) fprintf(file_ptr, "The percentage of 65+ year old people who exercise greater than 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 65+ year old people who exercise greater than 5 times per week is: %.2f%%\n", percentage65UpGreater5);
		}
		else {
			printf("\nError Accessing that file on your system!");
		}
		fclose(file_ptr);
	}//if
}//generateAgeExerciseStatistics()

void generateAgeAlcoholStatistics(struct listElement* head_ptr, int number) {
	struct listElement *temp = head_ptr;
	//Counters
	int _20Count = 0, _30Count = 0, _50Count = 0, _65Count = 0, _UpWardsCount = 0;

	//integer arrays for all age bractets for alcohol
	int _20Alcohol[4] = { 0,0,0,0 }, _30Alcohol[4] = { 0,0,0,0 }, _50Alcohol[4] = { 0,0,0,0 }, _65Alcohol[4] = { 0,0,0,0 }, _UpWardsAlcohol[4] = { 0,0,0,0 };

	//variables for alcohol
	int total18_20NoAlcohol, total20_30NoAlcohol, total30_50NoAlcohol, total50_65NoAlcohol, total65UpNoAlcohol;
	int total18_20Less2, total20_30Less2, total30_50Less2, total50_65Less2, total65UpLess2;
	int total18_20Less4, total20_30Less4, total30_50Less4, total50_65Less4, total65UpLess4;
	int total18_20Greater4, total20_30Greater4, total30_50Greater4, total50_65Greater4, total65UpGreater4;

	//Total variables for alcohol
	float percentage18_20NoAlcohol, percentage20_30NoAlcohol, percentage30_50NoAlcohol, percentage50_65NoAlcohol, percentage65UpNoAlcohol;
	float percentage18_20Less2, percentage20_30Less2, percentage30_50Less2, percentage50_65Less2, percentage65UpLess2;
	float percentage18_20Less4, percentage20_30Less4, percentage30_50Less4, percentage50_65Less4, percentage65UpLess4;
	float percentage18_20Greater4, percentage20_30Greater4, percentage30_50Greater4, percentage50_65Greater4, percentage65UpGreater4;

	while (temp != NULL) {
		if (temp->value.ageBracket == 1) _20Count++;
		if (temp->value.ageBracket == 2) _30Count++;
		if (temp->value.ageBracket == 3) _50Count++;
		if (temp->value.ageBracket == 4) _65Count++;
		if (temp->value.ageBracket == 5) _UpWardsCount++;

		if (temp->value.alcohol == 1 && temp->value.ageBracket == 1) _20Alcohol[0] += 1; //your 18-20 and don't exercise
		if (temp->value.alcohol == 1 && temp->value.ageBracket == 2) _30Alcohol[0] += 1; // 20-30 and don't exercise
		if (temp->value.alcohol == 1 && temp->value.ageBracket == 3) _50Alcohol[0] += 1; //30-50 and don't exercise
		if (temp->value.alcohol == 1 && temp->value.ageBracket == 4) _65Alcohol[0] += 1; //50-65 and don't exercise
		if (temp->value.alcohol == 1 && temp->value.ageBracket == 5) _UpWardsAlcohol[0] += 1; //65+ and don't exercise

		if (temp->value.alcohol == 2 && temp->value.ageBracket == 1) _20Alcohol[1] += 1; //your 18-20 and exercise less 3 
		if (temp->value.alcohol == 2 && temp->value.ageBracket == 2) _30Alcohol[1] += 1; // 20-30 and and exercise less 3 
		if (temp->value.alcohol == 2 && temp->value.ageBracket == 3) _50Alcohol[1] += 1; //30-50 and and exercise less 3 
		if (temp->value.alcohol == 2 && temp->value.ageBracket == 4) _65Alcohol[1] += 1; //50-65 and and exercise less 3 
		if (temp->value.alcohol == 2 && temp->value.ageBracket == 5) _UpWardsAlcohol[1] += 1; //65+ and exercise less 3 

		if (temp->value.alcohol == 3 && temp->value.ageBracket == 1) _20Alcohol[2] += 1; //your 18-20 and exercise less 5 
		if (temp->value.alcohol == 3 && temp->value.ageBracket == 2) _30Alcohol[2] += 1; // 20-30 and and exercise less 5
		if (temp->value.alcohol == 3 && temp->value.ageBracket == 3) _50Alcohol[2] += 1; //30-50 and and exercise less 5
		if (temp->value.alcohol == 3 && temp->value.ageBracket == 4) _65Alcohol[2] += 1; //50-65 and and exercise less 5
		if (temp->value.alcohol == 3 && temp->value.ageBracket == 5) _UpWardsAlcohol[2] += 1; //65+ and exercise less 5

		if (temp->value.alcohol == 4 && temp->value.ageBracket == 1) _20Alcohol[3] += 1; //your 18-20 and exercise > 5 
		if (temp->value.alcohol == 4 && temp->value.ageBracket == 2) _30Alcohol[3] += 1; // 20-30 and and exercise > 5 
		if (temp->value.alcohol == 4 && temp->value.ageBracket == 3) _50Alcohol[3] += 1; //30-50 and and exercise > 5 
		if (temp->value.alcohol == 4 && temp->value.ageBracket == 4) _65Alcohol[3] += 1; //50-65 and and exercise > 5 
		if (temp->value.alcohol == 4 && temp->value.ageBracket == 5) _UpWardsAlcohol[3] += 1; //50-65 and exercise > 5 
		temp = temp->next;
	}//while

	 //Calculate Alcohol statistics for age brackets
	total18_20NoAlcohol = _20Alcohol[0];
	total20_30NoAlcohol = _30Alcohol[0];
	total30_50NoAlcohol = _50Alcohol[0];
	total50_65NoAlcohol = _65Alcohol[0];
	total65UpNoAlcohol = _UpWardsAlcohol[0];

	total18_20Less2 = _20Alcohol[1];
	total20_30Less2 = _30Alcohol[1];
	total30_50Less2 = _50Alcohol[1];
	total50_65Less2 = _65Alcohol[1];
	total65UpLess2 = _UpWardsAlcohol[1];

	total18_20Less4 = _20Alcohol[2];
	total20_30Less4 = _30Alcohol[2];
	total30_50Less4 = _50Alcohol[2];
	total50_65Less4 = _65Alcohol[2];
	total65UpLess4 = _UpWardsAlcohol[2];

	total18_20Greater4 = _20Alcohol[3];
	total20_30Greater4 = _30Alcohol[3];
	total30_50Greater4 = _50Alcohol[3];
	total50_65Greater4 = _65Alcohol[3];
	total65UpGreater4 = _UpWardsAlcohol[3];

	//Total variables for Alcohol
	percentage18_20NoAlcohol = ((float)total18_20NoAlcohol / (float)_20Count) * 100;
	percentage20_30NoAlcohol = ((float)total20_30NoAlcohol / (float)_30Count) * 100;
	percentage30_50NoAlcohol = ((float)total30_50NoAlcohol / (float)_50Count) * 100;
	percentage50_65NoAlcohol = ((float)total50_65NoAlcohol / (float)_65Count) * 100;
	percentage65UpNoAlcohol = ((float)total65UpNoAlcohol / (float)_UpWardsCount) * 100;

	percentage18_20Less2 = ((float)total18_20Less2 / (float)_20Count) * 100;
	percentage20_30Less2 = ((float)total20_30Less2 / (float)_30Count) * 100;
	percentage30_50Less2 = ((float)total30_50Less2 / (float)_50Count) * 100;
	percentage50_65Less2 = ((float)total50_65Less2 / (float)_65Count) * 100;
	percentage65UpLess2 = ((float)total65UpLess2 / (float)_UpWardsCount) * 100;

	percentage18_20Less4 = ((float)total18_20Less4 / (float)_20Count) * 100;
	percentage20_30Less4 = ((float)total20_30Less4 / (float)_30Count) * 100;
	percentage30_50Less4 = ((float)total30_50Less4 / (float)_50Count) * 100;
	percentage50_65Less4 = ((float)total50_65Less4 / (float)_65Count) * 100;
	percentage65UpLess4 = ((float)total65UpLess4 / (float)_UpWardsCount) * 100;

	percentage18_20Greater4 = ((float)total18_20Greater4 / (float)_20Count) * 100;
	percentage20_30Greater4 = ((float)total20_30Greater4 / (float)_30Count) * 100;
	percentage30_50Greater4 = ((float)total30_50Greater4 / (float)_50Count) * 100;
	percentage50_65Greater4 = ((float)total50_65Greater4 / (float)_65Count) * 100;
	percentage65UpGreater4 = ((float)total65UpGreater4 / (float)_UpWardsCount) * 100;

	if (number == 0) {
		printf("=========================================================================================\n");
		if (_20Count == 0) printf("The total percentage of 18-20 year old people who don't drink alcohol is: 0.0%%\n");
		else printf("The total percentage of 18-20 year old people who don't drink alcohol is: %.2f%%\n", percentage18_20NoAlcohol);
		if (_30Count == 0) printf("The total percentage of 20-30 year old people who don't drink alcohol is: 0.0%%\n");
		else printf("The total percentage of 20-30 year old people who don't drink alcohol is: %.2f%%\n", percentage20_30NoAlcohol);
		if (_50Count == 0) printf("The total percentage of 30-50 year old people who don't drink alcohol is: 0.0%%\n");
		else printf("The total percentage of 30-50 year old people who don't drink alcohol is: %.2f%%\n", percentage30_50NoAlcohol);
		if (_65Count == 0) printf("The total percentage of 50-65 year old people who don't drink alcohol is: 0.0%%\n");
		else printf("The total percentage of 50-65 year old people who don't drink alcohol is: %.2f%%\n", percentage50_65NoAlcohol);
		if (_UpWardsCount == 0) printf("The total percentage of 65+ year old people who don't drink alcohol is: 0.0%%\n");
		else printf("The total percentage of 65+ year old people who don't drink alcohol is: %.2f%%\n", percentage65UpNoAlcohol);

		if (_20Count == 0) printf("The percentage of 18-20 year old people who drink less than 2 units per week is: 0.0%%\n");
		else printf("The percentage of 18-20 year old people whodrink less than 2 units per week is: %.2f%%\n", percentage18_20Less2);
		if (_30Count == 0) printf("The percentage of 20-30 year old people who drink less than 2 units per week is: 0.0%%\n");
		else printf("The percentage of 20-30 year old people who drink less than 2 units per week is: %.2f%%\n", percentage20_30Less2);
		if (_50Count == 0) printf("The percentage of 30-50 year old people who drink less than 2 units per week is: 0.0%%\n");
		else printf("The percentage of 30-50 year old people who drink less than 2 units per week is: %.2f%%\n", percentage30_50Less2);
		if (_65Count == 0) printf("The percentage of 50-65 year old people who drink less than 2 units per week is: 0.0%%\n");
		else printf("The percentage of 50-65 year old people who drink less than 2 units per week is: %.2f%%\n", percentage50_65Less2);
		if (_UpWardsCount == 0) printf("The percentage of 65+ year old people who drink less than 2 units per week is: 0.0%%\n");
		else printf("The percentage of 65+ year old people who drink less than 2 units per week is: %.2f%%\n", percentage65UpLess2);

		if (_20Count == 0) printf("The percentage of 18-20 year old people who drink less than 4 units per week is: 0.0%%\n");
		else printf("The percentage of 18-20 year old people who drink less than 4 units per week is: %.2f%%\n", percentage18_20Less4);
		if (_30Count == 0) printf("The percentage of 20-30 year old people who drink less than 4 units per week is: 0.0%%\n");
		else printf("The percentage of 20-30 year old people who drink less than 4 units per week is: %.2f%%\n", percentage20_30Less4);
		if (_50Count == 0) printf("The percentage of 30-50 year old people who drink less than 4 units per week is: 0.0%%\n");
		else printf("The percentage of 30-50 year old people who drink less than 4 units per week is: %.2f%%\n", percentage30_50Less4);
		if (_65Count == 0) printf("The percentage of 50-65 year old people who drink less than 4 units per week is: 0.0%%\n");
		else printf("The percentage of 50-65 year old people who drink less than 4 units per week is: %.2f%%\n", percentage50_65Less4);
		if (_UpWardsCount == 0) printf("The percentage of 65+ year old people who drink less than 4 units per week is: 0.0%%\n");
		else printf("The percentage of 65+ year old people who drink less than 4 units per week is: %.2f%%\n", percentage65UpLess4);

		if (_20Count == 0) printf("The percentage of 18-20 year old people who drink more than 4 units per week is: 0.0%%\n");
		else printf("The percentage of 18-20 year old people who drink more than 4 units per week is: %.2f%%\n", percentage18_20Greater4);
		if (_30Count == 0) printf("The percentage of 20-30 year old people who drink more than 4 units per week is: 0.0%%\n");
		else printf("The percentage of 20-30 year old people who drink more than 4 units per week is: %.2f%%\n", percentage20_30Greater4);
		if (_50Count == 0) printf("The percentage of 30-50 year old people who drink more than 4 units per week is: 0.0%%\n");
		else printf("The percentage of 30-50 year old people who drink more than 4 units per week is: %.2f%%\n", percentage30_50Greater4);
		if (_65Count == 0) printf("The percentage of 50-65 year old people who drink more than 4 units per week is: 0.0%%\n");
		else printf("The percentage of 50-65 year old people who drink more than 4 units per week is: %.2f%%\n", percentage50_65Greater4);
		if (_UpWardsCount == 0) printf("The percentage of 65+ year old people who drink more than 4 units per week is: 0.0%%\n");
		else printf("The percentage of 65+ year old people who drink more than 4 units per week is: %.2f%%\n", percentage65UpGreater4);
	}
	if (number == 1) {
		file_ptr = fopen("SurveyReport.txt", "a");//append statistics to the file

		if (file_ptr != NULL) {
			fprintf(file_ptr, "=========================================================================================\n");
			if (_20Count == 0) fprintf(file_ptr, "The total percentage of 18-20 year old people who don't drink alcohol is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 18-20 year old people who don't drink alcohol is: %.2f%%\n", percentage18_20NoAlcohol);
			if (_30Count == 0) fprintf(file_ptr, "The total percentage of 20-30 year old people who don't drink alcohol is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 20-30 year old people who don't drink alcohol is: %.2f%%\n", percentage20_30NoAlcohol);
			if (_50Count == 0) fprintf(file_ptr, "The total percentage of 30-50 year old people who don't drink alcohol is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 30-50 year old people who don't drink alcohol is: %.2f%%\n", percentage30_50NoAlcohol);
			if (_65Count == 0) fprintf(file_ptr, "The total percentage of 50-65 year old people who don't drink alcohol is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 50-65 year old people who don't drink alcohol is: %.2f%%\n", percentage50_65NoAlcohol);
			if (_UpWardsCount == 0) fprintf(file_ptr, "The total percentage of 65+ year old people who don't drink alcohol is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of 65+ year old people who don't drink alcohol is: %.2f%%\n", percentage65UpNoAlcohol);

			if (_20Count == 0) fprintf(file_ptr, "The percentage of 18-20 year old people who drink less than 2 units per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 18-20 year old people whodrink less than 2 units per week is: %.2f%%\n", percentage18_20Less2);
			if (_30Count == 0) fprintf(file_ptr, "The percentage of 20-30 year old people who drink less than 2 units per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 20-30 year old people who drink less than 2 units per week is: %.2f%%\n", percentage20_30Less2);
			if (_50Count == 0) fprintf(file_ptr, "The percentage of 30-50 year old people who drink less than 2 units per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 30-50 year old people who drink less than 2 units per week is: %.2f%%\n", percentage30_50Less2);
			if (_65Count == 0) fprintf(file_ptr, "The percentage of 50-65 year old people who drink less than 2 units per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 50-65 year old people who drink less than 2 units per week is: %.2f%%\n", percentage50_65Less2);
			if (_UpWardsCount == 0) fprintf(file_ptr, "The percentage of 65+ year old people who drink less than 2 units per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 65+ year old people who drink less than 2 units per week is: %.2f%%\n", percentage65UpLess2);

			if (_20Count == 0) fprintf(file_ptr, "The percentage of 18-20 year old people who drink less than 4 units per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 18-20 year old people who drink less than 4 units per week is: %.2f%%\n", percentage18_20Less4);
			if (_30Count == 0) fprintf(file_ptr, "The percentage of 20-30 year old people who drink less than 4 units per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 20-30 year old people who drink less than 4 units per week is: %.2f%%\n", percentage20_30Less4);
			if (_50Count == 0) fprintf(file_ptr, "The percentage of 30-50 year old people who drink less than 4 units per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 30-50 year old people who drink less than 4 units per week is: %.2f%%\n", percentage30_50Less4);
			if (_65Count == 0) fprintf(file_ptr, "The percentage of 50-65 year old people who drink less than 4 units per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 50-65 year old people who drink less than 4 units per week is: %.2f%%\n", percentage50_65Less4);
			if (_UpWardsCount == 0) fprintf(file_ptr, "The percentage of 65+ year old people who drink less than 4 units per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 65+ year old people who drink less than 4 units per week is: %.2f%%\n", percentage65UpLess4);

			if (_20Count == 0) fprintf(file_ptr, "The percentage of 18-20 year old people who drink more than 4 units per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 18-20 year old people who drink more than 4 units per week is: %.2f%%\n", percentage18_20Greater4);
			if (_30Count == 0) fprintf(file_ptr, "The percentage of 20-30 year old people who drink more than 4 units per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 20-30 year old people who drink more than 4 units per week is: %.2f%%\n", percentage20_30Greater4);
			if (_50Count == 0) fprintf(file_ptr, "The percentage of 30-50 year old people who drink more than 4 units per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 30-50 year old people who drink more than 4 units per week is: %.2f%%\n", percentage30_50Greater4);
			if (_65Count == 0) fprintf(file_ptr, "The percentage of 50-65 year old people who drink more than 4 units per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 50-65 year old people who drink more than 4 units per week is: %.2f%%\n", percentage50_65Greater4);
			if (_UpWardsCount == 0) fprintf(file_ptr, "The percentage of 65+ year old people who drink more than 4 units per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of 65+ year old people who drink more than 4 units per week is: %.2f%%\n", percentage65UpGreater4);
		}
		else {
			printf("\nError accessing the file SurveyReport.txt on your system!");
		}
		fclose(file_ptr);
	}
}//generateAgeAlcoholStatistics()

void generateIncomeStatistics(struct listElement* head_ptr, int number) {
	generateIncomeSmokingStatistics(head_ptr, number);
	generateIncomeExerciseStatistics(head_ptr, number);
	generateIncomeAlcoholStatistics(head_ptr, number);
}//generateIncomeStatisitcs()

void generateIncomeSmokingStatistics(struct listElement* head_ptr, int number) {
	struct listElement *temp = head_ptr;
	//Counters
	int noIncomeCount = 0, less20Count = 0, less40Count = 0, less60Count = 0, less80Count = 0, less100Count = 0, greater100Count = 0;

	//integer arrays for all income brackets for smoking
	int noIncomeSmoke[4] = { 0,0,0,0 }, less20000Smoke[4] = { 0,0,0,0 }, less40000Smoke[4] = { 0,0,0,0 }, less60000Smoke[4] = { 0,0,0,0 },
		less80000Smoke[4] = { 0,0,0,0 }, less100000Smoke[4] = { 0,0,0,0 }, greater100000Smoke[4] = { 0,0,0,0 };

	//variables for smokers for all incomes
	int totalNoIncomeSmokers, totalLess20000Smokers, totalLess40000Smokers, totalLess60000Smokers, totalLess80000Smokers, totalLess100000Smokers, totalGreater100000Smokers;
	int totalNoIncomeLess20, totalLess20000Less20, totalLess40000Less20, totalLess60000Less20, totalLess80000Less20, totalLess100000Less20, totalGreater100000Less20;
	int totalNoIncomeLess40, totalLess20000Less40, totalLess40000Less40, totalLess60000Less40, totalLess80000Less40, totalLess100000Less40, totalGreater100000Less40;
	int totalNoIncomeGreater40, totalLess20000Greater40, totalLess40000Greater40, totalLess60000Greater40, totalLess80000Greater40, totalLess100000Greater40, totalGreater100000Greater40;

	//Total variables for smokers for all incomes
	float percentageNoIncomeSmokers, percentageLess20000Smokers, percentageLess40000Smokers, percentageLess60000Smokers, percentageLess80000Smokers, percentageLess100000Smokers, percentageGreater100000Smokers;
	float percentageNoIncomeLess20, percentageLess20000Less20, percentageLess40000Less20, percentageLess60000Less20, percentageLess80000Less20, percentageLess100000Less20, percentageGreater100000Less20;
	float percentageNoIncomeLess40, percentageLess20000Less40, percentageLess40000Less40, percentageLess60000Less40, percentageLess80000Less40, percentageLess100000Less40, percentageGreater100000Less40;
	float percentageNoIncomeGreater40, percentageLess20000Greater40, percentageLess40000Greater40, percentageLess60000Greater40, percentageLess80000Greater40, percentageLess100000Greater40, percentageGreater100000Greater40;

	while (temp != NULL) {
		//count the total number of income brackets in the list for each income bracket
		if (temp->value.incomeBracket == 1) noIncomeCount++;
		if (temp->value.incomeBracket == 2) less20Count++;
		if (temp->value.incomeBracket == 3) less40Count++;
		if (temp->value.incomeBracket == 4) less60Count++;
		if (temp->value.incomeBracket == 5) less80Count++;
		if (temp->value.incomeBracket == 6) less100Count++;
		if (temp->value.incomeBracket == 7) greater100Count++;

		if (temp->value.cigarettes == 1 && temp->value.incomeBracket == 1) noIncomeSmoke[0] += 1; //No income and don't smoke
		if (temp->value.cigarettes == 1 && temp->value.incomeBracket == 2) less20000Smoke[0] += 1; //<20000 and don't smoke
		if (temp->value.cigarettes == 1 && temp->value.incomeBracket == 3) less40000Smoke[0] += 1; //<40000 and don't smoke
		if (temp->value.cigarettes == 1 && temp->value.incomeBracket == 4) less60000Smoke[0] += 1; //<60000 and don't smoke
		if (temp->value.cigarettes == 1 && temp->value.incomeBracket == 5) less80000Smoke[0] += 1; //<80000 and don't smoke
		if (temp->value.cigarettes == 1 && temp->value.incomeBracket == 6) less100000Smoke[0] += 1; //<100000 and don't smoke
		if (temp->value.cigarettes == 1 && temp->value.incomeBracket == 7) greater100000Smoke[0] += 1; //>100000 and don't smoke

		if (temp->value.cigarettes == 2 && temp->value.incomeBracket == 1) noIncomeSmoke[1] += 1; //No income and smoke <20
		if (temp->value.cigarettes == 2 && temp->value.incomeBracket == 2) less20000Smoke[1] += 1; //<20000 and smoke <20
		if (temp->value.cigarettes == 2 && temp->value.incomeBracket == 3) less40000Smoke[1] += 1; //<40000 and smoke <20
		if (temp->value.cigarettes == 2 && temp->value.incomeBracket == 4) less60000Smoke[1] += 1; //<60000 and smoke <20
		if (temp->value.cigarettes == 2 && temp->value.incomeBracket == 5) less80000Smoke[1] += 1; //<80000 and smoke <20
		if (temp->value.cigarettes == 2 && temp->value.incomeBracket == 6) less100000Smoke[1] += 1; //<100000 and smoke <20
		if (temp->value.cigarettes == 2 && temp->value.incomeBracket == 7) greater100000Smoke[1] += 1; //>100000 and smoke <20

		if (temp->value.cigarettes == 3 && temp->value.incomeBracket == 1) noIncomeSmoke[2] += 1; //No income and smoke <40
		if (temp->value.cigarettes == 3 && temp->value.incomeBracket == 2) less20000Smoke[2] += 1; //<20000 and smoke <40
		if (temp->value.cigarettes == 3 && temp->value.incomeBracket == 3) less40000Smoke[2] += 1; //<40000 and smoke <40
		if (temp->value.cigarettes == 3 && temp->value.incomeBracket == 4) less60000Smoke[2] += 1; //<60000 and smoke <40
		if (temp->value.cigarettes == 3 && temp->value.incomeBracket == 5) less80000Smoke[2] += 1; //<80000 and smoke <40
		if (temp->value.cigarettes == 3 && temp->value.incomeBracket == 6) less100000Smoke[2] += 1; //<100000 and smoke <40
		if (temp->value.cigarettes == 3 && temp->value.incomeBracket == 7) greater100000Smoke[2] += 1; //>100000 and smoke <40

		if (temp->value.cigarettes == 4 && temp->value.incomeBracket == 1) noIncomeSmoke[3] += 1; //No income and smoke >40
		if (temp->value.cigarettes == 4 && temp->value.incomeBracket == 2) less20000Smoke[3] += 1; //<20000 and smoke >40
		if (temp->value.cigarettes == 4 && temp->value.incomeBracket == 3) less40000Smoke[3] += 1; //<40000 and smoke >40
		if (temp->value.cigarettes == 4 && temp->value.incomeBracket == 4) less60000Smoke[3] += 1; //<60000 and smoke >40
		if (temp->value.cigarettes == 4 && temp->value.incomeBracket == 5) less80000Smoke[3] += 1; //<80000 and smoke >40
		if (temp->value.cigarettes == 4 && temp->value.incomeBracket == 6) less100000Smoke[3] += 1; //<100000 and smoke >40
		if (temp->value.cigarettes == 4 && temp->value.incomeBracket == 7) greater100000Smoke[3] += 1; //>100000 and smoke >40
		temp = temp->next;
	}//while

	 //Calculate Smoking statistics for income brackets
	totalNoIncomeSmokers = noIncomeSmoke[1] + noIncomeSmoke[2] + noIncomeSmoke[3];
	totalLess20000Smokers = less20000Smoke[1] + less20000Smoke[2] + less20000Smoke[3];
	totalLess40000Smokers = less40000Smoke[1] + less40000Smoke[2] + less40000Smoke[3];
	totalLess60000Smokers = less60000Smoke[1] + less60000Smoke[2] + less60000Smoke[3];
	totalLess80000Smokers = less80000Smoke[1] + less80000Smoke[2] + less80000Smoke[3];
	totalLess100000Smokers = less100000Smoke[1] + less100000Smoke[2] + less100000Smoke[3];
	totalGreater100000Smokers = greater100000Smoke[1] + greater100000Smoke[2] + greater100000Smoke[3];

	totalNoIncomeLess20 = noIncomeSmoke[1];
	totalLess20000Less20 = less20000Smoke[1];
	totalLess40000Less20 = less40000Smoke[1];
	totalLess60000Less20 = less60000Smoke[1];
	totalLess80000Less20 = less80000Smoke[1];
	totalLess100000Less20 = less100000Smoke[1];
	totalGreater100000Less20 = greater100000Smoke[1];

	totalNoIncomeLess40 = noIncomeSmoke[2];
	totalLess20000Less40 = less20000Smoke[2];
	totalLess40000Less40 = less40000Smoke[2];
	totalLess60000Less40 = less60000Smoke[2];
	totalLess80000Less40 = less80000Smoke[2];
	totalLess100000Less40 = less100000Smoke[2];
	totalGreater100000Less40 = greater100000Smoke[2];

	totalNoIncomeGreater40 = noIncomeSmoke[3];
	totalLess20000Greater40 = less20000Smoke[3];
	totalLess40000Greater40 = less40000Smoke[3];
	totalLess60000Greater40 = less60000Smoke[3];
	totalLess80000Greater40 = less80000Smoke[3];
	totalLess100000Greater40 = less100000Smoke[3];
	totalGreater100000Greater40 = greater100000Smoke[3];
	
	//Totals for Smoking for all income brackets
	percentageNoIncomeSmokers = ((float)totalNoIncomeSmokers / (float)noIncomeCount) * 100;
	percentageLess20000Smokers = ((float)totalLess20000Smokers / (float)less20Count) * 100;
	percentageLess40000Smokers = ((float)totalLess40000Smokers / (float)less40Count) * 100;
	percentageLess60000Smokers = ((float)totalLess60000Smokers / (float)less60Count) * 100;
	percentageLess80000Smokers = ((float)totalLess80000Smokers / (float)less80Count) * 100;
	percentageLess100000Smokers = ((float)totalLess100000Smokers / (float)less100Count) * 100;
	percentageGreater100000Smokers = ((float)totalGreater100000Smokers / (float)greater100Count) * 100;

	percentageNoIncomeLess20 = ((float)totalNoIncomeLess20 / (float)noIncomeCount) * 100;
	percentageLess20000Less20 = ((float)totalLess20000Less20 / (float)less20Count) * 100;
	percentageLess40000Less20= ((float)totalLess40000Less20 / (float)less40Count) * 100;
	percentageLess60000Less20 = ((float)totalLess60000Less20 / (float)less60Count) * 100;
	percentageLess80000Less20 = ((float)totalLess80000Less20 / (float)less80Count) * 100;
	percentageLess100000Less20 = ((float)totalLess100000Less20 / (float)less100Count) * 100;
	percentageGreater100000Less20 = ((float)totalGreater100000Less20 / (float)greater100Count) * 100;

	percentageNoIncomeLess40 = ((float)totalNoIncomeLess40 / (float)noIncomeCount) * 100;
	percentageLess20000Less40 = ((float)totalLess20000Less40 / (float)less20Count) * 100;
	percentageLess40000Less40 = ((float)totalLess40000Less40 / (float)less40Count) * 100;
	percentageLess60000Less40 = ((float)totalLess60000Less40 / (float)less60Count) * 100;
	percentageLess80000Less40 = ((float)totalLess80000Less40 / (float)less80Count) * 100;
	percentageLess100000Less40 = ((float)totalLess100000Less40 / (float)less100Count) * 100;
	percentageGreater100000Less40 = ((float)totalGreater100000Less40 / (float)greater100Count) * 100;

	percentageNoIncomeGreater40 = ((float)totalNoIncomeGreater40 / (float)noIncomeCount) * 100;
	percentageLess20000Greater40 = ((float)totalLess20000Greater40 / (float)less20Count) * 100;
	percentageLess40000Greater40 = ((float)totalLess40000Greater40 / (float)less40Count) * 100;
	percentageLess60000Greater40 = ((float)totalLess60000Greater40 / (float)less60Count) * 100;
	percentageLess80000Greater40 = ((float)totalLess80000Greater40 / (float)less80Count) * 100;
	percentageLess100000Greater40 = ((float)totalLess100000Greater40 / (float)less100Count) * 100;
	percentageGreater100000Greater40 = ((float)totalGreater100000Greater40 / (float)greater100Count) * 100;

	if (number == 0) {
		printf("==========================================================================\n");
		printf("\t\t\t  Income Statistics\n");
		printf("==========================================================================\n");
		if (noIncomeCount == 0) printf("The total percentage of people with no income who smoke is: 0.0%%\n");
		else printf("The total percentage of people with no income who smoke is: %.2f%%\n", percentageNoIncomeSmokers);
		if (less20Count == 0) printf("The total percentage of people who earn < 20000 and who smoke is: 0.0%%\n");
		else printf("The total percentage of people who earn < 20000 and who smoke is: %.2f%%\n", percentageLess20000Smokers);
		if (less40Count == 0) printf("The total percentage of people who earn < 40000 and who smoke is: 0.0%%\n");
		else printf("The total percentage of people who earn < 40000 and who smoke is: %.2f%%\n", percentageLess40000Smokers);
		if (less60Count == 0) printf("The total percentage of people who earn < 60000 and who smoke is: 0.0%%\n");
		else printf("The total percentage of people who earn < 60000 and who smoke is: %.2f%%\n", percentageLess60000Smokers);
		if (less80Count == 0) printf("The total percentage of people who earn < 80000 and who smoke is: 0.0%%\n");
		else printf("The total percentage of people who earn < 80000 and who smoke is: %.2f%%\n", percentageLess80000Smokers);
		if (less100Count == 0) printf("The total percentage of people who earn < 100000 and who smoke is: 0.0%%\n");
		else printf("The total percentage of people who earn < 100000 and who smoke is: %.2f%%\n", percentageLess100000Smokers);
		if (greater100Count == 0) printf("The total percentage of people who earn > 100000 and who smoke is: 0.0%%\n");
		else printf("The total percentage of people who earn > 100000 and who smoke is: %.2f%%\n", percentageGreater100000Smokers);

		if (noIncomeCount == 0) printf("The total percentage of people with no income and who smoke < 20 is: 0.0%%\n");
		else printf("The total percentage of people with no income and who smoke < 20 is: %.2f%%\n", percentageNoIncomeLess20);
		if (less20Count == 0) printf("The total percentage of people who earn < 20000 and who smoke < 20 is : 0.0%%\n");
		else printf("The total percentage of people who earn < 20000 and who smoke < 20 is: %.2f%%\n", percentageLess20000Less20);
		if (less40Count == 0) printf("The total percentage of people who earn < 40000 and who smoke < 20 is: 0.0%%\n");
		else printf("The total percentage of people who earn < 40000 and who smoke < 20 is: %.2f%%\n", percentageLess40000Less20);
		if (less60Count == 0) printf("The total percentage of people who earn < 60000 and who smoke < 20 is: 0.0%%\n");
		else printf("The total percentage of people who earn < 60000 and who smoke < 20 is: %.2f%%\n", percentageLess60000Less20);
		if (less80Count == 0) printf("The total percentage of people who earn < 80000 and who smoke < 20 is: 0.0%%\n");
		else printf("The total percentage of people who earn < 80000 and who smoke < 20 is: %.2f%%\n", percentageLess80000Less20);
		if (less100Count == 0) printf("The total percentage of people who earn < 100000 and who smoke < 20 is: 0.0%%\n");
		else printf("The total percentage of people who earn < 100000 and who smoke < 20 is: %.2f%%\n", percentageLess100000Less20);
		if (greater100Count == 0) printf("The total percentage of people who earn > 100000 and who smoke < 20 is: 0.0%%\n");
		else printf("The total percentage of people who earn > 100000 and who smoke < 20 is: %.2f%%\n", percentageGreater100000Less20);

		if (noIncomeCount == 0) printf("The total percentage of people with no income and who smoke < 40 is: 0.0%%\n");
		else printf("The total percentage of people with no income and who smoke < 40 is: %.2f%%\n", percentageNoIncomeLess40);
		if (less20Count == 0) printf("The total percentage of people who earn < 20000 and who smoke < 40 is : 0.0%%\n");
		else printf("The total percentage of people who earn < 20000 and who smoke < 40 is: %.2f%%\n", percentageLess20000Less40);
		if (less40Count == 0) printf("The total percentage of people who earn < 40000 and who smoke < 40 is: 0.0%%\n");
		else printf("The total percentage of people who earn < 40000 and who smoke < 40 is: %.2f%%\n", percentageLess40000Less40);
		if (less60Count == 0) printf("The total percentage of people who earn < 60000 and who smoke < 40 is: 0.0%%\n");
		else printf("The total percentage of people who earn < 60000 and who smoke < 40 is: %.2f%%\n", percentageLess60000Less40);
		if (less80Count == 0) printf("The total percentage of people who earn < 80000 and who smoke < 40 is: 0.0%%\n");
		else printf("The total percentage of people who earn < 80000 and who smoke < 40 is: %.2f%%\n", percentageLess80000Less40);
		if (less100Count == 0) printf("The total percentage of people who earn < 100000 and who smoke < 40 is: 0.0%%\n");
		else printf("The total percentage of people who earn < 100000 and who smoke < 40 is: %.2f%%\n", percentageLess100000Less40);
		if (greater100Count == 0) printf("The total percentage of people who earn > 100000 and who smoke < 40 is: 0.0%%\n");
		else printf("The total percentage of people who earn > 100000 and who smoke < 40 is: %.2f%%\n", percentageGreater100000Less40);

		if (noIncomeCount == 0) printf("The total percentage of people with no income and who smoke > 40 is: 0.0%%\n");
		else printf("The total percentage of people with no income and who smoke > 40 is: %.2f%%\n", percentageNoIncomeGreater40);
		if (less20Count == 0) printf("The total percentage of people who earn < 20000 and who smoke > 40 is : 0.0%%\n");
		else printf("The total percentage of people who earn < 20000 and who smoke > 40 is: %.2f%%\n", percentageLess20000Greater40);
		if (less40Count == 0) printf("The total percentage of people who earn < 40000 and who smoke > 40 is: 0.0%%\n");
		else printf("The total percentage of people who earn < 40000 and who smoke > 40 is: %.2f%%\n", percentageLess40000Greater40);
		if (less60Count == 0) printf("The total percentage of people who earn < 60000 and who smoke > 40 is: 0.0%%\n");
		else printf("The total percentage of people who earn < 60000 and who smoke > 40 is: %.2f%%\n", percentageLess60000Greater40);
		if (less80Count == 0) printf("The total percentage of people who earn < 80000 and who smoke > 40 is: 0.0%%\n");
		else printf("The total percentage of people who earn < 80000 and who smoke > 40 is: %.2f%%\n", percentageLess80000Greater40);
		if (less100Count == 0) printf("The total percentage of people who earn < 100000 and who smoke > 40 is: 0.0%%\n");
		else printf("The total percentage of people who earn < 100000 and who smoke > 40 is: %.2f%%\n", percentageLess100000Greater40);
		if (greater100Count == 0) printf("The total percentage of people who earn > 100000 and who smoke > 40 is: 0.0%%\n");
		else printf("The total percentage of people who earn > 100000 and who smoke > 40 is: %.2f%%\n", percentageGreater100000Greater40);
	}
	if (number == 1) {
		file_ptr = fopen("SurveyReport.txt", "a");//append statistics to the file

		if (file_ptr != NULL) {
			fprintf(file_ptr, "\n==========================================================================\n");
			fprintf(file_ptr, "\t\t\t  Income Statistics\n");
			fprintf(file_ptr, "==========================================================================\n");
			if (noIncomeCount == 0) fprintf(file_ptr, "The total percentage of people with no income who smoke is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people with no income who smoke is: %.2f%%\n", percentageNoIncomeSmokers);
			if (less20Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 20000 and who smoke is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 20000 and who smoke is: %.2f%%\n", percentageLess20000Smokers);
			if (less40Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 40000 and who smoke is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 40000 and who smoke is: %.2f%%\n", percentageLess40000Smokers);
			if (less60Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 60000 and who smoke is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 60000 and who smoke is: %.2f%%\n", percentageLess60000Smokers);
			if (less80Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 80000 and who smoke is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 80000 and who smoke is: %.2f%%\n", percentageLess80000Smokers);
			if (less100Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 100000 and who smoke is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 100000 and who smoke is: %.2f%%\n", percentageLess100000Smokers);
			if (greater100Count == 0) fprintf(file_ptr, "The total percentage of people who earn > 100000 and who smoke is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn > 100000 and who smoke is: %.2f%%\n", percentageGreater100000Smokers);

			if (noIncomeCount == 0) fprintf(file_ptr, "The total percentage of people with no income and who smoke < 20 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people with no income and who smoke < 20 is: %.2f%%\n", percentageNoIncomeLess20);
			if (less20Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 20000 and who smoke < 20 is : 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 20000 and who smoke < 20 is: %.2f%%\n", percentageLess20000Less20);
			if (less40Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 40000 and who smoke < 20 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 40000 and who smoke < 20 is: %.2f%%\n", percentageLess40000Less20);
			if (less60Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 60000 and who smoke < 20 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 60000 and who smoke < 20 is: %.2f%%\n", percentageLess60000Less20);
			if (less80Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 80000 and who smoke < 20 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 80000 and who smoke < 20 is: %.2f%%\n", percentageLess80000Less20);
			if (less100Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 100000 and who smoke < 20 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 100000 and who smoke < 20 is: %.2f%%\n", percentageLess100000Less20);
			if (greater100Count == 0) fprintf(file_ptr, "The total percentage of people who earn > 100000 and who smoke < 20 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn > 100000 and who smoke < 20 is: %.2f%%\n", percentageGreater100000Less20);

			if (noIncomeCount == 0) fprintf(file_ptr, "The total percentage of people with no income and who smoke < 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people with no income and who smoke < 40 is: %.2f%%\n", percentageNoIncomeLess40);
			if (less20Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 20000 and who smoke < 40 is : 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 20000 and who smoke < 40 is: %.2f%%\n", percentageLess20000Less40);
			if (less40Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 40000 and who smoke < 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 40000 and who smoke < 40 is: %.2f%%\n", percentageLess40000Less40);
			if (less60Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 60000 and who smoke < 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 60000 and who smoke < 40 is: %.2f%%\n", percentageLess60000Less40);
			if (less80Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 80000 and who smoke < 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 80000 and who smoke < 40 is: %.2f%%\n", percentageLess80000Less40);
			if (less100Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 100000 and who smoke < 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 100000 and who smoke < 40 is: %.2f%%\n", percentageLess100000Less40);
			if (greater100Count == 0) fprintf(file_ptr, "The total percentage of people who earn > 100000 and who smoke < 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn > 100000 and who smoke < 40 is: %.2f%%\n", percentageGreater100000Less40);

			if (noIncomeCount == 0) fprintf(file_ptr, "The total percentage of people with no income and who smoke > 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people with no income and who smoke > 40 is: %.2f%%\n", percentageNoIncomeGreater40);
			if (less20Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 20000 and who smoke > 40 is : 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 20000 and who smoke > 40 is: %.2f%%\n", percentageLess20000Greater40);
			if (less40Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 40000 and who smoke > 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 40000 and who smoke > 40 is: %.2f%%\n", percentageLess40000Greater40);
			if (less60Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 60000 and who smoke > 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 60000 and who smoke > 40 is: %.2f%%\n", percentageLess60000Greater40);
			if (less80Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 80000 and who smoke > 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 80000 and who smoke > 40 is: %.2f%%\n", percentageLess80000Greater40);
			if (less100Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 100000 and who smoke > 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 100000 and who smoke > 40 is: %.2f%%\n", percentageLess100000Greater40);
			if (greater100Count == 0) fprintf(file_ptr, "The total percentage of people who earn > 100000 and who smoke > 40 is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn > 100000 and who smoke > 40 is: %.2f%%\n", percentageGreater100000Greater40);
		}
		else {
			printf("\nError Accessing the file on your system");
		}
		fclose(file_ptr);
	}
}//generateIncomeSmokingStatistics()

void generateIncomeExerciseStatistics(struct listElement* head_ptr, int number) {
	struct listElement *temp = head_ptr;
	//Counters
	int noIncomeCount = 0, less20Count = 0, less40Count = 0, less60Count = 0, less80Count = 0, less100Count = 0, greater100Count = 0;

	//integer arrays for all income brackets for exercise
	int noIncomeExercise[4] = { 0,0,0,0 }, less20000Exercise[4] = { 0,0,0,0 }, less40000Exercise[4] = { 0,0,0,0 }, less60000Exercise[4] = { 0,0,0,0 },
		less80000Exercise[4] = { 0,0,0,0 }, less100000Exercise[4] = { 0,0,0,0 }, greater100000Exercise[4] = { 0,0,0,0 };

	//variables for smokers for all incomes
	int totalNoIncomeNoExercise, totalLess20000NoExercise, totalLess40000NoExercise, totalLess60000NoExercise, totalLess80000NoExercise, totalLess100000NoExercise, totalGreater100000NoExercise;
	int totalNoIncomeLess3, totalLess20000Less3, totalLess40000Less3, totalLess60000Less3, totalLess80000Less3, totalLess100000Less3, totalGreater100000Less3;
	int totalNoIncomeLess5, totalLess20000Less5, totalLess40000Less5, totalLess60000Less5, totalLess80000Less5, totalLess100000Less5, totalGreater100000Less5;
	int totalNoIncomeGreater5, totalLess20000Greater5, totalLess40000Greater5, totalLess60000Greater5, totalLess80000Greater5, totalLess100000Greater5, totalGreater100000Greater5;

	//Total variables for smokers for all incomes
	float percentageNoIncomeNoExercise, percentageLess20000NoExercise, percentageLess40000NoExercise, percentageLess60000NoExercise, percentageLess80000NoExercise, percentageLess100000NoExercise, percentageGreater100000NoExercise;
	float percentageNoIncomeLess3, percentageLess20000Less3, percentageLess40000Less3, percentageLess60000Less3, percentageLess80000Less3, percentageLess100000Less3, percentageGreater100000Less3;
	float percentageNoIncomeLess5, percentageLess20000Less5, percentageLess40000Less5, percentageLess60000Less5, percentageLess80000Less5, percentageLess100000Less5, percentageGreater100000Less5;
	float percentageNoIncomeGreater5, percentageLess20000Greater5, percentageLess40000Greater5, percentageLess60000Greater5, percentageLess80000Greater5, percentageLess100000Greater5, percentageGreater100000Greater5;

	while (temp != NULL) {
		//count the total number of income brackets in the list for each income bracket
		if (temp->value.incomeBracket == 1) noIncomeCount++;
		if (temp->value.incomeBracket == 2) less20Count++;
		if (temp->value.incomeBracket == 3) less40Count++;
		if (temp->value.incomeBracket == 4) less60Count++;
		if (temp->value.incomeBracket == 5) less80Count++;
		if (temp->value.incomeBracket == 6) less100Count++;
		if (temp->value.incomeBracket == 7) greater100Count++;

		if (temp->value.excerise == 1 && temp->value.incomeBracket == 1) noIncomeExercise[0] += 1; //No income and don't smoke
		if (temp->value.excerise == 1 && temp->value.incomeBracket == 2) less20000Exercise[0] += 1; //<20000 and don't smoke
		if (temp->value.excerise == 1 && temp->value.incomeBracket == 3) less40000Exercise[0] += 1; //<40000 and don't smoke
		if (temp->value.excerise == 1 && temp->value.incomeBracket == 4) less60000Exercise[0] += 1; //<60000 and don't smoke
		if (temp->value.excerise == 1 && temp->value.incomeBracket == 5) less80000Exercise[0] += 1; //<80000 and don't smoke
		if (temp->value.excerise == 1 && temp->value.incomeBracket == 6) less100000Exercise[0] += 1; //<100000 and don't smoke
		if (temp->value.excerise == 1 && temp->value.incomeBracket == 7) greater100000Exercise[0] += 1; //>100000 and don't smoke

		if (temp->value.excerise == 2 && temp->value.incomeBracket == 1) noIncomeExercise[1] += 1; //No income and smoke <20
		if (temp->value.excerise == 2 && temp->value.incomeBracket == 2) less20000Exercise[1] += 1; //<20000 and smoke <20
		if (temp->value.excerise == 2 && temp->value.incomeBracket == 3) less40000Exercise[1] += 1; //<40000 and smoke <20
		if (temp->value.excerise == 2 && temp->value.incomeBracket == 4) less60000Exercise[1] += 1; //<60000 and smoke <20
		if (temp->value.excerise == 2 && temp->value.incomeBracket == 5) less80000Exercise[1] += 1; //<80000 and smoke <20
		if (temp->value.excerise == 2 && temp->value.incomeBracket == 6) less100000Exercise[1] += 1; //<100000 and smoke <20
		if (temp->value.excerise == 2 && temp->value.incomeBracket == 7) greater100000Exercise[1] += 1; //>100000 and smoke <20

		if (temp->value.excerise == 3 && temp->value.incomeBracket == 1) noIncomeExercise[2] += 1; //No income and smoke <40
		if (temp->value.excerise == 3 && temp->value.incomeBracket == 2) less20000Exercise[2] += 1; //<20000 and smoke <40
		if (temp->value.excerise == 3 && temp->value.incomeBracket == 3) less40000Exercise[2] += 1; //<40000 and smoke <40
		if (temp->value.excerise == 3 && temp->value.incomeBracket == 4) less60000Exercise[2] += 1; //<60000 and smoke <40
		if (temp->value.excerise == 3 && temp->value.incomeBracket == 5) less80000Exercise[2] += 1; //<80000 and smoke <40
		if (temp->value.excerise == 3 && temp->value.incomeBracket == 6) less100000Exercise[2] += 1; //<100000 and smoke <40
		if (temp->value.excerise == 3 && temp->value.incomeBracket == 7) greater100000Exercise[2] += 1; //>100000 and smoke <40

		if (temp->value.excerise == 4 && temp->value.incomeBracket == 1) noIncomeExercise[3] += 1; //No income and smoke >40
		if (temp->value.excerise == 4 && temp->value.incomeBracket == 2) less20000Exercise[3] += 1; //<20000 and smoke >40
		if (temp->value.excerise == 4 && temp->value.incomeBracket == 3) less40000Exercise[3] += 1; //<40000 and smoke >40
		if (temp->value.excerise == 4 && temp->value.incomeBracket == 4) less60000Exercise[3] += 1; //<60000 and smoke >40
		if (temp->value.excerise == 4 && temp->value.incomeBracket == 5) less80000Exercise[3] += 1; //<80000 and smoke >40
		if (temp->value.excerise == 4 && temp->value.incomeBracket == 6) less100000Exercise[3] += 1; //<100000 and smoke >40
		if (temp->value.excerise == 4 && temp->value.incomeBracket == 7) greater100000Exercise[3] += 1; //>100000 and smoke >40
		temp = temp->next;
	}//while

	 //Calculate Exercise statistics for income brackets
	totalNoIncomeNoExercise = noIncomeExercise[0];
	totalLess20000NoExercise = less20000Exercise[0];
	totalLess40000NoExercise = less40000Exercise[0];
	totalLess60000NoExercise = less60000Exercise[0];
	totalLess80000NoExercise = less80000Exercise[0];
	totalLess100000NoExercise = less100000Exercise[0];
	totalGreater100000NoExercise = greater100000Exercise[0];

	totalNoIncomeLess3 = noIncomeExercise[1];
	totalLess20000Less3 = less20000Exercise[1];
	totalLess40000Less3 = less40000Exercise[1];
	totalLess60000Less3 = less60000Exercise[1];
	totalLess80000Less3 = less80000Exercise[1];
	totalLess100000Less3 = less100000Exercise[1];
	totalGreater100000Less3 = greater100000Exercise[1];

	totalNoIncomeLess5 = noIncomeExercise[2];
	totalLess20000Less5 = less20000Exercise[2];
	totalLess40000Less5 = less40000Exercise[2];
	totalLess60000Less5 = less60000Exercise[2];
	totalLess80000Less5 = less80000Exercise[2];
	totalLess100000Less5 = less100000Exercise[2];
	totalGreater100000Less5 = greater100000Exercise[2];

	totalNoIncomeGreater5 = noIncomeExercise[3];
	totalLess20000Greater5 = less20000Exercise[3];
	totalLess40000Greater5 = less40000Exercise[3];
	totalLess60000Greater5 = less60000Exercise[3];
	totalLess80000Greater5 = less80000Exercise[3];
	totalLess100000Greater5 = less100000Exercise[3];
	totalGreater100000Greater5 = greater100000Exercise[3];

	//Totals for Exercise for all income brackets
	percentageNoIncomeNoExercise = ((float)totalNoIncomeNoExercise / (float)noIncomeCount) * 100;
	percentageLess20000NoExercise = ((float)totalLess20000NoExercise / (float)less20Count) * 100;
	percentageLess40000NoExercise = ((float)totalLess40000NoExercise / (float)less40Count) * 100;
	percentageLess60000NoExercise = ((float)totalLess60000NoExercise / (float)less60Count) * 100;
	percentageLess80000NoExercise = ((float)totalLess80000NoExercise / (float)less80Count) * 100;
	percentageLess100000NoExercise = ((float)totalLess100000NoExercise / (float)less100Count) * 100;
	percentageGreater100000NoExercise = ((float)totalGreater100000NoExercise / (float)greater100Count) * 100;

	percentageNoIncomeLess3 = ((float)totalNoIncomeLess3 / (float)noIncomeCount) * 100;
	percentageLess20000Less3 = ((float)totalLess20000Less3 / (float)less20Count) * 100;
	percentageLess40000Less3 = ((float)totalLess40000Less3 / (float)less40Count) * 100;
	percentageLess60000Less3 = ((float)totalLess60000Less3 / (float)less60Count) * 100;
	percentageLess80000Less3 = ((float)totalLess80000Less3 / (float)less80Count) * 100;
	percentageLess100000Less3 = ((float)totalLess100000Less3 / (float)less100Count) * 100;
	percentageGreater100000Less3 = ((float)totalGreater100000Less3 / (float)greater100Count) * 100;

	percentageNoIncomeLess5 = ((float)totalNoIncomeLess5 / (float)noIncomeCount) * 100;
	percentageLess20000Less5 = ((float)totalLess20000Less5 / (float)less20Count) * 100;
	percentageLess40000Less5 = ((float)totalLess40000Less5 / (float)less40Count) * 100;
	percentageLess60000Less5 = ((float)totalLess60000Less5 / (float)less60Count) * 100;
	percentageLess80000Less5 = ((float)totalLess80000Less5 / (float)less80Count) * 100;
	percentageLess100000Less5 = ((float)totalLess100000Less5 / (float)less100Count) * 100;
	percentageGreater100000Less5 = ((float)totalGreater100000Less5 / (float)greater100Count) * 100;

	percentageNoIncomeGreater5 = ((float)totalNoIncomeGreater5 / (float)noIncomeCount) * 100;
	percentageLess20000Greater5 = ((float)totalLess20000Greater5 / (float)less20Count) * 100;
	percentageLess40000Greater5 = ((float)totalLess40000Greater5 / (float)less40Count) * 100;
	percentageLess60000Greater5 = ((float)totalLess60000Greater5 / (float)less60Count) * 100;
	percentageLess80000Greater5 = ((float)totalLess80000Greater5 / (float)less80Count) * 100;
	percentageLess100000Greater5 = ((float)totalLess100000Greater5 / (float)less100Count) * 100;
	percentageGreater100000Greater5 = ((float)totalGreater100000Greater5 / (float)greater100Count) * 100;

	if (number == 0) {
		printf("==========================================================================\n");
		if (noIncomeCount == 0) printf("The total percentage of people with no income who never exercise is: 0.0%%\n");
		else printf("The total percentage of people with no income who never exercise is: %.2f%%\n", percentageNoIncomeNoExercise);
		if (less20Count == 0) printf("The total percentage of people who earn < 20000 and who never exercise is: 0.0%%\n");
		else printf("The total percentage of people who earn < 20000 and who never exercise is: %.2f%%\n", percentageLess20000NoExercise);
		if (less40Count == 0) printf("The total percentage of people who earn < 40000 and who never exercise is: 0.0%%\n");
		else printf("The total percentage of people who earn < 40000 and who never exercise is: %.2f%%\n", percentageLess40000NoExercise);
		if (less60Count == 0) printf("The total percentage of people who earn < 60000 and who never exercise is: 0.0%%\n");
		else printf("The total percentage of people who earn < 60000 and who never exercise is: %.2f%%\n", percentageLess60000NoExercise);
		if (less80Count == 0) printf("The total percentage of people who earn < 80000 and who never exercise is: 0.0%%\n");
		else printf("The total percentage of people who earn < 80000 and who never exercise is: %.2f%%\n", percentageLess80000NoExercise);
		if (less100Count == 0) printf("The total percentage of people who earn < 100000 and who never exercise is: 0.0%%\n");
		else printf("The total percentage of people who earn < 100000 and who never exercise is: %.2f%%\n", percentageLess100000NoExercise);
		if (greater100Count == 0) printf("The total percentage of people who earn > 100000 and who never exercise is: 0.0%%\n");
		else printf("The total percentage of people who earn > 100000 and who never exercise is: %.2f%%\n", percentageGreater100000NoExercise);

		if (noIncomeCount == 0) printf("The total percentage of people with no income and who exercise < 3 times per week is: 0.0%%\n");
		else printf("The total percentage of people with no income and who exercise < 3 times per week is: %.2f%%\n", percentageNoIncomeLess3);
		if (less20Count == 0) printf("The total percentage of people who earn < 20000 and who exercise < 3 times per week is: 0.0%%\n");
		else printf("The total percentage of people who earn < 20000 and who exercise < 3 times per week is: %.2f%%\n", percentageLess20000Less3);
		if (less40Count == 0) printf("The total percentage of people who earn < 40000 and whoexercise < 3 times per week is: 0.0%%\n");
		else printf("The total percentage of people who earn < 40000 and who exercise < 3 times per week is: %.2f%%\n", percentageLess40000Less3);
		if (less60Count == 0) printf("The total percentage of people who earn < 60000 and who exercise < 3 times per week is: 0.0%%\n");
		else printf("The total percentage of people who earn < 60000 and who exercise < 3 times per week is: %.2f%%\n", percentageLess60000Less3);
		if (less80Count == 0) printf("The total percentage of people who earn < 80000 and who exercise < 3 times per week is: 0.0%%\n");
		else printf("The total percentage of people who earn < 80000 and who exercise < 3 times per week is: %.2f%%\n", percentageLess80000Less3);
		if (less100Count == 0) printf("The total percentage of people who earn < 100000 and who exercise < 3 times per week is: 0.0%%\n");
		else printf("The total percentage of people who earn < 100000 and who exercise < 3 times per week is: %.2f%%\n", percentageLess100000Less3);
		if (greater100Count == 0) printf("The total percentage of people who earn > 100000 and who exercise < 3 times per week is: 0.0%%\n");
		else printf("The total percentage of people who earn > 100000 and who exercise < 3 times per week is: %.2f%%\n", percentageGreater100000Less3);

		if (noIncomeCount == 0) printf("The total percentage of people with no income and who exercise < 5 times per week is: 0.0%%\n");
		else printf("The total percentage of people with no income and who exercise < 5 times per week is: %.2f%%\n", percentageNoIncomeLess5);
		if (less20Count == 0) printf("The total percentage of people who earn < 20000 and who exercise < 5 times per week is: 0.0%%\n");
		else printf("The total percentage of people who earn < 20000 and who exercise < 5 times per week is: %.2f%%\n", percentageLess20000Less5);
		if (less40Count == 0) printf("The total percentage of people who earn < 40000 and whoexercise < 5 times per week is: 0.0%%\n");
		else printf("The total percentage of people who earn < 40000 and who exercise < 5 times per week is: %.2f%%\n", percentageLess40000Less5);
		if (less60Count == 0) printf("The total percentage of people who earn < 60000 and who exercise < 5 times per week is: 0.0%%\n");
		else printf("The total percentage of people who earn < 60000 and who exercise < 5 times per week is: %.2f%%\n", percentageLess60000Less5);
		if (less80Count == 0) printf("The total percentage of people who earn < 80000 and who exercise < 5 times per week is: 0.0%%\n");
		else printf("The total percentage of people who earn < 80000 and who exercise < 5 times per week is: %.2f%%\n", percentageLess80000Less5);
		if (less100Count == 0) printf("The total percentage of people who earn < 100000 and who exercise < 5 times per week is: 0.0%%\n");
		else printf("The total percentage of people who earn < 100000 and who exercise < 5 times per week is: %.2f%%\n", percentageLess100000Less5);
		if (greater100Count == 0) printf("The total percentage of people who earn > 100000 and who exercise < 5 times per week is: 0.0%%\n");
		else printf("The total percentage of people who earn > 100000 and who exercise < 5 times per week is: %.2f%%\n", percentageGreater100000Less5);

		if (noIncomeCount == 0) printf("The total percentage of people with no income and who exercise > 5 times per week is: 0.0%%\n");
		else printf("The total percentage of people with no income and who exercise > 5 times per week is: %.2f%%\n", percentageNoIncomeGreater5);
		if (less20Count == 0) printf("The total percentage of people who earn < 20000 and who exercise > 5 times per week is: 0.0%%\n");
		else printf("The total percentage of people who earn < 20000 and who exercise > 5 times per week is: %.2f%%\n", percentageLess20000Greater5);
		if (less40Count == 0) printf("The total percentage of people who earn < 40000 and whoexercise > 5 times per week is: 0.0%%\n");
		else printf("The total percentage of people who earn < 40000 and who exercise > 5 times per week is: %.2f%%\n", percentageLess40000Greater5);
		if (less60Count == 0) printf("The total percentage of people who earn < 60000 and who exercise > 5 times per week is: 0.0%%\n");
		else printf("The total percentage of people who earn < 60000 and who exercise > 5 times per week is: %.2f%%\n", percentageLess60000Greater5);
		if (less80Count == 0) printf("The total percentage of people who earn < 80000 and who exercise > 5 times per week is: 0.0%%\n");
		else printf("The total percentage of people who earn < 80000 and who exercise > 5 times per week is: %.2f%%\n", percentageLess80000Greater5);
		if (less100Count == 0) printf("The total percentage of people who earn < 100000 and who exercise > 5 times per week is: 0.0%%\n");
		else printf("The total percentage of people who earn < 100000 and who exercise > 5 times per week is: %.2f%%\n", percentageLess100000Greater5);
		if (greater100Count == 0) printf("The total percentage of people who earn > 100000 and who exercise > 5 times per week is: 0.0%%\n");
		else printf("The total percentage of people who earn > 100000 and who exercise > 5 times per week is: %.2f%%\n", percentageGreater100000Greater5);
	}

	if (number = 1) {
		file_ptr = fopen("SurveyReport.txt", "a");//append statistics to the file

		if (file_ptr != NULL) {
			fprintf(file_ptr, "==========================================================================\n");
			if (noIncomeCount == 0) fprintf(file_ptr, "The total percentage of people with no income who never exercise is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people with no income who never exercise is: %.2f%%\n", percentageNoIncomeNoExercise);
			if (less20Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 20000 and who never exercise is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 20000 and who never exercise is: %.2f%%\n", percentageLess20000NoExercise);
			if (less40Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 40000 and who never exercise is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 40000 and who never exercise is: %.2f%%\n", percentageLess40000NoExercise);
			if (less60Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 60000 and who never exercise is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 60000 and who never exercise is: %.2f%%\n", percentageLess60000NoExercise);
			if (less80Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 80000 and who never exercise is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 80000 and who never exercise is: %.2f%%\n", percentageLess80000NoExercise);
			if (less100Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 100000 and who never exercise is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 100000 and who never exercise is: %.2f%%\n", percentageLess100000NoExercise);
			if (greater100Count == 0) fprintf(file_ptr, "The total percentage of people who earn > 100000 and who never exercise is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn > 100000 and who never exercise is: %.2f%%\n", percentageGreater100000NoExercise);

			if (noIncomeCount == 0) fprintf(file_ptr, "The total percentage of people with no income and who exercise < 3 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people with no income and who exercise < 3 times per week is: %.2f%%\n", percentageNoIncomeLess3);
			if (less20Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 20000 and who exercise < 3 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 20000 and who exercise < 3 times per week is: %.2f%%\n", percentageLess20000Less3);
			if (less40Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 40000 and whoexercise < 3 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 40000 and who exercise < 3 times per week is: %.2f%%\n", percentageLess40000Less3);
			if (less60Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 60000 and who exercise < 3 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 60000 and who exercise < 3 times per week is: %.2f%%\n", percentageLess60000Less3);
			if (less80Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 80000 and who exercise < 3 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 80000 and who exercise < 3 times per week is: %.2f%%\n", percentageLess80000Less3);
			if (less100Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 100000 and who exercise < 3 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 100000 and who exercise < 3 times per week is: %.2f%%\n", percentageLess100000Less3);
			if (greater100Count == 0) fprintf(file_ptr, "The total percentage of people who earn > 100000 and who exercise < 3 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn > 100000 and who exercise < 3 times per week is: %.2f%%\n", percentageGreater100000Less3);

			if (noIncomeCount == 0) fprintf(file_ptr, "The total percentage of people with no income and who exercise < 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people with no income and who exercise < 5 times per week is: %.2f%%\n", percentageNoIncomeLess5);
			if (less20Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 20000 and who exercise < 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 20000 and who exercise < 5 times per week is: %.2f%%\n", percentageLess20000Less5);
			if (less40Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 40000 and whoexercise < 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 40000 and who exercise < 5 times per week is: %.2f%%\n", percentageLess40000Less5);
			if (less60Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 60000 and who exercise < 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 60000 and who exercise < 5 times per week is: %.2f%%\n", percentageLess60000Less5);
			if (less80Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 80000 and who exercise < 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 80000 and who exercise < 5 times per week is: %.2f%%\n", percentageLess80000Less5);
			if (less100Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 100000 and who exercise < 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 100000 and who exercise < 5 times per week is: %.2f%%\n", percentageLess100000Less5);
			if (greater100Count == 0) fprintf(file_ptr, "The total percentage of people who earn > 100000 and who exercise < 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn > 100000 and who exercise < 5 times per week is: %.2f%%\n", percentageGreater100000Less5);

			if (noIncomeCount == 0) fprintf(file_ptr, "The total percentage of people with no income and who exercise > 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people with no income and who exercise > 5 times per week is: %.2f%%\n", percentageNoIncomeGreater5);
			if (less20Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 20000 and who exercise > 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 20000 and who exercise > 5 times per week is: %.2f%%\n", percentageLess20000Greater5);
			if (less40Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 40000 and whoexercise > 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 40000 and who exercise > 5 times per week is: %.2f%%\n", percentageLess40000Greater5);
			if (less60Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 60000 and who exercise > 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 60000 and who exercise > 5 times per week is: %.2f%%\n", percentageLess60000Greater5);
			if (less80Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 80000 and who exercise > 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 80000 and who exercise > 5 times per week is: %.2f%%\n", percentageLess80000Greater5);
			if (less100Count == 0) fprintf(file_ptr, "The total percentage of people who earn < 100000 and who exercise > 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn < 100000 and who exercise > 5 times per week is: %.2f%%\n", percentageLess100000Greater5);
			if (greater100Count == 0) fprintf(file_ptr, "The total percentage of people who earn > 100000 and who exercise > 5 times per week is: 0.0%%\n");
			else fprintf(file_ptr, "The total percentage of people who earn > 100000 and who exercise > 5 times per week is: %.2f%%\n", percentageGreater100000Greater5);
		}
		else {
			printf("\nError Accessing the file on your system");
		}
		fclose(file_ptr);
	}
}//generateIncomeExerciseStatistics()

void generateIncomeAlcoholStatistics(struct listElement* head_ptr, int number) {
	struct listElement *temp = head_ptr;
	//Counters
	int noIncomeCount = 0, less20Count = 0, less40Count = 0, less60Count = 0, less80Count = 0, less100Count = 0, greater100Count = 0;

	//integer arrays for all income brackets for alcohol
	int noIncomeAlcohol[4] = { 0,0,0,0 }, less20000Alcohol[4] = { 0,0,0,0 }, less40000Alcohol[4] = { 0,0,0,0 }, less60000Alcohol[4] = { 0,0,0,0 },
		less80000Alcohol[4] = { 0,0,0,0 }, less100000Alcohol[4] = { 0,0,0,0 }, greater100000Alcohol[4] = { 0,0,0,0 };

	//variables for alcohol for all incomes
	int totalNoIncomeNoAlcohol, totalLess20000NoAlcohol, totalLess40000NoAlcohol, totalLess60000NoAlcohol, totalLess80000NoAlcohol, totalLess100000NoAlcohol, totalGreater100000NoAlcohol;
	int totalNoIncomeLess2, totalLess20000Less2, totalLess40000Less2, totalLess60000Less2, totalLess80000Less2, totalLess100000Less2, totalGreater100000Less2;
	int totalNoIncomeLess4, totalLess20000Less4, totalLess40000Less4, totalLess60000Less4, totalLess80000Less4, totalLess100000Less4, totalGreater100000Less4;
	int totalNoIncomeGreater4, totalLess20000Greater4, totalLess40000Greater4, totalLess60000Greater4, totalLess80000Greater4, totalLess100000Greater4, totalGreater100000Greater4;

	//Total variables for alcohol for all incomes
	float percentageNoIncomeNoAlcohol, percentageLess20000NoAlcohol, percentageLess40000NoAlcohol, percentageLess60000NoAlcohol, percentageLess80000NoAlcohol, percentageLess100000NoAlcohol, percentageGreater100000NoAlcohol;
	float percentageNoIncomeLess2, percentageLess20000Less2, percentageLess40000Less2, percentageLess60000Less2, percentageLess80000Less2, percentageLess100000Less2, percentageGreater100000Less2;
	float percentageNoIncomeLess4, percentageLess20000Less4, percentageLess40000Less4, percentageLess60000Less4, percentageLess80000Less4, percentageLess100000Less4, percentageGreater100000Less4;
	float percentageNoIncomeGreater4, percentageLess20000Greater4, percentageLess40000Greater4, percentageLess60000Greater4, percentageLess80000Greater4, percentageLess100000Greater4, percentageGreater100000Greater4;

	while (temp != NULL) {
		//count the total number of income brackets in the list for each income bracket
		if (temp->value.incomeBracket == 1) noIncomeCount++;
		if (temp->value.incomeBracket == 2) less20Count++;
		if (temp->value.incomeBracket == 3) less40Count++;
		if (temp->value.incomeBracket == 4) less60Count++;
		if (temp->value.incomeBracket == 5) less80Count++;
		if (temp->value.incomeBracket == 6) less100Count++;
		if (temp->value.incomeBracket == 7) greater100Count++;

		if (temp->value.alcohol == 1 && temp->value.incomeBracket == 1) noIncomeAlcohol[0] += 1; 
		if (temp->value.alcohol == 1 && temp->value.incomeBracket == 2) less20000Alcohol[0] += 1; 
		if (temp->value.alcohol == 1 && temp->value.incomeBracket == 3) less40000Alcohol[0] += 1; 
		if (temp->value.alcohol == 1 && temp->value.incomeBracket == 4) less60000Alcohol[0] += 1; 
		if (temp->value.alcohol == 1 && temp->value.incomeBracket == 5) less80000Alcohol[0] += 1; 
		if (temp->value.alcohol == 1 && temp->value.incomeBracket == 6) less100000Alcohol[0] += 1; 
		if (temp->value.alcohol == 1 && temp->value.incomeBracket == 7) greater100000Alcohol[0] += 1; 

		if (temp->value.alcohol == 2 && temp->value.incomeBracket == 1) noIncomeAlcohol[1] += 1; 
		if (temp->value.alcohol == 2 && temp->value.incomeBracket == 2) less20000Alcohol[1] += 1; 
		if (temp->value.alcohol == 2 && temp->value.incomeBracket == 3) less40000Alcohol[1] += 1; 
		if (temp->value.alcohol == 2 && temp->value.incomeBracket == 4) less60000Alcohol[1] += 1; 
		if (temp->value.alcohol == 2 && temp->value.incomeBracket == 5) less80000Alcohol[1] += 1;
		if (temp->value.alcohol == 2 && temp->value.incomeBracket == 6) less100000Alcohol[1] += 1; 
		if (temp->value.alcohol == 2 && temp->value.incomeBracket == 7) greater100000Alcohol[1] += 1;

		if (temp->value.alcohol == 3 && temp->value.incomeBracket == 2) less20000Alcohol[2] += 1; 
		if (temp->value.alcohol == 3 && temp->value.incomeBracket == 3) less40000Alcohol[2] += 1; 
		if (temp->value.alcohol == 3 && temp->value.incomeBracket == 4) less60000Alcohol[2] += 1; 
		if (temp->value.alcohol == 3 && temp->value.incomeBracket == 5) less80000Alcohol[2] += 1; 
		if (temp->value.alcohol == 3 && temp->value.incomeBracket == 6) less100000Alcohol[2] += 1; 
		if (temp->value.alcohol == 3 && temp->value.incomeBracket == 7) greater100000Alcohol[2] += 1; 

		if (temp->value.alcohol == 4 && temp->value.incomeBracket == 1) noIncomeAlcohol[3] += 1; 
		if (temp->value.alcohol == 4 && temp->value.incomeBracket == 2) less20000Alcohol[3] += 1; 
		if (temp->value.alcohol == 4 && temp->value.incomeBracket == 3) less40000Alcohol[3] += 1; 
		if (temp->value.alcohol == 4 && temp->value.incomeBracket == 4) less60000Alcohol[3] += 1; 
		if (temp->value.alcohol == 4 && temp->value.incomeBracket == 5) less80000Alcohol[3] += 1; 
		if (temp->value.alcohol == 4 && temp->value.incomeBracket == 6) less100000Alcohol[3] += 1; 
		if (temp->value.alcohol == 4 && temp->value.incomeBracket == 7) greater100000Alcohol[3] += 1; 
		temp = temp->next;
	}//while

	 //Calculate Exercise statistics for income brackets
	totalNoIncomeNoAlcohol = noIncomeAlcohol[0];
	totalLess20000NoAlcohol = less20000Alcohol[0];
	totalLess40000NoAlcohol = less40000Alcohol[0];
	totalLess60000NoAlcohol = less60000Alcohol[0];
	totalLess80000NoAlcohol = less80000Alcohol[0];
	totalLess100000NoAlcohol = less100000Alcohol[0];
	totalGreater100000NoAlcohol = greater100000Alcohol[0];

	totalNoIncomeLess2 = noIncomeAlcohol[1];
	totalLess20000Less2 = less20000Alcohol[1];
	totalLess40000Less2 = less40000Alcohol[1];
	totalLess60000Less2 = less60000Alcohol[1];
	totalLess80000Less2 = less80000Alcohol[1];
	totalLess100000Less2 = less100000Alcohol[1];
	totalGreater100000Less2 = greater100000Alcohol[1];

	totalNoIncomeLess4 = noIncomeAlcohol[2];
	totalLess20000Less4= less20000Alcohol[2];
	totalLess40000Less4 = less40000Alcohol[2];
	totalLess60000Less4 = less60000Alcohol[2];
	totalLess80000Less4 = less80000Alcohol[2];
	totalLess100000Less4 = less100000Alcohol[2];
	totalGreater100000Less4 = greater100000Alcohol[2];

	totalNoIncomeGreater4 = noIncomeAlcohol[3];
	totalLess20000Greater4 = less20000Alcohol[3];
	totalLess40000Greater4 = less40000Alcohol[3];
	totalLess60000Greater4 = less60000Alcohol[3];
	totalLess80000Greater4 = less80000Alcohol[3];
	totalLess100000Greater4 = less100000Alcohol[3];
	totalGreater100000Greater4 = greater100000Alcohol[3];

	//Totals for Alcohol for all income brackets
	percentageNoIncomeNoAlcohol = ((float)totalNoIncomeNoAlcohol / (float)noIncomeCount) * 100;
	percentageLess20000NoAlcohol = ((float)totalLess20000NoAlcohol / (float)less20Count) * 100;
	percentageLess40000NoAlcohol = ((float)totalLess40000NoAlcohol / (float)less40Count) * 100;
	percentageLess60000NoAlcohol = ((float)totalLess60000NoAlcohol / (float)less60Count) * 100;
	percentageLess80000NoAlcohol = ((float)totalLess80000NoAlcohol / (float)less80Count) * 100;
	percentageLess100000NoAlcohol = ((float)totalLess100000NoAlcohol / (float)less100Count) * 100;
	percentageGreater100000NoAlcohol = ((float)totalGreater100000NoAlcohol / (float)greater100Count) * 100;

	percentageNoIncomeLess2 = ((float)totalNoIncomeLess2 / (float)noIncomeCount) * 100;
	percentageLess20000Less2 = ((float)totalLess20000Less2 / (float)less20Count) * 100;
	percentageLess40000Less2 = ((float)totalLess40000Less2 / (float)less40Count) * 100;
	percentageLess60000Less2 = ((float)totalLess60000Less2 / (float)less60Count) * 100;
	percentageLess80000Less2 = ((float)totalLess80000Less2 / (float)less80Count) * 100;
	percentageLess100000Less2 = ((float)totalLess100000Less2 / (float)less100Count) * 100;
	percentageGreater100000Less2 = ((float)totalGreater100000Less2 / (float)greater100Count) * 100;

	percentageNoIncomeLess4 = ((float)totalNoIncomeLess4 / (float)noIncomeCount) * 100;
	percentageLess20000Less4 = ((float)totalLess20000Less4 / (float)less20Count) * 100;
	percentageLess40000Less4 = ((float)totalLess40000Less4 / (float)less40Count) * 100;
	percentageLess60000Less4 = ((float)totalLess60000Less4 / (float)less60Count) * 100;
	percentageLess80000Less4 = ((float)totalLess80000Less4 / (float)less80Count) * 100;
	percentageLess100000Less4 = ((float)totalLess100000Less4 / (float)less100Count) * 100;
	percentageGreater100000Less4 = ((float)totalGreater100000Less4 / (float)greater100Count) * 100;

	percentageNoIncomeGreater4 = ((float)totalNoIncomeGreater4 / (float)noIncomeCount) * 100;
	percentageLess20000Greater4 = ((float)totalLess20000Greater4 / (float)less20Count) * 100;
	percentageLess40000Greater4 = ((float)totalLess40000Greater4 / (float)less40Count) * 100;
	percentageLess60000Greater4 = ((float)totalLess60000Greater4 / (float)less60Count) * 100;
	percentageLess80000Greater4 = ((float)totalLess80000Greater4 / (float)less80Count) * 100;
	percentageLess100000Greater4 = ((float)totalLess100000Greater4 / (float)less100Count) * 100;
	percentageGreater100000Greater4 = ((float)totalGreater100000Greater4 / (float)greater100Count) * 100;

	if (number == 0) {
		printf("==========================================================================\n");
		if (noIncomeCount == 0) printf("The percentage of people with no income who do not consume alcohol is: 0.0%%\n");
		else printf("The percentage of people with no income who do not consume alcohol is: %.2f%%\n", percentageNoIncomeNoAlcohol);
		if (less20Count == 0) printf("The percentage of people who earn < 20000 and who do not consume alcohol is: 0.0%%\n");
		else printf("The percentage of people who earn < 20000 and who do not consume alcohol is: %.2f%%\n", percentageLess20000NoAlcohol);
		if (less40Count == 0) printf("The percentage of people who earn < 40000 and do not consume alcohol is: 0.0%%\n");
		else printf("The percentage of people who earn < 40000 and who do not consume alcohol is: %.2f%%\n", percentageLess40000NoAlcohol);
		if (less60Count == 0) printf("The percentage of people who earn < 60000 and who do not consume alcohol is: 0.0%%\n");
		else printf("The percentage of people who earn < 60000 and who do not consume alcohol is: %.2f%%\n", percentageLess60000NoAlcohol);
		if (less80Count == 0) printf("The percentage of people who earn < 80000 and who do not consume alcohol is: 0.0%%\n");
		else printf("The percentage of people who earn < 80000 and who do not consume alcohol is: %.2f%%\n", percentageLess80000NoAlcohol);
		if (less100Count == 0) printf("The percentage of people who earn < 100000 and who do not consume alcohol is: 0.0%%\n");
		else printf("The percentage of people who earn < 100000 and who do not consume alcohol is: %.2f%%\n", percentageLess100000NoAlcohol);
		if (greater100Count == 0) printf("The percentage of people who earn > 100000 and who do not consume alcohol is: 0.0%%\n");
		else printf("The percentage of people who earn > 100000 and who do not consume alcohol is: %.2f%%\n", percentageGreater100000NoAlcohol);

		if (noIncomeCount == 0) printf("The percentage of people with no income & consume < 2 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people with no income & consume < 2 units of alcohol per week is: %.2f%%\n", percentageNoIncomeLess2);
		if (less20Count == 0) printf("The percentage of people who earn < 20000 & consume < 2 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people who earn < 20000 & consume < 2 units of alcohol per week is: %.2f%%\n", percentageLess20000Less2);
		if (less40Count == 0) printf("The percentage of people who earn < 40000 & consume < 2 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people who earn < 40000 & consume < 2 units of alcohol per week is: %.2f%%\n", percentageLess40000Less2);
		if (less60Count == 0) printf("The percentage of people who earn < 60000 & consume < 2 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people who earn < 60000 & consume < 2 units of alcohol per week is: %.2f%%\n", percentageLess60000Less2);
		if (less80Count == 0) printf("The percentage of people who earn < 80000 & consume < 2 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people who earn < 80000 & consume < 2 units of alcohol per week is: %.2f%%\n", percentageLess80000Less2);
		if (less100Count == 0) printf("The  percentage of people who earn < 100000 & consume < 2 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people who earn < 100000 & consume < 2 units of alcohol per week is: %.2f%%\n", percentageLess100000Less2);
		if (greater100Count == 0) printf("The percentage of people who earn > 100000 & consume < 2 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people who earn > 100000 & consume < 2 units of alcohol per week is: %.2f%%\n", percentageGreater100000Less2);

		if (noIncomeCount == 0) printf("The percentage of people with no income & consume < 4 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people with no income & consume < 4 units of alcohol per week is: %.2f%%\n", percentageNoIncomeLess4);
		if (less20Count == 0) printf("The percentage of people who earn < 20000 & consume < 4 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people who earn < 20000 & consume < 4 units of alcohol per week is: %.2f%%\n", percentageLess20000Less4);
		if (less40Count == 0) printf("The percentage of people who earn < 40000 & consume < 4 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people who earn < 40000 & consume < 4 units of alcohol per week is: %.2f%%\n", percentageLess40000Less4);
		if (less60Count == 0) printf("The percentage of people who earn < 60000 & consume < 4 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people who earn < 60000 & consume < 4 units of alcohol per week is: %.2f%%\n", percentageLess60000Less4);
		if (less80Count == 0) printf("The percentage of people who earn < 80000 & consume < 4 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people who earn < 80000 & consume < 4 units of alcohol per week is: %.2f%%\n", percentageLess80000Less4);
		if (less100Count == 0) printf("The percentage of people who earn < 100000 & consume < 4 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people who earn < 100000 & consume < 4 units of alcohol per week is: %.2f%%\n", percentageLess100000Less4);
		if (greater100Count == 0) printf("The percentage of people who earn > 100000 & consume < 4 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people who earn > 100000 & consume < 4 units of alcohol per week is: %.2f%%\n", percentageGreater100000Less4);

		if (noIncomeCount == 0) printf("The percentage of people with no income & consume > 4 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people with no income & consume > 4 units of alcohol per week is: %.2f%%\n", percentageNoIncomeGreater4);
		if (less20Count == 0) printf("The percentage of people who earn < 20000 & consume > 4 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people who earn < 20000 & consume > 4 units of alcohol per week is: %.2f%%\n", percentageLess20000Greater4);
		if (less40Count == 0) printf("The percentage of people who earn < 40000 & consume > 4 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people who earn < 40000 & consume > 4 units of alcohol per week is: %.2f%%\n", percentageLess40000Greater4);
		if (less60Count == 0) printf("The percentage of people who earn < 60000 & consume > 4 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people who earn < 60000 & consume > 4 units of alcohol per week is: %.2f%%\n", percentageLess60000Greater4);
		if (less80Count == 0) printf("The percentage of people who earn < 80000 & consume > 4 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people who earn < 80000 & consume > 4 units of alcohol per week is: %.2f%%\n", percentageLess80000Greater4);
		if (less100Count == 0) printf("The percentage of people who earn < 100000 & consume > 4 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people who earn < 100000 & consume > 4 units of alcohol per week is: %.2f%%\n", percentageLess100000Greater4);
		if (greater100Count == 0) printf("The percentage of people who earn > 100000 & consume > 4 units of alcohol per week is: 0.0%%\n");
		else printf("The percentage of people who earn > 100000 & consume > 4 units of alcohol per week is: %.2f%%\n", percentageGreater100000Greater4);
	}
	if (number == 1) {
		file_ptr = fopen("SurveyReport.txt", "a");//append statistics to the file

		if (file_ptr != NULL) {
			fprintf(file_ptr, "==========================================================================\n");
			if (noIncomeCount == 0) fprintf(file_ptr, "The percentage of people with no income who do not consume alcohol is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people with no income who do not consume alcohol is: %.2f%%\n", percentageNoIncomeNoAlcohol);
			if (less20Count == 0) fprintf(file_ptr, "The percentage of people who earn < 20000 and who do not consume alcohol is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 20000 and who do not consume alcohol is: %.2f%%\n", percentageLess20000NoAlcohol);
			if (less40Count == 0) fprintf(file_ptr, "The percentage of people who earn < 40000 and do not consume alcohol is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 40000 and who do not consume alcohol is: %.2f%%\n", percentageLess40000NoAlcohol);
			if (less60Count == 0) fprintf(file_ptr, "The percentage of people who earn < 60000 and who do not consume alcohol is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 60000 and who do not consume alcohol is: %.2f%%\n", percentageLess60000NoAlcohol);
			if (less80Count == 0) fprintf(file_ptr, "The percentage of people who earn < 80000 and who do not consume alcohol is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 80000 and who do not consume alcohol is: %.2f%%\n", percentageLess80000NoAlcohol);
			if (less100Count == 0) fprintf(file_ptr, "The percentage of people who earn < 100000 and who do not consume alcohol is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 100000 and who do not consume alcohol is: %.2f%%\n", percentageLess100000NoAlcohol);
			if (greater100Count == 0) fprintf(file_ptr, "The percentage of people who earn > 100000 and who do not consume alcohol is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn > 100000 and who do not consume alcohol is: %.2f%%\n", percentageGreater100000NoAlcohol);

			if (noIncomeCount == 0) fprintf(file_ptr, "The percentage of people with no income & consume < 2 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people with no income & consume < 2 units of alcohol per week is: %.2f%%\n", percentageNoIncomeLess2);
			if (less20Count == 0) fprintf(file_ptr, "The percentage of people who earn < 20000 & consume < 2 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 20000 & consume < 2 units of alcohol per week is: %.2f%%\n", percentageLess20000Less2);
			if (less40Count == 0) fprintf(file_ptr, "The percentage of people who earn < 40000 & consume < 2 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 40000 & consume < 2 units of alcohol per week is: %.2f%%\n", percentageLess40000Less2);
			if (less60Count == 0) fprintf(file_ptr, "The percentage of people who earn < 60000 & consume < 2 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 60000 & consume < 2 units of alcohol per week is: %.2f%%\n", percentageLess60000Less2);
			if (less80Count == 0) fprintf(file_ptr, "The percentage of people who earn < 80000 & consume < 2 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 80000 & consume < 2 units of alcohol per week is: %.2f%%\n", percentageLess80000Less2);
			if (less100Count == 0) fprintf(file_ptr, "The  percentage of people who earn < 100000 & consume < 2 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 100000 & consume < 2 units of alcohol per week is: %.2f%%\n", percentageLess100000Less2);
			if (greater100Count == 0) fprintf(file_ptr, "The percentage of people who earn > 100000 & consume < 2 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn > 100000 & consume < 2 units of alcohol per week is: %.2f%%\n", percentageGreater100000Less2);

			if (noIncomeCount == 0) fprintf(file_ptr, "The percentage of people with no income & consume < 4 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people with no income & consume < 4 units of alcohol per week is: %.2f%%\n", percentageNoIncomeLess4);
			if (less20Count == 0) fprintf(file_ptr, "The percentage of people who earn < 20000 & consume < 4 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 20000 & consume < 4 units of alcohol per week is: %.2f%%\n", percentageLess20000Less4);
			if (less40Count == 0) fprintf(file_ptr, "The percentage of people who earn < 40000 & consume < 4 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 40000 & consume < 4 units of alcohol per week is: %.2f%%\n", percentageLess40000Less4);
			if (less60Count == 0) fprintf(file_ptr, "The percentage of people who earn < 60000 & consume < 4 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 60000 & consume < 4 units of alcohol per week is: %.2f%%\n", percentageLess60000Less4);
			if (less80Count == 0) fprintf(file_ptr, "The percentage of people who earn < 80000 & consume < 4 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 80000 & consume < 4 units of alcohol per week is: %.2f%%\n", percentageLess80000Less4);
			if (less100Count == 0) fprintf(file_ptr, "The percentage of people who earn < 100000 & consume < 4 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 100000 & consume < 4 units of alcohol per week is: %.2f%%\n", percentageLess100000Less4);
			if (greater100Count == 0) fprintf(file_ptr, "The percentage of people who earn > 100000 & consume < 4 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn > 100000 & consume < 4 units of alcohol per week is: %.2f%%\n", percentageGreater100000Less4);

			if (noIncomeCount == 0) fprintf(file_ptr, "The percentage of people with no income & consume > 4 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people with no income & consume > 4 units of alcohol per week is: %.2f%%\n", percentageNoIncomeGreater4);
			if (less20Count == 0) fprintf(file_ptr, "The percentage of people who earn < 20000 & consume > 4 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr,"The percentage of people who earn < 20000 & consume > 4 units of alcohol per week is: %.2f%%\n", percentageLess20000Greater4);
			if (less40Count == 0) fprintf(file_ptr, "The percentage of people who earn < 40000 & consume > 4 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 40000 & consume > 4 units of alcohol per week is: %.2f%%\n", percentageLess40000Greater4);
			if (less60Count == 0) fprintf(file_ptr, "The percentage of people who earn < 60000 & consume > 4 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 60000 & consume > 4 units of alcohol per week is: %.2f%%\n", percentageLess60000Greater4);
			if (less80Count == 0) fprintf(file_ptr, "The percentage of people who earn < 80000 & consume > 4 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 80000 & consume > 4 units of alcohol per week is: %.2f%%\n", percentageLess80000Greater4);
			if (less100Count == 0) fprintf(file_ptr, "The percentage of people who earn < 100000 & consume > 4 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn < 100000 & consume > 4 units of alcohol per week is: %.2f%%\n", percentageLess100000Greater4);
			if (greater100Count == 0) fprintf(file_ptr, "The percentage of people who earn > 100000 & consume > 4 units of alcohol per week is: 0.0%%\n");
			else fprintf(file_ptr, "The percentage of people who earn > 100000 & consume > 4 units of alcohol per week is: %.2f%%\n", percentageGreater100000Greater4);
		}
		else {
			printf("\nError Accessing the file on your system");
		}
		fclose(file_ptr);
	}
}//generateIncomeAlcoholStatistics()

void generateGenderStatistics(struct listElement* head_ptr, int number) {
	struct listElement *temp;
	temp = head_ptr;
	//counters
	int femaleCounter = 0, maleCounter = 0;
	//integer arrays for male and female smokers
	int maleSmoke[4] = { 0,0,0,0 };
	int femaleSmoke[4] = { 0,0,0,0 };
	//integer array for male and female who exercise
	int maleExercise[4] = { 0,0,0,0 };
	int femaleExercise[4] = { 0,0,0,0 };
	//integer array for male and female alcohol
	int maleAlcohol[4] = { 0,0,0,0 };
	int	femaleAlcohol[4] = { 0,0,0,0 };

	//variables for smokers
	int totalFemaleSmokers, totalMaleSmokers;
	int totalMaleLess20, totalFemaleLess20;
	int totalMaleLess40, totalFemaleLess40;
	int totalMaleGreater40, totalFemaleGreater40;
	//Total variables for smokers
	float percentageMaleSmokers, percentageFemaleSmokers;
	float percentageMaleLess20, percentageFemaleLess20;
	float percentageMaleLess40, percentageFemaleLess40;
	float percentageMaleGreater40, percentageFemaleGreater40;
	
	//variables for exercise
	int totalMaleNoExercise, totalFemaleNoExercise;
	int totalMaleExerciseLess3, totalFemaleExerciseLess3;
	int totalMaleExerciseLess5, totalFemaleExerciseLess5;
	int totalMaleExerciseGreater5, totalFemaleExerciseGreater5;
	//Total variables for exercise
	float percentageMaleNoExercise, percentageFemaleNoExercise;
	float percentageMaleExerciseLess3, percentageFemaleExerciseLess3;
	float percentageMaleExerciseLess5, percentageFemaleExerciseLess5;
	float percentageMaleExerciseGreater5, percentageFemaleExerciseGreater5;

	//Variables for Alcohol
	int totalMaleNoAlcohol, totalFemaleNoAlcohol;
	int totalMaleLess2Units, totalFemaleLess2Units;
	int totalMaleLess4Units, totalFemaleLess4Units;
	int totalMaleGreater4Units, totalFemaleGreater4Units;
	//Total Variables for accohol
	float percentageMaleNoAlcohol, percentageFemaleNoAlcohol;
	float percentageMaleLess2Units, percentageFemaleLess2Units;
	float percentageMaleLess4Units, percentageFemaleLess4Units;
	float percentageMaleGreater4Units, percentageFemaleGreater4Units;

	while (temp != NULL) {
		//loop around the linked list to gather required information for statistics
		if (strcmp("m", temp->value.gender) == 0) maleCounter++;//count the total number if males in the database
		if (strcmp("f", temp->value.gender) == 0) femaleCounter++;//count the total number of females in the database

		/*====Male and female smokers====*/
		if (temp->value.cigarettes == 1 && (strcmp("m", temp->value.gender) == 0)) maleSmoke[0] += 1;
		if (temp->value.cigarettes == 1 && (strcmp("f", temp->value.gender) == 0)) femaleSmoke[0] += 1;

		if (temp->value.cigarettes == 2 && (strcmp("m", temp->value.gender) == 0)) maleSmoke[1] += 1;
		if (temp->value.cigarettes == 2 && (strcmp("f", temp->value.gender) == 0)) femaleSmoke[1] += 1;

		if (temp->value.cigarettes == 3 && (strcmp("m", temp->value.gender) == 0)) maleSmoke[2] += 1;
		if (temp->value.cigarettes == 3 && (strcmp("f", temp->value.gender) == 0)) femaleSmoke[2] += 1;

		if (temp->value.cigarettes == 4 && (strcmp("m", temp->value.gender) == 0)) maleSmoke[3] += 1;
		if (temp->value.cigarettes == 4 && (strcmp("f", temp->value.gender) == 0)) femaleSmoke[3] += 1;
		/*===Male and female who exercise check====*/
		if (temp->value.excerise == 1 && (strcmp("m", temp->value.gender) == 0)) maleExercise[0] += 1;
		if (temp->value.excerise == 1 && (strcmp("f", temp->value.gender) == 0)) femaleExercise[0] += 1;

		if (temp->value.excerise == 2 && (strcmp("m", temp->value.gender) == 0)) maleExercise[1] += 1;
		if (temp->value.excerise == 2 && (strcmp("f", temp->value.gender) == 0)) femaleExercise[1] += 1;

		if (temp->value.excerise == 3 && (strcmp("m", temp->value.gender) == 0)) maleExercise[2] += 1;
		if (temp->value.excerise == 3 && (strcmp("f", temp->value.gender) == 0)) femaleExercise[2] += 1;

		if (temp->value.excerise == 4 && (strcmp("m", temp->value.gender) == 0)) maleExercise[3] += 1;
		if (temp->value.excerise == 4 && (strcmp("f", temp->value.gender) == 0)) femaleExercise[3] += 1;
		//===Male and Female alcohol consumption check===
		if (temp->value.alcohol == 1 && (strcmp("m", temp->value.gender) == 0)) maleAlcohol[0] += 1;
		if (temp->value.alcohol == 1 && (strcmp("f", temp->value.gender) == 0)) femaleAlcohol[0] += 1;

		if (temp->value.alcohol == 2 && (strcmp("m", temp->value.gender) == 0)) maleAlcohol[1] += 1;
		if (temp->value.alcohol == 2 && (strcmp("f", temp->value.gender) == 0)) femaleAlcohol[1] += 1;

		if (temp->value.alcohol == 3 && (strcmp("m", temp->value.gender) == 0)) maleAlcohol[2] += 1;
		if (temp->value.alcohol == 3 && (strcmp("f", temp->value.gender) == 0)) femaleAlcohol[2] += 1;

		if (temp->value.alcohol == 4 && (strcmp("m", temp->value.gender) == 0)) maleAlcohol[3] += 1;
		if (temp->value.alcohol == 4 && (strcmp("f", temp->value.gender) == 0)) femaleAlcohol[3] += 1;
		temp = temp->next;
	}//while

	//Calculate Smoking statistics for male and female
	totalMaleSmokers = maleSmoke[1] + maleSmoke[2] + maleSmoke[3];
	totalFemaleSmokers = femaleSmoke[1] + femaleSmoke[2] + femaleSmoke[3];

	totalMaleLess20 = maleSmoke[1];
	totalFemaleLess20 = femaleSmoke[1];

	totalMaleLess40 = maleSmoke[2];
	totalFemaleLess40 = femaleSmoke[2];

	totalMaleGreater40 = maleSmoke[3];
	totalFemaleGreater40 = femaleSmoke[3];

	//calculate exercise statistics for male and female
	totalMaleNoExercise = maleExercise[0];
	totalFemaleNoExercise = femaleExercise[0];

	totalMaleExerciseLess3 = maleExercise[1];
	totalFemaleExerciseLess3 = femaleExercise[1];

	totalMaleExerciseLess5 = maleExercise[2];
	totalFemaleExerciseLess5 = femaleExercise[2];

	totalMaleExerciseGreater5 = maleExercise[3];
	totalFemaleExerciseGreater5 = femaleExercise[3];

	//calculate alcohol statistics for male and female
	totalMaleNoAlcohol = maleAlcohol[0];
	totalFemaleNoAlcohol = femaleAlcohol[0];

	totalMaleLess2Units = maleAlcohol[1];
	totalFemaleLess2Units = femaleAlcohol[1];

	totalMaleLess4Units = maleAlcohol[2];
	totalFemaleLess4Units = femaleAlcohol[2];

	totalMaleGreater4Units = maleAlcohol[3];
	totalFemaleGreater4Units = femaleAlcohol[3];

	//Totals for genders for Smoking
	percentageMaleSmokers = ((float)totalMaleSmokers / (float)maleCounter) * 100;
	percentageFemaleSmokers = ((float)totalFemaleSmokers / (float)femaleCounter) * 100;

	percentageMaleLess20 = ((float)totalMaleLess20 / (float)maleCounter) * 100;
	percentageFemaleLess20 = ((float)totalFemaleLess20 / (float)femaleCounter) * 100;

	percentageMaleLess40 = ((float)totalMaleLess40 / (float)maleCounter) * 100;
	percentageFemaleLess40 = ((float)totalFemaleLess40 / (float)femaleCounter) * 100;

	percentageMaleGreater40 = ((float)totalMaleGreater40 / (float)maleCounter) * 100;
	percentageFemaleGreater40 = ((float)totalFemaleGreater40 / (float)femaleCounter) * 100;

	//Totals for genders for exercise
	percentageMaleNoExercise = ((float)totalMaleNoExercise / (float)maleCounter) * 100;
	percentageFemaleNoExercise = ((float)totalFemaleNoExercise / (float)femaleCounter) * 100;

	percentageMaleExerciseLess3 = ((float)totalMaleExerciseLess3 / (float)maleCounter) * 100;
	percentageFemaleExerciseLess3 = ((float)totalFemaleExerciseLess3 / (float)femaleCounter) * 100;

	percentageMaleExerciseLess5 = ((float)totalMaleExerciseLess5 / (float)maleCounter) * 100;
	percentageFemaleExerciseLess5 = ((float)totalFemaleExerciseLess5 / (float)femaleCounter) * 100;

	percentageMaleExerciseGreater5 = ((float)totalMaleExerciseGreater5 / (float)maleCounter) * 100;
	percentageFemaleExerciseGreater5 = ((float)totalFemaleExerciseGreater5 / (float)femaleCounter) * 100;

	//Totals for genders for alcohol
	percentageMaleNoAlcohol= ((float)totalMaleNoAlcohol / (float)maleCounter) * 100;
	percentageFemaleNoAlcohol = ((float)totalFemaleNoAlcohol / (float)femaleCounter) * 100;

	percentageMaleLess2Units = ((float)totalMaleLess2Units / (float)maleCounter) * 100;
	percentageFemaleLess2Units = ((float)totalFemaleLess2Units / (float)femaleCounter) * 100;

	percentageMaleLess4Units = ((float)totalMaleLess4Units / (float)maleCounter) * 100;
	percentageFemaleLess4Units = ((float)totalFemaleLess4Units / (float)femaleCounter) * 100;

	percentageMaleGreater4Units = ((float)totalMaleGreater4Units / (float)maleCounter) * 100;
	percentageFemaleGreater4Units = ((float)totalFemaleGreater4Units / (float)femaleCounter) * 100;

	if (number == 0) {
		printf("==========================================================================\n");
		printf("\t\t\t  Gender Statistics\n");
		printf("==========================================================================\n");
		if (maleCounter == 0) printf("The total percentage of Males who smoke is: 0.00%%\n");
		else printf("The total percentage of Males who smoke is: %.2f%%\n", percentageMaleSmokers);
		if (femaleCounter == 0)  printf("The total percentage of Females who smoke is: 0.00%%\n");
		else printf("The total percentage of Females who smoke is: %.2f%%\n", percentageFemaleSmokers);
		if (maleCounter == 0) printf("The total percentage of Males who smoke < 20 is: 0.00%%");
		else printf("The total percentage of Males who smoke < 20 is: %.2f%%\n", percentageMaleLess20);
		if (femaleCounter == 0) printf("The total percentage of Females who smoke < 20 is: 0.00%%\n");
		else printf("The total percentage of Females who smoke < 20 is: %.2f%%\n", percentageFemaleLess20);
		if (maleCounter == 0) printf("The total percentage of Males who smoke < 40 is: 0.00%%\n");
		else printf("The total percentage of Males who smoke < 40 is: %.2f%%\n", percentageMaleLess40);
		if (femaleCounter == 0) printf("The total percentage of Females who smoke < 40 is: 0.00%%\n");
		else printf("The total percentage of Females who smoke < 40 is: %.2f%%\n", percentageFemaleLess40);
		if (maleCounter == 0) printf("The total percentage of Males who smoke > 40 is: 0.00%%\n");
		else printf("The total percentage of Males who smoke > 40 is: %.2f%%\n", percentageMaleGreater40);
		if (femaleCounter == 0) printf("The total percentage of Females who smoke > 40 is: 0.00%%\n");
		else printf("The total percentage of Females who smoke > 40 is: %.2f%%\n", percentageFemaleGreater40);
		printf("==================================================================================\n");
		if (maleCounter == 0) printf("The total percentage of Males who don't exercise is: 0.00%%\n");
		else printf("The total percentage of Males who don't exercise is: %.2f%%\n", percentageMaleNoExercise);
		if (femaleCounter == 0) printf("The total percentage of Females who don't exercise is: 0.00%%\n");
		else printf("The total percentage of Females who don't exercise is: %.2f%%\n", percentageFemaleNoExercise);
		if (maleCounter == 0) printf("The total percentage of Males who exercise less than 3 times per week is: 0.00%%\n");
		else printf("The total percentage of Males who exercise less than 3 times per week is: %.2f%%\n", percentageMaleExerciseLess3);
		if (femaleCounter == 0) printf("The total percentage of Femaleswho exercise less than 3 times per week is: 0.00%%\n");
		else printf("The total percentage of Femaleswho exercise less than 3 times per week is: %.2f%%\n", percentageFemaleExerciseLess3);
		if (maleCounter == 0) printf("The total percentage of Males who exercise less than 5 times per week is: 0.00%%\n");
		else printf("The total percentage of Males who exercise less than 5 times per week is: %.2f%%\n", percentageMaleExerciseLess5);
		if (femaleCounter == 0) printf("The total percentage of Femaleswho exercise less than 5 times per week is: 0.00%%\n");
		else printf("The total percentage of Femaleswho exercise less than 5 times per week is: %.2f%%\n", percentageFemaleExerciseLess5);
		if (maleCounter == 0) printf("The total percentage of Males who exercise greater than 5 times per week is: 0.00%%\n");
		else printf("The total percentage of Males who exercise greater than 5 times per week is: %.2f%%\n", percentageMaleExerciseGreater5);
		if (femaleCounter == 0) printf("The total percentage of Females who exercise greater than 5 times per week is: 0.00%%\n");
		else printf("The total percentage of Femaleswho exercise greater than 5 times per week is: %.2f%%\n", percentageFemaleExerciseGreater5);
		printf("==================================================================================\n");
		if (maleCounter == 0) printf("The total percentage of Males who consume no alcohol is: 0.00%%\n");
		else printf("The total percentage of Males who consume no alcohol is: %.2f%%\n", percentageMaleNoAlcohol);
		if (femaleCounter == 0) printf("The total percentage of Females who consume no alcohol is: 0.00%%\n");
		else printf("The total percentage of Females who consume no alcohol is: %.2f%%\n", percentageFemaleNoAlcohol);
		if (maleCounter == 0) printf("The total percentage of Males who consume less than 2 units of alcohol is: 0.00%%\n");
		else printf("The total percentage of Males who consume less than 2 units of alcohol is: %.2f%%\n", percentageMaleLess2Units);
		if (femaleCounter == 0) printf("The total percentage of Females who consume less than 2 units alcohol is: 0.00%%\n");
		else printf("The total percentage of Females who consume less than 2 units alcohol is: %.2f%%\n", percentageFemaleLess2Units);
		if (maleCounter == 0) printf("The total percentage of Males who consume less than 4 units of alcohol is: 0.00%%\n");
		else printf("The total percentage of Males who consume less than 4 units of alcohol is: %.2f%%\n", percentageMaleLess4Units);
		if (femaleCounter == 0) printf("The total percentage of Females who consume less than 4 units alcohol is: 0.00%%\n");
		else printf("The total percentage of Females who consume less than 4 units alcohol is: %.2f%%\n", percentageFemaleLess4Units);
		if (maleCounter == 0) printf("The total percentage of Males who consume greater than 4 units of alcohol is: 0.00%%\n");
		else printf("The total percentage of Males who consume greater than 4 units of alcohol is: %.2f%%\n", percentageMaleGreater4Units);
		if (femaleCounter == 0) printf("The total percentage of Females who consume greater than 4 units alcohol is: 0.00%%\n\n");
		else printf("The total percentage of Females who consume greater than 4 units alcohol is: %.2f%%\n\n", percentageFemaleGreater4Units);
	}
	if (number == 1) {
		file_ptr = fopen("SurveyReport.txt", "a");//append statistics to the file

		if (file_ptr != NULL) {
			fprintf(file_ptr, "\n==========================================================================\n");
			fprintf(file_ptr, "\t\t\t  Gender Statistics\n");
			fprintf(file_ptr, "==========================================================================\n");
			if (maleCounter == 0) fprintf(file_ptr, "The total percentage of Males who smoke is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Males who smoke is: %.2f%%\n", percentageMaleSmokers);
			if (femaleCounter == 0)  fprintf(file_ptr, "The total percentage of Females who smoke is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Females who smoke is: %.2f%%\n", percentageFemaleSmokers);
			if (maleCounter == 0) fprintf(file_ptr, "The total percentage of Males who smoke < 20 is: 0.00%%");
			else fprintf(file_ptr, "The total percentage of Males who smoke < 20 is: %.2f%%\n", percentageMaleLess20);
			if (femaleCounter == 0) fprintf(file_ptr, "The total percentage of Females who smoke < 20 is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Females who smoke < 20 is: %.2f%%\n", percentageFemaleLess20);
			if (maleCounter == 0) fprintf(file_ptr, "The total percentage of Males who smoke < 40 is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Males who smoke < 40 is: %.2f%%\n", percentageMaleLess40);
			if (femaleCounter == 0) fprintf(file_ptr, "The total percentage of Females who smoke < 40 is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Females who smoke < 40 is: %.2f%%\n", percentageFemaleLess40);
			if (maleCounter == 0) fprintf(file_ptr, "The total percentage of Males who smoke > 40 is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Males who smoke > 40 is: %.2f%%\n", percentageMaleGreater40);
			if (femaleCounter == 0) fprintf(file_ptr, "The total percentage of Females who smoke > 40 is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Females who smoke > 40 is: %.2f%%\n", percentageFemaleGreater40);
			fprintf(file_ptr, "==================================================================================\n");
			if (maleCounter == 0) fprintf(file_ptr, "The total percentage of Males who don't exercise is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Males who don't exercise is: %.2f%%\n", percentageMaleNoExercise);
			if (femaleCounter == 0) fprintf(file_ptr, "The total percentage of Females who don't exercise is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Females who don't exercise is: %.2f%%\n", percentageFemaleNoExercise);
			if (maleCounter == 0) fprintf(file_ptr, "The total percentage of Males who exercise less than 3 times per week is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Males who exercise less than 3 times per week is: %.2f%%\n", percentageMaleExerciseLess3);
			if (femaleCounter == 0) fprintf(file_ptr, "The total percentage of Femaleswho exercise less than 3 times per week is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Femaleswho exercise less than 3 times per week is: %.2f%%\n", percentageFemaleExerciseLess3);
			if (maleCounter == 0) fprintf(file_ptr, "The total percentage of Males who exercise less than 5 times per week is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Males who exercise less than 5 times per week is: %.2f%%\n", percentageMaleExerciseLess5);
			if (femaleCounter == 0) fprintf(file_ptr, "The total percentage of Femaleswho exercise less than 5 times per week is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Femaleswho exercise less than 5 times per week is: %.2f%%\n", percentageFemaleExerciseLess5);
			if (maleCounter == 0) fprintf(file_ptr, "The total percentage of Males who exercise greater than 5 times per week is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Males who exercise greater than 5 times per week is: %.2f%%\n", percentageMaleExerciseGreater5);
			if (femaleCounter == 0) fprintf(file_ptr, "The total percentage of Females who exercise greater than 5 times per week is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Femaleswho exercise greater than 5 times per week is: %.2f%%\n", percentageFemaleExerciseGreater5);
			fprintf(file_ptr, "==================================================================================\n");
			if (maleCounter == 0) fprintf(file_ptr, "The total percentage of Males who consume no alcohol is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Males who consume no alcohol is: %.2f%%\n", percentageMaleNoAlcohol);
			if (femaleCounter == 0) fprintf(file_ptr, "The total percentage of Females who consume no alcohol is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Females who consume no alcohol is: %.2f%%\n", percentageFemaleNoAlcohol);
			if (maleCounter == 0) fprintf(file_ptr, "The total percentage of Males who consume less than 2 units of alcohol is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Males who consume less than 2 units of alcohol is: %.2f%%\n", percentageMaleLess2Units);
			if (femaleCounter == 0) fprintf(file_ptr, "The total percentage of Females who consume less than 2 units alcohol is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Females who consume less than 2 units alcohol is: %.2f%%\n", percentageFemaleLess2Units);
			if (maleCounter == 0) fprintf(file_ptr, "The total percentage of Males who consume less than 4 units of alcohol is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Males who consume less than 4 units of alcohol is: %.2f%%\n", percentageMaleLess4Units);
			if (femaleCounter == 0) fprintf(file_ptr, "The total percentage of Females who consume less than 4 units alcohol is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Females who consume less than 4 units alcohol is: %.2f%%\n", percentageFemaleLess4Units);
			if (maleCounter == 0) fprintf(file_ptr, "The total percentage of Males who consume greater than 4 units of alcohol is: 0.00%%\n");
			else fprintf(file_ptr, "The total percentage of Males who consume greater than 4 units of alcohol is: %.2f%%\n", percentageMaleGreater4Units);
			if (femaleCounter == 0) fprintf(file_ptr, "The total percentage of Females who consume greater than 4 units alcohol is: 0.00%%\n\n");
			else fprintf(file_ptr, "The total percentage of Females who consume greater than 4 units alcohol is: %.2f%%\n\n", percentageFemaleGreater4Units);
		}
		else {
			printf("\nError Accessing the file on your system");
		}
		if (head_ptr != NULL) {
			printf("Successfully printed statistics to a file called SurveyReport.txt\n");
		}
		fclose(file_ptr);
	}//if
}//generateGenderStatistics()

int displayLength(struct listElement* head_ptr) {
	struct listElement *temp;
	int count = 0;
	temp = head_ptr;

	if (head_ptr == NULL) {
		count = 0;
	}
	else {
		/*Loop through the list and count the elements of the link list as you loop*/
		while (temp != NULL) {
			count++;//increment count
			temp = temp->next;//move to the last node
		}//while
	}
	return count; //return the number of nodes in the linked list
}

int search(struct listElement* head_ptr, int pps) {
	int found = 0;//0 if we haven't found the pps
	int count = 0;
	struct listElement *temp;
	temp = head_ptr;

	while (temp != NULL) {
		count++;
		if (temp->value.ppsNum == pps){//if actaul pps num and temp_pps number are the same we have a match
			found = 1; //found is 1 if successful
		}//if to compare

		temp = temp->next;
	}//while

	return found; //returns either 0 or 1. 1 if it has found the id number

}//search()

void sortList(struct listElement** head_ptr) {
	struct listElement *temp;
	struct listElement *next;

	temp = *head_ptr;//temp pointing at the head of the list
	next = (*head_ptr)->next;//next pointing at the first element in the list
	
	/*Tempary Variables for the variable swap*/
	int tmpPps, tmpAgeBracket, tmpIncomeBracket, tmpExercise, tmpAlcohol, tmpCigarettes;
	char tmpFirstName[15]; char tmpLastName[15]; char tmpAddress[40]; char tmpEmail[30]; char tmpGender[2];
	
	while (next != NULL) {
		while (next != temp) {
			if (next->value.ppsNum < temp->value.ppsNum) {
				tmpPps = temp->value.ppsNum;
				temp->value.ppsNum = next->value.ppsNum;
				next->value.ppsNum = tmpPps;

				strcpy(tmpFirstName, temp->value.firstName);
				strcpy(temp->value.firstName, next->value.firstName);
				strcpy(next->value.firstName, tmpFirstName);

				strcpy(tmpLastName, temp->value.lastName);
				strcpy(temp->value.lastName, next->value.lastName);
				strcpy(next->value.lastName, tmpLastName);

				strcpy(tmpAddress, temp->value.address);
				strcpy(temp->value.address, next->value.address);
				strcpy(next->value.address, tmpAddress);

				strcpy(tmpEmail, temp->value.email);
				strcpy(temp->value.email, next->value.email);
				strcpy(next->value.email, tmpEmail);

				strcpy(tmpGender, temp->value.gender);
				strcpy(temp->value.gender, next->value.gender);
				strcpy(next->value.gender, tmpGender);

				tmpAgeBracket = temp->value.ageBracket;
				temp->value.ageBracket = next->value.ageBracket;
				next->value.ageBracket = tmpAgeBracket;

				tmpIncomeBracket = temp->value.incomeBracket;
				temp->value.incomeBracket = next->value.incomeBracket;
				next->value.incomeBracket = tmpIncomeBracket;

				tmpExercise = temp->value.excerise;
				temp->value.excerise = next->value.excerise;
				next->value.excerise = tmpExercise;

				tmpAlcohol = temp->value.alcohol;
				temp->value.alcohol = next->value.alcohol;
				next->value.alcohol = tmpAlcohol;

				tmpCigarettes = temp->value.cigarettes;
				temp->value.cigarettes = next->value.cigarettes;
				next->value.cigarettes = tmpCigarettes;
			}
			temp = temp->next;
		}
		temp = *head_ptr;
		next = next->next;
	}
	/*http://stackoverflow.com/questions/5526750/linked-list-sorting-in-c*/
}//sortList()

void printToFile(struct listElemnt* head_ptr) {
	file_ptr = fopen("SurveyReport.txt", "w");
	//write records to a file, over writing the file each time
	struct listElement *temp;

	temp = head_ptr;
	if (file_ptr != NULL) {

		if (head_ptr == NULL) {//nothing to display if head Pointer is Null
			printf("\nThere are no Surveys in the Database\n");
		}
		else {
			fprintf(file_ptr, "=============================================================\n");
			fprintf(file_ptr, "\t\t\tAll Surveys\n");
			fprintf(file_ptr, "=============================================================\n");
			while (temp != NULL) {
				//print elements to file
				fprintf(file_ptr, "\nPPS Number: %d\n", temp->value.ppsNum);
				fprintf(file_ptr, "Name: %s %s\n", temp->value.firstName, temp->value.lastName);
				fprintf(file_ptr, "Address: %s\n", temp->value.address);
				fprintf(file_ptr, "Email Addres: %s\n", temp->value.email);
				fprintf(file_ptr, "Gender: %s\n", temp->value.gender);

				if (temp->value.ageBracket == 1)  fprintf(file_ptr, "Age Bracket: 18-20 Years\n");
				if (temp->value.ageBracket == 2)  fprintf(file_ptr, "Age Bracket: 20-30 Years\n");
				if (temp->value.ageBracket == 3)  fprintf(file_ptr, "Age Bracket: 30-50 Years\n");
				if (temp->value.ageBracket == 4)  fprintf(file_ptr, "Age Bracket: 50-65 Years\n");
				if (temp->value.ageBracket == 5)  fprintf(file_ptr, "Age Bracket: 65+ Years\n");

				if (temp->value.incomeBracket == 1)  fprintf(file_ptr, "Income Bracket: No Income\n");
				if (temp->value.incomeBracket == 2)  fprintf(file_ptr, "Income Bracket: < 20,000\n");
				if (temp->value.incomeBracket == 3)  fprintf(file_ptr, "Income Bracket: < 40,000\n");
				if (temp->value.incomeBracket == 4)  fprintf(file_ptr, "Income Bracket: < 60,000\n");
				if (temp->value.incomeBracket == 5)  fprintf(file_ptr, "Income Bracket: < 80,000\n");
				if (temp->value.incomeBracket == 6)  fprintf(file_ptr, "Income Bracket: < 100,000\n");
				if (temp->value.incomeBracket == 7)  fprintf(file_ptr, "Income Bracket: > 100,000\n");

				if (temp->value.excerise == 1)  fprintf(file_ptr, "Exercise: Never\n");
				if (temp->value.excerise == 2)  fprintf(file_ptr, "Exercise: Less Than 3 times per week\n");
				if (temp->value.excerise == 3)  fprintf(file_ptr, "Exercise: Less than 5 times per week\n");
				if (temp->value.excerise == 4)  fprintf(file_ptr, "Exercise: More than 5 times per week\n");

				if (temp->value.alcohol == 1)  fprintf(file_ptr, "Alcohol Consumption: None\n");
				if (temp->value.alcohol == 2)  fprintf(file_ptr, "Alcohol Consumption: Less than 2 Units\n");
				if (temp->value.alcohol == 3)  fprintf(file_ptr, "Alcohol Consumption: Less than 4 units\n");
				if (temp->value.alcohol == 4)  fprintf(file_ptr, "Alcohol Consumption: More than 4 units\n");

				if (temp->value.cigarettes == 1)  fprintf(file_ptr, "Cigarette Consumption: None\n");
				if (temp->value.cigarettes == 2)  fprintf(file_ptr, "Cigarette Consumption: Less than 20\n");
				if (temp->value.cigarettes == 3)  fprintf(file_ptr, "Cigarette Consumption: Less than 40\n");
				if (temp->value.cigarettes == 4)  fprintf(file_ptr, "Cigarette Consumption: More than 40\n");
	
				temp = temp->next;//move to the last node
			}//while

			printf("\nSuccessfully printed records to file called SurveyReport.txt\n"); //let the user know the records were printed to file
		}
		fclose(file_ptr);//close the file
	}
	else {
		printf("\nError opening file on your system!");/*Error handling so the program doesn't crash if it
																 can't open the file for some reason*/
	}
}//printToFile()

void save(struct listElemnt* head_ptr) {
	//file_ptr = fopen("Surveys.txt", "w");
	struct listElement *temp;
	temp = head_ptr;

	if (head_ptr == NULL) {//nothing to display if head Pointer is Null so delete the Survey.txt file 
		remove("Surveys.txt");//remove the file
	}
	else {
		file_ptr = fopen("Surveys.txt", "w+");//open the file to write
		if (file_ptr != NULL) {//if the file is not empty
			while (temp != NULL) {
				//print elements to file
				fprintf(file_ptr, "%d", temp->value.ppsNum);
				fprintf(file_ptr, " %s", temp->value.firstName);
				fprintf(file_ptr, " %s", temp->value.lastName);
				fprintf(file_ptr, " %s", temp->value.address);
				fprintf(file_ptr, " %s", temp->value.email);
				fprintf(file_ptr, " %s", temp->value.gender);
				fprintf(file_ptr, " %d", temp->value.ageBracket);
				fprintf(file_ptr, " %d", temp->value.incomeBracket);
				fprintf(file_ptr, " %d", temp->value.excerise);
				fprintf(file_ptr, " %d", temp->value.alcohol);
				fprintf(file_ptr, " %d\n", temp->value.cigarettes);
				temp = temp->next;//move to the last node
			}//while
			fclose(file_ptr);//close file
		}
	}
}//save()

void load(struct listElemnt** head_ptr) {
	struct listElement *newNode;
	file_ptr = fopen("Surveys.txt", "r+");//open the file to read

	if (file_ptr != NULL) {
		while (!feof(file_ptr)) {//while not at the end of the file

			newNode = (struct listElement*)malloc(sizeof(struct listElement));
			//read in values from Surveys.txt and put them into their respective variables
			fscanf(file_ptr, "%d %s %s %s %s %s %d %d %d %d %d\n", &newNode->value.ppsNum, newNode->value.firstName, newNode->value.lastName,
																	newNode->value.address, newNode->value.email, newNode->value.gender,
																	&newNode->value.ageBracket, &newNode->value.incomeBracket,
																	&newNode->value.excerise, &newNode->value.alcohol, &newNode->value.cigarettes);
			newNode->next = *head_ptr; //newNode->next is now the head_ptr
			*head_ptr = newNode;//head pointer is now pointing at new node
		}//while
		fclose(file_ptr);//close the file
	}
	else {
		printf("\nError opening File!!");//error handling
	}
}//load()

void editMenu() {
	printf("\n========Edit Menu============\n");
	printf("Type 1 to edit First Name\n");
	printf("Type 2 to edit Last Name\n");
	printf("Type 3 to edit Address\n");
	printf("Type 4 to edit Email\n");
	printf("Type 5 to edit Gender\n");
	printf("Type 6 to edit Age Bracket\n");
	printf("Type 7 to edit Income Bracket\n");
	printf("Type 8 to edit Exercise\n");
	printf("Type 9 to edit Alcohol Consumption\n");
	printf("Type 10 to edit Cigarette Consumption\n");
	printf("Type -1 to return to Main Menu!\n");
}//editMenu()

int validateEmail(char email[]) {
	int validate = 0;
	int count1 = 0;
	int count2 = 0;
	char validCharacter1 = '@';
	char validCharacter2 = '.';
	char emailEnd[] = ".com";
	char *lastFour;//holds the last four characters of the email

	lastFour = &email[strlen(email) - 4];//gets the last four characters of the email and stores them in a char array

	for (int i = 0; i < strlen(email); i++) {//loop around the length of the email string
		if (validCharacter1 == email[i]) {
			count1++;
		}
		if (validCharacter2 == email[i]) {
			count2++;
		}
	}//for
	if (count1 > 0 && count2 > 0) {
		if (strcmp(lastFour, emailEnd) == 0) {
			validate = 1;
		}
	}
	return validate;//returns a 1 if the email is a valid one with a '@', '.' and finshes in ".com". otherwise the function returns a 0
}//validateEmail()


