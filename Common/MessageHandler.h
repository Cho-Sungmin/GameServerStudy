#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H


#include "Packet.h"
#include "MessageQueue.h"
#include "UserInfo.h"
#include <map>
#include "Parser.h"
#include "UserRedis.h"
#include <functional>
#include "Debug.h"


class MessageHandler {

	const string m_logFileName;
	MessageQueue& 	m_msgQ;
	LOG* m_pLog;

public:

	//--- Constructor ---//

	MessageHandler() = default;

	MessageHandler( MessageQueue& queue , const string& fileName )
					: 	m_msgQ( queue ),
				 		m_logFileName( fileName )
	{

		// init something //
		m_pLog = LOG::getInstance( m_logFileName );
	}


	//--- Functions ---//

	void acceptHandler( SessionManager& sessionMgr , int clntSocket , const std::string& welcomeMSG = "" )
	{
		// Set a new session //
		sessionMgr.newSession( clntSocket );

		// Validate HB timer //
		sessionMgr.validate( clntSocket );

		// Enqueue welcome MSG //
		Packet 	welcomePacket( PACKET_TYPE::NOTI , FUNCTION_CODE::NONE , welcomeMSG.length() , clntSocket , welcomeMSG.c_str() );
		m_msgQ.enqueue( welcomePacket );

	}

	void inputHandler( int clntSocket )
	{
		Packet msg;

		try {
			//*** TODO: Verify user informations ---//


			TCP::recv_packet<Packet>( clntSocket , &msg );
			m_pLog->writeLOG( msg , LOG::TYPE::RECV );

			msg.head.sessionID = clntSocket;

			switch( msg.head.type ) {
			
				case PACKET_TYPE::HB		: //--- TYPE : HB ---//
					onHeartbeat();
					break;

				case PACKET_TYPE::REQ	: //--- TYPE : REQ ---//
				{
					onRequest( msg );
					break;
				}
				case PACKET_TYPE::RES	: //--- TYPE : RES ---//
					break;
				case PACKET_TYPE::NOTI	: //--- TYPE : NOTI ---//
					break;
				default	 	:
					break;
			}

		}
		catch( TCP::Connection_Ex e )
		{
			throw e;
		}
		catch( Not_Found_Ex e )
		{
			cout << "[inputHandler] " << e.what() << endl;
		}

	}

	void invalidHandler()
	{
		cout << "Captured invalid type of packet" << endl;
	}

	void onHeartbeat()
	{
		//std::cout << "<<< [HB]  -^-v-^-" << std::endl;		
	}

	void onRequest( const Packet& msg )
	{
		m_msgQ.enqueue( msg );
	}

	void onSignIn( const string& id , const std::string& pw )
	{
		
	}

	void resCreateNewRoom( void* lParam , void* rParam )
	{

	}

	void resEnterRoom( void* lParam , void* rParam )
    {
        Packet* pMsg = reinterpret_cast<Packet*>( lParam );
		SessionManager* pSessionMgr = reinterpret_cast<SessionManager*>( rParam );
        Session& session = pSessionMgr->getSessionById( pMsg->head.sessionID );     // To update user information in the session, if it's verified user.

		//--- Extract userInfo from message ---//
        UserInfo data;      // User information from REQ_MSG.
        Parser::parseUserInfo( data , pMsg->data , ' ' );
        
		//--- Result from redis HMGET command ---//
        const string result = UserRedis::getInstance()->hmgetUserInfo( data );

		//--- Set response packet ---//
        pMsg->head.type = PACKET_TYPE::RES;
        pMsg->head.len = result.length();

        //--- CASE : The request is VALID ---//
        if( result != "NULL" )
        {
            //--- Update session data ---//
            Parser::parseUserInfo( session.m_userInfo , result.c_str() , ' ' );
            session.m_userInfo.SetPw( "" );

            pMsg->head.func = FUNCTION_CODE::RES_ENTER_LOBBY_SUCCESS;
        }
        //--- CASE : The request is INVALID ---//
        else
        {
            pMsg->head.func = FUNCTION_CODE::RES_ENTER_LOBBY_FAIL;
        }
    }

