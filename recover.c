#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    // Check command-line arguments
    if (argc != 2)
    {
        printf("ERROR: Usage: ./recover [filename]\n");
        return 1;
    }

    // Open file passed in command-line
    FILE *infile = fopen(argv[1], "r");

    // Check file validity
    if (infile == NULL)
    {
        printf("File could not be opened.\n");
        return 1;
    }

    // Buffer for reading in bytes from memory card
    BYTE buffer[512];

    int jpeg_count = 0;

    char *outfile_name = malloc(sizeof(char) * 7);
    FILE *outfile = NULL;

    // Read in memory from card in 512-byte chunks
    while (fread(buffer, 1, 512, infile) != 0)
    {
        // If current chunk = beginning of new jpeg file, create new jpeg and output memory data
        if ((buffer[0] == 0xff) && (buffer[1] == 0xd8) && (buffer[2] == 0xff) && ((buffer[3] & 0xf0) == 0xe0))
        {
            // Close current file, if any
            if (jpeg_count > 0)
            {
                fclose(outfile);
            }

            // Create new file named "###.jpg"
            sprintf(outfile_name, "%03i.jpg", jpeg_count);
            outfile = fopen(outfile_name, "w");

            // Write current 512-byte chunk to outfile
            fwrite(buffer, 1, 512, outfile);

            // Increment file counter
            jpeg_count++;
        }
        // Else, if current chunk is not beginning of new file, continue writing to current file
        else if (jpeg_count > 0)
        {
            fwrite(buffer, 1, 512, outfile);
        }
    }

    fclose(infile);
    fclose(outfile);
    free(outfile_name);

    return 0;
}
