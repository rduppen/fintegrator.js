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

#include <iostream>

#include <cmf/TConnection.hpp>
#include "fgi/nodejs/Connection.hpp"
#include "fgi/nodejs/Session.hpp"
#include "fgi/nodejs/MessageProducer.hpp"
#include "fgi/nodejs/MessageConsumer.hpp"

using namespace v8;

namespace fgi {
    namespace nodejs {

        Persistent<Function> Session::constructor;

        Session::Session(  cmf::TSession *theSession ) :
            m_session( theSession ),
            m_bTransactional( false )
        {
        }

        Session::Session( void ) 
        {
        }

        void Session::createFgiSession( Connection *theConnection,
                                        bool isTransactional,
                                        bool autoAcknowledge ) 
        {
            m_mode = autoAcknowledge ? CMF::AUTO_ACKNOWLEDGE : CMF::CLIENT_ACKNOWLEDGE;
            m_connection = theConnection;
            m_connection->acquire();
#ifdef FGI_35
            m_session.reset( new cmf::TSession( theConnection->m_connection.createSession( isTransactional, m_mode ) ) );
#else        
            m_session.reset( new cmf::TSession( theConnection->m_connection.createSession( isTransactional ) ) );      
#endif
        }

        Session::~Session() {
            m_connection->release();
        }

        void Session::Init( v8::Handle<v8::Object> exports ) 
        {
            NanScope();
            Local<FunctionTemplate> tpl = FunctionTemplate::New( New ) ;
            tpl->SetClassName( NanNew( "FgiSession" ) );
            
            tpl->InstanceTemplate()->SetInternalFieldCount( 4 );
            NODE_SET_PROTOTYPE_METHOD( tpl, "createProducer", CreateProducer );
            NODE_SET_PROTOTYPE_METHOD( tpl, "createConsumer", CreateConsumer );
            NODE_SET_PROTOTYPE_METHOD( tpl, "commit", Commit );
            NODE_SET_PROTOTYPE_METHOD( tpl, "rollback", Rollback );

            NanAssignPersistent(constructor, tpl->GetFunction());
            exports->Set( NanNew( "FgiSession"), constructor );
        }

        NAN_METHOD( Session::New ) 
        {
            NanScope();
            if ( args.IsConstructCall() ) {
                Session* obj = new Session();
                obj->Wrap( args.This() );
                NanReturnValue( args.This() );
            } else {
                const int argc = 1;
                Local<Value> argv[argc] = { args[0] };
                NanReturnValue( constructor->NewInstance( argc, argv ) );
            }
        }

        
        Local<Value> handleException( const CORBA::UserException &e ) 
        {
            return NanThrowError( e._name() );
        }

        NAN_METHOD( Session::CreateProducer ) 
        {
            NanScope();
            try {
                std::string destination = *NanAsciiString( args[0] );    
                Session* session = ObjectWrap::Unwrap<Session>( args.This() );
                Handle<Value> obj = MessageProducer::New( args ); 
                MessageProducer *producer = ObjectWrap::Unwrap<MessageProducer>( obj->ToObject() );
                cmf::TDestination dest = session->m_session->createDestination( destination );
                producer->setFgiProducer( new cmf::TMessageProducer( session->m_session->createProducer( dest ) ) );
                producer->setDestination( destination );
                NanReturnValue( obj );        
            }
            catch ( const CORBA::UserException &e ) {
                return handleException( e );
            }
            catch ( const CORBA::SystemException &e ) {
                return NanThrowError( e._name() );
            }
        }

        NAN_METHOD( Session::CreateConsumer ) 
        {       
            NanScope();
            if ( args.Length() == 0 || args[0]->IsUndefined() || !args[0]->IsString() ) {
                return NanThrowError( "No destination specified" );
            }
            try {
                std::string destination = *NanAsciiString( args[0] );    
                Session* session = ObjectWrap::Unwrap<Session>( args.This() );
                Handle<Value> obj = MessageConsumer::New( args ); 
                MessageConsumer *consumer = ObjectWrap::Unwrap<MessageConsumer>( obj->ToObject() );
                cmf::TDestination dest = session->m_session->createDestination( destination );
                consumer->setFgiConsumer( new cmf::TMessageConsumer( session->m_session->createConsumer( dest ) ) );
                consumer->setSession( session );
                consumer->setDestination( destination );
                consumer->setTransactional( session->m_session->isTransacted() );
                consumer->setShouldAcknowledge( session->m_mode == CMF::CLIENT_ACKNOWLEDGE );
                NanReturnValue( obj );        
            }
            catch ( const CORBA::UserException &e ) {
                return NanThrowError( e._name() );
            }
            catch ( const CORBA::SystemException &e ) {
                return NanThrowError( e._name() );
            }
        }
        
        NAN_METHOD( Session::Commit ) 
        {
            NanScope();
            try {
                Session* session = ObjectWrap::Unwrap<Session>( args.This() );
                session->m_session->commit();                
                NanReturnUndefined();
            }
            catch ( const CORBA::UserException &e ) {
                return NanThrowError( e._name() );
            }
            catch ( const CORBA::SystemException &e ) {
                return NanThrowError( e._name() );
            }
        }
        
        NAN_METHOD( Session::Rollback ) 
        {
            NanScope();
            try {
                Session* session = ObjectWrap::Unwrap<Session>( args.This() );
                session->m_session->rollback();                
                NanReturnUndefined();
            }
            catch ( const CORBA::UserException &e ) {
                return NanThrowError( e._name() );
            }
            catch ( const CORBA::SystemException &e ) {
                return NanThrowError( e._name() );
            }
        }
        
        void Session::acquire( void ) 
        {
            Ref();
        }
        
        void Session::release( void ) 
        {
            if ( refs_ > 0 ) {
                Unref();
            }
        }

    }}    

// End module //
