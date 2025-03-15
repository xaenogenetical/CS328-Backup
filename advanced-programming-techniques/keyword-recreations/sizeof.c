
int sizeof_var(void *declaration_location)
{
    // declaration_location is the memory address where the variable type is defined
    // declaration must end with a null byte
    // User is responsible for ensuring that the location provided is correct

    char *reader = declaration_location;
    int size = 0;
    size = size + *reader & 0x01;
    reader++;

    return size;
lab:
    return 0;
}