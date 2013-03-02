importScripts("dict.js");

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
      this.search_words();
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

Player.prototype.search_words = function() {
  for (var i=0; i<dict.length; i++){
    if(this.word_is_present(i, 0, [])){
      self.postMessage(dict[i]);
    }
  }
}

Player.prototype.word_is_present = function(word, letter, curPos){
  //Return true if the dict[word] is present on the grid
  //This function assumes that dict[word].slice(0,letter) is present
  //curPos indicates where we currently are on the grid
  if (letter === dict[word].length) return true;

  var letters = this.grid[dict[word][letter]];

  //The next letter in the word is not on the grid
  if (letters === undefined) return false;

  for (var i=0; i<letters.length; i++){

    if (letters[i].clean &&
       this.position_is_adjacent(curPos, letters[i].pos) ) {

         letters[i].clean = false;
         var is_present = this.word_is_present(word, letter+1, letters[i].pos);
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


p = new Player([4,4]);
self.onmessage = function(event) {
  p.add_letter(event.data);
};
