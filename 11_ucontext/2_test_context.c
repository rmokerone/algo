#include <stdio.h>
#include <ucontext.h>


void func1(void *arg){
    puts("1");
    puts("1111");
}

void context_test()
{
    char stack[1024*128];
    ucontext_t child, main;

    getcontext(&child);
    child.uc_stack.ss_sp = stack;
    child.uc_stack.ss_size = sizeof(stack);
    child.uc_stack.ss_flags = 0;
    child.uc_link = &main; // 后续执行的上下文

    makecontext(&child,  (void (*)(void)) func1, 0);

    swapcontext(&main, &child);

    puts("main");
}

int main()
{
    context_test();

    puts("over");

    return 0;
}