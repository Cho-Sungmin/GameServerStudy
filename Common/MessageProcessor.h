#ifndef MESSAGE_PROCESSOR_H
#define MESSAGE_PROCESSOR_H

#include <map>
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
		InputByteStream msg;
		Header header;

		void *outparameters[] = { &msg };
		void **inparameters = inParams;

		try {
			m_msgQ.dequeue( msg );
			header.read( msg );
			msg.reUse();

			auto itr = m_hMap.find( header.func );
			if( itr != m_hMap.end() )
			{
				itr->second( inparameters , outparameters );
			}

			if( msg.getRemainLength() > 0 )
			{
				TCP::send_packet( header.sessionId , msg );
				LOG::getInstance()->writeLOG( msg , LOG::TYPE::SEND );		
			}
			msg.close();
		}
		catch( Empty_Ex e ) {
		}
		catch( TCP::Transmission_Ex e ) {
			msg.close();
		}
		catch( TCP::Connection_Ex e ) {
			msg.close();
			throw e;
		}

	}
};



#endif
