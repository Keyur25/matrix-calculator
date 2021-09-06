#include <stdio.h>
#include <string.h>
#include <windows.h>

#define MAXROW 10
#define MAXCOLUMN 10
#define MAXNUM 32767
#define FILENAME "savedMatrices.txt"
#define APPEND "ab"
#define READ "rb"

FILE *filePointer;
char space[1];
char newLine[1];

void emptyInputBuffer(void)
{
    char ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

int userInput(char message[], int max)
{
    int choice = -1;
    printf("%s: ", message);
    while (!scanf("%d", &choice) || choice<1 || choice>max)
    {
        printf("Invalid Choice! Enter Integer Between 1-%d!\n", max);
        emptyInputBuffer();
        printf("%s: ", message);
    }
    emptyInputBuffer();
    return choice;
}

void printMatrix(int rows, int columns, int (*pointer)[columns])
{
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < columns; col++)
        {
             printf("%d ", *( *(pointer + row) + col));
        }
        printf("\n");
    }
    printf("\n");
}

int openFile(char purpose[])
{
    filePointer=fopen(FILENAME, purpose);
    if (filePointer == NULL)
    {
        printf("%s file failed to open!\n", FILENAME);
        return 0;
    }
    return 1;
}

void addToFile(int rows, int columns, int matrix[rows][columns])
{
    char space[] = " ";
    char newLine[] = "\n";
    putw(rows, filePointer);
    fwrite(space, sizeof(space), 1, filePointer);
    putw(columns, filePointer);
    fwrite(newLine, sizeof(newLine), 1, filePointer);
    fwrite(matrix, sizeof(int), rows*columns, filePointer);
    fwrite(newLine, sizeof(newLine), 1, filePointer);
}

int readRow(void)
{
    int rows = getw(filePointer);
    fread(space, sizeof(space), 2, filePointer);
    return rows;
}

int readCol(void)
{
    int columns = getw(filePointer);
    fread(newLine, sizeof(newLine), 2, filePointer);
    return columns;
}

void copyMatrix(int rows, int columns, int matrix1[rows][columns], int matrix2[rows][columns])
{
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < columns; col++)
        {
            matrix1[row][col] = matrix2[row][col];
        }
    }
}

int findMatrix(int choice, int row, int column, int matrix[row][column])
{
    int count = 0;
    while (1)
    {
        int rows = readRow();
        int columns = readCol();
        if (rows<1) //checks if there is anything left to read in the file
        {
            if (count==0)//checks if the function found any matrix at all
            {
                printf("Error! Could Not Find Matricies.\n");
                fclose(filePointer);
                return 0;
            }
            break;
        }
        int currentMatrix[rows][columns];
        fread(currentMatrix, sizeof(int), rows*columns, filePointer);
        if (rows==row && columns==column)
        {
            if (choice==++count)
            {
                copyMatrix(rows, columns, matrix, currentMatrix);
                break;
            }
        }
        fread(newLine, sizeof(newLine), 2, filePointer);
    }
    return 1;
}

int listCertainMatricies(int row, int col)
{
    if (openFile(READ) == 0)
    {
        return 0;
    }
    int count = 0;
    printf("\nAll Defined Matricies With %d Row/s And %d Column/s\n", row, col); 
    while (1)
    {
        int rows = readRow();
        int columns = readCol();
        if (rows<1) //checks if there is anything left to read in the file
        {
            if (count==0) //checks if the function found any matrix at all
            {
                printf("Error! No Matricies Defined With Given Number Of Rows And Columns!\nMatricies Can Be Defined From Main Menu.\n"); 
            }
            break;
        }
        int currentMatrix[rows][columns];
        fread(currentMatrix, sizeof(int), rows*columns, filePointer);
        if (rows==row && columns==col)
        {
            count++;
            printf("Matrix %d: \n", count);
            printMatrix(rows, columns, currentMatrix);    
        }
        fread(newLine, sizeof(newLine), 2, filePointer);
    }
    fclose(filePointer);
    return count;    
}

