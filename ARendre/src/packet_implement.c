#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <string.h>
#include <netinet/in.h>
#include <time.h>
#include "packet_interface.h"


/* Extra code */
/* Your code will be inserted here */

pkt_t* pkt_new()
{
	return calloc(1,sizeof(pkt_t));
}

void pkt_del(pkt_t *pkt)
{
	if(pkt == NULL){
		return;
	}
	if(pkt_get_length (pkt)!= 0){
		free(pkt->payload);
	}
	free(pkt);
}

ptypes_t pkt_get_type  (const pkt_t* pkt)
{
	return pkt->type;
}

uint8_t  pkt_get_tr(const pkt_t* pkt)
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

uint32_t pkt_get_timestamp(const pkt_t* pkt)
{
	return pkt->timestamp;
}

uint32_t pkt_get_crc1(const pkt_t* pkt)
{
	return pkt->crc1;
}

const char* pkt_get_payload(const pkt_t* pkt)
{
	if(pkt_get_tr(pkt) !=0){
		return NULL;
	}
	return pkt->payload;
}

uint32_t pkt_get_crc2   (const pkt_t* pkt)
{
	if(pkt_get_payload(pkt) == NULL || pkt_get_tr(pkt) != 0){
		return 0;
	}
	return pkt->crc2;
}

pkt_status_code pkt_set_type(pkt_t *pkt, const ptypes_t type)
{
	if(type != PTYPE_DATA && type != PTYPE_ACK && type != PTYPE_NACK){
		return E_TYPE;
	}
	pkt->type = type;
	return PKT_OK;
}

pkt_status_code pkt_set_tr(pkt_t *pkt, const uint8_t tr)
{
	if(pkt_get_type(pkt) != PTYPE_DATA && tr != 0){
		return E_TR;
	}
	pkt->tr=tr;
	return PKT_OK;
}

pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window)
{
	if(window > MAX_WINDOW_SIZE){
		return E_WINDOW;
	}
	pkt->window=window;
	return PKT_OK;
}

pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum)
{
	pkt->seqnum=seqnum;
	return PKT_OK;
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length)
{
	// network byte order
	if(sizeof(length)>512){
		return E_LENGTH;
	}
	pkt->length=length;
	return PKT_OK;
}

pkt_status_code pkt_set_timestamp(pkt_t *pkt, const uint32_t timestamp)
{
	pkt->timestamp=timestamp;
	return PKT_OK;
}

pkt_status_code pkt_set_crc1(pkt_t *pkt, const uint32_t crc1)
{
	pkt->crc1=crc1;
	return PKT_OK;
}


pkt_status_code pkt_set_payload(pkt_t *pkt, const char *data, const uint16_t length)
{


	if(length>MAX_PAYLOAD_SIZE){
		return E_LENGTH;
	}
	if(pkt_get_length(pkt) ==0 || data ==NULL){
		free(pkt->payload);
		pkt_set_length(pkt,0);
	}

	pkt->payload=(char *) malloc(sizeof(char )*length);
	if(pkt->payload == NULL){
		return E_NOMEM;
	}
	memcpy(pkt->payload,data,length);
	pkt_set_length(pkt,length);
	return PKT_OK;
}

pkt_status_code pkt_set_crc2(pkt_t *pkt, const uint32_t crc2)
{
	if(pkt_get_tr(pkt) !=0 || pkt_get_payload(pkt) == NULL)
	{
		return E_CRC;
	}
	pkt->crc2=crc2;
	return PKT_OK;
}
pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{
	//	check there is a header
	if(len<12){
		return E_NOHEADER;
	}
	//set type, tr et window
	memcpy(pkt,data,sizeof(uint8_t));

	//	Check si le type copier est bon
	if(pkt->type != PTYPE_DATA && pkt->type != PTYPE_ACK && pkt->type != PTYPE_NACK){
		return E_TYPE;
	}
	//	Check si le tr est cohérent
	if(pkt->type != PTYPE_DATA && pkt->tr != 0){
		return E_TR;
	}

	//set seqnum
	memcpy(&(pkt->seqnum),data+1,sizeof(uint8_t));

	//set length
	memcpy(&(pkt->length),data+2,sizeof(uint16_t));
	pkt->length=ntohs(pkt->length);
	if(pkt->length > 512){
		return E_LENGTH;
	}
	//set timestamp
	memcpy(&(pkt->timestamp),data+4,sizeof(uint32_t));

	//check if crc1 is coherent & set crc1
	memcpy(&(pkt->crc1),data+8,sizeof(uint32_t));
	pkt->crc1=ntohl(pkt->crc1);

	uint32_t crc1 = crc32(0L,Z_NULL,0);
	unsigned char *buf=(unsigned char*) malloc(8);

	if(buf == NULL){
		return E_NOMEM;
	}
	memcpy(buf,data,8);
	crc1 = crc32(crc1,buf,8);

	if(crc1 != pkt->crc1){
		return E_CRC;
	}
	free(buf);

	//set payload, check if crc2 is coherent, set crc2
	if(pkt->length != 0 && len != (12+ sizeof(char)*pkt->length +4) && pkt->tr ==0){
		return E_UNCONSISTENT;
	}
	else if(pkt->length == 0 && len != 12 && pkt->tr ==0){	 // signifie pas de payload
		return E_UNCONSISTENT;
	}
	else if(pkt->length == 0 && len == 12 && pkt->tr == 1){
		return PKT_OK;
	}

	if(pkt_get_tr(pkt) == 0 && pkt->length != 0){ //check shorten package
		pkt_set_payload(pkt,data+12,pkt->length);
		uint16_t size = pkt_get_length(pkt);
		uint32_t crc2_calc = crc32(0L,Z_NULL,0);
		unsigned char *buf_crc2 = (unsigned char *)malloc(size);
		if(buf_crc2 == NULL){
			return E_NOMEM;
		}
		memcpy(buf_crc2,data+12,size);
		crc2_calc = crc32(crc2_calc,buf_crc2,size);
		memcpy(&(pkt->crc2),data+12+size,4);
		pkt->crc2 = ntohl(pkt->crc2);
		if(crc2_calc != pkt->crc2){
			return E_CRC;
		}
		free(buf_crc2);
	}
	return PKT_OK;
}

pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{
	fprintf(stderr, "packet_interface =>  pkt_encode() - 1\n");
	if(*len < 12){
		return E_NOMEM;
	}
	//copy type, tr & window
	memcpy(buf,pkt,sizeof(uint8_t));
	//copy seqnum
	memcpy(buf+1,&(pkt->seqnum),sizeof(uint8_t));

	//chekc if buff is big enough
	if(pkt->length >512){
		fprintf(stderr, "packet_interface =>  pkt_encode() - pkt->length >512\n");
		return E_LENGTH;
	}
	//change to nbo and copy length
	uint16_t *taille_ptr = (uint16_t *)malloc(sizeof(uint16_t));
	*taille_ptr = htons(pkt->length);
	memcpy(buf+2,taille_ptr,sizeof(uint16_t));
	free(taille_ptr);

	//copy timestamp
	memcpy(buf+4,&(pkt->timestamp),sizeof(uint32_t));

	//copy crc1
	uint32_t crc1 = crc32(0L, Z_NULL, 0);
	unsigned char *buf1=(unsigned char*) malloc(8);
	if(buf1 ==NULL){
		fprintf(stderr, "packet_interface =>  pkt_encode() - buf1 == NULL \n");
		return E_NOMEM;
	}
	memcpy(buf1,buf,8);

	crc1 = crc32(crc1,buf1,8);
	uint32_t bufferCRC1=htonl(crc1);
	memcpy(buf+8,&bufferCRC1,sizeof(uint32_t));
	free(buf1);
	//check if package got a payload
	if(pkt_get_tr(pkt) != 0){
		fprintf(stderr, "packet_interface =>  pkt_encode() - PKT_OK - pkt_get_tr(pkt) != 0\n");
		*len=12;
		return PKT_OK;
	}

	//check if buf is big enough
	if(*len < (size_t)pkt_get_length(pkt)+12){
		fprintf(stderr, "packet_interface =>  pkt_encode() - *len < (size_t)pkt_get_length(pkt)+16 : *len : %zu - pkt_get_length(pkt)+16) : %d \n",*len,pkt_get_length(pkt)+16);
		return E_NOMEM;
	}
	//copy payload
	memcpy(buf+12,pkt->payload,pkt_get_length(pkt));

	// compute crc2, set payload, set crc2
	if(pkt_get_payload(pkt) != NULL && pkt_get_tr(pkt) == 0)
	{

		unsigned char *buf2=(unsigned char *) malloc(pkt_get_length(pkt));
		if(buf2==NULL){
			fprintf(stderr, "packet_interface =>  pkt_encode() - buf2 == NULL \n");
			return E_NOMEM;
		}
		uint32_t crc2=crc32(0L, Z_NULL, 0);

		memcpy(buf2,pkt->payload,pkt_get_length(pkt));

		crc2 = crc32(crc2,buf2,pkt_get_length(pkt));
		crc2 = htonl(crc2);
		memcpy(buf+12+pkt_get_length(pkt),&crc2,sizeof(uint32_t));
		free(buf2);
		//update len
		*len=12+pkt_get_length(pkt);

	}
	fprintf(stderr, "packet_interface =>  pkt_encode() - 2 PKT_OK \n");
	return PKT_OK;
}
