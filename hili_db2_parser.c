//add a size lock in the function "kill_flow"

//To find the structure send-log interface, please search "STRUCTURE_INTERFACE_HERE"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
//#include "hili_dummy_mitm.h"
#include "fifo_cache.h"
#include "hili_db2_parser_content.h"
#include "hili_se_send_2_linux.h"


//#define PRINT 
#define TEST_NEW_JOINT
#define BLACKLIST_JUDGEMENT
#define PASSWD_FILLIN

int temp_opr_log;
int temp_ses_log;


//改成正向输出带符号
void hili_db2_parse_itoa (int n, char s[])
{
    int i, sign, len;
    char temp;
    if((sign=n) < 0)//记录符号
        n = -n;//使n成为正数
    
    i = 0;
    do {
        s[i++] = n%10 + '0';//取下一个数字
    }
    while((n/=10) > 0);//删除该数字
    if(sign<0)
        s[i++] = '-';
    s[i] = '\0';
    len = strlen(s);
    for(i=0; i<len/2; i++){
        temp = s[i];
        s[i] = s[len-1-i];
        s[len-1-i] = temp;
    }
    
} 

int hili_db2_print_type(int x)
{
	printf("PTR->CONTENT_TYPE == ");
    switch (x)
    {
        case 0X1041: printf("EXCSAT");break;
        case 0X1443: printf("EXCSATRD");break;
        case 0X106d: printf("ACCSEC");break;
        case 0x14ac: printf("ACCSECRD");break;
        case 0x106e: printf("SECCHK");break;
        case 0x1219: printf("SECCHKRM");break;
        case 0x11b4: printf("SVCERRNO");break;
        case 0x2001: printf("ACCRDB");break;
        case 0x2201: printf("ACCRDBRM");break;
        case 0x2204: printf("RDBNACRM");break;
        case 0x2014: printf("EXCSQLSET");break;
        case 0x2414: printf("SQLSTT");break;
        case 0x2450: printf("SQLATTR");break;
        case 0x200d: printf("PRPSQLSTT");break;
        case 0x2408: printf("SQLCARD");break;
        case 0x2411: printf("SQLDARD");break;
        case 0x2008: printf("DSCSQLSTT");break;
        case 0x200c: printf("OPNQRY");break;
        case 0x2205: printf("OPNQRYRM");break;
        case 0x2212: printf("OPNQFLRM");break;
        case 0x220b: printf("ENDQRYRM");break;
        case 0x241a: printf("QRYDSC");break;
        case 0x241b: printf("QRYDTA");break;
        case 0x2005: printf("CLSQRY");break;
        case 0x200e: printf("RDBCMM");break;
        case 0x220c: printf("ENDUOWRM");break;
		case 0x2412: printf("SQLDTA");break;
		case 0x1c00: printf("MONITORRD");break;
        default : printf("%x\n, x");return -1;
    }
    printf("\n");
    return 0;
}

/*  ============================================
    show the key data
    ============================================    */
int hili_db2_print_header(hili_db2_parser_t *ptr)
{
    printf("===============================================\n");
    printf("===============================================\n");
    printf("hili_db2_parser_t print:\n");
    if(SQLSTT == ptr->need_to_log)
        printf("*   ptr->need_to_log is SQLSTT\n");
    /* else if(ptr->need_to_log == QRYDTA)
        printf("*   ptr->need_to_log is QRYDTA\n"); */
    else if(SECCHK == ptr->need_to_log)
        printf("*   ptr->need_to_log is SECCHK\n");
    else if(SQLDTA == ptr->need_to_log)
        printf("*   ptr->need_to_log is SQLDTA\n");
    else 
        printf("*   ptr->need_to_log is NULL\n");
    //printf("*   ptr->content_type %s\n", content_type_array[ptr->content_type]);
    printf("*   ptr->content_type is ");
    switch (ptr->content_type)
    {
        case 0X1041: printf("EXCSAT");break;
        case 0X1443: printf("EXCSATRD");break;
        case 0X106d: printf("ACCSEC");break;
        case 0x14ac: printf("ACCSECRD");break;
        case 0x106e: printf("SECCHK");break;
        case 0x1219: printf("SECCHKRM");break;
        case 0x11b4: printf("SVCERRNO");break;
        case 0x2001: printf("ACCRDB");break;
        case 0x2201: printf("ACCRDBRM");break;
        case 0x2204: printf("RDBNACRM");break;
        case 0x2014: printf("EXCSQLSET");break;
        case 0x2414: printf("SQLSTT");break;
        case 0x2450: printf("SQLATTR");break;
        case 0x200d: printf("PRPSQLSTT");break;
        case 0x2408: printf("SQLCARD");break;
        case 0x2411: printf("SQLDARD");break;
        case 0x2008: printf("DSCSQLSTT");break;
        case 0x200c: printf("OPNQRY");break;
        case 0x2205: printf("OPNQRYRM");break;
        case 0x2212: printf("OPNQFLRM");break;
        case 0x220b: printf("ENDQRYRM");break;
        case 0x241a: printf("QRYDSC");break;
        case 0x241b: printf("QRYDTA");break;
        case 0x2005: printf("CLSQRY");break;
        case 0x200e: printf("RDBCMM");break;
        case 0x220c: printf("ENDUOWRM");break;
		case 0x2412: printf("SQLDTA");break;
		case 0x1c00: printf("MONITORRD");break;
        default : printf("unknown");break;
    }
    printf("\n");
    printf("*   ptr->direction %u\n", ptr->direction);
    printf("*   ptr->drda_pkt_length %u\n", ptr->drda_pkt_length);
    printf("*   ptr->request_fifo_cache_ptr->size %u\n", ptr->request_fifo_cache_ptr->size);
    printf("*   ptr->response_fifo_cache_ptr->size %u\n", ptr->response_fifo_cache_ptr->size);

    printf("===============================================\n");

    return HILI_DB2_COMPLETE;
}

/*  ============================================
    determine whether this pkt worth parsing
    ============================================    */
int hili_db2_worth_parse(hili_db2_content_type_e x)
{
	if(x == 0x106e || x == 0x2414 || x == 0x241a || x == 0x241b || x == 0x2412 || x == 0x2201)
		return 1;
	else
		return 0;
}
//CVMX_SHARED static uint64_t drda_session_log_id = 0;

/*  ============================================
    initiate the details of parser structure
    ============================================    */
int hili_db2_parse_structure_init(hili_db2_parser_t *drda_flow_ptr)
{
	hili_db2_parser_t *ptr = drda_flow_ptr;
    
    int ii;
    ptr->content_type = 0;
    ptr->need_to_log = UNSET;
    ptr->drda_pkt_length = 0;
    ptr->qrydsc_len = 0;
    ptr->qrydta_len = 0;
    //ptr->last_log_direction = HILI_DB2_DIRECTION_RESPONSE;
    
    ptr->magic_number = 0;
    ptr->request_fifo_cache_ptr = idpi_util_fifo_cache_malloc();
    ptr->response_fifo_cache_ptr = idpi_util_fifo_cache_malloc();
    if(!ptr->request_fifo_cache_ptr || !ptr->response_fifo_cache_ptr)
    {
        printf("fifo cache malloc HILI_DB2_ERROR\n");
		return -1;
    }
	return 0;
}

/**
 *	 ============================================
 *   initiate the  parser structure
 *   ============================================    	
 */
