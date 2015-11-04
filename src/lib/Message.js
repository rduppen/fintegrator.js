var check = require('check-types');
var RoutingSlip = require( './RoutingSlip' );

function Message( ) {
    this.routingslip = new RoutingSlip();
}

module.exports = Message;
