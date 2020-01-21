bool contains(unsigned short *array,unsigned short * toBeSearched){
    bool found = false;
    for(i = 0; i < sizeof(*array) / sizeof(unsigned short); i++)
    {
        if(*array[i] == *toBeSearched){
            found = true
            break;
        }
    }
    return found;
}
