#include <stdio.h>
#include <stdlib.h> //malloc
#include <zlib.h> //bytef
#include <string.h> // memcpy
#include <netinet/in.h> //htons
#include <time.h> //timeval
#include "packet_interface.h"

pkt_t * pkt_new(){

	pkt_t *rec=malloc(sizeof(pkt_t));
	if(rec==NULL){
		return NULL;
	}
	rec->type=0; // 2 bits [1,3]
	rec->tr=0; // 1 bit
	rec->window=0; // 5 bits [0,31]
	rec->seqnum=0; // 8 bits [0,255]
	rec->length=0; // 16 bits Nbo [0,512]
	rec->timestamp=0; // 32 bits
	rec->crc1=0; // 32 bits Nbo
	rec->payload=NULL; //
	rec->crc2=0; // 32 bits Nbo

	return rec;
}

void pkt_del(pkt_t * pkt){
  if (pkt != NULL) {
        if (pkt->payload != NULL) {
            free(pkt->payload);
        }
        free(pkt);
  }
}
ptypes_t pkt_get_type  (const pkt_t* pkt)
{
    return pkt->type;
}
uint8_t pkt_get_tr(const pkt_t* pkt)
{
  return pkt->tr;
}
uint8_t  pkt_get_window(const pkt_t* pkt)
{
    return pkt->window;
}
uint8_t  pkt_get_seqnum(const pkt_t* pkt)
{
    return pkt->seqnum;
}
uint16_t pkt_get_length(const pkt_t* pkt)
{
    return pkt->length;
}
uint32_t pkt_get_timestamp   (const pkt_t* pkt)
{
    return pkt->timestamp;
}
uint32_t pkt_get_crc1(const pkt_t* pkt)
{
    return pkt->crc1;
}
const char* pkt_get_payload(const pkt_t* pkt)
{
    return pkt->payload;
}
uint32_t pkt_get_crc2(const pkt_t* pkt)
{
    return pkt->crc2;
}

/* Setters pour les champs obligatoires du paquet. Si les valeurs
 * fournies ne sont pas dans les limites acceptables, les fonctions
 * doivent renvoyer un code d'erreur adapté.
 * Les valeurs fournies sont dans l'endianness native de la machine!
 */
pkt_status_code pkt_set_type(pkt_t *pkt, const ptypes_t type)
{
    pkt->type = type;
    return PKT_OK;
}
pkt_status_code pkt_set_tr(pkt_t *pkt, const uint8_t tr)
{
  if(tr==0||tr==1){
    pkt->tr=tr;
    return PKT_OK;
  }
  return E_TR;
}
pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window)
{
    if(window>31)
    {
        return E_WINDOW;
    }
    else{
        pkt->window=window;
        return PKT_OK;
    }
}
pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum)
{
    pkt->seqnum=seqnum;
    return PKT_OK;
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length)
{
    if (length > 512) {
        return E_LENGTH;
    }
    else{
        pkt->length=length;
        return PKT_OK;
    }
}

pkt_status_code pkt_set_timestamp(pkt_t *pkt, const uint32_t timestamp)
{
    pkt->timestamp = timestamp;
    return PKT_OK;
}

