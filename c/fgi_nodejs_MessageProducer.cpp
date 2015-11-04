// Module -------------------------------------------------------------
//
//	MESSAGEPRODUCER.CPP
//	(c) 2014 Able, Sharing Innovation
//
// Author:
//	Remko Duppen
//
// --------------------------------------------------------------------

// Description --------------------------------------------------------
//
//	A MessageProducer sends messages to the Fintegrator message bus
//
// --------------------------------------------------------------------

#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include <nan.h>

#include <iostream>

#include "fgi/nodejs/MessageProducer.hpp"
#include "fgi/nodejs/Message.hpp"
#include <cmf/TMessage.hpp>
#include <cmf/types>


using namespace v8;


namespace fgi {
    namespace nodejs {
        
        Persistent<Function> MessageProducer::constructor;


        MessageProducer::MessageProducer() 
        {
        }

        void MessageProducer::setFgiProducer( cmf::TMessageProducer *theProducer )
        {
            m_producer.reset( theProducer );
        }

        void MessageProducer::setDestination( const std::string &theDestination )
        {
            m_destination = theDestination;
        }

        MessageProducer::~MessageProducer() {
        }

        void MessageProducer::Init( v8::Handle<v8::Object> exports ) 
        {
            Local<FunctionTemplate> tpl = FunctionTemplate::New( New ) ;
            tpl->SetClassName( NanNew( "FgiMessageProducer" ) );
            tpl->InstanceTemplate()->SetInternalFieldCount( 1 );
            NODE_SET_PROTOTYPE_METHOD( tpl, "send", Send );
            NanAssignPersistent( constructor, tpl->GetFunction() );
            exports->Set( NanNew( "FgiMessageProducer"), constructor );
        }

        NAN_METHOD( MessageProducer::New ) 
        {
            NanScope();
            if ( args.IsConstructCall() ) {
                MessageProducer* obj = new MessageProducer();
                obj->Wrap( args.This() );
                NanReturnValue( args.This() );
            } else {
                const int argc = 1;
                Local<Value> argv[argc] = { args[0] };
                NanReturnValue( constructor->NewInstance( argc, argv ) );
            }
        }

        
        // Function //
        NAN_METHOD( MessageProducer::Send ) 
        // Description ---------------------------------------------------
        //     sends a message
        // ---------------------------------------------------------------
        {
            NanScope();
            try {
                MessageProducer* producer = ObjectWrap::Unwrap<MessageProducer>( args.This() );
                Local<Object> object = args[0]->ToObject();               
                std::auto_ptr<cmf::TMessage> msg( Message::create( object, producer->m_destination ) );
                producer->m_producer->send( *msg.get() );
                object->Set( Message::OID, NanNew( msg->getOID().c_str() ) );
                object->Set( Message::PRIORITY, Number::New( *msg->getPriority() ) );
                object->Set( Message::DESTINATION, NanNew( producer->m_destination.c_str() ) );
                NanReturnValue( object );
            }
            catch ( CORBA::UserException &e ) {
                return NanThrowError( e._name() );
            }
            catch ( CORBA::SystemException &e ) {
                return NanThrowError( e._name() );
            }
        }
        // End function //
    }
}