void* hili_db2_parse_flow_init(db2_parse_init_info_t* mitm_parse_init_info_ptr)
{
    hili_db2_parser_t *ptr = hili_db2_alloc_parser();
    
    if (!ptr) 
    {
        printf("error\n");
        return NULL;
    }
    
    //printf("HILI_DB2_WAIT_FOR_BUFFER0 at line %d\n",__LINE__);
    //ptr->parse_state = __hili_db2_PARSE_STATE_INIT;
    ptr->pre_len = 0;
    ptr->tail_len = 0;
    ptr->mitm_flow_ptr = mitm_parse_init_info_ptr->mitm_flow_ptr;
    ptr->flow_id = mitm_parse_init_info_ptr->flow_id;
    ptr->client_ip = mitm_parse_init_info_ptr->cli_ip;
    ptr->server_ip = mitm_parse_init_info_ptr->srv_ip;
    printf("\nptr->client_ip is %u, and ptr->server_ip is %u at line %d\n\n", ptr->client_ip, ptr->server_ip, __LINE__);
    ptr->client_port = mitm_parse_init_info_ptr->cli_port;
    ptr->server_port = mitm_parse_init_info_ptr->srv_port;
	ptr->function_flags = mitm_parse_init_info_ptr->function_flags;
	strcpy(ptr->primary_account, mitm_parse_init_info_ptr->primary_account);
    memcpy(ptr->cli_mac, mitm_parse_init_info_ptr->cli_mac, 6);
	memcpy(ptr->srv_mac, mitm_parse_init_info_ptr->srv_mac, 6);
	ptr->session_id = mitm_parse_init_info_ptr->session_id;
    ptr->request_bytes = 0;

	uint64_t login_time = hili_get_ms_time64();
    
    uint8_t flow_sign[21];
    void *temp;
	
    temp = flow_sign;
    *((uint8_t *) temp) = 0x04;
    temp += sizeof(uint8_t);
    *((uint32_t *) temp) = ptr->client_ip;
    temp += sizeof(uint32_t);
    *((uint16_t *) temp) = ptr->client_port;
    temp += sizeof(uint16_t);
    *((uint32_t *) temp) = ptr->server_ip;
    temp += sizeof(uint32_t);
    *((uint16_t *) temp) = ptr->server_port;
    temp += sizeof(uint16_t);
    *((uint64_t *) temp) = hili_get_ms_time64();
    
	ptr->session_handle = hili_send_module_send_log_prepare(ptr->session_id, 0, 0x15, SESSION_LOG);
	hili_send_module_send_log_add_bytes(ptr->session_handle, SES_FLOW_SIGN, flow_sign, 21, 0, SEND_NOT_IMMEDIATELY);
    hili_send_module_send_log_add_ipv4(ptr->session_handle, SES_SRC_IP, ptr->client_ip, SEND_NOT_IMMEDIATELY);
	hili_send_module_send_log_add_ipv4(ptr->session_handle, SES_DST_IP, ptr->server_ip, SEND_NOT_IMMEDIATELY);
	hili_send_module_send_log_add_int(ptr->session_handle, SES_SRC_PORT, ptr->client_port, SEND_NOT_IMMEDIATELY);
	hili_send_module_send_log_add_int(ptr->session_handle, SES_DST_PORT, ptr->server_port, SEND_NOT_IMMEDIATELY);
	hili_send_module_send_log_add_bytes(ptr->session_handle, SES_MAIN_ACCOUNT, ptr->primary_account, strlen(ptr->primary_account), 0, SEND_NOT_IMMEDIATELY);
	hili_send_module_send_log_add_bytes(ptr->session_handle, SES_SRC_MAC, ptr->cli_mac, strlen(ptr->cli_mac), 0, SEND_NOT_IMMEDIATELY);
	hili_send_module_send_log_add_bytes(ptr->session_handle, SES_DST_MAC, ptr->srv_mac, strlen(ptr->srv_mac), 0, SEND_NOT_IMMEDIATELY);
	hili_send_module_send_log_add_time(ptr->session_handle, SES_LOGIN_TIME, login_time, SEND_NOT_IMMEDIATELY);
	
    hili_db2_parse_structure_init(ptr);

    return (void*)ptr;
}


/*  ============================================
    release the  parser structure
    ============================================    */
int hili_db2_parse_kill_flow(void* drda_flow_ptr)
{
    hili_db2_parser_t *ptr = (hili_db2_parser_t *)drda_flow_ptr;
	uint64_t logout_time =  hili_get_ms_time64();
    if (ptr)
    {
        if(ptr->session_handle)
		{
			hili_send_module_send_log_add_time(ptr->session_handle, SES_LOGOUT_TIME, logout_time, SEND_NOT_IMMEDIATELY);
			temp_ses_log = hili_send_module_send_log_send_immediately(ptr->session_handle);
            printf("\nSEND SESSION LOG is %d at line %d\n\n", temp_ses_log, __LINE__);
            hili_send_module_send_log_finish (ptr->session_handle);
		}
		if(ptr->operation_handle)
		{
            temp_opr_log = hili_send_module_send_log_send_immediately(ptr->operation_handle);
            printf("\nSEND OPERATION LOG %d is %d at line %d\n\n", ptr->operation_handle, temp_opr_log, __LINE__);
			hili_send_module_send_log_finish (ptr->operation_handle);
		}
		
        if(HILI_DB2_DIRECTION_REQUEST == ptr->direction)
		{
            idpi_util_fifo_cache_clear(ptr->request_fifo_cache_ptr);
		}
        else if(HILI_DB2_DIRECTION_RESPONSE == ptr->direction)
            idpi_util_fifo_cache_clear(ptr->response_fifo_cache_ptr);
        else{
            idpi_util_fifo_cache_clear(ptr->request_fifo_cache_ptr);
            idpi_util_fifo_cache_clear(ptr->response_fifo_cache_ptr);
        }
        
        if(ptr->request_fifo_cache_ptr && ptr->request_fifo_cache_ptr->size == 0)
            idpi_util_fifo_cache_free(ptr->request_fifo_cache_ptr);
        else if(ptr->response_fifo_cache_ptr && ptr->response_fifo_cache_ptr->size == 0)
            idpi_util_fifo_cache_free(ptr->response_fifo_cache_ptr);
        
		
        hili_db2_free_parser(ptr);
    }

    return HILI_DB2_COMPLETE;
}

/*  ========================================================================
    pkt_length and content_type recorded, prepare for hili_db2_parse_payload
    ========================================================================  */
int hili_db2_parse_header(hili_db2_parser_t* drda_flow_ptr, uint64_t offset)
{
    uint8_t load_buffer[HILI_DB2_HEAD_LEN];
    hili_db2_parser_t *ptr = drda_flow_ptr;
    idpi_util_fifo_cache_t* fifo_cache_ptr;
	
    if(HILI_DB2_DIRECTION_REQUEST == ptr->direction)
    {
        fifo_cache_ptr = ptr->request_fifo_cache_ptr;
    }
    else
    {
        fifo_cache_ptr = ptr->response_fifo_cache_ptr;
    }
	
	idpi_util_fifo_cache_read(fifo_cache_ptr, offset, HILI_DB2_HEAD_LEN, load_buffer);
    if(ptr->qrydta_len==0){
        ptr->drda_pkt_length = load_buffer[0] << 8;
        ptr->drda_pkt_length += load_buffer[1];
    }
    
    ptr->content_type = load_buffer[8] << 8;
    ptr->content_type += load_buffer[9];
    
    return HILI_DB2_COMPLETE;
}

