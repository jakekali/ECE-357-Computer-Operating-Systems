#include <stdio.h>
#include <unistd.h>
int main() {
int cnt = 0;
	for(;;) {
printf("%d\n", cnt);
cnt++;
sleep(1);
}
}
