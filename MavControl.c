//==============================================================================
//
// Title:		MavControl
// Purpose:		A short description of the command-line tool.
//
// Created on:	4/26/2016 at 1:35:09 PM by Yunhua Liu.
// Copyright:	Flextronics. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files
#define MAVLINK_C2000 0

#include <windows.h>

#include <rs232.h>
#include <ansi_c.h>

#include "MavCtrl_CVI.h"

//#include <math.h>
#include <math_ex.h>

//#define MAVLINK

#include "./include/kirkwood/mavlink.h"
#include "./include/kirkwood/kirkwood.h"

//#include "./include/mavlink_types.h"
//#include "./include/protocol.h"
//#include "./include/common/mavlink.h"
//#include "./include/kirkwood/mavlink_msg_kirkwood_test.h"
//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

/// HIFN  Explain how to use this command-line tool.
/// HIPAR name/The name of the command-line tool.
static void usage (char *name)
{
	fprintf (stderr, "usage: %s <argument>\n", name);
	fprintf (stderr, "A short summary of the functionality.\n");
	fprintf (stderr, "    <argument>    is an argument\n");
	exit (1);
}

//==============================================================================
// Global variables

//==============================================================================
// Global functions

/// HIFN  The main entry-point function.
/// HIPAR argc/The number of command-line arguments.
/// HIPAR argc/This number includes the name of the command-line tool.
/// HIPAR argv/An array of command-line arguments.
/// HIPAR argv/Element 0 contains the name of the command-line tool.
/// HIRET Returns 0 if successful.
int main (int argc, char *argv[])
{
	//if (argc < 2 || argc > 3)
	//	usage (argv[0]);
	int port = 12;

	char input_buffer[140];    
	//printf("-0.1 absf() = %f\n",fabsf(-0.1));

	printf("=======LabWindows/CVI MavCtrl CmdLine Ver 1.0.0.1================\n");
	printf("Please enter COM port number:");
	memset(input_buffer,0,140); 
	gets(input_buffer);
	port = atoi(input_buffer);
	int cmd_type = 0;
	
	int portOpenRet = OpenComConfig (port, "", 115200, 0, 8, 1, 512, 512);
	
	if( portOpenRet < 0)
	{
		printf("Open serial port error, errCode=%d\n",portOpenRet);
		printf("Exit...\n",portOpenRet);
	    getchar();
	    return 0;
	}

	
	printf("0:Send encode test command(DM36x)\n");
	printf("1:Send flight control command(STM32)\n");
	printf("Please enter your choice:");
	gets(input_buffer);
	int input = atoi(input_buffer);
	//int input = getchar();
	if(input == 0)
	{
	  cmd_type = 0;	
	}
	else
	{
	  cmd_type = 1;	
	}
	
	printf("Type 1 to Leave MavCtrl Mode.\n");
	
	while(1)
	{
		Sleep(200);
		
		memset(input_buffer,0,140);
		
		if( cmd_type == 0)
		{
		  printf("DM36X>");	
		}
		else
		{
		  printf("STM32>");		
		}
		gets(input_buffer);
		
		if(strcmp(input_buffer , "1") == 0)  
		{
			printf("Leave MavCtrl Mode...\n");
			break;
		}
		mavlink_query(port,cmd_type,10000,input_buffer);
	}
	
	
	CloseCom(port);

	printf("Exit...\n");
	getchar();
	return 0;
}


