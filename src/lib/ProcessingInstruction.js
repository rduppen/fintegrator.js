
function ProcessingInstruction( dest, op ) {
    this.destination = dest;
    this.operation = op;
    this.onSuccess;
    this.onError;
    this.processed;
    console.log( "object" );
}

ProcessingInstruction.prototype.toXml = function() {
    var result = "<ProcessingInstruction destination=\"" + this.destination + "\" >\n" + 
            this.operation.toXml();
    if ( this.onSuccess && this.onSuccess.length > 0 ) {
        result += "<onSuccess>\n";
        for ( var i = 0; i < this.onSuccess.length; i++ ) {
            result += this.onSuccess[ i ].toXml();
        }            
        result += "</onSuccess>\n";
    }
    result += "</ProcessingInstruction>\n";
    return result;
}

module.exports = ProcessingInstruction;
