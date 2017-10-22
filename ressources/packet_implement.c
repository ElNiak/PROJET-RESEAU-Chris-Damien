#include <stdio.h>
#include <stdlib.h> //malloc
#include <zlib.h> //bytef
#include <string.h> // memcpy
#include <netinet/in.h> //htons
#include <time.h> //timeval
#include "packet_interface.h"

pkt_t * pkt_new(){

	struct pkt_t *rec=malloc(sizeof(pkt_t));
	if(rec==NULL){
		return NULL;
	}
	uint32_t longh=htonl(0);
	uint16_t shorth=htons(0);
	rec->type=0; // 2 bits [1,3]
	rec->tr=0; // 1 bit
	rec->window=0; // 5 bits [0,31]
	rec->seqnum=0; // 8 bits [0,255]
	rec->length=shorth; // 16 bits Nbo [0,512]
	rec->timestamp=0; // 32 bits
	rec->crc1=longh; // 32 bits Nbo
	rec->payload=NULL; //
	rec->crc2=longh; // 32 bits Nbo

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
uint8_t get_tr(const pkt_t* pkt){
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
    uint16_t get=nthos(pkt->length);
    return get;
}
uint32_t pkt_get_timestamp   (const pkt_t* pkt)
{
    return pkt->timestamp;
}
uint32_t pkt_get_crc1(const pkt_t* pkt)
{
    uint32_t get=ntohl(pkt->crc1);
    return get;
}
const char* pkt_get_payload(const pkt_t* pkt)
{
    return pkt->payload;
}
uint32_t pkt_get_crc2(const pkt_t* pkt)
{
    uint32_t get=ntohl(pkt->crc2);
    return get;
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
        uint8_t set=htons(length);
        pkt->length=set;
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
    uint32_t set=htonl(crc1);
    pkt->crc1=set;
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
        pkt->payload=(char *) calloc(length,sizeof(char));
        if (pkt->payload == NULL) {
            return E_NOMEM;
        }

        memcpy(pkt->payload,(char *)data,length);
        return PKT_OK;
    }
}
pkt_status_code pkt_set_crc2(pkt_t *pkt, const uint32_t crc2)
{
    uint32_t set=htonl(crc2);
    pkt->crc2=set;
    return PKT_OK;
}
pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{
  return E_NOMEM;
}
pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{
  return E_NOMEM;
}