pkt_status_code pkt_set_crc1(pkt_t *pkt, const uint32_t crc1)
{
    pkt->crc1=crc1;
    return PKT_OK;
}
pkt_status_code pkt_set_payload(pkt_t *pkt, const char *data, const uint16_t length)
{
    if(length > 512){
        return E_LENGTH;
    }
    else{
        if(pkt->payload != NULL) {
            free(pkt->payload);
        }
        pkt_set_length(pkt, length);
        pkt->payload=(char *) malloc(sizeof(char)*length);
        if (pkt->payload == NULL) {
            return E_NOMEM;
        }
        memcpy(pkt->payload,data,length);
        return PKT_OK;
    }
}
pkt_status_code pkt_set_crc2(pkt_t *pkt, const uint32_t crc2)
{
    pkt->crc2=crc2;
    return PKT_OK;
}
pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{

		if(len<12)
		{
			pkt_del(pkt);
			return E_NOHEADER;
		}
		//check crc1
		uLong crc1=crc32(0L,Z_NULL,0);
		memcpy((void*)crc1,&data[8],4);
		uLong header_to_crc1=crc32(0L,(const unsigned char *)&data[0],4);
		if(header_to_crc1!=crc1)
		{
			pkt_del(pkt);
			return E_CRC;
		}
		pkt_status_code err=pkt_set_crc1(pkt,crc1);
		if(err!=PKT_OK){
			pkt_del(pkt);
			return err;
		}
		//set type
		ptypes_t type=data[0] >>6;
		if(type==PTYPE_DATA||type==PTYPE_ACK)
		{
			pkt_set_type(pkt,type);
		}else if(type==PTYPE_NACK){
		 //todo
	 	}else{
			fprintf(stderr,"unknown type");
			pkt_del(pkt);
			return E_TYPE;
		}
		//set tr
		uint8_t tr=data[0]&0b00000100;
		tr=tr>>2;
		if(tr==1&&pkt_get_type(pkt)!=PTYPE_DATA){
			//ignore packet
		}
		if(tr==1&&pkt_get_type(pkt)==PTYPE_DATA){
			pkt_set_type(pkt,PTYPE_NACK);
		}
		err=pkt_set_tr(pkt,tr);
		if(err!=PKT_OK){
			pkt_del(pkt);
			return E_TR;
		}
		//set window
		uint8_t window=data[0]&0b00011111 ;
    err = pkt_set_window(pkt,window);
		if(err!=PKT_OK) return err;
		//set seqnum
		pkt_set_seqnum(pkt,data[1]);
		//set length
		uint16_t length;
		memcpy((void *)&length,(const void *)&data[2],sizeof(length));
    pkt_set_length(pkt,length);
    if(ntohs(pkt_get_length(pkt)) > 512 || len>528)
    {
        fprintf(stderr,"(decode) length");
        pkt_del(pkt);
        return E_LENGTH;
    }
		//set timestamp
		uint32_t timest;
		memcpy((void *)&timest,(const void*)&data[4],sizeof(timest));
		pkt_set_timestamp(pkt,timest);
		//check if the data contains a payload
		// if not, the function is over
		if(len==12)
		{
			return PKT_OK;
		}
		//check if length is coherent
		if((uint16_t) len != length + 16){
				pkt_del(pkt);
				return E_UNCONSISTENT;
		}
		//check crc2
		uLong crc2=crc32(0L,Z_NULL,0);
		memcpy((void*)crc2,&data[len-4],4);
		uLong payload_to_crc2=crc32(0L,(const unsigned char *)&data[12],length);
		if(payload_to_crc2!=crc2)
		{
			pkt_del(pkt);
			return E_CRC;
		}
		err=pkt_set_crc2(pkt,crc2);
		if(err!=PKT_OK){
			pkt_del(pkt);
			return err;
		}
		//set payload
		char * payload = (char *)malloc(sizeof(char)*ntohs(pkt_get_length(pkt)));
		memcpy((void *)payload,(const void *)&data[12],sizeof(char)*length);
		err=pkt_set_payload(pkt,payload,ntohs(pkt_get_length(pkt)));
		if(err!=PKT_OK){
			pkt_del(pkt);
			return err;
		}
		return PKT_OK;
}
pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{
	size_t min=12;
	if(*len<min){
		return E_TYPE;
	}
	if(ntohs(pkt_get_length(pkt))==0)
	{
		//set type, tr and window
		uint8_t type=(pkt_get_type(pkt)<<6);
		uint8_t tr=(pkt_get_tr(pkt)<<5);
		uint8_t window=pkt_get_window(pkt);
		uint8_t type_tr_window=type|tr|window;
		buf[0]=(char)type_tr_window;
		//set seqnum
		uint8_t seqnum=pkt_get_seqnum(pkt);
		buf[1]=(char)seqnum;
		//set length
		uint16_t length=ntohs(pkt_get_length(pkt));
		memcpy((void*)&buf[2],(const void*)&length,sizeof(length));
		//set timestamp
		uint32_t timestamp=pkt_get_timestamp(pkt);
		memcpy((void*)&buf[4],(const void*)&timestamp,sizeof(timestamp));
		//set crc1
		uLong crc1=crc32(0l,Z_NULL,0);
		crc1=htonl(crc32(crc1,(const Bytef *)buf,8));
		memcpy((void*)&buf[8],(const void*)&crc1,sizeof(uLong));
		return PKT_OK;
	}
	//check if the buffer is big enough
	size_t seg_length=(size_t)ntohs(pkt_get_length(pkt))+16;
	if(*len<seg_length)
	{
		return E_TR;
	}
	//set type, tr and window
	uint8_t type=(pkt_get_type(pkt)<<6);
	uint8_t tr=(pkt_get_tr(pkt)<<5);
	uint8_t window=pkt_get_window(pkt);
	uint8_t type_tr_window=type|tr|window;
	buf[0]=(char)type_tr_window;
	//set seqnum
	uint8_t seqnum=pkt_get_seqnum(pkt);
	buf[1]=(char)seqnum;
	//set length
	uint16_t length=pkt_get_length(pkt);
	memcpy((void*)&buf[2],(const void*)&length,sizeof(length));
	//set timestamp
	uint32_t timestamp=pkt_get_timestamp(pkt);
	memcpy((void*)&buf[4],(const void*)&timestamp,sizeof(timestamp));
	//set crc1
	uLong crc1=crc32(0l,Z_NULL,0);
	crc1=htonl(crc32(crc1,(const Bytef *)buf,8));
	memcpy((void*)&buf[8],(const void*)&crc1,sizeof(uLong));
	//set payload
	memcpy((void *)&buf[12],(const void *)pkt_get_payload(pkt),pkt_get_length(pkt));
	//set crc2
	uLong crc2=crc32(0l,Z_NULL,0);
	crc2=htonl(crc32(crc2,(const Bytef *)buf,12+pkt_get_length(pkt)));
	memcpy((void*)&buf[12*pkt_get_length(pkt)],(const void*)&crc1,sizeof(uLong));
	return PKT_OK;
}
