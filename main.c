#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>

#include "SimplifiedAES.h"

int main(int argc, char* argv[])
{
    char opt;
    char* optstring = "dek:i:o:t";
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
            memcpy(k, optarg, 2);
            key_is_set = true;
            break;
        case 't':
            self_testing();
            exit(0);
        default:
            ShowHelp();
            exit(1);
        }
    }


    if (in_file_is_set && out_file_is_set && method_is_set)
    {
        if (!key_is_set)
        {
            GetKey(&k);
        }

        action(in_file_name, out_file_name, k);
    }
    else
    {
        ShowHelp();
    }

    return 0;
}
