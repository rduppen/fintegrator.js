// Header -------------------------------------------------------------
//
//	MESSAGECONSUMER.HPP
//	(c) 2014 Able, Sharing Innovation
//
// Author:
//	Remko Duppen
//
// --------------------------------------------------------------------

// Description --------------------------------------------------------
//
//	A MessageConsumer receives Fintegrator messages
//
// --------------------------------------------------------------------

#ifndef __FGI_NODEJS_MESSAGECONSUMER_HPP__
#define __FGI_NODEJS_MESSAGECONSUMER_HPP__

#include <nan.h>
#include <string>
#include <memory>

#include <cmf/TMessageConsumer.hpp>
#include <utils/task>

#include <ace/Semaphore.h>

namespace cmf {
    class TConnection;
}

namespace fgi { 
    namespace nodejs {
    
        class Session;

        class MessageConsumer : public node::ObjectWrap {
        public:
            static void Init( v8::Handle<v8::Object> exports );
            
        protected:
            static NAN_METHOD( New );
            void setFgiConsumer( cmf::TMessageConsumer *theConsumer );
            void setDestination( const std::string &theDestination );
            void setTransactional( bool isTransactional );
            void setSession( Session *session ); 
            void setShouldAcknowledge( bool shouldAcknowledge );
            bool shouldAcknowledge() {
                return m_bShouldAcknowledge;
            }
            
            void close( void ) {
                m_bClosed = true;
            }

            // Session is the factory for this class
            friend class Session;
            
        private:  
            class thread_t : public utils::task<ACE_MT_SYNCH> {
            public:
                thread_t( MessageConsumer &parent );
                int service( void );
                void release() {
                    m_semaphore.release();
                }
                cmf::TMessage *getMessage() {
                    return m_message;
                }
            private:
                MessageConsumer &m_parent;
                cmf::TMessage *m_message;
                ACE_Semaphore m_semaphore;
            };
        
            static NAN_METHOD( Receive );
            static NAN_METHOD( OnMessage );
            static NAN_METHOD( Acknowledge );
            static NAN_METHOD( Close );
            static NAN_METHOD( GetId );
            
            static void onMessageHandler( uv_async_t *handle, int status );

        
            explicit MessageConsumer();
            ~MessageConsumer();
            v8::Handle<v8::Object> getListener( void );
            
            void setListener( v8::Handle<v8::Value> theListener );
            
            static v8::Persistent<v8::Function> constructor;
            std::auto_ptr<cmf::TMessageConsumer> m_consumer;  
            std::string m_destination;
            v8::Persistent<v8::Function> m_listener;
            
            thread_t m_thread;
            uv_async_t m_async;
            bool m_bIsTransactional;            
            bool m_bShouldAcknowledge;
            bool m_bClosed;
            Session *m_session;
            unsigned int m_id;
        };
    }
}    

#endif

// End header //