int hili_db2_parse_blacklist_process(hili_db2_parser_t* drda_flow_ptr)
{
	return 0;
	/* hili_db2_parser_t *ptr = drda_flow_ptr;
	blacklist_cache_info_t *blacklist_ptr;
	int blacklist_result;
#ifndef CONTENT_H_IN_TEST
	blacklist_ptr = (blacklist_cache_info_t *)hili_common_fpa_alloc(CVM_FPA_128B_POOL);
#endif	
#ifdef CONTENT_H_IN_TEST
	blacklist_ptr = (blacklist_cache_info_t *)malloc(128);
#endif

	blacklist_ptr->protocol_code = 0x15;
	blacklist_ptr->direction = ptr->direction;
	
	if(HILI_DB2_DIRECTION_REQUEST == ptr->direction)
    {
		if(ptr->request_fifo_cache_ptr == NULL)
        {
			printf("[ERROR]request_fifo_cache_ptr is null at line %d\n",__LINE__);
		#ifndef CONTENT_H_IN_TEST
			hili_common_fpa_free(blacklist_ptr, CVM_FPA_128B_POOL, CVM_FPA_128B_POOL_SIZE/CVMX_CACHE_LINE_SIZE);
		#endif
		#ifdef CONTENT_H_IN_TEST
			free(blacklist_ptr);
		#endif
			return HILI_DB2_ERROR;
		}
		
		blacklist_ptr->fifo_cache = ptr->request_fifo_cache_ptr;
		blacklist_result = blacklist_judge_bycache(blacklist_ptr);
		
		#ifndef CONTENT_H_IN_TEST
			hili_common_fpa_free(blacklist_ptr, CVM_FPA_128B_POOL, CVM_FPA_128B_POOL_SIZE/CVMX_CACHE_LINE_SIZE);
		#endif
		#ifdef CONTENT_H_IN_TEST
			free(blacklist_ptr);
		#endif
		
		switch(blacklist_result)
		{
			case 0://PASS
				return 0;
			case 1://WARNING
				printf("[NOTE]blacklist judgement module return WARNING at line %d\n",__LINE__);
				return 1;
			case 2://BLOCKING REQUEST
				printf("[NOTE]blacklist judgement module return BLOCKING REQUEST at line %d\n",__LINE__);
				return 2;
			case 3://BLOCKING SESSION
				printf("[NOTE]blacklist judgement module return BLOCKING SESSION at line %d\n",__LINE__);
				return 3;
			default:return HILI_DB2_ERROR;
		}
	}
	else{
		if(!ptr->response_fifo_cache_ptr)
        {
			printf("[ERROR]response_fifo_cache_ptr is null at line %d\n",__LINE__);
		#ifndef CONTENT_H_IN_TEST
			hili_common_fpa_free(blacklist_ptr, CVM_FPA_128B_POOL, CVM_FPA_128B_POOL_SIZE/CVMX_CACHE_LINE_SIZE);
		#endif
		#ifdef CONTENT_H_IN_TEST
			free(blacklist_ptr);
		#endif
			return HILI_DB2_ERROR;
		}
		
		blacklist_ptr->fifo_cache = ptr->response_fifo_cache_ptr;
		blacklist_result = blacklist_judge_bycache(blacklist_ptr);
		
		#ifndef CONTENT_H_IN_TEST
			hili_common_fpa_free(blacklist_ptr, CVM_FPA_128B_POOL, CVM_FPA_128B_POOL_SIZE/CVMX_CACHE_LINE_SIZE);
		#endif
		#ifdef CONTENT_H_IN_TEST
			free(blacklist_ptr);
		#endif
		
		switch(blacklist_result)
		{
			case 0://PASS
				return 0;
			case 1://WARNING
				printf("[NOTE]blacklist judgement module return WARNING at line %d\n",__LINE__);
				return 1;
			case 2://BLOCKING REQUEST
				printf("[NOTE]blacklist judgement module return BLOCKING REQUEST at line %d\n",__LINE__);
				return 2;
			case 3://BLOCKING SESSION
				printf("[NOTE]blacklist judgement module return BLOCKING SESSION at line %d\n",__LINE__);
				return 3;
			default:return HILI_DB2_ERROR;
		}
	} */
}

//[NOTE] MAX para_num is set to be 64
int hili_db2_parse_paramode_logging(hili_db2_parser_t* drda_flow_ptr, uint64_t offset)
{
	hili_db2_parser_t *ptr = drda_flow_ptr;
	idpi_util_fifo_cache_t* fifo_cache_ptr;
	uint8_t para_buffer[HILI_DB2_PARA_HEAD] = {0};
	uint32_t para_len[64];
	
	fifo_cache_ptr = ptr->request_fifo_cache_ptr;
	
	int temp_log_location1 = ptr->log_location[1];
	char temp_for_string0, temp_for_string1;
    char para_type_record[64];
	
	
	//get para_type_record[] and para_len[]
	int i;
    for(i=0; i<ptr->num_para; i++)
    {
    
        idpi_util_fifo_cache_read(fifo_cache_ptr, ptr->log_location[0] + 3*i, 1, para_type_record+i);

        switch(para_type_record[i])
        {
            case PARA_STRING:
				idpi_util_fifo_cache_read(fifo_cache_ptr, temp_log_location1+1, 1, &temp_for_string0);
				idpi_util_fifo_cache_read(fifo_cache_ptr, temp_log_location1+2, 1, &temp_for_string1);
				para_len[i] = temp_for_string0<<8;
				para_len[i] += temp_for_string1;
				para_len[i]+= 3;
                break;
            case PARA_INT:
                para_len[i] = 5;
                break;
            default:break;
        }
        temp_log_location1 += para_len[i];
				
    }
	
	int count, para_len_sum = 0;
	for(i=0; i<ptr->num_para; i++)
    {
		idpi_util_fifo_cache_read(fifo_cache_ptr, ptr->log_location[0]+3*i, 3, para_buffer);
		idpi_util_fifo_cache_read(fifo_cache_ptr, ptr->log_location[0]+3*i, 3, para_buffer+3);
		idpi_util_fifo_cache_read(fifo_cache_ptr, ptr->log_location[0]+3*i, 3, para_buffer+6);
		
		sprintf(para_buffer,",[PAR%d]:",i);
		//hili_send_module_send_log_add_bytes(ptr->operation_handle, OPR_COMMAND, para_buffer, 9, 0, SEND_NOT_IMMEDIATELY);
		
		int cur_int;
		switch(para_type_record[i])
		{
			case PARA_STRING:
				//hili_send_module_send_log_add_bytes2(ptr->operation_handle, OPR_COMMAND, fifo_cache_ptr\
					, offset+ptr->log_location[1]+para_len_sum+3-1, para_len[i]-3, 0, SEND_NOT_IMMEDIATELY);
				break;
			case PARA_INT:
				idpi_util_fifo_cache_read(fifo_cache_ptr, ptr->log_location[1]+para_len_sum, para_len[i]\
					, para_buffer);
				cur_int = para_buffer[1]<<24;
				cur_int += para_buffer[2]<<16;
				cur_int += para_buffer[3]<<8;
				cur_int += para_buffer[4];
				//hili_send_module_send_log_add_int(ptr->operation_handle, OPR_COMMAND, cur_int, SEND_NOT_IMMEDIATELY);
				break;
			default:break;
		}
		para_len_sum += para_len[i];
		memset(para_buffer, 0, HILI_DB2_PARA_HEAD*sizeof(uint8_t));
	}
}

int hili_db2_parse_passwd_fillin_process(hili_db2_parser_t* drda_flow_ptr, char *username, uint8_t *password)
{
	return 0;
	/* hili_db2_parser_t *ptr = drda_flow_ptr;
	passwd_fillin_info_t *fillin_ptr;
	int count, passwd_fillin_result;
#ifndef CONTENT_H_IN_TEST
	fillin_ptr = (passwd_fillin_info_t *)hili_common_fpa_alloc(CVM_FPA_256B_POOL);
#endif	
#ifdef CONTENT_H_IN_TEST
	fillin_ptr = (passwd_fillin_info_t *)malloc(128);
#endif
	fillin_ptr->src_addr = ptr->client_ip;
	fillin_ptr->src_port = ptr->client_port;
	fillin_ptr->dst_addr = ptr->server_ip;
	fillin_ptr->dst_port = ptr->server_port;
	fillin_ptr->protocol_code = 0x15;
	strcpy(fillin_ptr->primary_account, ptr->primary_account);
	fillin_ptr->determined_account_flag = (ptr->function_flags&0x08)? 1:0;
	strcpy(fillin_ptr->user_account, username);
	for(count=0; count<64; count++)
		fillin_ptr->user_password[count] = 0;
	
	if(HILI_DB2_DIRECTION_REQUEST == ptr->direction)
    {
		if(ptr->request_fifo_cache_ptr == NULL)
        {
			printf("[ERROR]request_fifo_cache_ptr is null at line %d\n",__LINE__);
		#ifndef CONTENT_H_IN_TEST
			hili_common_fpa_free(fillin_ptr, CVM_FPA_256B_POOL, CVM_FPA_256B_POOL_SIZE/CVMX_CACHE_LINE_SIZE);
		#endif
		#ifdef CONTENT_H_IN_TEST
			free(fillin_ptr);
		#endif
			return HILI_DB2_ERROR;
		}
		
		passwd_fillin_result = passwd_fillin(fillin_ptr);
		
		
		switch(passwd_fillin_result)
		{
			case 0://FILLIN DENIED
				printf("[NOTE]password fillin module denied your fillin request at line %d\n",__LINE__);
				#ifndef CONTENT_H_IN_TEST
					hili_common_fpa_free(fillin_ptr, CVM_FPA_256B_POOL, CVM_FPA_256B_POOL_SIZE/CVMX_CACHE_LINE_SIZE);
				#endif
				#ifdef CONTENT_H_IN_TEST
					free(fillin_ptr);
				#endif
				return 0;
			case 1://FILLIN SUCCESS
				strcpy(password, fillin_ptr->user_password);
				printf("[NOTE]password fillin module is in function at line %d\n",__LINE__);
				#ifndef CONTENT_H_IN_TEST
					hili_common_fpa_free(fillin_ptr, CVM_FPA_256B_POOL, CVM_FPA_256B_POOL_SIZE/CVMX_CACHE_LINE_SIZE);
				#endif
				#ifdef CONTENT_H_IN_TEST
					free(fillin_ptr);
				#endif
				return 1;
			case 2://BLOCKING REQUEST
				printf("[NOTE]password fillin module force to block the session at once at line %d\n",__LINE__);
				#ifndef CONTENT_H_IN_TEST
					hili_common_fpa_free(fillin_ptr, CVM_FPA_256B_POOL, CVM_FPA_256B_POOL_SIZE/CVMX_CACHE_LINE_SIZE);
				#endif
				#ifdef CONTENT_H_IN_TEST
					free(fillin_ptr);
				#endif
				return 2;
			default:
				printf("[ERROR]password fillin module failed to judge this command at line %d\n",__LINE__);
				return HILI_DB2_ERROR;
		}
	}
	else{
		return HILI_DB2_ERROR;
	} */
	
}

