var Operation = require( "./Operation" );

function Delivery( ) {
}

Delivery.prototype = new Operation( "delivery" );
Delivery.prototype.toXml = function() {
    return "<operation xsi:type=\"op:" + this.type + "\" >\n" + 
           "</operation>\n";
}

module.exports = Delivery;
