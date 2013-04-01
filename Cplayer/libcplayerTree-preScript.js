var Module = {
  'print': function(text) { 
        var type = text.slice(0,3);
        switch(type) {
            case "DBG":
            case "ERR":
                self.postMessage({'type':"log","message":text});
                break;
            default:
                self.postMessage({'type':"word","word":text});
        }
  }
};
