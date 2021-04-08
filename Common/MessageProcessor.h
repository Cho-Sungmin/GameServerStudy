#ifndef MESSAGE_PROCESSOR_H
#define MESSAGE_PROCESSOR_H

#include "MessageQueue.h"
#include "TCP.h"
#include <map>
#include <functional>
#include "Debug.h"


using namespace std;

LOG* LOG::pInstance;

class MessageProcessor {

	MessageQueue& 							m_msgQ	;
	map<int,function<void(void*,void*)>> 	m_hMap	;

	const string 	m_logFileName;
	LOG* 			m_pLog;
	

public:
	
	//--- Constructor ---//

	MessageProcessor() = default;
		
	MessageProcessor( MessageQueue& queue , const string& fileName )
			: 	m_msgQ( queue ),
				m_logFileName( fileName )
	{
		// init something //
		m_pLog = LOG::getInstance( m_logFileName );
	}
	
	template <typename T>
	void registerProcedure( T& obj )
	{
		//m_hMap.insert( std::make_pair( key , procedure ) );
		obj.registerHandler( m_hMap );	
	}

	void processMSG( void* lParam = nullptr , void* rParam  = nullptr )
	{
		Packet msg;

		try {

			m_msgQ.dequeue( msg );
			auto itr = m_hMap.find( msg.head.func );

			if( itr != m_hMap.end() )
				itr->second( &msg , lParam );

			TCP::send_packet<Packet>( msg.head.sessionID , msg );
			m_pLog->writeLOG( msg , LOG::TYPE::SEND );
		}
		catch( Empty_Ex e ) {
			//std::cout << e.what() << std::endl;
		}
		catch( TCP::Transmission_Ex e ) {
			cout << e.what() << endl;
		}

		
	}

};

#endif
