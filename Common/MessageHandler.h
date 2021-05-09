#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <map>
#include <functional>

#include "TCP.h"
#include "Header.h"
#include "MessageQueue.h"
#include "UserInfo.h"
#include "Parser.h"
#include "UserRedis.h"
#include "Debug.h"
#include "RoomManager.h"

using namespace std;


class MessageHandler {

	const string m_logFileName;
	MessageQueue &m_msgQ;
	LOG *m_pLog;

public:

	//--- Constructor ---//

	MessageHandler() = default;

	MessageHandler( MessageQueue &queue , const string &fileName )
					: 	m_msgQ( queue ),
				 		m_logFileName( fileName )
	{

		// init something //
		m_pLog = LOG::getInstance( m_logFileName );
	}


	//--- Functions ---//

	void acceptHandler( SessionManager &sessionMgr , int clntSocket , const string &welcomeMSG = "" )
	{
		// Set a new session //
		sessionMgr.newSession( clntSocket );

		// Validate HB timer //
		sessionMgr.validate( clntSocket );

		// Enqueue welcome MSG //
		OutputByteStream payload( welcomeMSG.length() + 4 );
		payload.write( welcomeMSG );

		Header header( PACKET_TYPE::NOTI , FUNCTION_CODE::WELCOME , payload.getLength() , clntSocket );

		OutputByteStream packet( Header::SIZE + header.len );
		header.write( packet );
		packet << payload;
		m_msgQ.enqueue( InputByteStream( packet ) );
	}

	void inputHandler( int clntSocket )
	{
		InputByteStream msg( TCP::MPS );

		try {
			//--- Verify user informations ---//

			TCP::recv_packet( clntSocket , msg );
			m_pLog->writeLOG( msg , LOG::TYPE::RECV );
			Header header; header.read( msg );
			msg.flush();

			switch( header.type )
			{
				case PACKET_TYPE::HB : //--- TYPE : HB ---//
					onHeartbeat();
					break;
				case PACKET_TYPE::REQ : //--- TYPE : REQ ---//
				{
					onRequest( msg );
					break;
				}
				case PACKET_TYPE::RES : //--- TYPE : RES ---//
					break;
				case PACKET_TYPE::NOTI : //--- TYPE : NOTI ---//
					break;
				default :
					break;
			}
		}
		catch( TCP::Connection_Ex e )
		{
			throw e;
		}
	}

	void invalidHandler()
	{
		cout << "Captured invalid type of packet" << endl;
	}

	void onHeartbeat()
	{
		//cout << "<<< [HB]  -^-v-^-" << endl;		
	}

	void onRequest( InputByteStream &msg )
	{
		m_msgQ.enqueue( move( msg ) );
	}

	void onSignIn( const string &id , const string &pw )
	{
		
	}

	void resCreateNewRoom( void *lParam , void *rParam )
	{

	}

	void resEnterRoom( void **inParams , void **outParams )
    {
		SessionManager *pSessionMgr = reinterpret_cast<SessionManager*>( inParams[0] );
        InputByteStream *pPacket = reinterpret_cast<InputByteStream*>( outParams[0] );	
		Header header; header.read( *pPacket );

		try{
			Session &session = pSessionMgr->getSessionById( header.sessionId );     // To update user information in the session, if it's verified user.

			//--- Extract userInfo from message ---//
			UserInfo userInfo;      // User information from REQ_MSG.
			userInfo.read( *pPacket );

			//--- Result from redis HMGET command ---//
			UserRedis::getInstance()->hmgetUserInfo( userInfo );

			//--- Set response packet ---//
			OutputByteStream payload( TCP::MPS );
			userInfo.write( payload );

			header.type = PACKET_TYPE::RES;
			header.len = payload.getLength();

			OutputByteStream resPacket( Header::SIZE + header.len );

			//--- CASE : The request is VALID ---//
			if( userInfo.getId() != "" )
			{
				//--- Update session data ---//
				session.m_userInfo = userInfo;
				session.m_userInfo.setPw( "" );

				header.func = FUNCTION_CODE::RES_ENTER_LOBBY_SUCCESS;
				header.write( resPacket );
				resPacket << payload;
			}
			//--- CASE : The request is INVALID ---//
			else
			{
				header.func = FUNCTION_CODE::RES_ENTER_LOBBY_FAIL;
				header.len= 0;
				header.write( resPacket );
			}

			*pPacket = InputByteStream(resPacket);
		}
		catch( Not_Found_Ex e )
        {
            cout << "[verifyUserInfo] " << e.what() << endl;
        }
    }

