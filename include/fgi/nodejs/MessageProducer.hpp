// Header -------------------------------------------------------------
//
//	MESSAGEPRODUCER.HPP
//	(c) 2014 Able, Sharing Innovation
//
// Author:
//	Remko Duppen
//
// --------------------------------------------------------------------

// Description --------------------------------------------------------
//
//	A MessageProducer sends Fintegrator messages
//
// --------------------------------------------------------------------

#ifndef _FGI_NODEJS_PRODUCER_HPP__
#define _FGI_NODEJS_PRODUCER_HPP__

#include <nan.h>
#include <string>
#include <memory>
#include <cmf/TSession.hpp>

namespace cmf {
    class TConnection;
}


namespace fgi { 
    namespace nodejs {

        class MessageProducer : public node::ObjectWrap {
        public:
            static void Init( v8::Handle<v8::Object> exports );
        protected:
            void setFgiProducer( cmf::TMessageProducer *theProducer );
            void setDestination( const std::string &theDestination );
            static NAN_METHOD( New );
            // Session is the factory for this class
            friend class Session;
        private:  
            static NAN_METHOD( Send );
            
            explicit MessageProducer();
            ~MessageProducer();

            void setConnection( const cmf::TConnection &theConnection );

            static v8::Persistent<v8::Function> constructor;
            std::auto_ptr<cmf::TMessageProducer> m_producer;  
            std::string m_destination;
        };
    }
}    

#endif

// End header //