void add(int row, int col, int matrix1[row][col], int matrix2[row][col], int result[row][col])
{
    for (int r = 0; r < row; r++)
    {
        for (int c = 0; c < col; c++)
        {
            result[r][c] = matrix1[r][c] + matrix2[r][c];
        }
    }
}

void subtract(int row, int col, int matrix1[row][col], int matrix2[row][col], int result[row][col])
{
    for (int r = 0; r < row; r++)
    {
        for (int c = 0; c < col; c++)
        {
            result[r][c] = matrix1[r][c] - matrix2[r][c];
        }
    }
}

void addOrSubtract(char word[], char sign)
{
    printf("Two matrices must have an equal number of rows and columns to be %s.\nDefine the number of rows and columns.\n", word);
    int row = userInput("Rows", MAXROW);
    int col = userInput("Columns", MAXCOLUMN);
    int options = listCertainMatricies(row, col);
    if (options==0) return;
    printf("Choose First Matrix From The List Above\n");
    int choice1 = userInput("Matrix Number", options);
    printf("Choose Second Matrix From The List Above\n");
    int choice2 = userInput("Matrix Number", options);
    if (openFile(READ) == 0) return;
    int matrix1[row][col], matrix2[row][col], result[row][col];
    if (findMatrix(choice1, row, col, matrix1) == 0) return;
    fclose(filePointer);
    if (openFile("rb") == 0) return;
    if (findMatrix(choice2, row, col, matrix2) == 0) return;
    fclose(filePointer);
    if (sign=='+')
    {
        add(row, col, matrix1, matrix2, result);
    }
    else
    {
        subtract(row, col, matrix1, matrix2, result);
    }
    printf("\nRESULT: \n");
    printMatrix(row, col, result);
    Sleep(1500);
}

int multiplyInput(int *row, int *col1, int *col2)
{
    printf("For matrix multiplication, the number of columns in the first matrix must be equal to the number of rows in the second matrix.\nPlease define the number of rows and columns for the first matrix.\n");
    *row = userInput("Rows", MAXROW);
    *col1 = userInput("Columns", MAXCOLUMN);
    printf("Please define the number of columns for the second matrix.\nRows: %d \n", *col1);
    *col2 = userInput("Columns", MAXCOLUMN); 
}

void multiply(int row, int col1, int col2, int matrix1[row][col1], int matrix2[col1][col2], int result[row][col2])
{
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col2; j++)
        {
            int total = 0;
            for (int k = 0; k < col1; k++)
            {
                total = total + (matrix1[i][k] * matrix2[k][j]);    //this function finds the dot product of the every row in matrix 1 
            }                                                       //and every column in matrix 2 and assigns it to the result matrix
            result[i][j] = total;
        }
    }   
}

void multiplyMenu(void)
{
    int row, col1, col2;
    multiplyInput(&row, &col1, &col2);
    printf("\nFirst Matrix Options");
    int options1 = listCertainMatricies(row, col1);
    Sleep(1000);
    printf("Second Matrix Options");
    int options2 = listCertainMatricies(col1, col2);
    if (options1==0 || options2==0) return;
    printf("Select First Matrix\n");
    int choice1 = userInput("Matrix Number", options1);
    printf("Select Second Matrix\n");
    int choice2 = userInput("Matrix Number", options2);
    if (openFile(READ) == 0) return;
    int matrix1[row][col1], matrix2[col1][col2];
    if (findMatrix(choice1, row, col1, matrix1) == 0) return;
    if (openFile("rb") == 0) return;
    if (findMatrix(choice2, col1, col2, matrix2) == 0) return;
    fclose(filePointer);          
    int result[row][col2];
    multiply(row, col1, col2, matrix1, matrix2, result);
    printf("\nRESULT: \n");
    printMatrix(row, col2, result);
    Sleep(1500);
}

int checkDeterminant(int matrix[2][2])
{
    int a = matrix[0][0], b = matrix[0][1], c = matrix[1][0], d = matrix[1][1];
    int determinant = (a*d) - (b*c);
    if (determinant == 0)
    {
        printf("Matrix is singular so it doesn't have an inverse.\n");
    }
    return determinant;
}

