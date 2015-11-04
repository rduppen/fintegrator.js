"use strict";
var path = require( 'path' );
var fs = require( 'fs' );
var lib = path.join( path.dirname( fs.realpathSync(__filename) ), '../lib' );

var fgi = require( lib + '/fintegrator.node' );
var Message = require( lib + '/Message.js' );
var RoutingSlip = require( lib + '/RoutingSlip.js' );
var ProcessingInstruction = require( lib + '/ProcessingInstruction.js' );
var Delivery = require( lib + '/Delivery.js' );

module.exports.Message = Message;
module.exports.RoutingSlip = RoutingSlip;
module.exports.ProcessingInstruction = ProcessingInstruction;
module.exports.Delivery = Delivery;
module.exports.createConnection = fgi.createConnection;
