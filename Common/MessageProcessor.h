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

	unique_ptr<InputByteStream> m_ibstream;
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
		Header header;

		try {	
			m_msgQ.dequeue( m_ibstream );
			header.read( *m_ibstream );
			m_ibstream->reUse();

			void *outparameters[] = { m_ibstream.get() };
			void **inparameters = inParams;

			auto itr = m_hMap.find( header.func );
			if( itr != m_hMap.end() )
			{
				itr->second( inparameters , outparameters );
			}

			if( m_ibstream->getRemainLength() > 0 )
			{
				TCP::send_packet( header.sessionId , *m_ibstream );
				LOG::getInstance()->writeLOG( *m_ibstream , LOG::TYPE::SEND );		
			}
			m_ibstream->reUse();
		}
		catch( Empty_Ex e ) {
		}
		catch( TCP::Transmission_Ex e ) {
			m_ibstream->flush();
		}
		catch( TCP::Connection_Ex e ) {
			m_ibstream->flush();
			throw e;
		}

	}
};



#endif