int hili_db2_parse_passwd_fillin_replacing(hili_db2_parser_t* drda_flow_ptr, uint8_t *password, uint8_t password_length)
{
	//密码代填调用后，fifo cache里面密码之前的部分保留，删除原密码，密码之后的部分剪切到临时空间里面
	//把原密码贴到fifo cache的尾部，把临时空间里面的内容剪切回来，接到fifo cache尾部，释放该临时空间
	//改动ptr->drda_pkt_length的值，减去原密码长度，加上新密码长度
	/* hili_db2_parser_t *ptr = drda_flow_ptr;
	uint8_t new_pwd_len;
	uint16_t new_pkt_len, new_sub_len;
	new_pwd_len = strlen(password);
	new_pkt_len = ptr->drda_pkt_length - ptr->log_length[2] + new_pwd_len;
	uint8_t *temp_buf, *temp_loc, *length_changer;
	temp_buf = (uint8_t*)malloc((new_pkt_len)*sizeof(uint8_t));
	
	(new_pkt_len<ptr->drda_pkt_length)? idpi_util_fifo_cache_read(ptr->request_fifo_cache_ptr, 0, new_pkt_len, temp_buf):
		idpi_util_fifo_cache_read(ptr->request_fifo_cache_ptr, 0, ptr->drda_pkt_length, temp_buf);
	idpi_util_fifo_cache_remove(ptr->request_fifo_cache_ptr, ptr->drda_pkt_length, NULL);
	ptr->drda_pkt_length = new_pkt_len;
	ptr->log_length[2] = new_pwd_len;
	
	length_changer = (uint8_t*)&new_pkt_len;
	temp_buf[0] = length_changer[1];
	temp_buf[1] = length_changer[0];
	
	new_sub_len = HILI_DB2_PARA_HEAD_LEN + new_pwd_len;
	length_changer = (uint8_t*)&new_sub_len;
	temp_buf[ptr->log_location[2]-HILI_DB2_PARA_HEAD_LEN] = length_changer[1];
	temp_buf[ptr->log_location[2]-HILI_DB2_PARA_HEAD_LEN+1] = length_changer[0];
	
	temp_loc = &temp_buf[ptr->log_location[2]];
	strcpy(temp_loc, password);
	
	idpi_util_fifo_cache_add(ptr->request_fifo_cache_ptr, temp_buf, new_pkt_len);
	int i;
	for(i=0;i<new_pkt_len;i++)
	{
		if(temp_buf[i]<0x10)
			printf("0x0%x ",temp_buf[i]);
		else
			printf("0x%x ",temp_buf[i]);
		if((i+1)%8==0)
			printf("\n");
	}
	printf("\n");
	free(temp_buf); */
	return 0;
}

/*  ============================================
    locate the key data in the payload      
    ============================================    */
