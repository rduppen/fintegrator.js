
function Message( ) {
}

Message.prototype.setRoutingSlip = function( a ) {
    console.log( "test" + a );
}


function RoutingSlip() {
}


module.exports = Message;

//exports.Message = Message;
//exports.RoutingSlip = RoutingSlip;