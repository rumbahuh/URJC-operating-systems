#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Returns true if word contains any letter
*/
static int
hasletter(const char *word)
{
    for (int i = 0; word[i] != '\0'; i++) {
        if ((word[i] >= 'a' && word[i] <= 'z') || (word[i] >= 'A' && word[i] <= 'Z'))
            return 1;
    }

    return 0;
}

/*
Convert word to uppercase, storing only lowercase letters
*/
static char 
*tocapital(const char *word)
{
    int len = strlen(word);
    char *result = malloc(len + 1);
    if (!result) return NULL;
    
    int j = 0;
    for (int i = 0; word[i] != '\0'; i++) {
        if (word[i] >= 'a' && word[i] <= 'z') {
            result[j++] = word[i] - ('a' - 'A');
        } else if (word[i] >= 'A' && word[i] <= 'Z') result[j++] = word[i];
    }
    result[j] = '\0'; // last char in any string

    return result;
}

/*
Insertion sort for array of strings
*/
static void
insertionsort(int num, char *cadena[])
{
    for (int i = 1; i < num; i++) {
        char *key = cadena[i];
        int j = i - 1;
        while (j >= 0 && strcmp(cadena[j], key) > 0) {
            cadena[j + 1] = cadena[j];
            j--;
        }
        cadena[j + 1] = key;
    }
}

int
main(int argc, char *argv[])
{
	argc--;
	argv++;

	int i = 0;
	int valid_count = 0;
	int index = 0;

    if (argc < 1) {
        fprintf(stderr, "error: no arguments\n");
        return EXIT_FAILURE;
    }
    
    // Counter for words with letters
    for (; i < argc; i++) {
        if (hasletter(argv[i])) valid_count++;
    }
    
    if (valid_count == 0) return 0;
    
    // Allocate array for strings to be changed
    char **words = malloc(sizeof(char *) * valid_count);
    if (!words) return EXIT_FAILURE;
    
    for (int j = 0; j < argc; j++) {
        if (hasletter(argv[j])) {
            words[index] = tocapital(argv[j]);
            if (!words[index]) {
               // free before exiting after failure
                for (int k = 0; k < index; k++)
                    free(words[k]);
                free(words);

                return EXIT_FAILURE;
            }

            index++;
        }
    }
    
    insertionsort(valid_count, words);

	// Print words and ignore duplicates
	for (int l = 0; l < valid_count; l++) {
		if (l == 0 || strcmp(words[l], words[l-1]) != 0) {
			printf("%s\n", words[l]);
		}
		if (l > 0) {
			free(words[l-1]);
		}
	}
	
	if (valid_count > 0) {
		free(words[valid_count - 1]);
	}
    
    free(words);
    return 0;
}