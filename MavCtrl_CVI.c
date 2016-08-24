#include <windows.h>

#include <rs232.h>
#include <ansi_c.h>

#include <math_ex.h> 

#include "./include/kirkwood/mavlink.h"
#include "./include/kirkwood/kirkwood.h"

#include "./include/mavlink_helpers.h"

int serial_write_message(int port, mavlink_message_t *message)
{
	uint8_t buf[MAVLINK_MAX_PACKET_LEN];

	// Translate message to buffer
	size_t len = mavlink_msg_to_send_buffer((uint8_t*)buf, message);

	if (0)
	{
		//printf("MAVSTX: 0x%02x PLEN: 0x%02x SEQ: 0x%02x SYS: 0x%02x COM: 0x%02x MSG: 0x%02x CRC: 0x%04x\n", 	buf[0],
		//																							buf[1],
		//																							buf[2],
		//																							buf[3],
		//																							buf[4],
		//																							buf[5], mavlink_message_crcs[buf[5]]);
		for (int i = 0; i < len; i++)
		{
			printf("%02x ", buf[i]);
			if (i % 32 == 0 && i != 0)
				printf("\n");
		}
		printf("\n");
	}

	// message length error
	if (len > MAVLINK_MAX_PACKET_LEN)
	{
		fprintf(stderr, "\nFATAL ERROR: MESSAGE LENGTH IS LARGER THAN BUFFER SIZE\n");
	}

	// Write buffer to serial port, locks port while writing
	int written = ComWrt(port,buf,len); 

	return written;
}




//--port        the comport sentinel is connected
//
//--test_type   0:KIRKWOOD_CMD_ENC_TEST,1:KIRKWOOD_CMD_FC_TEST,other:KIRKWOOD_CMD_FC_TEST
//
//--t_cmd       KIRKWOOD_CMD_ENC_TEST: "t api system version"
//              KIRKWOOD_CMD_FC_TEST:  "esc version" NOTE:NOT "t esc version"
//--return      1:CMD PASSED, 0:CMD FAILED, <0: ERROR

int send_t_cmd(int port,int test_type,const char arg[7][20],int arg_cnt)
{
	//--t api system version
    mavlink_message_t msg = {0}; 
    mavlink_kirkwood_test_t kirkwood_test = {0};
    //kirkwood_test.device  = KIRKWOOD_CMD_ENC_TEST;
	kirkwood_test.device  = KIRKWOOD_CMD_FC_TEST; 
    kirkwood_test.status  = KIRKWOOD_CMD_TEST_STATUS_START;
    kirkwood_test.verbose = 0;
    if(test_type == 0)
	{
		kirkwood_test.device  = KIRKWOOD_CMD_ENC_TEST;

		char * kwTestArgPtr[] = { kirkwood_test.arg0,
								  kirkwood_test.arg1,
								  kirkwood_test.arg2,
								  kirkwood_test.arg3,
								  kirkwood_test.arg4,
								  kirkwood_test.arg5,
								  kirkwood_test.arg6
								};
		if( arg_cnt > 6)
		{
			arg_cnt = 6;
		}

		if( arg_cnt < 0)
		{
			arg_cnt = 0;
		}

		kirkwood_test.argc    = arg_cnt; 
		
		for(int i=0; i<arg_cnt; i++)
		{
			strcpy(kwTestArgPtr[i],arg[i]);
		}
	}
	else
	{
		kirkwood_test.device  = KIRKWOOD_CMD_FC_TEST;
	  
	    
        char * kwTestArgPtr[] = { kirkwood_test.arg0,
								  kirkwood_test.arg1,
								  kirkwood_test.arg2,
								  kirkwood_test.arg3,
								  kirkwood_test.arg4,
								  kirkwood_test.arg5,
								  kirkwood_test.arg6
								};
		if( arg_cnt > 6)
		{
			arg_cnt = 6;
		}

		if( arg_cnt < 0)
		{
			arg_cnt = 0;
		}

		kirkwood_test.argc    = arg_cnt; 
		
		for(int i=0; i<arg_cnt; i++)
		{
			strcpy(kwTestArgPtr[i],arg[i]);
		}
		
		kirkwood_test.argc    = kirkwood_test.argc + 1; //--Number of arguments plus 1
								                        //--1 is reserved for 't'
	}
    

    mavlink_msg_kirkwood_test_encode(VIDEO_ENCODER_APP_COMP_ID,255,&msg,&kirkwood_test);

    int bytesWrite =  serial_write_message(port,&msg);
	
	return bytesWrite;
	

}