	void resRoomList( void **inParams , void **outParams )
    {
        InputByteStream *pPacket = reinterpret_cast<InputByteStream*>( outParams[0] );
		//SessionManager *pSessionMgr = reinterpret_cast<SessionManager*>( rParam );
		Header header; header.read( *pPacket );
        
		//--- Result from redis LRANGE command ---//
        const list<Room> result = UserRedis::getInstance()->lrangeRoomList();

		header.type = PACKET_TYPE::RES;

		OutputByteStream resPacket( TCP::MPS );
	
        //--- CASE : The request is VALID ---//
        if( result.size() > 0 )
        {
			//--- Set response packet ---//
			OutputByteStream payload( TCP::MPS );

			for( auto room : result )
			{
				room.write( payload );
			}
			header.len = payload.getLength();
			header.func = FUNCTION_CODE::RES_ROOM_LIST_SUCCESS;

			header.write( resPacket );
			resPacket << payload;
        }
        //--- CASE : The request is INVALID ---//
        else
        {
            header.func = FUNCTION_CODE::RES_ROOM_LIST_FAIL;
			header.len = 0;
			header.write( resPacket );
        }
		
		*pPacket = InputByteStream( resPacket );
    }

	void resJoinGame( void **inParams , void **outParams )
	{
		SessionManager *pSessionMgr = reinterpret_cast<SessionManager*>( inParams[0] );
		list<RoomManager> *pRoomList = reinterpret_cast<list<RoomManager>*>( inParams[1] );
		InputByteStream *pPacket = reinterpret_cast<InputByteStream*>( outParams[0] );
		Header header; header.read( *pPacket );

		//--- Extract room id from message ---//
        string roomId;
        pPacket->read( roomId );

		//--- Extract user id from message ---//
        string userId;
        pPacket->read( userId );

		UserInfo userInfo;
		userInfo.setId( userId );
  
		//--- Result from redis HMGET command ---//
		UserRedis::getInstance()->hmgetUserInfo( userInfo );

		//--- Update session's pointer in the room list ---//
		bool result = false;

		for( auto &itr : *pRoomList)
		{
			if( itr.isEqual( roomId ) )
			{
				itr.acceptSession( pSessionMgr->getSessionById( header.sessionId ) );
				result = true;

				break;
			}
		}

		header.type = PACKET_TYPE::RES;
		OutputByteStream resPacket( TCP::MPS );
	
        //--- CASE : The request is VALID ---//
        if( result )
        {
			//--- Set response packet ---//
			OutputByteStream payload( TCP::MPS );
			//--- TODO : Payload ---//

			header.len = payload.getLength();
			header.func = FUNCTION_CODE::RES_JOIN_GAME_SUCCESS;

			header.write( resPacket );
			resPacket << payload;
        }
        //--- CASE : The request is INVALID ---//
        else
        {
            header.func = FUNCTION_CODE::RES_JOIN_GAME_FAIL;
			header.len = 0;
			header.write( resPacket );
        }
		
		*pPacket = InputByteStream( resPacket );
	}
	void resMakeRoom( void **inParams , void **outParams )
	{
		list<RoomManager> *pRoomList = reinterpret_cast<list<RoomManager>*>( inParams[0] );
		InputByteStream *pPacket = reinterpret_cast<InputByteStream*>( outParams[0] );	
		Header header; header.read( *pPacket );
		Room room; room.read( *pPacket );

		if( UserRedis::getInstance()->hmsetNewRoom( room ) == true )
			UserRedis::getInstance()->hmgetRoom( room );

		//--- Set response packet ---//
		OutputByteStream payload( TCP::MPS );
		room.write( payload );

		header.type = PACKET_TYPE::RES;
		header.len = payload.getLength();

		OutputByteStream resPacket( Header::SIZE + header.len );

        //--- CASE : The request is valid ---//
        if( room.roomId != "" )
        {
			//--- Add new room in the list ---//
			pRoomList->emplace_back(room);
			header.func = FUNCTION_CODE::RES_MAKE_ROOM_SUCCESS;
			header.write( resPacket );
			resPacket << payload;
        }
		//--- CASE : The request is invalid ---//
		else
		{
			header.func = FUNCTION_CODE::RES_MAKE_ROOM_FAIL;
			header.len = 0;
			header.write( resPacket );
		}

		InputByteStream ibstream( resPacket );
		*pPacket = resPacket;
	}


	void registerHandler( map<int , function<void(void**,void**)>> &h_map )
    {

        h_map.insert( make_pair( (int)FUNCTION_CODE::REQ_ENTER_LOBBY , 
                                        [this](void **in , void **out) 
                                        { 
                                            this->resEnterRoom(in,out); 
                                        } 
                                    ) 
                    );
		h_map.insert( make_pair( (int)FUNCTION_CODE::REQ_JOIN_GAME , 
										[this](void **in , void **out) 
										{ 
											this->resJoinGame(in,out); 
										} 
									) 
					);

		h_map.insert( make_pair( (int)FUNCTION_CODE::REQ_ROOM_LIST , 
										[this](void **in , void **out) 
										{ 
											this->resRoomList(in,out); 
										} 
									) 
					);

		h_map.insert( make_pair( (int)FUNCTION_CODE::REQ_MAKE_ROOM , 
                                        [this](void **in , void **out) 
                                        { 
                                            this->resMakeRoom(in,out); 
                                        } 
                                    ) 
                    );
        //h_map[VERIFY] = [this](void* l,void* r) {this->verifyUserInfo(l,r);};
    }
};

#endif
