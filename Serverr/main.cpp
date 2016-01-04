#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "UpgradeApplication.h"

int main(int argc, char** argv)
{
	if(argc >= 2)
	{
		if(argc == 2)
		{
			if(memcmp(argv[1], "--version", 9) == 0)
			{
				printf("version:1.0");
				return 0;
			}
			else
			{
				return 0;
			}
		}
	}
    // 守护进程
    //daemon(0, 1);  //  使程序进入后台工作
    while(1)
    {
        pid_t cpid, w;
        int status;
        cpid = fork();
        if(cpid < 0)
        {
     	    perror("fork error!");
     	    exit(EXIT_FAILURE);
        }
        else if(0 == cpid) // child process
        {
        	 //执行进程任务
        	UpgradeApplication app;
        	return app.run(argc,argv);
        }

         /***********如果子进程结束重开子进程**************/
    	 w = waitpid(cpid, &status, WUNTRACED | WCONTINUED);
    	 if (w == -1)
    	 {
    		 perror("waitpid\n");
    	 }

    	 sleep(3);
    }
}
