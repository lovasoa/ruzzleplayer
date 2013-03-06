var fs = require('fs');

var dict = {};

console.log("Loading dictionary file in memory");
fs.readFile('/dev/stdin', 'utf8', function(err, data) {
  console.log("Extracting words");
  var words = removeAccents(data).split("\n");

  console.log("Building tree");
  for (var i=0;i<words.length;i++){
	var word = words[i].toUpperCase();
	if (word.length > 2 && word.match("-")===null ) {
		var subDict = dict;
		for(var j=0;j<word.length;j++){
			var letter = word[j]; 
			if(subDict[letter] === undefined) subDict[letter] = {};
			subDict = subDict[letter];
		}
		subDict[''] = 1;
	}
  }
  console.log("Shortening the dictionary")
  shortenDict(dict);

  console.log("Building binary file")
  var buf = new Buffer(10000000);
  var buflen = saveDictToBuffer(buf, 0, "", dict);
  fs.writeFileSync("BinaryDict.bin", buf.slice(0,buflen));
});

function shortenDict(subDict) {
    for (var l in subDict){
        if (l !== '') {
            shortenDict(subDict[l]);
            var keys = Object.keys(subDict[l]);
            if (keys.length === 1 && keys[0]!=='') {
                subDict[l+keys[0]] = subDict[l][keys[0]];
                delete subDict[l];
            }
            if (keys.length === 1 && keys[0]==='') {
                subDict[l] = 1;
            }
        }
    }
}

function saveDictToBuffer(buffer, offset, string, subDict){
//console.log("saveDictToBuffer", buffer.slice(0,5), offset, string, subDict);
    buffer.writeUInt8(string.length, offset);
    offset += 1;
    offset += buffer.write(string, offset);
    var keys = (subDict===1) ? [] : Object.keys(subDict);
    buffer.writeUInt8(keys.length, offset);
    offset += 1;
    var intLength = 4;
    var endOfNodeOffset = offset + intLength*keys.length;
    for (var k=0; k<keys.length; k++){
        var key = keys[k];
        if (key==='' && subDict[key]===1) {
            buffer.writeUInt32BE(0, offset+intLength*k);
        } else {
            buffer.writeUInt32BE(endOfNodeOffset, offset+intLength*k);
            endOfNodeOffset = saveDictToBuffer(buffer, endOfNodeOffset, key, subDict[key]);
        }
    }
    return endOfNodeOffset;
}

function removeAccents (s){
            var r=s.toLowerCase();
            r = r.replace(new RegExp(/[àáâãäå]/g),"a");
            r = r.replace(new RegExp(/æ/g),"ae");
            r = r.replace(new RegExp(/ç/g),"c");
            r = r.replace(new RegExp(/[èéêë]/g),"e");
            r = r.replace(new RegExp(/[ìíîï]/g),"i");
            r = r.replace(new RegExp(/ñ/g),"n");                
            r = r.replace(new RegExp(/[òóôõö]/g),"o");
            r = r.replace(new RegExp(/œ/g),"oe");
            r = r.replace(new RegExp(/[ùúûü]/g),"u");
            r = r.replace(new RegExp(/[ýÿ]/g),"y");
            return r;
};
