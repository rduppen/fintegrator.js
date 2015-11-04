// Module -----------------------------------------------------
//     
//    FINTEGRATOR.CPP
//    Fintegrator addon for Nodejs
//    (c) 2014 Able, Sharing Innovation
//
// Author:
//    Remko Duppen
// ------------------------------------------------------------

#include <nan.h>
#include <v8.h>
#include <node.h>


#include <cmf/TMessage.hpp>

#include "fgi/nodejs/Connection.hpp"
#include "fgi/nodejs/Session.hpp"
#include "fgi/nodejs/MessageProducer.hpp"
#include "fgi/nodejs/MessageConsumer.hpp"

#include "CMF_c.hh"
#include <ace/ARGV.h>
#include <ace/CORBA_macros.h>
#include <tao/TAO_Internal.h>

CORBA::ORB_ptr s_orb;

void orb_init( void )
{
        TAO::ORB::default_svc_conf_entries( NULL,
                                            "static Server_Strategy_Factory \"-ORBConcurrency reactive\"", // thread-per-connection\"",
                                            "static Client_Strategy_Factory \"-ORBTransportMuxStrategy EXCLUSIVE\"" );
//                                            -ORBClientConnectionHandler RW\"" );
        ACE_ARGV corba_args;
//        corba_args.add( "-ORBDebug" );
        int iArgs = corba_args.argc();
        s_orb = CORBA::ORB_init( iArgs, corba_args.argv() );              
}


CORBA::ORB_ptr getOrb() 
{
    return s_orb;
}




// Function //
v8::Handle<v8::Value> CreateConnection( const v8::Arguments& args ) 
// Description -----------------------------------------------------
//     Factory method for connections
// -----------------------------------------------------------------
{
    NanScope();
    v8::Handle<v8::Value> retval = fgi::nodejs::Connection::NewInstance( args );
    fgi::nodejs::Connection* connection = node::ObjectWrap::Unwrap<fgi::nodejs::Connection>( retval->ToObject() );
    connection->acquire();
    NanReturnValue( retval );
}
// End function //

// Function //
void InitAll( v8::Handle<v8::Object> exports, v8::Handle<v8::Object> module ) 
// Description --------------------------------------------------------------------
//     Registers the objects and the factory method
// --------------------------------------------------------------------------------
{
    using namespace fgi::nodejs;
    orb_init();
    Connection::Init( exports );
    Session::Init( exports );
    MessageProducer::Init( exports );
    MessageConsumer::Init( exports );
    exports->Set( v8::String::NewSymbol( "createConnection" ),
        v8::FunctionTemplate::New( CreateConnection )->GetFunction() );
}
// End function //

NODE_MODULE( fintegrator, InitAll )

// End module //