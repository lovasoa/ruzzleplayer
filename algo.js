var dict = ["bu", "va", "bave"];

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
      this.start_search();
    }
  }
}

Player.prototype.add_letter = function (letter) {
  if (this.grid[letter] === undefined){
    this.grid[letter] = {
      pos : this.curPos.slice(0),  //.slice(0) clones the array
      clean : true
    };
  } else {
    this.grid[letter].push(this.curPos);
  }
  
  this.letter_added();
}

Player.prototype.start_search = function() {
  for (var i=0; i<dict.length; i++){
    if(this.word_is_present(i, 0, [])) alert(dict[i]);
  }
}

Player.prototype.word_is_present = function(word, letter, curSquare){
  //Return true if the dict[word] is present on the grid
  //This function assumes that dict[word].slice(0,letter) is present
  //curPos indicates where we currently are on the grid
  
  if (letter === dict[word].length) return true;

  var letters = this.grid[dict[word][letter]];

  //The next letter in the word is not on the grid
  if (letters === undefined) return false;

  for (var i=0; i<letters.length; i++){
    if (letters[i].clean &&
       this.position_is_adjacent(curSquare.pos, letters[i].pos) &&
       this.word_is_present(word, letter+1, letters[i].pos)){
        return true;
    }
  }
  
}

Player.prototype.position_is_adjacent = function(pos1, pos2){
  //The empty position is adjacent to all other positions
  if (pos1.length===0) return true;
  var dist = (pos1[0]-pos2[0])*(pos1[0]-pos2[0]) + (pos1[1]-pos2[1])*(pos1[1]-pos2[1]);
  if (dist===1 || dist===2) return true;
  return false;
}
