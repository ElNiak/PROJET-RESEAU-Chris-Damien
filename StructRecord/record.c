#include<stdlib.h>
#include <netinet/in.h>
#include "record.h"
#include <string.h>


struct record * record_init(){

	struct record *rec=malloc(sizeof(struct record));
	if(rec==NULL){
		return NULL;
	}
	unsigned int longh=htonl(0);
	unsigned int shorth=htons(0);
	rec->Type=0; // 2 bits [1,3]
	rec->TR=0; // 1 bit
	rec->Window=0; // 5 bits [0,31]
	rec->Seqnum=0; // 8 bits [0,255]
	rec->Length=shorth; // 16 bits Nbo [0,512]
	rec->Timestamp=0; // 32 bits 
	rec->CRC1=longh; // 32 bits Nbo
	rec->Payload=NULL; //
	rec->CRC2=longh; // 32 bits Nbo

	return rec;
}

void record_free(struct record *r){
	if(r->Payload!=NULL){
		free(r->Payload);
	}
	free(r);
}
unsigned int get_Type(struct record *r){
	return r->Type;
}
unsigned int get_TR(struct record *r){
	return r->TR;
}
unsigned int get_Window(struct record *r){
	return r->Window;
}
unsigned int get_Seqnum(struct record *r){
	return r->Seqnum;
}
unsigned int get_Length(struct record *r){
	unsigned int get=ntohs(r->Length);
	return get;
}
unsigned int get_Timestamp(struct record *r){
	return r->Timestamp;
}
unsigned int get_CRC1(struct record *r){
	unsigned int get=ntohl(r->CRC1);
	return get;
}
unsigned int get_Payload(struct record *r, unsigned char *buf, int n){

	if(n<1||r->Payload==NULL){
		return 2;
	}
	if(n>512) n=512;
	int length=(int)get_Length(r);
	if(length<n) n=length;
	memcpy(buf, r->Payload, n);	
	return 0;

}
unsigned int get_CRC2(struct record *r){
	unsigned int get=ntohl(r->CRC2);
	return get;
}
unsigned int set_Type(struct record *r,unsigned int Type){
	if(Type==0||Type>3){
		return 600;
	}
	r->Type=Type;
	return r->Type;
}
unsigned int set_TR(struct record *r,unsigned int TR){
	if(TR==0||TR==1){
		r->TR=TR;
		return 0;
	}
	return 600;
}
unsigned int set_Window(struct record *r,unsigned int Window){
	if(Window>31){
		return 600;
	}
	r->Window=Window;
	return r->Window;
}
unsigned int set_Seqnum(struct record *r,unsigned int Seqnum){
	if(Seqnum>255){
		return 600;
	}
	r->Seqnum=Seqnum;
	return r->Seqnum;
}
unsigned int set_Length(struct record *r, unsigned int Length){
	if(Length>512){
		return 600;
	}
	unsigned int set=htons(Length);
	r->Length=set;
	return 0;
}
unsigned int set_Timestamp(struct record *r, unsigned int Timestamp){
	r->Timestamp=Timestamp;
	return 0;
}
unsigned int set_CRC1(struct record *r,unsigned int CRC1){
	unsigned int set=htonl(CRC1);
	r->CRC1=set;
	return 0;
}
unsigned int set_Payload(struct record *r, unsigned char* buf, int n){
	
	if(n<0){
		return 2;
	}
	if(n>512){
		return 4;
	}

	if(r->Payload!=NULL){
		free(r->Payload);
	}
	if(n==0||buf==NULL){
		free(r->Payload);
		r->Payload=NULL;	
		set_Length(r,0);
		return 0;	
	}
	set_Length(r,(unsigned int)n);
	unsigned char* step=malloc(sizeof(unsigned char)*(n));
	memcpy(step, buf, n);
	r->Payload=step;
	return 0;

}
unsigned int set_CRC2(struct record *r, unsigned int CRC2){
	unsigned int set=htonl(CRC2);
	r->CRC2=set;
	return 0;
}
