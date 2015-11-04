// Header -------------------------------------------------------------
//
//	SESSION.HPP
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

#ifndef __FGI_NODEJS_SESSION_HPP__
#define __FGI_NODEJS_SESSION_HPP__

#include <node.h>
#include <nan.h>


namespace cmf {
    class TSession;
    class TConnection;
}

namespace fgi {
    namespace nodejs {
    
        class Connection;

        class Session : public node::ObjectWrap {
        public:
            static void Init( v8::Handle<v8::Object> exports );
        protected:
            void createFgiSession( Connection *theConnection,
                                   bool isTransactional,
                                   bool doAutoAcknowledge );                                   
            static NAN_METHOD( New );
            
            void acquire();
            void release();
            
            friend class Connection;
            friend class MessageConsumer;
        private:
            explicit Session( cmf::TSession *theSession );
            explicit Session( void );
            ~Session();
        
        
            static NAN_METHOD( CreateProducer );
            static NAN_METHOD( CreateConsumer );
            static NAN_METHOD( Commit );
            static NAN_METHOD( Rollback );
            
            static v8::Persistent<v8::Function> constructor;
            Connection *m_connection;
            std::auto_ptr<cmf::TSession> m_session;  
            bool m_bTransactional;
            CMF::AcknowledgeMode m_mode;
        };
    }
}

#endif

// End header //