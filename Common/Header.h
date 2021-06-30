#ifndef HEADER_H
#define HEADER_H


#include <iostream>
#include <stdexcept>

#include "unistd.h"
#include "InputByteStream.h"
#include "OutputByteStream.h"

#define MAX_LINE	200

using namespace std;


//*** MSG TYPE ***//

enum PACKET_TYPE : uint8_t {
	HB = 0,
	NOTI = 1,
	REQ,
	RES,
	MSG
};

enum FUNCTION_CODE : uint16_t {

	//--- Request ---//
	REQ_VERIFY = 0,
	REQ_SIGN_IN,
	REQ_MAKE_ROOM,
	REQ_ENTER_LOBBY,
	REQ_ROOM_LIST,
	REQ_JOIN_GAME,
	REQ_REPLICATION,

	//--- Response ---//
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

	//--- Others ---//
	NOTI_WELCOME,
	NOTI_REPLICATION,
	NOTI_BYE,
	CHAT,
	SUCCESS,
	FAIL,
	REJECT,
	EXIT, 
	ANY,
	NONE,

	MAX_CODE
};


typedef struct header {
	int8_t type = 0;
	uint16_t func = 0;
	int len = 0;
	uint32_t sessionId = 0;


	//--- Constructor ---//

	header() = default;

	header( int8_t _t , uint16_t _f , int _l , uint32_t _i ) 
		: type(_t) , func(_f) , len(_l) , sessionId(_i) {}

	static const int SIZE = sizeof(type) + sizeof(func) + sizeof(len) + sizeof(sessionId);


	//--- Stream functions ---//

	void read( InputByteStream &ibstream )
	{
		ibstream.read( type );
		ibstream.read( func );
		ibstream.read( len );
		ibstream.read( sessionId );
	}

	void write( OutputByteStream &obstream )
	{
		obstream.write( type );
		obstream.write( func );
		obstream.write( len );
		obstream.write( sessionId );
	}

	void insert_front( OutputByteStream &obstream )
	{
		obstream >> SIZE;
		int cur = obstream.getLength();
		obstream.flush();

		obstream.write( type );
		obstream.write( func );
		obstream.write( len );
		obstream.write( sessionId );

		obstream.setCursor( cur );
	}

	//--- Operator ---//

	struct header &operator=( const struct header &h ){
		type = h.type;
		func = h.func;
		len	= h.len;
		sessionId = h.sessionId;

		return *this;
	}

	friend std::ostream &operator<<( std::ostream &os , const struct header &h ) {
		return os << "[" << to_string( h.type )<< "] [" << to_string( h.func ) << "] [" << to_string( h.len ) << "]  [" << to_string( h.sessionId ) << "]";
	}


}Header;


#endif
