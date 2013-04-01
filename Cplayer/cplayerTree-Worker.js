var dictfile = 'dict.bin';

importScripts("libcplayerTree-compiled.js");

var load_dictfile = Module.cwrap('load_dictfile', 'number', ['string']);
var init_grid = Module.cwrap('init_grid', 'number', ['string']);
var find_words = Module.cwrap('find_words');

var letters_buffer = "";

var NUM_LETTERS = 4*4;
var err;

self.postMessage({type:"loading", text:"Loading the dictionary..."});
Module["FS_createLazyFile"]('/', 'dictionary.bin', dictfile, true, true);
err = load_dictfile("/dictionary.bin");
log("Dictionary opened");
if (err !== 0) throw "Error in load_dictfile";
self.postMessage({type:"ready"});

self.onmessage = function(event) {
    var msg = event.data;
    switch (msg.type){
        case "init":
            NUM_LETTERS = msg.size[0]*msg.size[1];
            break;
        case "add_letter":
          letters_buffer += msg.letter;
          if (letters_buffer.length === NUM_LETTERS){
            err = init_grid(letters_buffer);
            log("Grid initialized");
            if (err!==0) throw "Unable to create a grid";
            var t = new Date().getTime();
            self.postMessage({type:"start"});
            find_words();
            self.postMessage({type:"end",time:(new Date().getTime() - t)});
          }
          break;
    }
};

function log(msg) {
    self.postMessage({'type':"log", msg:msg})
}
