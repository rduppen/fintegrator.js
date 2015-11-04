//var fgi = require( './src/lib' );

var fgi = require( 'fintegrator' );


var connection = fgi.createConnection( "corbaloc:iiop:angmar.syntel.nl:16007/Master" );
var connection2 = fgi.createConnection( "corbaloc:iiop:angmar.syntel.nl:16007/Master" );

//var txSession = connection.createSession( true );
var session = connection.createSession( false );
var session2 = connection2.createSession( false );

console.log( "Session created" );

//var producer = txSession.createProducer( "TEST_QUEUE" ); 

var message = new fgi.Message();
var pi = new fgi.ProcessingInstruction( "step1", new fgi.Delivery );
pi.onSuccess = new Array();
pi.onSuccess[0] = new fgi.ProcessingInstruction( "step2-1", new fgi.Delivery );
pi.onSuccess[1] = new fgi.ProcessingInstruction( "step2-2", new fgi.Delivery );
message.routingslip = new fgi.RoutingSlip( pi ) ;


console.log( JSON.stringify( message.routingslip, true ) );
console.log( message.routingslip.toXml() );

//message.message = "The Quick Brown Fox Jumps Over The Lazy Dog";
var message = { message: 'The Quick Brown Fox Jumps Over The Lazy Dog',
                type: 'DontCare'
                };
//                priority: '5' 

//console.log( producer.send( message ) );
//console.log( producer.send( message ) );
//console.log( producer.send( message ) );
//console.log( producer.send( message ) );
//console.log( producer.send( message ) );
//console.log( producer.send( message ) );
//console.log( producer.send( message ) );
//console.log( producer.send( message ) );
//console.log( producer.send( message ) );
//txSession.commit();

var consumer = session.createConsumer( "TEST_QUEUE" );
var consumer2 = session2.createConsumer( "TEST_QUEUE" );

consumer.onMessage( function( s ) {
    console.error( consumer.getId() + ":" + JSON.stringify( s ) );
    if ( s ) {
        consumer.acknowledge( s );
    }
});

//consumer2.onMessage( function( s ) {
//    console.error( consumer2.getId() + ":" + JSON.stringify( s ) );
//    if ( s ) {
        //consumer2.acknowledge( s );
//    }
//});


//console.log( consumer.receive( 5000 ) );

//session.commit();

//var http = require('http');
 
// Configure our HTTP server to respond with Hello World to all requests.
//var server = http.createServer(function (request, response) {
//response.writeHead(200, {"Content-Type": "text/plain"});
//response.end("Hello World\n");
//});
 
// Listen on port 8000, IP defaults to 127.0.0.1
//server.listen( 3000 );

start = Date.now();
setTimeout( function(){ 
        console.log( Date.now() - start ); 
        connection2.close();
        console.log( Date.now() - start ); 
    }, 
    5000 );
    
console.log( "hier !" );

