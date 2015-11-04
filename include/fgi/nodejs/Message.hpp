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
//  Message object constants
//
// --------------------------------------------------------------------

#ifndef __FGI_NODEJS_MESSAGE_HPP__
#define __FGI_NODEJS_MESSAGE_HPP__

#include <v8.h>

namespace cmf {
    class TMessage;
}

namespace fgi {

    namespace nodejs {

        class Message {
        public:
            static v8::Persistent<v8::String> MESSAGE;
            static v8::Persistent<v8::String> OID;
            static v8::Persistent<v8::String> PRIORITY;
            static v8::Persistent<v8::String> TYPE;
            static v8::Persistent<v8::String> DESTINATION;
            static v8::Persistent<v8::String> PROPERTIES;            
            
            static v8::Local<v8::Object> create( cmf::TMessage *theMessage );
            static cmf::TMessage *create( v8::Local<v8::Object> theObject,
                                          const std::string &theDestination );

        };
    
    }
}



#endif

// End module //