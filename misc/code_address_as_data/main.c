#include <stdio.h>

void hello()
{
    printf("Hello, World!\n");
}

int main()
{
    void *address = (void *)hello;
    printf("Address of hello function: %p\n", address);
    printf("Calling hello function using address...\n");
    ((void (*)(void))address)(); // Cast address to function pointer and call it
    printf("Function call completed.\n");
    void (*func_ptr)() = hello; // A more portable (and safer) version
    func_ptr();
    return 0;
}