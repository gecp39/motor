/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|         Copyright (c) 1998-2008 Texas Instruments Incorporated           |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission is hereby granted to licensees of Texas Instruments           |**
**| Incorporated (TI) products to use this computer program for the sole     |**
**| purpose of implementing a licensee product based on TI products.         |**
**| No other rights to reproduce, use, or disseminate this computer          |**
**| program, whether in part or in whole, are granted.                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>		/* low-level i/o */
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <pthread.h>
#include <time.h>
#include <sys/stat.h>

#include <signal.h>

#include <asm/arch/davinci_pwm.h>

//-----------------------------------------------------
//add by jiun
#define sockConnect "/var/run/pwmsocket"
#define BUFFSIZE 255
#define REMOTEFOCUSD_FOCUS_MOTOR_DEVICE_FILE	"/dev/focusmotor"
/* Set the motor forward direction */
#define MOTOR_SET_FORWARD		_IO (MOTOR_MAGIC, 0)
/* Set the motor backward direction */
#define MOTOR_SET_BACKWARD		_IO (MOTOR_MAGIC, 1)
/* Set the walk steps value of motor */
#define MOTOR_WALK_STEPS		_IOW (MOTOR_MAGIC, 3, DWORD)
/* Clear the current walk steps value of motor */
#define MOTOR_WALK_STEPS_CLR	_IO (MOTOR_MAGIC, 4)

int bctrlFocusMotor;
int bForward;
int bBackWard;
int iStep;
//-----------------------------------------------------

void *thread_recvcmd(void *arg) 
{
	//connect with socket...
	int sockFd, cfd, sockSize, szRecv;
	int iSelect;
	int maxfdp;

	int irecvReturn;
	char acRecMsg[BUFFSIZE];
	char *acValue;
	struct timeval timeout;
	struct sockaddr_un sock;
	fd_set fds;

	timeout.tv_sec = 2;               //make select wait up to 5 second for data
    timeout.tv_usec = 0;              //and 0 milliseconds
	cfd = -1;
	
	sockFd = socket(AF_UNIX, SOCK_STREAM, 0);
	
	unlink (sockConnect);
	
	sock.sun_family = AF_UNIX;
	strcpy(sock.sun_path, sockConnect);
	sockSize = sizeof(sock.sun_family) + strlen(sock.sun_path);
	if(bind(sockFd, (struct sockaddr *)&sock, sockSize) != 0)
	{
		printf("Failed bind()\n");
	}
	
	listen(sockFd, 7);
	maxfdp = sockFd + 1;
	
	while(1)
	{	
		FD_ZERO(&fds);
		FD_SET(sockFd, &fds);

		//select 監控fd_set裡的fd是否"有"可讀寫
		if (select(maxfdp + 1, &fds, NULL, NULL, &timeout) > 0)
		{
			if(FD_ISSET(sockFd, &fds))		//檢查fd文件是否可讀寫，且判斷是否為socket的fd
			{
				if ((cfd = accept(sockFd, &sock, &szRecv)) > 0)	printf("connect with client  by cfd : %d", cfd);
			}
		}	
		
		if (cfd)
		{
			irecvReturn = recv(cfd, acRecMsg, BUFFSIZE,0);
			if (irecvReturn == 0)	cfd = -1;		//close connect, release cfd...
			else if (irecvReturn > 0)
			{
				if (strncmp(acRecMsg, "L", 1) == 0)
				{
					acValue = strchr(message, '_');
					*(acValue ++) = '\0';
					iStep = atoi(acValue);
					bctrlFocusMotor = 1;
					bForward = 1;
				}
				else if (strncmp(acRecMsg, "R", 1) == 0)
				{
					acValue = strchr(message, '_');
					*(acValue ++) = '\0';
					iStep = atoi(acValue);
					bctrlFocusMotor = 1;
					bBackward = 1;
				}
			}
		}
	
	}
	
	close(sockFd);
	return NULL;
}

int main(int argc, char *argv[])
{
	int fdFocusMotorDevice;
	bctrlFocusMotor = 0;
	bForward = 0;
	bBackWard = 0;
	iStep = 0;
	
	daemon(0, 0);				//daemon mode

//-----------------------------------------------------
//add by jiun
	pthread_t thread_ID1;
	pthread_create(&thread_ID1, NULL, thread_recvcmd, NULL);

//-----------------------------------------------------

	while(1)
	{
		if (bctrlFocusMotor)
		{	
			bctrlFocusMotor = 0;
			// read fdFocusMotor fd
			fdFocusMotorDevice = open(REMOTEFOCUSD_FOCUS_MOTOR_DEVICE_FILE, O_WRONLY);
			
			if (fdFocusMotorDevice < 0)
			{
				REMOTEFOCUSD_DBGPRINT("Failed to open device\n");
				bctrlFocusMotor = -1;
				break;
			}
			
			//Eexcute cmd....
			if (bForward)
			{
				bForward = 0;
				ForwardFoomMotor(fdFocusMotorDevice, iStep);
			}
			else if (bBackward)
			{
				bBackward = 0;
				BackwardFoomMotor(fdFocusMotorDevice, iStep);
			}
			
			signal(SIGINT, sig_exit);
			signal(SIGTERM, sig_exit);
		}
		
		printf("waitting.......\n");
		sleep(1);
	}
	pthread_join (thread_ID1, NULL );
}

//--------------------------------------------------------

int ForwardFoomMotor(int fdMotorDevice, DWORD dwStep)
{
	ioctl(fdMotorDevice, MOTOR_SET_FORWARD);
	iRet = ioctl(fdMotorDevice, MOTOR_WALK_STEPS, dwStep);
	usleep(REMOTEFOCUSD_ADJUST_STEP * REMOTEFOCUSD_FOCUS_ONESTEP_TIME);
	usleep(1000000);
}

int BackwardFoomMotor(int fdMotorDevice, DWORD dwStep)
{
	ioctl(fdMotorDevice, MOTOR_SET_BACKWARD);
	iRet = ioctl(fdMotorDevice, MOTOR_WALK_STEPS, dwStep);
	usleep(1000000);
}