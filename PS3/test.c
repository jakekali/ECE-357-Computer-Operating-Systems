       #include <sys/wait.h>
main()
{
int ws= -1;
f1();
if (fork()==0)
f1();

f1();
wait(&ws);


printf( "%d %d\n" , WIFEXITED(ws), ws);

return ws >> 8;
}
void f1()
{
static int i=10;
printf("%d\n",i);
i++;
}
