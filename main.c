#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <time.h>

#include "SimplifiedAES.h"

int main(int argc, char* argv[])
{
    int opt;
    char* optstring = "deki:o:t";
    bool (*action)(char*, char*, Key);

    char* in_file_name;
    char* out_file_name;

    bool in_file_is_set = false;
    bool out_file_is_set = false;
    bool key_is_set = false;
    bool method_is_set = false;

    Key k;

    while ((opt = getopt(argc, argv, optstring)) != -1)
    {
        switch (opt)
        {
        case 'd':
            action = FileDecrypt;
            method_is_set = true;
            break;
        case 'e':
            action = FileEncrypt;
            method_is_set = true;
            break;
        case 'i':
            in_file_name = optarg;
            in_file_is_set = true;
            break;
        case 'o':
            out_file_name = optarg;
            out_file_is_set = true;
            break;
        case 'k':
            key_is_set = true;
            break;
        case 't':
            self_testing();
            exit(EXIT_SUCCESS);
        default:
            ShowHelp();
            exit(EXIT_FAILURE);
        }
    }

    if (in_file_is_set && out_file_is_set && method_is_set)
    {
        // Use user specified key
        if (key_is_set)
        {
            GetKey(&k);
        }
        // Use random
        else
        {
            uint16_t rnd;
            FILE* key_file;

            if (action == FileEncrypt)
            {
                rnd = RandGenerator(time(NULL) % 0xFF);
                memcpy(k, &rnd, 2);

                key_file = fopen(".rnd", "wb");

                if (!key_file)
                {
                    printf("Key file .rnd could not be opened for write!\n");
                    exit(EXIT_FAILURE);
                }

                fwrite(k, 2, 1, key_file);
            }
            else
            {
                key_file = fopen(".rnd", "rb");

                if (!key_file)
                {
                    printf("Key file .rnd not found!\n");
                    exit(EXIT_FAILURE);
                }

                fread(k, 2, 1, key_file);
            }

            fclose(key_file);
        }

        action(in_file_name, out_file_name, k);
    }
    else
    {
        ShowHelp();
    }

    return 0;
}

