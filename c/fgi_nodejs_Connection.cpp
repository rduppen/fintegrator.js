// Module -------------------------------------------------------------
//
//	CONNECTION.CPP
//	(c) 2014 Able, Sharing Innovation
//
// Author:
//	Remko Duppen
//
// --------------------------------------------------------------------

// Description --------------------------------------------------------
//
//	A Connection to the Fintegrator message bus
//  Factory for Session objects
//
// --------------------------------------------------------------------

#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include <nan.h>

#include <iostream>
#include <sstream>

#include "fgi/nodejs/Connection.hpp"
#include "fgi/nodejs/Session.hpp"

#include "CMF_c.hh"
#include "Route_c.hh"

using namespace v8;

CORBA::ORB_ptr getOrb( void );

namespace {
    CMF::Dispatcher_var s_dispatcher = CMF::Dispatcher::_nil();

    CMF::Dispatcher_var getDispatcher( const std::string &url ) {
        if ( CORBA::is_nil( s_dispatcher ) ) {
            try {
                CORBA::Object_ptr object = getOrb()->string_to_object( url.c_str() );
                if ( !CORBA::is_nil( object ) ) {
                    CMF::Master_var master = CMF::Master::_narrow( object );
                    if ( !CORBA::is_nil( master ) ) {
                        s_dispatcher = master->getDispatcher();
                    } else {
                        throw std::runtime_error( "Located Servant is not a Dispatcher::Master" );
                    }
                } else {
                    throw std::runtime_error( "Dispatcher::Master unreachable" );
                }
            }
            catch ( const CORBA::UserException &e ) {
                std::ostringstream os;
                os << "exception caught: " << e._name();
                throw std::runtime_error( os.str() );
            }
            catch ( const CORBA::SystemException &e ) {
                std::ostringstream os;
                os << "exception caught: " << e._name();
                throw std::runtime_error( os.str() );
            }
        }
        return s_dispatcher;
    }

}

namespace fgi {
    namespace nodejs {
    


        Persistent<Function> Connection::constructor;


        Connection::Connection( const std::string &url, 
                                const std::string &user, 
                                const std::string &password )
        {
            try {
                m_connection = getDispatcher( url )->createConnection( user.c_str(), password.c_str() );
                m_bConnected = true;
            }
            catch ( const std::runtime_error &e )  {
                m_bConnected = false;
                m_strFailureReason = e.what();
            }                
        }   

        Connection::~Connection() {
        }

        void Connection::Init(  v8::Handle<v8::Object> exports ) 
        {
            Local<FunctionTemplate> tpl = FunctionTemplate::New( New) ;
            tpl->SetClassName( NanNew( "FgiConnection" ) );
            tpl->InstanceTemplate()->SetInternalFieldCount( 2 );
            NODE_SET_PROTOTYPE_METHOD( tpl, "createSession", CreateSession );
            NODE_SET_PROTOTYPE_METHOD( tpl, "close", Close );
            
            NanAssignPersistent( constructor, tpl->GetFunction() );
            exports->Set( NanNew( "FgiConnection"), constructor );
        }

        
        NAN_METHOD( Connection::New ) 
        {            
            NanScope();
            if ( args.Length() == 0 || args[0]->IsUndefined() || !args[0]->IsString() ) {
                return NanThrowError( "server url expected" );
            }
            std::string url = *NanAsciiString( args[0] );    
            std::string user = "anonymous";
            if ( args.Length() > 1 && !args[1]->IsUndefined() && args[1]->IsString() ) {
                user = *NanAsciiString( args[1] );
            }
            std::string password = "";
            if ( args.Length() > 2 && !args[2]->IsUndefined() && args[2]->IsString() ) {
                password = *NanAsciiString( args[2] );
            }
            if ( args.IsConstructCall() ) {
                Connection* obj = new Connection( url, user, password );
                if ( obj->m_bConnected ) {
                    obj->Wrap( args.This() );
                    NanReturnValue( args.This() );
                } else {
                    std::string message = "connection failed, reason: " + obj->m_strFailureReason;
                    return NanThrowError( message.c_str() );
                }
            } else {
                const int argc = 1;
                Local<Value> argv[ argc ] = { String::New( url.c_str() ) };
                NanReturnValue( constructor->NewInstance( argc, argv ) );
            }
        }

        NAN_METHOD( Connection::NewInstance ) {
            NanScope();
            const unsigned argc = 1;
            Handle<Value> argv[ argc ] = { args[0] };  
            Local<Object> instance = constructor->NewInstance( argc, argv );
            NanReturnValue( instance );
        }

        NAN_METHOD( Connection::CreateSession ) 
        {
            NanScope();
            Handle<Value> obj = Session::New( args );            
            Session *session = ObjectWrap::Unwrap<Session>( (*obj)->ToObject() );
            
            Connection* connection = ObjectWrap::Unwrap<Connection>( args.This() );
            connection->addSession( session );
            bool bTransactional = true;
            if ( args.Length() > 0 && !args[0]->IsUndefined() && args[0]->IsBoolean() ) {
                bTransactional = args[0]->ToBoolean()->Value();
            }
            bool bAutoAck = false;
            if ( args.Length() > 1 && !args[1]->IsUndefined() && args[1]->IsBoolean() ) {
                bTransactional = args[1]->ToBoolean()->Value();
            }            
            session->createFgiSession( connection,
                                       bTransactional,
                                       bAutoAck );
            NanReturnValue( obj );
        }
        
        NAN_METHOD( Connection::Close ) 
        {
            NanScope();
            Connection* connection = ObjectWrap::Unwrap<Connection>( args.This() );
            connection->close();
            NanReturnUndefined();
        }
        
        void Connection::close() 
        {
            for ( sessions_t::iterator itr = m_sessions.begin(); itr != m_sessions.end(); itr++ ) {                
                (*itr)->release();
            }            
            // TODO - just calling Unref() doesn't guarantee that the connection will be garbage 
            //        collected, we might want to implement this differently
            release();
            // release again to get rid of the reference from createConnection()
            release();
        }
        
        void Connection::addSession( Session *theSession ) 
        {
            m_sessions.push_back( theSession );
        }
            
        void Connection::acquire( void ) {
            Ref();
        }
        
        void Connection::release( void ) {
            if ( refs_ > 0 ) {
                Unref();
            }
        }
    }
    
}

// End module //