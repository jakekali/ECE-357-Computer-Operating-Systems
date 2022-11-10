#include <signal.h>
int flag = 0;
void handler(int signum)
{
    if (signum != 18)
    flag = signum;	
static int cnt=0;
cnt++;
printf("\n%d\n", signum); 
return;
}
int main(int argc,char ** argv)
{
    for(int i = 1; i < 32; i++) {
        if(i == 2 | i == 20) {
            continue; 
        }
signal(i,handler);
    }
for(;;) {
printf("hello %d \n", flag);
sleep(1);

//scanf(&s);
}
}

