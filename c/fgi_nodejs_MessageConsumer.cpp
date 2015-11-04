// Module -------------------------------------------------------------
//
//	SESSION.CPP
//	(c) 2014 Able, Sharing Innovation
//
// Author:
//	Remko Duppen
//
// --------------------------------------------------------------------

// Description --------------------------------------------------------
//
//	A Session is the factory for MessageConsumers and MessageProducers
//  it's also the scope for transactions
//
// --------------------------------------------------------------------

#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include <nan.h>
#include <node.h>

#include <iostream>

#include "fgi/nodejs/MessageConsumer.hpp"
#include "fgi/nodejs/Session.hpp"
#include "fgi/nodejs/Message.hpp"
#include <cmf/TMessage.hpp>
#include <cmf/exception>
#include <cmf/types>
#include <utils/task>
#include <ace/OS.h>

using namespace v8;


namespace fgi {
    namespace nodejs {
    
        namespace {
            static unsigned int s_consumerId = 0;
        }
        
        
        Persistent<Function> MessageConsumer::constructor;

        MessageConsumer::MessageConsumer():
            m_thread( *this ),
            m_id ( s_consumerId++ ),
            m_bClosed( false )
        {
        }

        void MessageConsumer::setFgiConsumer( cmf::TMessageConsumer *theConsumer )
        {
            m_consumer.reset( theConsumer );
        }

        void MessageConsumer::setDestination( const std::string &theDestination )
        {
            m_destination = theDestination;
        }
        
        void MessageConsumer::setTransactional( bool isTransactional ) 
        {
            m_bIsTransactional = isTransactional;
        }
        
        void MessageConsumer::setShouldAcknowledge( bool ack ) 
        {
            m_bShouldAcknowledge = ack;
        }
        
        void MessageConsumer::setSession( Session *session ) 
        {
            m_session = session;
            m_session->acquire();
        }
        

        MessageConsumer::~MessageConsumer() {
            m_session->release();
        }

        void MessageConsumer::Init( v8::Handle<v8::Object> exports ) {
            NanScope();
            Local<FunctionTemplate> tpl = FunctionTemplate::New( New ) ;
            tpl->SetClassName( NanNew( "FgiMessageConsumer" ) );
            tpl->InstanceTemplate()->SetInternalFieldCount( 5 );
          
            NODE_SET_PROTOTYPE_METHOD( tpl, "receive", Receive );
            NODE_SET_PROTOTYPE_METHOD( tpl, "onMessage", OnMessage );
            NODE_SET_PROTOTYPE_METHOD( tpl, "acknowledge", Acknowledge );
            NODE_SET_PROTOTYPE_METHOD( tpl, "close", Close );
            NODE_SET_PROTOTYPE_METHOD( tpl, "getId", GetId );

            NanAssignPersistent(constructor, tpl->GetFunction());
            exports->Set( NanNew( "FgiMessageConsumer"), constructor );
        }

        NAN_METHOD( MessageConsumer::New ) {
            NanScope();
            if ( args.IsConstructCall() ) {
                MessageConsumer* obj = new MessageConsumer();
                obj->Wrap( args.This() );
                NanReturnValue( args.This() );
            } else {
                const int argc = 1;
                Local<Value> argv[argc] = { args[0] };
                NanReturnValue( constructor->NewInstance( argc, argv ) );
            }
        }
        

        NAN_METHOD( MessageConsumer::Receive ) 
        {
            try {
                NanScope();
                MessageConsumer* consumer = ObjectWrap::Unwrap<MessageConsumer>( args.This() );
                long timeout = 5000;
                if ( args.Length() > 0 && !args[0]->IsUndefined() ) {
                    timeout = args[0]->ToUint32()->Value();                                
                }
                std::auto_ptr<cmf::TMessage> msg( consumer->m_consumer->receive( timeout ) );
                if ( msg.get() != NULL ) {
                    NanReturnValue( Message::create( msg.get() ) );
                } else {
                    NanReturnNull();
                }
            }
            catch ( CORBA::UserException &e ) {
                return NanThrowError( e._name() );
            }
            catch ( CORBA::SystemException &e ) {
                return NanThrowError( e._name() );
            }
        }
        
