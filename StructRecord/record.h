#ifndef __RECORD_H_
#define __RECORD_H_

#include <stdio.h>  /* FILE */


structs record{

	unsigned int Type:2;
	unsigned int TR:1;
	unsigned int Window:5;
	unsigned int Seqnum:8;
	unsigned int Length:16;//network byte order cfr function hton(s) et nton(s)
	unsigned int Timestamp;
	unsigned int CRC1;//network byte order cfr function hton(s) et nton(s)
	unsigned char* Payload;
	unsigned int CRC2;//network byte order cfr function hton(s) et nton(s)
};


/**
* Initialise un enregistrement
* @pre: r!= NULL
* @post: record_get_type(r) == 0 && record_get_length(r) == 0
*		 && record_has_footer(r) == 0
* @return: 1 en cas d'erreur, 0 sinon
*/
struct record *record_init();

/**
* Libere les ressources consommees par un enregistrement
* @pre: r!= NULL
*/
void record_free(struct record *r);

unsigned int get_TR(struct record *r);

unsigned int get_Type(struct record *r);

unsigned int get_Window(struct record *r);

unsigned int get_Seqnum(struct record *r);

unsigned int get_Length(struct record *r);

unsigned int get_Timestamp(struct record *r);

unsigned int get_CRC1(struct record *r);

unsigned int get_Payload(struct record *r, unsigned char *buf, int n);

unsigned int get_CRC2(struct record *r);

unsigned int set_Type(struct record *r,unsigned int Type);

unsigned int set_TR(struct record *r,unsigned int TR);

unsigned int set_Window(struct record *r,unsigned int Window);

unsigned int set_Seqnum(struct record *r,unsigned int Seqnum);

unsigned int set_Length(struct record *r, unsigned int Length);

unsigned int set_Timestamp(struct record *r, unsigned int Timestamp);

unsigned int set_CRC1(struct record *r,unsigned int CRC1);

unsigned int set_Payload(struct record *r, unsigned char* buf, int n);

unsigned int set_CRC2(struct record *r, unsigned int CRC2);

#endif /* __RECORD_H_ */
