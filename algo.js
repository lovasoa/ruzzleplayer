self.postMessage({type:"loading", text:"Loading the dictionary..."});
importScripts("longDict.js");
self.postMessage({type:"loading", text:"Loading the algorithm..."});

function Player (size) {
  this.grid = {};
  this.curPos = [0,0];
  this.size = size;
}

Player.prototype.letter_added = function () {
  //This function increments the position, and start the algorithm
  // if the grid is full
  this.curPos[0] ++;
  if (this.curPos[0] === this.size[0]) {
    this.curPos[0] = 0;
    this.curPos[1] ++;
    if (this.curPos[1] === this.size[1]) {
      //The grid is full, start testing words
      var t = new Date().getTime();
      self.postMessage({type:"start"});
      this.search_words_JSON(dict, "");
      self.postMessage({type:"end",time:(new Date().getTime() - t)});
    }
  }
}

Player.prototype.add_letter = function (letter) {
  var square = {
      pos : this.curPos.slice(0),  //.slice(0) clones the array
      clean : true
  };
  if (this.grid[letter] === undefined){
    this.grid[letter] = [square];
  } else {
    this.grid[letter].push(square);
  }
  
  this.letter_added();
}

Player.prototype.search_words_JSON = function(curDict, curWord) {
    if(this.word_is_present_JSON(curWord, 0, [])){
        for (var i in curDict){
            if (curDict[i]===1) self.postMessage({type:"word",word:curWord});
            else this.search_words_JSON(curDict[i], curWord+i);
        }
    }
}

Player.prototype.word_is_present_JSON = function(word, letter, curPos){
  //Return true if the dict[word] is present on the grid
  //This function assumes that dict[word].slice(0,letter) is present
  //curPos indicates where we currently are on the grid
  if (letter === word.length) return true;

  var letters = this.grid[word[letter]];

  //The next letter in the word is not on the grid
  if (letters === undefined) return false;

  for (var i=0; i<letters.length; i++){

    if (letters[i].clean &&
       this.position_is_adjacent(curPos, letters[i].pos) ) {

         letters[i].clean = false;
         var is_present = this.word_is_present_JSON(word, letter+1, letters[i].pos);
         letters[i].clean = true;

         if (is_present) return true;
       }
  }
  return false;
}

Player.prototype.position_is_adjacent = function(pos1, pos2){
  //The empty position is adjacent to all other positions
  if (pos1.length===0) return true;
  var dist = (pos1[0]-pos2[0])*(pos1[0]-pos2[0]) + (pos1[1]-pos2[1])*(pos1[1]-pos2[1]);
  if (dist===1 || dist===2) return true;
  return false;
}



self.onmessage = function(event) {
    var msg = event.data;
    switch (msg.type){
        case "init":
            this.p = new Player(msg.size);
            break;
        case "add_letter":
          this.p.add_letter(msg.letter);
          break;
    }
};

self.postMessage({type:"ready"});
