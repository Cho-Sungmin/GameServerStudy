#ifndef PACKET_H
#define PACKET_H

#include "string.h"
#include "unistd.h"
#include <iostream>
#include <stdexcept>


#define MAX_LINE	200


//*** MSG TYPE ***//

enum PACKET_TYPE {
	HB	= 0,
	NOTI = 1,
	REQ	,
	RES	,
	MSG
};

//*** REQUEST MSG ***//
enum FUNCTION_CODE {

	//--- Function Key ---//
	REQ_VERIFY	= 0	,
	REQ_SIGN_IN 	,
	REQ_MAKE_ROOM	,
	REQ_ENTER_LOBBY	,
	REQ_ROOM_LIST	,
	REQ_JOIN_GAME	,

	//--- Result Code ---//
	RES_VERIFY_SUCCESS,
	RES_SIGN_IN_SUCCESS,
	RES_MAKE_ROOM_SUCCESS,
	RES_ENTER_LOBBY_SUCCESS,
	RES_ROOM_LIST_SUCCESS,
	RES_JOIN_GAME_SUCCESS,
	RES_VERIFY_FAIL,
	RES_SIGN_IN_FAIL,
	RES_MAKE_ROOM_FAIL,
	RES_ENTER_LOBBY_FAIL,
	RES_ROOM_LIST_FAIL,
	RES_JOIN_GAME_FAIL,
	SUCCESS	,
	FAIL	,
	REJECT	,
	EXIT 	, 
	ANY 	,
	NONE
};


//*** RESPONSE MSG ***//


typedef struct header {
	int 	type		= 0	;
	int		func		= 0	;
	int 	len			= 0 ;
	int		sessionID	= 0 ;


	//--- Constructor ---//

	header()	= default;

	header( int _t , int _f , int _l , int _i ) 
		: type(_t) , func(_f) , len(_l) , sessionID(_i) {}


	//--- Operator ---//

	void operator=( const struct header& h ){
		type 		= h.type		;
		func 		= h.func		;
		len	 		= h.len 		;
		sessionID 	= h.sessionID	;
	}

	friend std::ostream& operator<<( std::ostream& os , const struct header& h ) {
		return os << "[" << h.type << "] [" << h.func << "] [" << h.len << "] ";
	}


}HEAD;

typedef char 	DATA;


typedef struct Packet {
	HEAD	head						;
	DATA	data[ MAX_LINE ] = { 0 , }	;


	//--- Constructor ---//

	Packet() = default;

	Packet( int _t , int _f , int _l , int _i ) 
		: head(_t,_f,_l,_i) 
	{
	
	}

	Packet( int _t , int _f , int _l , int _i , const char* _data ) 
		: head(_t,_f,_l,_i) 
	{
		memset( data , 0x00  , MAX_LINE );
		memcpy( data , _data , _l 		);
	}


	//--- Operator ---//

	void operator=( const Packet& p ){
		head = p.head;
		memcpy( data , p.data , p.head.len );
	}
	
	Packet& operator+( const Packet& p )
	{
		char tmp[ MAX_LINE ];
	
		if( head.len + p.head.len > MAX_LINE )
			throw std::out_of_range("Out of range : Data Length");

		memcpy( tmp 			, data 		, head.len 		); 
		memcpy( tmp+head.len 	, p.data 	, p.head.len 	);

		head.len += p.head.len;
		memcpy( data , tmp , head.len );

		return *this;
	}

	Packet& operator+=( const Packet& p ) 
	{
		return *this + p;
	}

	friend std::ostream& operator<<( std::ostream& os , const Packet& p ){
		return os << p.head << "[" << p.data << "] " ;  
	}

	//--- Functions ---//
	
	void setPacket( int _t , int _f , int _l , int _i , const char* _data )
	{
		head.type 		= _t;
		head.func 		= _f;
		head.len 		= _l;
		head.sessionID 	= _i;

		memset( data , 0x00  , MAX_LINE );
		memcpy( data , _data , _l );
	}

}Packet;



#endif
