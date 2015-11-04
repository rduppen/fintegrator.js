var fgi = require( './build/Release/fintegrator' );


var obj = fgi.createConnection( "corbaloc:iiop:angmar.syntel.nl:16007/Master" );

var session = obj.createSession( true );

console.log( "Session created" );

var producer = session.createProducer( "TEST_QUEUE" ); 

//var message = new Message();
//message.message = "The Quick Brown Fox Jumps Over The Lazy Dog";
//var message = { message: 'The Quick Brown Fox Jumps Over The Lazy Dog',
//                type: 'DontCare'
//                };
//                priority: '5' 


console.time( "send" );

for ( i = 1; i < 1000; i++ ) {
    console.log( producer.send( { message: "message " + i,
                                  type: 'DontCare',
                                  priority: 5,
                                  properties: { 
                                    "test" : "a test property",
                                    "p2" : "another test property"
                                  }
                                } ) );
}

session.commit();
console.timeEnd( "send" );
