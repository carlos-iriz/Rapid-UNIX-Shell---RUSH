// Carlos Irizarry Castro
// The following program is a simple UNIX shell where the user types in a command and the shell creates a child process 
// that executes the command. The shell is called rush (Rapid Unix SHell)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>

#define MAX_LEN 255
char **path = NULL;
char error_message[30] = "An error has occurred\n"; 

// Function takes the the plain text of the user input as a string and removes the whitespace from the string by making a new string
// that starts at the first non-whitespace character and ends at the last non-whitespace character.
void del_whitespace(char *input_str){ 

    int start = 0;
    int end = strlen(input_str) - 1;

    // Finds the index of the first non-whitespace character by incrementing the start variable for every whitespace char that is found
    while (input_str[start] == ' ' || input_str[start] == '\t' || input_str[start] == '\n') {
        start++;
    }

    // Finds the index of the first non-whitespace character from the end
    while (end > start && (input_str[end] == ' ' || input_str[end] == '\t' || input_str[end] == '\n')) {
        end--;
    }

    // Shifts the remaining characters to the beginning of the string
    for (int i = 0; i <= end - start; i++) {
        input_str[i] = input_str[start + i];
    }

    // Adds the Null-terminator to the new string
    input_str[end - start + 1] = '\0';
}


// Function takes in the plain input from the command line and also takes in what will be where the formatted output will be stored
// The function eliminates the whitespace in the front and back of the string using the del_whitespace function. With the new input
// the function will save each character into a temp string which is then added to the formatted_input array when whitespace is detected
// and will continue until the end of the given input string.

int delimit_input_str(char *input_str, char *formatted_input[]) {

    // Clear the contents of formatted_str for whenever we read in a new formatted_input
    for (int i = 0; formatted_input[i] != NULL; i++) {
        free(formatted_input[i]);
        formatted_input[i] = NULL;
    }

    del_whitespace(input_str); // Calls del_whitespace function which removes starting and ending whitespace from string
    
    int i = 0;
    int x = 0;
    char temp_str[MAX_LEN];

    while (input_str[i] != '\0') {
        while (isspace(input_str[i]) != 0) { // Skips whitespace after a string is inserted into array
            i++;
        }

        int j = 0;
        while (input_str[i] != '\0' && !isspace(input_str[i])) { // Stores characters into temp_str until a whitespace is encountered
            temp_str[j] = input_str[i];
            i++;
            j++;
        }
        temp_str[j] = '\0'; // Null-terminate the temp_str once whitespace is found

        formatted_input[x] = malloc(strlen(temp_str) + 1); // Allocate memory for formatted_str[x] and copy temp_str into it
        strcpy(formatted_input[x], temp_str);
        x++;
    }

    int formatted_length = 0;

    while(formatted_input[formatted_length] != NULL){ // Loops through final formatted_length array to save the length
        formatted_length ++;
    }
        
    return formatted_length;
    
    }
// Returns what kind of command user inputted (parallel, redirect, or "normal")
int check_input_type(char *input[]){ 
    int i = 0;
    while(input[i] != NULL){
        if((strcmp(input[i], "&") == 0)){ // Means it is a parallel command
            return 2;
        }

        else if((strcmp(input[i], ">") == 0)){ //Means we are doing a redirect command
            return 1;
        }
        i++;
    }
    return 0; // means we are doing a "Normal Operation" (Not parallel or redirect)
}

