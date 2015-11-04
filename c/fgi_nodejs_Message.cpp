// Module -------------------------------------------------------------
//
//	MESSAGE.CPP
//	(c) 2014 Able, Sharing Innovation
//
// Author:
//	Remko Duppen
//
// --------------------------------------------------------------------

// Description --------------------------------------------------------
//
// Constants for the message object
//
// --------------------------------------------------------------------

#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include <node.h>
#include <nan.h>

#include "fgi/nodejs/Message.hpp"
#include <cmf/TMessage.hpp>
#include <cmf/types>

using namespace v8;


namespace fgi {
    namespace nodejs {
        const char *s_keyMessage     = "message";
        const char *s_keyOid         = "oid";
        const char *s_keyPriority    = "priority";
        const char *s_keyType        = "type";
        const char *s_keyDestination = "destination";
        const char *s_keyProperties  = "properties";
    
        Persistent<String> Message::MESSAGE     = Persistent<String>::New( NanNew( s_keyMessage ) );
        Persistent<String> Message::OID         = Persistent<String>::New( NanNew( s_keyOid ) );
        Persistent<String> Message::PRIORITY    = Persistent<String>::New( NanNew( s_keyPriority ) );
        Persistent<String> Message::TYPE        = Persistent<String>::New( NanNew( s_keyType ) );
        Persistent<String> Message::DESTINATION = Persistent<String>::New( NanNew( s_keyDestination ) );
        Persistent<String> Message::PROPERTIES  = Persistent<String>::New( NanNew( s_keyProperties ) );
        
        
        Local<Object> Message::create( cmf::TMessage *msg ) 
        // Description ------------------------------------------------------------------------------
        //     Marshals a cmf::TMessage to a nodejs message object
        // ------------------------------------------------------------------------------------------
        {
            Local<Object> result = Object::New();
            result->Set( MESSAGE, NanNew( msg->getMessage() ) );
            result->Set( OID, NanNew( msg->getOID().c_str() ) );
            result->Set( TYPE, NanNew( msg->getJmsType().c_str() ) );
            result->Set( PRIORITY, NanNew( static_cast<unsigned int>(  *msg->getPriority() ) ) );
            result->Set( DESTINATION, NanNew( msg->getConsumer().c_str() ) );
            if ( !msg->getProperties().empty() ) {
                Local<Object> properties = Object::New();
                for ( cmf::TMessage::properties_t::const_iterator itr = msg->getProperties().begin(); itr != msg->getProperties().end(); itr++ ) {
                    properties->Set( NanNew( itr->first.c_str() ), NanNew( itr->second.c_str() ) );
                }
                result->Set( Message::PROPERTIES, properties );
            }
            return result;
        }
        
        // Function //
        cmf::TMessage *Message::create( Local<Object> object, const std::string &theDestination ) 
        // Description ------------------------------------------------------------------------------
        //     Marshals a nodejs message object to a cmf::TMessage
        // ------------------------------------------------------------------------------------------
        {
            long prio = static_cast<long>( NanUInt32OptionValue( object, Message::PRIORITY, 5 ) );           
            std::auto_ptr<cmf::TMessage> msg( new cmf::TMessage( "NodeJs",
                                                                 theDestination,
                                                                 *NanAsciiString( object->Get( Message::MESSAGE ) ),
                                                                 cmf::getCmfType( *NanAsciiString( object->Get( Message::TYPE ) ) ),
                                                                 prio,
                                                                 "ERRORS" ) );        
            Local<Value> properties = object->Get( Message::PROPERTIES );
            if ( !properties->IsUndefined() && properties->IsObject() ) {
                Local<Object> p = properties->ToObject();
                Local<Array> names = p->GetPropertyNames();
                for ( uint32_t i = 0; i < names->Length(); i++ ) {
                    msg->addProperty( *NanAsciiString( names->Get( i ) ), *NanAsciiString( p->Get( names->Get( i ) ) ) );
                }
            }
            return msg.release();
        }
        // End function //


    }
}

// End module