var Module = {
  'print': function(text) { 
        self.postMessage({
        	'type':"log",
        	"msg":text,
        	"info":"This message was printed by the C code."
        });
  }
};

var _found_word_callback = function(strPtr) {
	self.postMessage({
		'type' : 'word',
		'word' : Pointer_stringify(strPtr)
	});
}