//int msgSize = 264; //NOT USE, but keep here.

//--port        the comport sentinel is connected
//
//--test_type   0:KIRKWOOD_CMD_ENC_TEST,1:KIRKWOOD_CMD_FC_TEST,other:KIRKWOOD_CMD_FC_TEST
//
//--time_out_ms 0:KIRKWOOD_CMD_ENC_TEST,1:KIRKWOOD_CMD_FC_TEST,other:KIRKWOOD_CMD_FC_TEST
//
//--t_cmd       KIRKWOOD_CMD_ENC_TEST: "t api system version"
//              KIRKWOOD_CMD_FC_TEST:  "esc version" NOTE:NOT "t esc version"
//--return      1:CMD PASSED, 0:CMD FAILED, <0: ERROR

int mavlink_query(int port,int test_type,int timeout_ms,const char * t_cmd)
{
	char arg[7][20];
	int  arg_cnt = 0;

	memset(arg,0,7*20);
	
	char * t_next = t_cmd;
	
	if(strlen(t_cmd) < 2)
	{
		return 0;
	}
	
	for(int i=0; i< 7;i++)
	{
		char *p = strstr(t_next," "); 
		
		if(p != NULL)
		{
		    memcpy(arg[i],t_next,p -t_next);
			arg_cnt++;
			t_next = p + 1;
			continue;
		}
		else
		{
			p = t_cmd + strlen(t_cmd);
			memcpy(arg[i],t_next,p -t_next);
			arg_cnt++; 
			break;
		}
	}
	
	send_t_cmd(port,test_type,arg,arg_cnt);
	//Sleep(1000);
	//int msgSize= sizeof(mavlink_message_t);
	//int msgSize = 264;

	mavlink_message_t rcvMsg = {0};
	mavlink_status_t status; 
	
	int exit_loop = 0;
	
	DWORD t_start,t_now;
	t_start = GetTickCount();
	
	while(1)
	{
		t_now = GetTickCount();
		
		if(t_now - t_start > timeout_ms)
		{
		  printf("ERROR: timeout - %u\n",timeout_ms);
		  break;
		}
		
		if(exit_loop != 0)
		{
			break;
		}
		
		char rdBuffer[10];
		memset(rdBuffer,0,10);
		
		int bytesRd =  ComRd(port,rdBuffer,1);  

		if( bytesRd != 1 )
		{
			continue;
		}

		
		//printf("\n----------------------------------------------------\n");
		//for(int i=0; i<msgSize ; i++)
		//{
		//	printf("%02X",rdBuffer[i]);
		//}
		//printf("\n----------------------------------------------------\n");
		//printf("\n==========Press enter to read next Msg!========\n");
		//getchar();

		
		int8_t parse_result = mavlink_parse_char(MAVLINK_COMM_1,rdBuffer[0],&rcvMsg,&status);

		//memcpy(&rcvMsg,(mavlink_message_t * )rdBuffer,msgSize);

		if( rcvMsg.msgid == MAVLINK_MSG_ID_KIRKWOOD_TEST && parse_result!=0)
		{

			mavlink_kirkwood_test_t kirkwood_test_rcv = {0};   
		    
		
			mavlink_msg_kirkwood_test_decode(&rcvMsg,&kirkwood_test_rcv);

			char dataPtr[121] = {0};

			

			switch (kirkwood_test_rcv.status)
			{
				case KIRKWOOD_CMD_TEST_STATUS_INFO:
				{
					//dataPtr = kirkwood_test_rcv.arg0;
					memcpy( dataPtr,kirkwood_test_rcv.arg0,120);
					dataPtr[120] = 0x00;
					printf( "%s" , dataPtr);
					break;
				}
				case KIRKWOOD_CMD_TEST_STATUS_OK:
				{
					printf("REMOTE RUNNING TEST\n");
					break;
				}
				case KIRKWOOD_CMD_TEST_STATUS_PASS:
				case KIRKWOOD_CMD_TEST_STATUS_FAIL:
				{
					int passed = kirkwood_test_rcv.status == KIRKWOOD_CMD_TEST_STATUS_PASS ? 1 : 0;
					//readingResults = false;
					/* Print the result here , user doenst have info abot the status message */
					if ( passed == 1 )
						printf("%s" ,"SUCCESS\n");
					else
						printf("%s" ,"FAILED\n");

					exit_loop = 1;//exit loop
					
					break;
				}
				default:
					printf("Read response %d\n", kirkwood_test_rcv.status);
			}
		}
	}
	
	return exit_loop;
}