void inverseCalculations(int matrix[2][2], float inverse[2][2])
{
    inverse[0][0] = matrix[1][1];
    inverse[0][1] = -1 * matrix[0][1];
    inverse[1][0] = -1 * matrix[1][0];
    inverse[1][1] = matrix[0][0];
}

void printInverse(int determinant, float inverse[2][2])
{
    printf("\nRESULT: \n");
    for (int r = 0; r < 2; r++)
    {
        for (int c = 0; c < 2; c++)
        {
            inverse[r][c] = (1.0/determinant)*inverse[r][c];
            printf("%f ", inverse[r][c]);
        }
        printf("\n");
    }
    printf("\n");
    Sleep(1500);  
}

void inverse2By2(void)
{
    printf("\nSelect 2*2 Matrix To Inverse");
    int options = listCertainMatricies(2, 2);
    int choice = userInput("Matrix Number", options);
    if (openFile(READ) == 0) return;
    int matrix[2][2];
    if (findMatrix(choice, 2, 2, matrix) == 0) return;
    fclose(filePointer);
    int determinant = checkDeterminant(matrix);
    if (determinant==0) return;
    float inverse[2][2];
    inverseCalculations(matrix, inverse);
    printInverse(determinant, inverse);
}

void defineMatrix(void)
{
    printf("Define the number of rows and columns for the new matrix.\n");
    int rows = userInput("Rows", MAXROW);
    int columns = userInput("Columns", MAXCOLUMN);
    int matrix[rows][columns];
    printf("Enter Numbers Left To Right From Top To Bottom.\n");
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < columns; col++)
        {
            printf("Position [%d][%d]\n", row, col);
            matrix[row][col] = userInput("Integer", MAXNUM);
        }
    }
    printf("\nMatrix:\n");
    printMatrix(rows, columns, matrix);
    printf("Would you like to save this matrix? (1 - yes or 2 - no)");
    if (userInput("Choice", 2) == 2) 
    {
        printf("Matrix Not Saved!\n"); 
        return;
    }
    if (openFile(APPEND) == 0) return;
    addToFile(rows, columns, matrix);
    fclose(filePointer);
    printf("Matrix Saved Successfully!\n");
}

void listMatrices(void)
{
    if (openFile(READ) == 0) return;
    int count = 0;
    while (1)
    {
        int rows = readRow(), columns = readCol();
        if (rows<1) //checks if there is anything left to read in the file
        {
            if (count==0) {printf("No Matricies Have Been Defined!\nMatricies Can Be Defined From Main Menu.\n");} //checks if the function found any matrix at all
            break;
        }
        int currentMatrix[rows][columns];
        fread(currentMatrix, sizeof(int), rows*columns, filePointer);
        printf("Matrix %d: \n", ++count);
        printMatrix(rows, columns, currentMatrix);
        fread(newLine, sizeof(newLine), 2, filePointer);
    }
    fclose(filePointer);
}

void goTo(int choice)
{
    switch (choice)
    {
        case 1: addOrSubtract("added", '+'); break;
        case 2: addOrSubtract("subtracted", '-'); break;
        case 3: multiplyMenu(); break;
        case 4: inverse2By2(); break;
        case 5: defineMatrix(); break;
        case 6: listMatrices(); break;
    }
}

void displayMenu(void)
{
    printf("************************\n");
    printf("       MAIN MENU\n");
    printf("\n");
    printf("     1 - Add\n");
    printf("     2 - Subtract\n");
    printf("     3 - Multiply\n");
    printf("     4 - Inverse 2*2\n");
    printf("     5 - Define Matrix\n");
    printf("     6 - List Matrices\n");
    printf("     7 - Quit\n");
    printf("************************\n");
}

void run(void)
{
    printf("\nDefine matricies first before\n using them in calculations!\n");
    printf("\n    MATRIX CALCULATOR\n\n");
    while (1)
    {
        displayMenu();
        int userOption = userInput("Choice", 7);
        if (userOption==7)
        {
            break;
        }
        goTo(userOption);
    }
}

int main(void)
{
    run();
    return 0;
}