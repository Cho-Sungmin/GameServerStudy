#ifndef MESSAGE_PROCESSOR_H
#define MESSAGE_PROCESSOR_H

#include <map>
#include <memory>
#include <functional>

#include "MessageQueue.h"
#include "TCP.h"
#include "Debug.h"

using namespace std;

class MessageProcessor {
	MessageQueue &m_msgQ;
	map<int , function<void(void**,void**)>> m_hMap;
	
public:
	//--- Constructor ---//
	MessageProcessor() = default;
	MessageProcessor( MessageQueue &queue )
			: 	m_msgQ( queue )
	{
		// init something //
	}
	
	template <typename T>
	void registerProcedure( T &obj )
	{
		obj.registerHandler( m_hMap );	
	}
	
	void processMSG( void **inParams=nullptr , void **outParams=nullptr )
	{
		unique_ptr<InputByteStream> ibstream;
		Header header;
		SessionManager *pSessionMgr = static_cast<SessionManager*>(inParams[0]);
		list<RoomManager> *pRoomList = static_cast<list<RoomManager>*>(inParams[1]);

		try {	
			m_msgQ.dequeue( ibstream );
			header.read( *ibstream );
			ibstream->reUse();

			void *outparameters[] = { ibstream.get() };
			void **inparameters = inParams;

			auto itr = m_hMap.find( header.func );
			if( itr != m_hMap.end() )
			{
				itr->second( inparameters , outparameters );
			}

			//--- 요청에 대한 응답이 있는 경우 ---//
			if( ibstream->getRemainLength() > 0 )
			{
				TCP::send_packet( header.sessionId , *ibstream );
				LOG::getInstance()->writeLOG( *ibstream , LOG::TYPE::SEND );			
			}
		}
		catch( Empty_Ex &e ) {
		}
		catch( TCP::Transmission_Ex &e ) {
		}
		catch( TCP::TCP_Ex &e )
		{
			throw;
		}
		catch( out_of_range &e )
		{
		}

	}
};



#endif