int hili_db2_parse_payload(hili_db2_parser_t* drda_flow_ptr, uint64_t offset, uint8_t direction)
{
    hili_db2_parser_t *ptr = drda_flow_ptr;
    uint16_t len_flag;
    uint16_t typ_flag;
	uint64_t start_time, end_time;
    uint8_t tab_[2] = {0x2f, 0x2e};
	
    uint8_t parser_cursor[HILI_DB2_PARA_HEAD_LEN] = {0, 0, 0, 0};
    idpi_util_fifo_cache_t* fifo_cache_ptr;

    if(HILI_DB2_DIRECTION_REQUEST == ptr->direction)
    {
        fifo_cache_ptr = ptr->request_fifo_cache_ptr;
    }
    else
    {
        fifo_cache_ptr = ptr->response_fifo_cache_ptr;
    }

	if(!ptr)
	{
		printf("[ERROR]ptr initializing failed at line %d\n",__LINE__);
		return HILI_DB2_ERROR;
	}
    //identify whether the pkt deserve parsing
	switch(ptr->content_type)
	{
		case HILI_DB2_TYPE_SQLSTT:
		{
			printf("HILI_DB2_TYPE_SQLSTT\n");
			ptr->need_to_log = SQLSTT;
			ptr->log_location[0] = (HILI_DB2_HEAD_LEN + HILI_DB2_PARA_HEAD_LEN);
			idpi_util_fifo_cache_read(fifo_cache_ptr, offset + ptr->log_location[0] - HILI_DB2_PARA_HEAD_LEN, HILI_DB2_PARA_HEAD_LEN, parser_cursor);

			while(ptr->log_location[0] < ptr->drda_pkt_length)
			{
				len_flag = parser_cursor[0];
				len_flag = len_flag << 8;
				len_flag += parser_cursor[1];
				typ_flag = parser_cursor[2];
				typ_flag = typ_flag << 8;
				typ_flag += parser_cursor[3];
				
				if(len_flag == 0 && typ_flag == DRDA_DATA_DAT)//found!
				{
					ptr->log_length[0] = ptr->drda_pkt_length - ptr->log_location[0];
					break;
				}
				idpi_util_fifo_cache_read(fifo_cache_ptr, offset + ptr->log_location[0] - HILI_DB2_PARA_HEAD_LEN, HILI_DB2_PARA_HEAD_LEN, parser_cursor);
				ptr->log_location[0] += 1;
			}

			if(HILI_DB2_DIRECTION_REQUEST == direction)
			{
				if(ptr->operation_handle)
				{
					// end_time = hili_get_ms_time64();
					// hili_send_module_send_log_add_time(ptr->operation_handle, OPR_END_TIME, end_time, SEND_NOT_IMMEDIATELY);
                    // temp_opr_log = hili_send_module_send_log_send_immediately(ptr->operation_handle);
                    // printf("\nSEND OPERATION LOG %d is %d at line %d\n\n", ptr->operation_handle, temp_opr_log, __LINE__);
					// hili_send_module_send_log_finish(ptr->operation_handle);
					// ptr->operation_handle = 0;
                    hili_send_module_send_log_add_bytes(ptr->operation_handle, OPR_COMMAND, tab_, 2, SPILT_NOT_LAST, SEND_NOT_IMMEDIATELY);
				}
                else {
                    uint8_t flow_sign[21];
                    void *temp;
                    start_time = hili_get_ms_time64();
                    
                    temp = flow_sign;
                    *((uint8_t *) temp) = 0x04;
                    temp += sizeof(uint8_t);
                    *((uint32_t *) temp) = ptr->client_ip;
                    temp += sizeof(uint32_t);
                    *((uint16_t *) temp) = ptr->client_port;
                    temp += sizeof(uint16_t);
                    *((uint32_t *) temp) = ptr->server_ip;
                    temp += sizeof(uint32_t);
                    *((uint16_t *) temp) = ptr->server_port;
                    temp += sizeof(uint16_t);
                    *((uint64_t *) temp) = start_time;
                    
                    ptr->operation_handle = hili_send_module_send_log_prepare(ptr->session_id, 0, 0x15, OPERATION_LOG);
                    printf("\nPREPARE OPERATION LOG %d at line %d\n\n", ptr->operation_handle, __LINE__);
                    
                    hili_send_module_send_log_add_bytes(ptr->operation_handle, OPR_FLOW_SIGN, flow_sign, 21, 0, SEND_NOT_IMMEDIATELY);
                    hili_send_module_send_log_add_bytes(ptr->operation_handle, OPR_SIGN, temp, 8, 0, SEND_NOT_IMMEDIATELY);
                    hili_send_module_send_log_add_time(ptr->operation_handle, OPR_START_TIME, start_time, SEND_NOT_IMMEDIATELY);
                }
			}
			else {
				printf("[ERROR]wrong direction for pkt SQLSTT at line %d\n",__LINE__);
				return HILI_DB2_ERROR;
			}
			
            printf("\nSTARTING POINT: %d\nLENGTH: %d\n\n", offset+ptr->log_location[0], ptr->log_length[0]);
			hili_send_module_send_log_add_bytes2(ptr->operation_handle, OPR_COMMAND, ptr->request_fifo_cache_ptr\
				, offset+ptr->log_location[0]-1, ptr->log_length[0], SPILT_NOT_LAST, SEND_NOT_IMMEDIATELY);
			
			if(ptr->function_flags&0x02)
			{
				switch(hili_db2_parse_blacklist_process(ptr))
				{
					case 0:
						break;
					case 1:
						/* [TODO]relevant warning operation */
					case 2:
						/* [TODO]relevant request-blocking operation */
					case 3:
						/* [TODO]relevant session-blocking operation */
						break;
					default: 
						printf("[ERROR]blacklist module failed to judge this command at line %d\n",__LINE__);
						break;
				}
				//2015-9-6：添加审计日志
			}
            ptr->request_bytes += ptr->drda_pkt_length;
			return ptr->response_fifo_cache_ptr->size;
            break;
		}
		
		case HILI_DB2_TYPE_SQLDTA:
		{
			printf("HILI_DB2_TYPE_SQLDTA\n");
			ptr->num_para = 0;
			ptr->need_to_log = SQLDTA;
			uint16_t temp;
			ptr->log_location[0] = (HILI_DB2_HEAD_LEN + HILI_DB2_PARA_HEAD_LEN);
			idpi_util_fifo_cache_read(fifo_cache_ptr, offset + ptr->log_location[0] - HILI_DB2_PARA_HEAD_LEN, HILI_DB2_PARA_HEAD_LEN, parser_cursor);
			len_flag = parser_cursor[0];
			len_flag = len_flag << 8;
			len_flag += parser_cursor[1];
			typ_flag = parser_cursor[2];
			typ_flag = typ_flag << 8;
			typ_flag += parser_cursor[3];
			if(typ_flag == 0x0010)
				ptr->num_para = (len_flag - 9 - HILI_DB2_PARA_HEAD_LEN)/3;
			
			ptr->log_location[0] += 3;
			ptr->log_length[0] = ptr->num_para*3;
			//idpi_util_fifo_cache_read(fifo_cache_ptr, offset + ptr->log_location[0], 3*ptr->num_para, parser_cursor);    
			
			//printf("ptr->log_location[0] is %d at line %d\n",ptr->log_location[0],__LINE__);
			//printf("len_flag is 0x%x at line %d\n",len_flag,__LINE__);
			ptr->log_location[1] = HILI_DB2_HEAD_LEN + len_flag + HILI_DB2_PARA_HEAD_LEN ;
			idpi_util_fifo_cache_read(fifo_cache_ptr, offset + ptr->log_location[1] - HILI_DB2_PARA_HEAD_LEN, HILI_DB2_PARA_HEAD_LEN, parser_cursor);
			
			if(HILI_DB2_DIRECTION_REQUEST == direction)
			{
				if(ptr->operation_handle)
				{
					// end_time = hili_get_ms_time64();
					// hili_send_module_send_log_add_time(ptr->operation_handle, OPR_END_TIME, end_time, SEND_NOT_IMMEDIATELY);
                    // temp_opr_log = hili_send_module_send_log_send_immediately(ptr->operation_handle);
                    // printf("\nSEND OPERATION LOG %d is %d at line %d\n\n", ptr->operation_handle, temp_opr_log, __LINE__);
					// hili_send_module_send_log_finish(ptr->operation_handle);
					// ptr->operation_handle = 0;
                    
                    
                    //hili_send_module_send_log_add_bytes(ptr->operation_handle, OPR_COMMAND, tab_, 4, 0, SEND_NOT_IMMEDIATELY);
				}
				//ptr->operation_handle = hili_send_module_send_log_prepare(ptr->session_id, 0, 0x15, OPERATION_LOG);
                //printf("\nPREPARE OPERATION LOG %d at line %d\n\n", ptr->operation_handle, __LINE__);
                else {
                    printf("[ERROR]wrong initiation for pkt SQLDTA at line %d\n",__LINE__);
                }
			}
			else
			{
				printf("[ERROR]wrong direction for pkt SQLDTA at line %d\n",__LINE__);
				return HILI_DB2_ERROR;
			}
			
			int cc=0;
			while(ptr->log_location[1] < ptr->drda_pkt_length)
			{
				cc++;
				printf(" ptr->drda_pkt_length is %d, ptr->log_location[1] is %d at line %d\n", ptr->drda_pkt_length,ptr->log_location[1],__LINE__);
				len_flag = parser_cursor[0];
				len_flag = len_flag << 8;
				len_flag += parser_cursor[1];
				typ_flag = parser_cursor[2];
				typ_flag = typ_flag << 8;
				typ_flag += parser_cursor[3];
				
				if(typ_flag == 0x147a)//found!
				{
					ptr->log_length[1] = len_flag - HILI_DB2_PARA_HEAD_LEN - 1;
					ptr->log_location[1] += 1;
					break;
				}
				idpi_util_fifo_cache_read(fifo_cache_ptr, offset + ptr->log_location[1] - HILI_DB2_PARA_HEAD_LEN, HILI_DB2_PARA_HEAD_LEN, parser_cursor);
				ptr->log_location[1] += 1;
			}
			
			hili_db2_parse_paramode_logging(ptr, offset);
            ptr->request_bytes += ptr->drda_pkt_length;
			
			return ptr->request_fifo_cache_ptr->size;
            break;
		}
		
        
		case HILI_DB2_TYPE_QRYDSC:
		{
			printf("HILI_DB2_TYPE_QRYDSC\n");
			ptr->need_to_log = QRYDTA;
			ptr->log_location[0] = (HILI_DB2_HEAD_LEN) + ptr->qrydsc_len;
			ptr->log_length[0] = ptr->drda_pkt_length - ptr->log_location[0];
            uint8_t end_flag[1] = {0x2e};
			if(direction == HILI_DB2_DIRECTION_RESPONSE)
			{
				if(!ptr->operation_handle)
				{
					
                    printf("[ERROR]NO request for pkt QRYDTA at line %d\n",__LINE__);
                    return HILI_DB2_ERROR;
				}
                else {
                    hili_send_module_send_log_add_bytes(ptr->operation_handle, OPR_COMMAND, end_flag, 1, SPILT_LAST, SEND_NOT_IMMEDIATELY);
                    hili_send_module_send_log_add_long(ptr->operation_handle, OPR_REQ_SIZE, ptr->request_bytes, SEND_NOT_IMMEDIATELY);
                    hili_send_module_send_log_add_long(ptr->session_handle, SES_REQ_SIZE, ptr->request_bytes, SEND_NOT_IMMEDIATELY);
                }
			}
			else
			{
				printf("[ERROR]WRONG direction for pkt QRYDTA at line %d\n",__LINE__);
				return HILI_DB2_ERROR;
			}
            
            uint16_t col_count = 0, col_type[21] = {0},  col_size[21] = {0}, float_dec[21] = {0};
            int log_description_left =  ptr->qrydsc_len - HILI_DB2_HEAD_LEN - 15 - 3;
            log_description_left = log_description_left/3;
            int log_length_left = ptr->log_length[0];
            
            //record the type and width of each column
            while(log_description_left > 0 && col_count <20) {
                idpi_util_fifo_cache_read(fifo_cache_ptr, offset+HILI_DB2_HEAD_LEN+3+3*col_count, 3, parser_cursor); 
                switch(parser_cursor[0]){
                    case 0x02:
                    case 0x03:
                    case 0x04:
                    case 0x05:
                        col_type[col_count] = 3;
                        col_size[col_count] = parser_cursor[1]<<8;
                        col_size[col_count] += parser_cursor[2];
                        break;
                    case 0x3e:
                    case 0x3f:
                        col_type[col_count] = 1;
                        col_size[col_count] = 1;//in these cases, the length of string is not fixed, col_size will be determined later
                        break;
                    case 0x37:
                        col_type[col_count] = 2;
                        col_size[col_count] = parser_cursor[1]<<8;
                        col_size[col_count] += parser_cursor[2];
                        col_size[col_count] = col_size[col_count]*2;//in these cases, 0x52→0x00 0x52
                    case 0x39:
                        col_type[col_count] = 1;
                        col_size[col_count] = 2;//in these cases, 0x52→0x00 0x52
                        break;
                    case 0x0f:
                        col_type[col_count] = 4;
                        col_size[col_count] = parser_cursor[1]/2 + 1;
                        float_dec[col_count] = parser_cursor[2];
                        break;
                    default:
                        col_type[col_count] = 2;
                        col_size[col_count] = parser_cursor[1]<<8;
                        col_size[col_count] += parser_cursor[2];
                        break;
                }
                
                log_description_left --;
                col_count ++;
            }
            
            int i = 0, cur_loc = 1;
            uint8_t dot[1] = {0x2e}, space[1] = {0x20};
            //write each colume to the log, row by row
            while(log_length_left > 0) {
                idpi_util_fifo_cache_read(fifo_cache_ptr, offset+ptr->qrydsc_len+HILI_DB2_HEAD_LEN+cur_loc, 2, parser_cursor); 
                printf("parser_cursor[0]=%x, parser_cursor[1]=%x\n",parser_cursor[0],parser_cursor[1]);
                if(col_type[i] == 4){
                    //printf("cur_loc=%d, i=%d, col_type[i]=%d, col_size[i]=%d!!! at line %d\n",cur_loc,i,col_type[i],col_size[i], __LINE__);
                    printf("col_size[i]=%d, float_dec[i]=%d!!! at line %d\n",col_size[i],float_dec[i], __LINE__);
                    uint8_t parser_f_cursor[1] = {0};
                    idpi_util_fifo_cache_read(fifo_cache_ptr, offset+ptr->qrydsc_len+HILI_DB2_HEAD_LEN+cur_loc+col_size[i], 1, parser_f_cursor);
                    printf("parser_f_cursor[0]=0x%x!!! at line %d\n",parser_f_cursor[0], __LINE__);
                    parser_f_cursor[0] = parser_f_cursor[0] & 0x0f;
                    printf("parser_f_cursor[0]=0x%x!!! at line %d\n",parser_f_cursor[0], __LINE__);
                    
                    //if the parameter type is float
                    if(parser_f_cursor[0] == 0x0c){
                        uint8_t now_float[1] = {0}, now_f_cursor[1] = {0};
                        int f_i, digit, first_no_zero;
                        first_no_zero = 0;
                        int int_len = col_size[i]*2-1-float_dec[i];
                        printf ("cur_loc is %d at line %d\n",cur_loc,__LINE__);
                        for(f_i = 1, digit = 0; f_i <= col_size[i]-1; f_i++){
                            idpi_util_fifo_cache_read(fifo_cache_ptr, offset+ptr->qrydsc_len+HILI_DB2_HEAD_LEN+cur_loc+f_i, 1, now_f_cursor); 
                            
                            now_float[0] = now_f_cursor[0]>>4;
                            if(first_no_zero == 0 && now_float[0] != 0){
                                first_no_zero = 1;
                            }
                            now_float[0] += 48;
                            printf("now_f_cursor[0]=0x%x, now_float[0]=0x%x at line %d\n",now_f_cursor[0],now_float[0],__LINE__);
                            digit ++;
                            if(first_no_zero == 1|| digit == int_len-1){
                                hili_send_module_send_log_add_bytes(ptr->operation_handle, OPR_REPLY, now_float, 1, COMPLETE_NO_SPILT, SEND_NOT_IMMEDIATELY);
                            }
                             
                            
                            printf("digit=%d, int_len=%d at line %d\n",digit,int_len,__LINE__);
                            if(digit == int_len) {
                                hili_send_module_send_log_add_bytes(ptr->operation_handle, OPR_REPLY, dot, 1, COMPLETE_NO_SPILT, SEND_NOT_IMMEDIATELY);
                            }
                                                
                            now_float[0] = now_f_cursor[0]&0x0f;
                            if(first_no_zero == 0 && now_float[0] != 0){
                                first_no_zero = 1;
                            }
                            now_float[0] += 48;
                            printf("now_f_cursor[0]=0x%x, now_float[0]=0x%x at line %d\n",now_f_cursor[0],now_float[0],__LINE__);
                            digit ++;
                            if(first_no_zero == 1 || digit == int_len-1){
                                hili_send_module_send_log_add_bytes(ptr->operation_handle, OPR_REPLY, now_float, 1, COMPLETE_NO_SPILT, SEND_NOT_IMMEDIATELY);
                            }
                            if(digit == int_len){
                                hili_send_module_send_log_add_bytes(ptr->operation_handle, OPR_REPLY, dot, 1, COMPLETE_NO_SPILT, SEND_NOT_IMMEDIATELY);
                            }
                        }
                        
                        idpi_util_fifo_cache_read(fifo_cache_ptr, offset+ptr->qrydsc_len+HILI_DB2_HEAD_LEN+cur_loc+f_i, 1, now_f_cursor); 
                        now_float[0] = now_f_cursor[0]>>4;
                        now_float[0] += 48;
                        printf("now_f_cursor[0]=0x%x, now_float[0]=0x%x at line %d\n",now_f_cursor[0],now_float[0],__LINE__);
                        hili_send_module_send_log_add_bytes(ptr->operation_handle, OPR_REPLY, now_float, 1, COMPLETE_NO_SPILT, SEND_NOT_IMMEDIATELY);
                        cur_loc += col_size[i];
                        log_length_left -= col_size[i];
                        i++;
                    }
                    else {
                        cur_loc ++;
                        log_length_left --;
                    }
                }
                
                if(parser_cursor[0] == 0x00) {
                    //列之间的分隔符如何设定？暂且设为0x09
                    parser_cursor[0] = 0x20;
                    hili_send_module_send_log_add_bytes(ptr->operation_handle, OPR_REPLY, parser_cursor, 1, COMPLETE_NO_SPILT, SEND_NOT_IMMEDIATELY);
                    cur_loc ++;
                    log_length_left --;
                    continue;
                }
                else if(parser_cursor[0] == 0xff) {
                    //换行符如何设定？暂且设为0x0a
                    parser_cursor[0] = 0x0d;
                    hili_send_module_send_log_add_bytes(ptr->operation_handle, OPR_REPLY, parser_cursor, 1, COMPLETE_NO_SPILT, SEND_NOT_IMMEDIATELY);
                    cur_loc ++;
                    log_length_left --;
                    continue;
                }
                
                
                switch(col_type[i]) {
                    case 1: {
                        col_size[i] = parser_cursor[0]+1;
                        //printf("cur_loc=%d, i=%d, col_type[i]=%d, col_size[i]=%d!!! at line %d\n",cur_loc,i,col_type[i],col_size[i], __LINE__);
                        hili_send_module_send_log_add_bytes2(ptr->operation_handle, OPR_REPLY, fifo_cache_ptr,\
                            offset+ptr->qrydsc_len+HILI_DB2_HEAD_LEN+cur_loc+1, col_size[i]-1, COMPLETE_NO_SPILT, SEND_NOT_IMMEDIATELY);
                        cur_loc += col_size[i];
                        log_length_left -= col_size[i];
                        i++;
                    }
                        break;
                    case 2: {
                        //printf("cur_loc=%d, i=%d, col_type[i]=%d, col_size[i]=%d!!! at line %d\n",cur_loc,i,col_type[i],col_size[i], __LINE__);
                        hili_send_module_send_log_add_bytes2(ptr->operation_handle, OPR_REPLY, fifo_cache_ptr,\
                            offset+ptr->qrydsc_len+HILI_DB2_HEAD_LEN+cur_loc, col_size[i], COMPLETE_NO_SPILT, SEND_NOT_IMMEDIATELY);
                        cur_loc += col_size[i];
                        log_length_left -= col_size[i];
                        i++;
                    }
                        break;
                    case 3:{
                        //按照反序把整型数读进 operation_handle
                        //printf("cur_loc=%d, i=%d, col_type[i]=%d, col_size[i]=%d!!! at line %d\n",cur_loc,i,col_type[i],col_size[i], __LINE__);
                        int ii, swift_ = 0, now_int = 0;
                        char number[20];
                        uint8_t now_cursor[1] = {0};
                        long now_long = 0;
                        if(col_size[i]<=4){
                            for(ii=0; ii<col_size[i]; ii++){
                                idpi_util_fifo_cache_read(fifo_cache_ptr, offset+ptr->qrydsc_len+HILI_DB2_HEAD_LEN+cur_loc+ii, 1, now_cursor); 
                                now_int += now_cursor[0]<<(swift_*8);
                                printf("now_cursor[0]=%x, now_int=%x\n",now_cursor[0],now_int);
                                swift_ ++;
                            }
                            hili_db2_parse_itoa(now_int, number);
                            hili_send_module_send_log_add_bytes(ptr->operation_handle, OPR_REPLY, number, strlen(number), 0, SEND_NOT_IMMEDIATELY);
                            
                        }
                        else {
                            for(ii=0; ii<col_size[i]; ii++){
                                idpi_util_fifo_cache_read(fifo_cache_ptr, offset+ptr->qrydsc_len+HILI_DB2_HEAD_LEN+cur_loc+ii, 1, now_cursor); 
                                now_long += now_cursor[0]<<(swift_*8);
                                swift_ ++;
                            }
                            hili_db2_parse_itoa(now_int, number);
                            hili_send_module_send_log_add_bytes(ptr->operation_handle, OPR_REPLY, number, strlen(number), 0, SEND_NOT_IMMEDIATELY);
                            
                        }
                        hili_send_module_send_log_add_bytes(ptr->operation_handle, OPR_REPLY, space, 1, 0, SEND_NOT_IMMEDIATELY);
                        cur_loc += col_size[i];
                        log_length_left -= col_size[i];
                        i++;
                    }
                        break;
                    default: {
                        i=0;
                    }
                        break;
                }
                
            }
			
            printf("\nSTARTING POINT: %d\nLENGTH: %d\n\n", offset+ptr->log_location[0], ptr->log_length[0]);
			
            hili_send_module_send_log_add_bytes(ptr->operation_handle, OPR_REPLY, end_flag, 1, COMPLETE_NO_SPILT, SEND_NOT_IMMEDIATELY);   
                
			hili_send_module_send_log_add_long(ptr->operation_handle, OPR_REP_SIZE, ptr->drda_pkt_length, SEND_NOT_IMMEDIATELY);
            hili_send_module_send_log_add_long(ptr->session_handle, SES_REP_SIZE, ptr->drda_pkt_length, SEND_NOT_IMMEDIATELY);
            
			end_time = hili_get_ms_time64();
			hili_send_module_send_log_add_time(ptr->operation_handle, OPR_END_TIME, end_time, SEND_NOT_IMMEDIATELY);
            temp_opr_log = hili_send_module_send_log_send_immediately(ptr->operation_handle);
            printf("\nSEND OPERATION LOG %d is %d at line %d\n\n", ptr->operation_handle, temp_opr_log, __LINE__);
			hili_send_module_send_log_finish(ptr->operation_handle);
            ptr->operation_handle = 0;
			
            ptr->qrydsc_len = 0;
            ptr->qrydta_len = 0;
			return ptr->response_fifo_cache_ptr->size;
            break;
		}
		
		case HILI_DB2_TYPE_SECCHK:
		{
			printf("HILI_DB2_TYPE_SECCHK\n");
			ptr->need_to_log = SECCHK;
			ptr->log_location[0] = (HILI_DB2_HEAD_LEN + HILI_DB2_PARA_HEAD_LEN);
			
			idpi_util_fifo_cache_read(fifo_cache_ptr, offset + ptr->log_location[0] - HILI_DB2_PARA_HEAD_LEN, HILI_DB2_PARA_HEAD_LEN, parser_cursor);
			
			uint8_t username[20];
			//uint8_t *password;
			uint8_t password[16];
			uint8_t password_length;
			int kk;
			int i = 0;
			int j = 0;
			while(j < 50)
			{
				len_flag = parser_cursor[0];
				len_flag = len_flag << 8;
				len_flag += parser_cursor[1];
				typ_flag = parser_cursor[2];
				typ_flag = typ_flag << 8;
				typ_flag += parser_cursor[3];
				int ii;
				//printf("len_flag is 0x%x, typ_flag is 0x%x at line %d\n",len_flag,typ_flag,__LINE__);
				//printf("len_flag is %d,at line %d\n",len_flag,__LINE__);
				if(DRDA_DATA_RLT_DBS_NAM==typ_flag||DRDA_DATA_PWD==typ_flag ||DRDA_DATA_UID_TGT_SYS==typ_flag)
				{
					ptr->log_length[i] = len_flag - HILI_DB2_PARA_HEAD_LEN;
					if(DRDA_DATA_UID_TGT_SYS==typ_flag)
					{
						uint8_t username_length = ptr->log_length[i];
						idpi_util_fifo_cache_read(fifo_cache_ptr, offset + ptr->log_location[i], username_length, username);
						
						ii = 0;
						for(;ii<username_length;ii++)
						{
                            printf("%x -> ", username[ii]);
							username[ii] = HILI_DB2_E2A[username[ii]];
                            printf("%x :  ", username[ii]);
                            printf("%c\n", username[ii]);
						}
                        
						hili_send_module_send_log_add_bytes(ptr->session_handle, SES_LOGIN_NAME, username, username_length, 0, SEND_NOT_IMMEDIATELY);
					}
					else if(DRDA_DATA_PWD==typ_flag)
					{
						password_length = ptr->log_length[i];
						//password = (uint8_t*)malloc(password_length*sizeof(uint8_t));
						idpi_util_fifo_cache_read(fifo_cache_ptr, offset + ptr->log_location[i], password_length, password);
						
						ii = 0;
						for(;ii<password_length;ii++)
						{
							password[ii] = HILI_DB2_E2A[password[ii]];
							//printf("%c",*(password+ii));
						}
					}
					if(i >= 2)
						break;
					else
						ptr->log_location[i+1] = ptr->log_location[i];
					i ++;
				}
				j ++;
				ptr->log_location[i] += len_flag;
				//printf("ptr->log_location[%d] = %d at line %d\n",i,ptr->log_location[i],__LINE__);
				idpi_util_fifo_cache_read(fifo_cache_ptr, offset + ptr->log_location[i] - HILI_DB2_PARA_HEAD_LEN, HILI_DB2_PARA_HEAD_LEN, parser_cursor);
			}
			
			
			
			if(ptr->function_flags&0x04)
			{
				switch(hili_db2_parse_passwd_fillin_process(ptr, username, password))
				{
					case 0:
						break;
					case 1:
						/* [TODO]password-replacing operation in fifo_cache*/
						hili_db2_parse_passwd_fillin_replacing(ptr, password, password_length);
						break;
					case 2:
						/* [TODO]relevant session-blocking operation */
						break;
					default: 
						return HILI_DB2_ERROR;
				}
			}
			//free(password);
			return ptr->request_fifo_cache_ptr->size;
            break;
		}
		
		case HILI_DB2_TYPE_ACCRDBRM:
		{
			printf("HILI_DB2_TYPE_ACCRDBRM\n");
			ptr->log_location[0] = (HILI_DB2_HEAD_LEN + HILI_DB2_PARA_HEAD_LEN);
			
			uint8_t version[10];
			
			int i = 0;
			int j = 0;
			while(j < 50)
			{
				idpi_util_fifo_cache_read(fifo_cache_ptr, offset + ptr->log_location[0] - HILI_DB2_PARA_HEAD_LEN, HILI_DB2_PARA_HEAD_LEN, parser_cursor);
				len_flag = parser_cursor[0];
				len_flag = len_flag << 8;
				len_flag += parser_cursor[1];
				typ_flag = parser_cursor[2];
				typ_flag = typ_flag << 8;
				typ_flag += parser_cursor[3];
				
				if(typ_flag==DRDA_DATA_PRD_SPC_IDF)
				{
					uint8_t version_length = len_flag - HILI_DB2_PARA_HEAD_LEN;
					idpi_util_fifo_cache_read(fifo_cache_ptr, offset + ptr->log_location[0], version_length, version);
					
					int ii = 0;
					//printf("\nversion_length is %d, version is ",ptr->version_length);
					for(;ii<version_length;ii++)
					{
						version[ii] = HILI_DB2_E2A[version[ii]];
						printf("%c\n",*(version+ii));
					}
					//printf(" at line %d\n",__LINE__);
					hili_send_module_send_log_add_bytes(ptr->session_handle, SES_PRO_VER, version, version_length, 0, SEND_NOT_IMMEDIATELY);
					break;
				}
				else
				{
					ptr->log_location[0] ++;
				}
				j ++;
			}
			return ptr->request_fifo_cache_ptr->size;
            break;
		}
		default:
			return HILI_DB2_ERROR;
	}
}