	void resRoomList( void* lParam , void* rParam )
    {
        Packet* pMsg = reinterpret_cast<Packet*>( lParam );
		SessionManager* pSessionMgr = reinterpret_cast<SessionManager*>( rParam );
        Session& session = pSessionMgr->getSessionById( pMsg->head.sessionID );     // To update user information in the session, if it's verified user.
        
		//--- Result from redis LRANGE command ---//
        const string& result = UserRedis::getInstance()->lrangeRoomList();

		//--- Set response packet ---//
		pMsg->setPacket( PACKET_TYPE::RES , 0 , result.length() , pMsg->head.sessionID , result.c_str() );

        //--- CASE : The request is VALID ---//
        if( result != "NULL" )
        {
            pMsg->head.func = FUNCTION_CODE::RES_ROOM_LIST_SUCCESS;
        }
        //--- CASE : The request is INVALID ---//
        else
        {
            pMsg->head.func = FUNCTION_CODE::RES_ROOM_LIST_FAIL;
        }
    }

	void resJoinGame( void* lParam , void* rParam )
	{
		Packet* pMsg = reinterpret_cast<Packet*>( lParam );
		SessionManager* pSessionMgr = reinterpret_cast<SessionManager*>( rParam );

		//--- Extract userInfo from message ---//
        UserInfo data;      // User information from REQ_MSG.
        Parser::parseUserInfo( data , pMsg->data , ' ' );
        
		//--- Result from redis HMGET command ---//
		const string& result = UserRedis::getInstance()->hmgetUserInfo( data );

		// *** TODO : 해당 Room의 세션포인터 세팅 ---//

		
	}
	void resMakeRoom( void* lParam , void* rParam )
	{
		Packet* pMsg = reinterpret_cast<Packet*>( lParam );

		pMsg->head.type = PACKET_TYPE::RES;
		pMsg->head.len = 0;
		
		RoomSchema room;
		parseRoomInfo( room , pMsg->data , ',' );

		string result = "";

		if( UserRedis::getInstance()->hmsetNewRoom( room ) == true )
			result = UserRedis::getInstance()->hmgetRoom( room.id );

		//--- Set response packet ---//
		pMsg->setPacket( PACKET_TYPE::RES , FUNCTION_CODE::RES_MAKE_ROOM_SUCCESS , result.length() , pMsg->head.sessionID , result.c_str() );

        //--- CASE : The request is INVALID ---//
        if( result == "NULL" )
        {
            pMsg->head.func = FUNCTION_CODE::RES_ROOM_LIST_FAIL;
        }

       
	}


	void registerHandler( std::map  <
                                        int , function< void(void*,void*) > 
                                    >& h_map )
    {

        h_map.insert( std::make_pair( (int)FUNCTION_CODE::REQ_ENTER_LOBBY , 
                                        [this](void* l , void* r) 
                                        { 
                                            this->resEnterRoom(l,r); 
                                        } 
                                    ) 
                    );
		h_map.insert( std::make_pair( (int)FUNCTION_CODE::REQ_JOIN_GAME , 
										[this](void* l , void* r) 
										{ 
											this->resJoinGame(l,r); 
										} 
									) 
					);

		h_map.insert( std::make_pair( (int)FUNCTION_CODE::REQ_ROOM_LIST , 
										[this](void* l , void* r) 
										{ 
											this->resRoomList(l,r); 
										} 
									) 
					);

		h_map.insert( std::make_pair( (int)FUNCTION_CODE::REQ_MAKE_ROOM , 
                                        [this](void* l , void* r) 
                                        { 
                                            this->resMakeRoom(l,r); 
                                        } 
                                    ) 
                    );
        //h_map[VERIFY] = [this](void* l,void* r) {this->verifyUserInfo(l,r);};
    }
};

#endif
