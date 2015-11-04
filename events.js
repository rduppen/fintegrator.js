require('events');
EE = require('events').EventEmitter;
ee = new EE();

die = false;

ee.on( 'die', 
        function() {
            console.log( "Dying" );
            die = true;
        } );

setTimeout( function() {
    console.log( "Emitting" );
    ee.emit('die');
}, 1 );


var http = require('http');
 
// Configure our HTTP server to respond with Hello World to all requests.
var server = http.createServer(function (request, response) {
response.writeHead(200, {"Content-Type": "text/plain"});
response.end("Hello World\n");
});
 
// Listen on port 8000, IP defaults to 127.0.0.1
server.listen(3000);
console.log('done');
    