/*  ============================================
    drda parsing process
    all the parameters with the prefix "pktbuf" may be related to the cache
    ============================================    */
int hili_db2_parse_processing(void* drda_flow_ptr, db2_data_exchange_t *mitm_data_up_ptr)
{
    printf("\n\nDB2_PARSE_BEGIN WITH LEN %d AT LINE %d\n", mitm_data_up_ptr->buf_len, __LINE__);
	hili_db2_parser_t *ptr = (hili_db2_parser_t *)drda_flow_ptr;
    uint32_t buf_left = 0;
	void *buf = (*mitm_data_up_ptr).buf_ptr;
	uint32_t buf_len = mitm_data_up_ptr->buf_len;
    uint32_t rest_in_fifo;
    
    ptr->pre_len = ptr->tail_len;
    ptr->tail_len = 0;
    uint64_t cache_offset = 0;
	ptr->direction = mitm_data_up_ptr->direction;
	
    if(!buf)
    {
        printf("Cannot read the buf at line %d\n",__LINE__);
        return HILI_DB2_ERROR;
    }
    
    if(!ptr)
    {
        printf("Cannot initiate the main parser at line %d\n",__LINE__);
        return HILI_DB2_ERROR;
    }
	
	//printf("ptr->direction is %d at line %d\n",direction,__LINE__);
    if(HILI_DB2_DIRECTION_REQUEST == ptr->direction)
    {
        //all the TCP pkt to the cache
        printf("db2_cache_begin at line %d\n", __LINE__);
        if(buf_len != idpi_util_fifo_cache_add(ptr->request_fifo_cache_ptr, buf, buf_len))
        {
            printf("hili_db2_parse_cache_message(ptr) == HILI_DB2_ERROR\n");
            return HILI_DB2_ERROR;
        }
		rest_in_fifo = ptr->request_fifo_cache_ptr->size;//renew the rest_in_fifo
    }
    else if(HILI_DB2_DIRECTION_RESPONSE == ptr->direction)
    {
        printf("db2_cache_begin at line %d\n", __LINE__);
        if(buf_len != idpi_util_fifo_cache_add(ptr->response_fifo_cache_ptr, buf, buf_len))
        {
            printf("hili_db2_parse_cache_message(ptr) == HILI_DB2_ERROR\n");
            return HILI_DB2_ERROR;
        }
		rest_in_fifo = ptr->response_fifo_cache_ptr->size;//renew the rest_in_fifo
    }
    else
    {
        printf("invalid direction!\n");
        return HILI_DB2_ERROR;
    }
	
    printf("db2_submit_prepare at line %d\n", __LINE__);
    db2_downdata_exchange_t* db2_data_submit = (db2_downdata_exchange_t *)hili_db2_alloc_parser();
    if(db2_data_submit == NULL){
        return HILI_DB2_ERROR;
    }
    if(HILI_DB2_DIRECTION_REQUEST == ptr->direction){
        db2_data_submit->fifo_cache_ptr = ptr->request_fifo_cache_ptr;
    }
    else{
        db2_data_submit->fifo_cache_ptr = ptr->response_fifo_cache_ptr;
    }

    db2_data_submit->direction = ptr->direction;
    db2_data_submit->ftp_parse_control.srv_ip = 0;
    db2_data_submit->ftp_parse_control.srv_port = 0;
    db2_data_submit->ftp_parse_control.oob_data_info = 0;
    db2_data_submit->ftp_parse_control.ftp_parse_flowsession_ptr = 0;
	//[NOTE]each circle for a db2 pkt
    
    printf("\nDB2_cycle_begin at line %d\n", __LINE__);
	while(1)
	{
		if(rest_in_fifo < HILI_DB2_HEAD_LEN)
			break;

		hili_db2_parse_header(ptr, cache_offset);
        printf("drda_pkt_length is %d at line %d\n", ptr->drda_pkt_length,  __LINE__);
		if(ptr->drda_pkt_length < HILI_DB2_HEAD_LEN)
		{
			printf("[ERROR]parse header at line %d\n",__LINE__);
			return HILI_DB2_ERROR;
		}
		int type_define = hili_db2_print_type(ptr->content_type);
		if(type_define == -1)
		{
			printf("[NOTE]unknown pkt at line %d\n", __LINE__);
			return HILI_DB2_ERROR;
		}
		
        //对回复进行特殊处理，包合并
        if(HILI_DB2_TYPE_QRYDSC == ptr->content_type && ptr->qrydsc_len==0){
            if(rest_in_fifo < ptr->drda_pkt_length+HILI_DB2_HEAD_LEN) {
                ptr->tail_len = rest_in_fifo;
                break;
            }
            
            uint16_t type_next;
            uint8_t load_buffer[HILI_DB2_HEAD_LEN];
            idpi_util_fifo_cache_read(ptr->response_fifo_cache_ptr, cache_offset+ptr->drda_pkt_length\
            , HILI_DB2_HEAD_LEN, load_buffer);
            
            type_next = load_buffer[8] << 8;
            type_next += load_buffer[9];
            
            if(type_next == 0x241b) {
                ptr->qrydsc_len = ptr->drda_pkt_length;
                ptr->qrydta_len = load_buffer[0] << 8;
                ptr->qrydta_len += load_buffer[1];
                printf("ptr->qrydsc_len is %d at line %d\n", ptr->qrydsc_len, __LINE__);
                printf("ptr->qrydta_len is %d at line %d\n", ptr->qrydta_len, __LINE__);
                ptr->drda_pkt_length += ptr->qrydta_len;
                printf("ptr->drda_pkt_length is %d at line %d\n", ptr->drda_pkt_length, __LINE__);
            }
        }
        
		if(rest_in_fifo >= ptr->drda_pkt_length)
		{
			if(hili_db2_worth_parse(ptr->content_type))
            {
				//printf("the packet is %x\n", ptr->content_type);
				int ii;
                for(ii=0;ii<3;ii++)
                    ptr->log_location[ii] = 0;
                for(ii=0;ii<3;ii++)
                    ptr->log_length[ii] = 0;
				
				//密码代填调用后，fifo cache里面密码之前的部分保留，删除原密码，密码之后的部分剪切到临时空间里面
				//把原密码贴到fifo cache的尾部，把临时空间里面的内容剪切回来，接到fifo cache尾部，释放该临时空间
				//改动ptr->drda_pkt_length的值，减去原密码长度，加上新密码长度
				//parse_assert下面返回的parse_assert就是新的buf_len
				int parse_assert = hili_db2_parse_payload(ptr, cache_offset, ptr->direction);
				if(parse_assert < 0)
				{
					printf("[ERROR]parse beginning at line %d\n",__LINE__);
					return HILI_DB2_ERROR;
				}
			}
            cache_offset += ptr->drda_pkt_length;
            rest_in_fifo -= ptr->drda_pkt_length;
		}
		else {
            printf("rest_in_fifo is %d at line %d\n",rest_in_fifo,__LINE__);
			printf("waiting for new buffer at line %d\n",__LINE__);
            ptr->tail_len = rest_in_fifo;
            break;
			//return HILI_DB2_COMPLETE;
		}
	}
	
    if(HILI_DB2_DIRECTION_REQUEST == ptr->direction){
		idpi_util_fifo_cache_remove(ptr->request_fifo_cache_ptr, ptr->pre_len, NULL);
		//rest_in_fifo = ptr->request_fifo_cache_ptr->size;
		printf("CACHED_LENGTH is %d at line %d\n\n", ptr->request_fifo_cache_ptr->size,__LINE__);
	}
	else {
		idpi_util_fifo_cache_remove(ptr->response_fifo_cache_ptr, ptr->pre_len, NULL);
		//rest_in_fifo = ptr->response_fifo_cache_ptr->size;
		printf("CACHED_LENGTH is %d at line %d\n\n", ptr->response_fifo_cache_ptr->size,__LINE__); 
	}
    
    
    
    int a;
    hili_dummy_mitm_processing(ptr->mitm_flow_ptr, db2_data_submit);
    
    if(HILI_DB2_DIRECTION_REQUEST == ptr->direction){
		idpi_util_fifo_cache_remove(ptr->request_fifo_cache_ptr, buf_len- ptr->tail_len, NULL);
		//rest_in_fifo = ptr->request_fifo_cache_ptr->size;
		printf("CACHED_LENGTH is %d at line %d\n\n", ptr->request_fifo_cache_ptr->size,__LINE__);
	}
	else {
		idpi_util_fifo_cache_remove(ptr->response_fifo_cache_ptr, buf_len - ptr->tail_len, NULL);
		//rest_in_fifo = ptr->response_fifo_cache_ptr->size;
		printf("CACHED_LENGTH is %d at line %d\n\n", ptr->response_fifo_cache_ptr->size,__LINE__); 
	}
    
    hili_db2_free_parser(db2_data_submit);
    
    return HILI_DB2_COMPLETE;
}