        NAN_METHOD( MessageConsumer::Acknowledge )
        {
            NanScope();
            MessageConsumer* consumer = ObjectWrap::Unwrap<MessageConsumer>( args.This() );
            if ( consumer->shouldAcknowledge() ) {
                if ( args.Length() > 0 && !args[0]->IsUndefined() ) {
                    Local<Object> msg = args[0]->ToObject();
                    std::string oid = *NanAsciiString( msg->Get( Message::OID ) );
                    try {
                        consumer->m_consumer->acknowledge( oid ); 
                    }
                    catch ( const CORBA::UserException &e ) {
                        return NanThrowError( e._name() );
                    }
                    catch ( const CORBA::SystemException &e ) {
                        return NanThrowError( e._name() );
                    }
                    consumer->m_thread.release();
                }
            }
            NanReturnUndefined();
        }
        
        Handle<Object> MessageConsumer::getListener( void ) {
            return m_listener;
        }
        
        void MessageConsumer::setListener( Handle<Value> theListener ) 
        {
            m_listener = Persistent<Function>::New( Handle<Function>::Cast( theListener ) );
        }


        
        void MessageConsumer::onMessageHandler( uv_async_t *handle, int status ) 
        {
            NanScope();
            MessageConsumer *consumer = static_cast<MessageConsumer*>( handle->data );
            if ( consumer != NULL && !consumer->m_bClosed ) {
                std::auto_ptr<cmf::TMessage> msg( consumer->m_thread.getMessage() );
                Handle<Function> cb =  Handle<Function>::Cast( consumer->getListener() );            
                Handle<Value> argv[1] = { Message::create( msg.get() ) };
                cb->Call( Context::GetCurrent()->Global(), 1, argv );                     
            } else {
                Handle<Function> cb =  Handle<Function>::Cast( consumer->getListener() );            
                Handle<Value> argv[1] = { NanNew( "Consumer closed"  ) };
                cb->Call( Context::GetCurrent()->Global(), 1, argv );                                     
                consumer->Unref();
            }
        }
        
        
       
        // Constructor //
        MessageConsumer::thread_t::thread_t( MessageConsumer &theParent ):
            m_parent( theParent )
            // Description ----------------------------------------
            //
            // ----------------------------------------------------
        {
        
        }
        // End function //

        // Function //
        int MessageConsumer::thread_t::service() 
        {
            try {
                m_semaphore.acquire();
                while ( !m_parent.m_bClosed ) {
                    m_message = m_parent.m_consumer->receive( 1000 );
                    if ( m_message != NULL && !m_parent.m_bClosed ) {
                        m_parent.m_async.data = &m_parent;
                        uv_async_send( &m_parent.m_async );
                        if ( m_parent.shouldAcknowledge() ) {
                            // wait for the message handler to ack the message
                            // before attempting to read the next message
                            m_semaphore.acquire();
                        }
                    }
                }
            }
            catch ( ... ) {
            }
            return 0;
        }
        // End function //
        
        // Function //
        NAN_METHOD( MessageConsumer::OnMessage ) 
        // Description -----------------------------------------------------------------
        //     Registers a nodejs function as listener 
        // -----------------------------------------------------------------------------
        {
            NanScope();
            MessageConsumer* consumer = ObjectWrap::Unwrap<MessageConsumer>( args.This() );
            if ( consumer->m_bIsTransactional ) {
                return NanThrowError( "onMessage() can not be used with a transactional session" );
            }
            if ( args.Length() != 1 || !args[0]->IsFunction() ) {
                return NanThrowError( "A callback function expected" );
            }
            uv_async_init( uv_default_loop(), &consumer->m_async, MessageConsumer::onMessageHandler );
            consumer->setListener( args[0]->ToObject() );
            consumer->m_thread.activate();
            NanReturnUndefined();
        }
        // End function //
        
        NAN_METHOD( MessageConsumer::Close ) 
        {
            NanScope();
            MessageConsumer* consumer = ObjectWrap::Unwrap<MessageConsumer>( args.This() );
            consumer->close();
            NanReturnUndefined();
        }
        
        NAN_METHOD( MessageConsumer::GetId )
        {
            NanScope();
            MessageConsumer* consumer = ObjectWrap::Unwrap<MessageConsumer>( args.This() );
            NanReturnValue( NanNew<Number>( consumer->m_id ) );
        }
        
    }
    
}

// End module //