var ProcessingInstruction = require( "./ProcessingInstruction.js" );


function RoutingSlip( pi ) {
    this.processingInstruction = pi;
}


RoutingSlip.prototype.toXml = function() {
    return "<?xml version=\"1.0\" ?>" + 
        "<routing:routingSlip xmlns:routing=\"http://www.syntel.nl/framework/routing\" xmlns:op=\"http://www.syntel.nl/framework/routing/operations\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">\n" + 
        this.processingInstruction.toXml() +
        "</routing:routingSlip>\n";
}

RoutingSlip.prototype.fromXml = function() {    
}


module.exports = RoutingSlip;
