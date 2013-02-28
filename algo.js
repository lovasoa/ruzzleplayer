function Player (size) {
  this.grid = {};
  this.curPos = [0,0];
  this.size = size;
}

Player.prototype.increment_pos = function () {
  this.curPos[0] ++;
  if (this.curPos[0] === this.size[0]) {
    this.curPos[0] = 0;
    this.curPos[1] ++;
  }
}

Player.prototype.add_letter = function (letter) {
  if (this.grid[letter] === undefined) this.grid[letter] = [this.curPos.slice()];
  else this.grid[letter].push(this.curPos);
  
  if (this.curPos[0] === this.size[0] && this.curPos[1] === this.size[1]){
    this.search_words();
  }else {
    this.increment_pos();
  }
}