//Function gets inputted arguement array and executes the command that the user specificed in the input variable
int execute_command(char *input[]) {

    if (path == NULL) {
        return -1;
    }
    
    char **temp_path = NULL; 
    temp_path = (char **)malloc(sizeof(char *));
    temp_path[0] = (char *)malloc(MAX_LEN);  // Allocate memory for the path
    temp_path[1] = NULL;

    int i = 0;
    while (path[i] != NULL)
    {
        strcpy(temp_path[0], path[i]);  // Copy the path into the allocated memory
        strcat(temp_path[0], "/");
        strcat(temp_path[0], input[0]);
        
        int status = access(temp_path[0], F_OK | X_OK);
        if (status == 0)
        {   
            //printf("Error 1\n");
            execv(temp_path[0], input);
            write(STDERR_FILENO, error_message, strlen(error_message));
            fflush(stderr);
            exit(1);
        }
        i++;
    }

    // Free the allocated memory for temp_path
    free(temp_path[0]);
    free(temp_path);

    return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////// Declaration of functions ends

int main(int argc, char *argv[])
{
    char *command_line_input = NULL;
    size_t command_line_size = 0;
    int input_type;
    char *formatted_input[30];

    if (argc > 1) //If the command line is less than 1 it is an error
    {
        write(STDERR_FILENO, error_message, strlen(error_message));
        fflush(stderr);
        exit(1);
    }

    path = (char **)malloc(sizeof(char *)); //Initializes path
    path[0] = (char *)malloc(strlen("/bin") + 1);
    strcpy(path[0], "/bin");
    path[1] = NULL;

    while(1==1){
    
        printf("rush> ");
        fflush(stdout);

        int input_line = getline(&command_line_input, &command_line_size, stdin); // saves text to command_line_args

        if (input_line == -1) // Getline has failed throw an error
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            fflush(stderr);
            continue;
        }

        // This section of code checks for whitespace and makes sure that an error is not thrown if any is encountered

        int i = 0;
        int blank_check = 0;

        while(command_line_input[i] != '\0'){ // Loops through user input stored on string until the null character is reached
            if(isspace(command_line_input[i]) != 0){ //If a non-whitespace character is found, break and save index of command in i
                break; //Breaks out of loop
            }
            if(command_line_size == i + 1){ // if command_line_size is equal to i  + 1 that means the input is blank
                blank_check = 1; //Blank_check being set to 1 means the input should be ignored
            }
            i++;
        }

        // i is at the command index, if we loop through the whole command_line_input, we continue
        while(command_line_input[i] != '\0'){
            if(isspace(command_line_input[i] != 0)){
                break;
            }

            if(command_line_size == i + 1){ // if command_line_size is equal to i  + 1 that means the input past the non-whitespace is blank
                blank_check = 1; //Blank_check being set to 1 means the input should be ignored
            }
            i++;
        }

        if(blank_check == 1){ //If blank check equals 1 it means we continue onto the next iteration of the while loop (blank input is not an error)
            continue; //If blank check equals 1 it means we continue onto the next iteration of the while loop (blank input is not an error)
        }

        // User input is stored into an "array of strings" without any whitespace in between
        int formatted_length = delimit_input_str(command_line_input, formatted_input);


        if(formatted_length == 0){ //Empty command is ignored
            continue;
        }

////////////////////////////////////////////////////////// Section that covers built in commands

        if (strcmp(formatted_input[0], "exit") == 0) //If exit commmand is found we perform the exit operation
        {
            if(formatted_length == 1) //Means exit had no argument therefore it is a vaild command
            {
                exit(0);
            }
            else{ //If exit has anything else we throw an error
                write(STDERR_FILENO, error_message, strlen(error_message));
                fflush(stderr);
                continue;
            }
        }

        else if (strcmp(formatted_input[0], "cd") == 0)  //If cd commmand is found we perform the cd operation
        {
            if (formatted_length == 2){ //Means we have only 1 arguement which is valid so we run cd
                if (chdir(formatted_input[1]) == 0)
                {   
                    continue;
                }
                else{ //There was an error in chdir throw an error
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    fflush(stderr);
                    continue;
                }
            }
            else{ //If there is no arguement or if there are more than 1 arguement throw an error
                write(STDERR_FILENO, error_message, strlen(error_message));
                fflush(stderr);
                continue;
            }
        }

        else if (strcmp(formatted_input[0], "path") == 0){  //If cd commmand is found we perform the cd operation
            if (formatted_length > 1){
                int i = 0;
                while (i < formatted_length){
                    path = (char **)realloc(path, (i + 2) * sizeof(char *));
                    path[i] = (char *)malloc(strlen(formatted_input[i]) + 1);
                    strcpy(path[i], formatted_input[i]);
                    i++;
                }
                path[i] = NULL;
            }
            else{ //Path is not given any arguements
                free(path[0]);
                free(path);
                path = NULL;
            }
            continue;
        }
        
        //If the Program reaches this point the input was not a built in command and will now check for "Normal", parallel, or redirect commands
        //input_type uses the check_input_type function to see what kind of command the user has read in
        input_type = check_input_type(formatted_input); 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Normal case
        if (input_type == 0) { // If input is not parallel or redirect
            int status;

            pid_t pid = fork(); // Create child process

            if (pid == 0) {
                // Child process
                status = execute_command(formatted_input); // Execute command

                if (status == -1) { // If command execution fails, throw error
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    fflush(stderr);
                    exit(1);
                }

                exit(1); // Exit child process
            }

            if (pid <= -1) {// If creating child process fails, throw error
                write(STDERR_FILENO, error_message, strlen(error_message));
                fflush(stderr);
                continue;
            }

             else {
                waitpid(pid, &status, 0); // Parent waits for the child to finish

                if (status == -1) {// If child process encounters an error, exit
                    exit(1);
                }
            }
        }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        else if (input_type == 1) { // If input is redirect
            int index_file; // Stors index of the file for redirection
            int status; // Variable to store the status of the command execution

            int check = 0;
            for (int i = 0; i < formatted_length; i++) { // Loop through formatted input and check if more than 1 ">" 
                if ((strcmp(formatted_input[i], ">") == 0)) {
                    index_file = i + 1; // Save the location of the file output name in formatted_input
                    check++;
                }
            }

            if (check > 1) { // If more than 1 ">" found, display an error message and continue
                write(STDERR_FILENO, error_message, strlen(error_message));
                fflush(stderr);
                continue;
            }

            if ((formatted_length > index_file + 1)) { // If there are arguments after ">", throw error
                write(STDERR_FILENO, error_message, strlen(error_message));
                fflush(stderr);
                continue;
            }

            pid_t pid = fork(); // Create a child process
            if (pid < 0) { // If creating child process fails, throw error
                write(STDERR_FILENO, error_message, strlen(error_message));
                fflush(stderr);
                continue;
            } 

            else if (pid == 0) { // Child process is success: redirect output to a file and execute command
                int redirect_file = open(formatted_input[index_file], O_CREAT | O_WRONLY | O_TRUNC, 0664);

                if (redirect_file == -1) {// If opening the file fails, throw error
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    fflush(stderr);
                    exit(1);
                }

                if (dup2(redirect_file, STDOUT_FILENO) < 0) {// If redirect fails, throw error
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    fflush(stderr);
                    exit(1);
                }

                close(redirect_file);

                status = execute_command(formatted_input); // Execute command

                if (status == -1) { // If command execution fails, throw error
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    fflush(stderr);
                    continue;
                }

                exit(1); // Exit the child process
            } 
            
            else { // Wait for the child to finish
                waitpid(pid, &status, 0);
            }
        }


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Parallel Case
        else if (input_type == 2) { // If input is parallel
            int num_processes = 0; // Variable to store the number of child processes
            int i = 0;
            int start = 0; // Starting index for a command in formatted input

            while (formatted_input[i] != NULL) {
                if (strcmp(formatted_input[i], "&") == 0) {
                    formatted_input[i] = NULL;
                    pid_t pid = fork();

                    if (pid < 0) { // If creating child process fails, throw an error
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        fflush(stderr);
                        break;
                    }
                    
                    else if (pid == 0) { // Child process: Execute the command
                        int status = execute_command(formatted_input + start);

                        if (status == -1) { // If command execution fails, throw an error
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            fflush(stderr);
                            exit(1);
                        }

                        exit(0);
                    } 
                    
                    else { // Parent process hits, update process count and start index
                        num_processes++;
                        start = i + 1;
                    }
                }
                i++;
            }

            if (start < i) {
                pid_t pid = fork();

                if (pid < 0) { // If child process fails, throw an error
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    fflush(stderr);
                } 
                
                else if (pid == 0) { // Child process success, execute command
                    int status = execute_command(formatted_input + start);

                    if (status == -1) { // If command fails, throw an error
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        fflush(stderr);
                        exit(1);
                    }

                    exit(0);
                } 
                
                else { // Parent process hits update process count
                    num_processes++;
                }
            }

            while (num_processes-- > 0) {
                int status;
                wait(&status);

                if (status == -1) { // If a child process encounters an error, throw an error
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    fflush(stderr);
                }
            }
        }
    } //While loop ends here
} //Main ends here

