// Header -------------------------------------------------------------
//
//	CONNECTION.HPP
//	(c) 2014 Able, Sharing Innovation
//
// Author:
//	Remko Duppen
//
// --------------------------------------------------------------------

// Description --------------------------------------------------------
//
//	A Connection to the Fintegrator Message Bus
//  Used to create Session objects
//
// --------------------------------------------------------------------

#ifndef __FGI_NODEJS_CONNECTION_HPP__
#define __FGI_NODEJS_CONNECTION_HPP__

#include <nan.h>
#include <vector>

#include <cmf/TConnection.hpp>

namespace fgi {
    namespace nodejs {
        class Session;

        class Connection : public node::ObjectWrap {
        public:
            static void Init( v8::Handle<v8::Object> exports );
            static NAN_METHOD( NewInstance );  
            void acquire( void );
        protected:        
            void release( void );
                
            friend class Session;
        private:
            explicit Connection( const std::string &url,
                                 const std::string &user,
                                 const std::string &password );
            ~Connection();

            static NAN_METHOD( New );
            static NAN_METHOD( CreateSession );
            static NAN_METHOD( Close );
            
            static v8::Persistent<v8::Function> constructor;
            
            void close();
            void addSession( Session *session );
            
            cmf::TConnection m_connection;
            bool m_bConnected;
            std::string m_strFailureReason;
            typedef std::vector<Session*> sessions_t;
            sessions_t m_sessions;
        };
    }
}

#endif
// End header //